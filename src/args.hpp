//
//  rsidImpu.cpp
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
    std::string gwas_file, dbsnp_file, out_file;
    std::string g_chr, g_pos, g_A1, g_A2, g_p;
    std::string d_chr, d_pos, d_A1, d_A2, d_rsid; 
};

void print_help();

Params parse_args(int argc, char* argv[]);

#endif
