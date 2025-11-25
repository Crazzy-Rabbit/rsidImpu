//
//  gwas.cpp
//  rsidImpu
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//

#include "gwas.hpp"
#include "linereader.hpp"
#include "allele.hpp"
#include "util.hpp"
#include "writer.hpp"  // support .gz format out
#include "gwasQC.hpp" // basic QC
#include "log.hpp"
#include "dbsnp.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <deque>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

void process_gwas(const Params& P,
                    const DBMap& mapdb)
{
    // threads
    #ifdef _OPENMP
    if (P.threads > 0) {
        omp_set_num_threads(P.threads);
        LOG_INFO("Using threads = " + std::to_string(P.threads));
    }
    #endif

    deque<string> gwas_lines;
    string line;
    
    //================ 1. 读取 GWAS header =================
    LineReader reader(P.gwas_file);
    if (!reader.getline(line)) {
        LOG_ERROR("Empty GWAS summary file.");
        exit(1);
    }

    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    auto header = split(line);
    
    // find the col, CHR, POS, A1, A2, P 列
    int gCHR = find_col(header, P.g_chr);
    int gPOS = find_col(header, P.g_pos);
    int gA1  = find_col(header, P.g_A1);
    int gA2  = find_col(header, P.g_A2);
    int gP   = find_col(header, P.g_p);

    if (gCHR<0||gPOS<0||gA1<0||gA2<0||gP<0){
        LOG_ERROR("GWAS header incomplete.");
        exit(1);
    }

    // format = smr
    int colFreq = -1, colBeta = -1, colSe = -1, colN = -1;
    bool is_smr = (P.format == "smr");

    if (is_smr){
        colFreq = find_col(header, P.col_freq);
        colBeta = find_col(header, P.col_beta);
        colSe   = find_col(header, P.col_se);
        colN    = find_col(header, P.col_n);

        if (colFreq<0 || colBeta<0 || colSe<0 || colN<0) {
            LOG_ERROR("Error: SMR format requires freq, beta, se, n columns.");
            exit(1);
        }
    }

    // format = smr
    // int colFreq = -1, colBeta = -1, colSe = -1, colN = -1;
    // bool is_smr = (P.format == "smr");

    // if (is_smr){
    //     colFreq = find_col(header, P.col_freq);
    //     colBeta = find_col(header, P.col_beta);
    //     colSe   = find_col(header, P.col_se);
    //     colN    = find_col(header, P.col_n);

    //     if (colFreq<0 || colBeta<0 || colSe<0 || colN<0) {
            // LOG_ERROR("Error: SMR format requires freq, beta, se, n columns.");
            // exit(1);
    //     }
    // }
    
    //================ 2. 读入 GWAS 数据行 =================
    while (reader.getline(line)){
        if (line.empty()) continue;
        gwas_lines.push_back(line);
    }
    
    size_t n = gwas_lines.size();
    LOG_INFO("Loaded GWAS lines (data): " + std::to_string(n));

    //================ 基础 QC：过滤无效 N/beta/se/freq/P =================
    vector<bool> keep_qc(gwas_lines.size(), true);

    int idx_beta = find_col(header, P.col_beta);
    int idx_se   = find_col(header, P.col_se);
    int idx_freq = find_col(header, P.col_freq);
    int idx_pv   = find_col(header, P.g_p);     // pval
    int idx_n    = find_col(header, P.col_n);
    double maf   = P.maf_threshold;
    
    gwas_basic_qc(gwas_lines, header, idx_beta, idx_se, idx_freq, idx_pv, idx_n, keep_qc, maf);

    vector<bool>    keep(n,false);
    vector<string>  rsid_vec(n);
    
    // ---------- 把 QC 过滤结果同步到 keep ----------
    for (size_t i = 0; i < n; i++){
        if (!keep_qc[i]) keep[i] = false;
    }

    //================ 3. 匹配 dbSNP，确定哪些行有 rsid =================
#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (long i=0; i<(long)n; i++){
        auto f = split(gwas_lines[i]);
        
        int max_col = gCHR;
        if (gPOS > max_col) max_col = gPOS;
        if (gA1  > max_col) max_col = gA1;
        if (gA2  > max_col) max_col = gA2;
        if (gP   > max_col) max_col = gP;

        if ((int)f.size() <= max_col) continue;

        // chr-bucket DBMap
        string key = make_key(f[gCHR], f[gPOS], f[gA1], f[gA2]);
        string chr = norm_chr(f[gCHR]);

        auto it_chr = mapdb.find(chr);
        if (it_chr != mapdb.end()) {
            auto it = it_chr->second.find(key);
            if (it != it_chr->second.end()) {
                keep[i] = true;
                rsid_vec[i] = it->second;
            }
        }
    }
    
    if (P.remove_dup_snp) {
        gwas_remove_dup(
            gwas_lines,
            header,
            gP,          // P 列 index
            rsid_vec,
            keep
        );
    }
    
    //================ 4. 构建 Writer（自动 txt / gz） =================
    bool out_is_gz = ends_with(P.out_file, ".gz");

    std::string out_main    = P.out_file;
    std::string out_unmatch = out_is_gz ?
                                (P.out_file + ".unmatched.gz") :
                                (P.out_file + ".unmatched");
    
    Writer fout(out_main, P.format);
    Writer funm(out_unmatch, P.format);

    if (!fout.good() || !funm.good()) {
        LOG_ERROR("Error opening output file.");
        exit(1);
    }
    
    // header 
    if (is_smr){
        fout.write_smr_header();
        funm.write_smr_header();
    } else {
        std::string header_line;
        for (size_t i=0; i<header.size(); ++i){
            if (i) header_line += '\t';
            header_line += header[i];
        }
        header_line += '\t';
        header_line += "SNP";

        fout.write_line(header_line);
        funm.write_line(header_line);
    }
    
    // outfile 
    for (size_t i=0; i<n; i++){
        gwas_lines[i].erase(
            std::remove(gwas_lines[i].begin(), gwas_lines[i].end(), '\r'),
            gwas_lines[i].end()
        );

        if (!keep[i]) {
            // unmatched 行直接写到 unmatched 文件
            funm.write_line(gwas_lines[i]);
            continue;
        }

        // 只对 matched 行进行格式化输出
        if (is_smr) {
            // ------------- SMR 格式输出 -------------
            auto f = split(gwas_lines[i]);

            int max_col2 = colFreq;
            max_col2 = std::max(max_col2, colBeta);
            max_col2 = std::max(max_col2, colSe);
            max_col2 = std::max(max_col2, colN);
            max_col2 = std::max(max_col2, gA1);
            max_col2 = std::max(max_col2, gA2);
            max_col2 = std::max(max_col2, gP);

            if ((int)f.size() <= max_col2) {
                // 万一这一行列数不够，当 unmatched 处理
                funm.write_line(gwas_lines[i]);
                continue;
            }

            // SNP A1 A2 freq beta se P N
            string out_line =
                rsid_vec[i]   + "\t" +
                f[gA1]        + "\t" +
                f[gA2]        + "\t" +
                f[colFreq]    + "\t" +
                f[colBeta]    + "\t" +
                f[colSe]      + "\t" +
                f[gP]         + "\t" +
                f[colN];

            fout.write_line(out_line);
        } else {
            // ------------- 默认 gwas 格式输出 -------------
            // 原始行 + SNP
            string out_line = gwas_lines[i] + "\t" + rsid_vec[i];
            fout.write_line(out_line);
        }
    }
}