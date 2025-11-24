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
#include <fstream>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

void process_gwas(const Params& P,
                    const unordered_map<string,string>& mapdb)
{
    vector<string> gwas_lines;
    string line;

    LineReader reader(P.gwas_file);
    reader.getline(line);
    auto header = split(line);

    int gCHR = find_col(header, P.g_chr);
    int gPOS = find_col(header, P.g_pos);
    int gA1  = find_col(header, P.g_A1);
    int gA2  = find_col(header, P.g_A2);
    int gP   = find_col(header, P.g_p);

    if (gCHR<0||gPOS<0||gA1<0||gA2<0||gP<0){
        cerr << "GWAS header incomplete.\n"; exit(1);
    }

    while (reader.getline(line)){
        if (!line.empty()) gwas_lines.push_back(line);
    }

    size_t n = gwas_lines.size();
    vector<bool> keep(n,false);
    vector<string> rsid_vec(n);

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

        string key = make_key(f[gCHR], f[gPOS], f[gA1], f[gA2]);
        auto it = mapdb.find(key);
        if (it != mapdb.end()){
            keep[i] = true;
            rsid_vec[i] = it->second;
        }
    }

    ofstream fout(P.out_file);
    for (size_t i=0; i<header.size(); i++){
        if (i) fout << '\t';
        fout << header[i];
    }
    fout << '\t' << "SNP\n";

    ofstream funm(P.out_file + ".unmatched");

    for (size_t i=0; i<n; i++){
        if (keep[i])
            fout << gwas_lines[i] << '\t' << rsid_vec[i] << '\n';
        else
            funm << gwas_lines[i] << '\n';
    }

    fout.close();
    funm.close();
}