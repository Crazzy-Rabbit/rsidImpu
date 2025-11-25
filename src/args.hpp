//
//  args.hpp
//  rsidImpu
//
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//

#ifndef RSIDIMPU_ARGS_HPP
#define RSIDIMPU_ARGS_HPP

#include <string>
#include <vector>
#include <map>

struct Params {
    // ---------- Required input/output files ----------
    std::string gwas_file;
    std::string dbsnp_file;
    std::string out_file;

    // ---------- GWAS columns ----------
    std::string g_chr;
    std::string g_pos;
    std::string g_A1;
    std::string g_A2;
    std::string g_p;

    // ---------- QC options (NEW) ----------
    bool remove_dup_snp;      // --remove-dup-snp
    double maf_threshold;     // --maf (default 0.01)

    // ---------- Threads ----------
    int    threads;          // --threads
    
    // ---------- Logging ----------
    bool        log_enabled; // --log
    std::string log_file;

    // ---------- Output format columns ----------
    std::string format;
    std::string col_freq;
    std::string col_beta;
    std::string col_se;
    std::string col_n;

    // ---------- dbSNP columns ----------
    std::string d_chr;
    std::string d_pos;
    std::string d_A1;
    std::string d_A2;
    std::string d_rsid;
};

void print_help();
Params parse_args(int argc, char* argv[]);

#endif
