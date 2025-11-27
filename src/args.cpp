//
//  args.cpp
//  rsidImpu
//
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//


#include "args.hpp"
#include "log.hpp"
#include <iostream>
#include <cstdlib>
#include <set>

using namespace std;

void print_help(){
    cerr
    << "\nRequired arguments:\n"
    << "  --gwas-summary     FILE     GWAS summary statistics (txt/tsv or .gz)\n"
    << "  --dbsnp            FILE     dbSNP annotation file or PLINK .bim (.gz supported)\n"
    << "  --out              FILE     Output file (only matched rows retained)\n\n"

    << "GWAS columns (optional, with default names):\n"
    << "  --chr              COL      Chromosome column name   (default: CHR)\n"
    << "  --pos              COL      Base position column     (default: POS)\n"
    << "  --A1               COL      Effect allele column     (default: A1)\n"
    << "  --A2               COL      Other allele column      (default: A2)\n"
    << "  --pval             COL      P-value column           (default: P)\n\n"

    << "dbSNP columns (ALL REQUIRED):\n"
    << "  --dbchr            COL      Chromosome column in dbSNP\n"
    << "  --dbpos            COL      Position column in dbSNP\n"
    << "  --dbA1             COL      Allele1 (REF) column in dbSNP\n"
    << "  --dbA2             COL      Allele2 (ALT) column in dbSNP\n"
    << "  --dbrsid           COL      RSID column in dbSNP\n\n"

    << "------------------------------------------------------------------\n"
    << "Additional optional parameters (output format):\n"
    << "  --format           STR      Output format (default: gwas)\n"
    << "                              Options:\n"
    << "                                 gwas  : original GWAS header + SNP (default)\n"
    << "                                 cojo  : COJO input format (SNP,A1,A2,freq,b,se,p,N)\n\n"
    << "  --freq             COL      Frequency column in GWAS file   (default: freq)\n"
    << "  --beta             COL      Beta/Effect column              (default: beta)\n"
    << "  --se               COL      Standard error column           (default: se)\n"
    << "  --n                COL      Sample size column              (default: N)\n\n"

    << "Quality Control (QC) options:\n"
    << "  --remove-dup-snp            Remove duplicate SNPs (keep lowest P)\n"
    << "  --maf              VAL      MAF threshold, default = 0.01\n\n"

    << "Multi-threading:\n"
    << "  --threads          INT      Number of threads to use (default: 1)\n\n"
    << "Logging:\n"
    << "  --log              FILE     Write log messages to FILE\n\n"

    << "Example:\n"
    << "  rsidImpu \\\n"
    << "    --gwas-summary gwas.txt --dbsnp dbsnp.txt --out out.txt \\\n"
    << "    --chr CHR --pos POS --A1 A1 --A2 A2 --pval P \\\n"
    << "    --dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID \\\n"
    << "    --remove-dup-snp --maf 0.01 --format cojo --freq Freq --beta Beta --se SE --n N\n"
    << "******************************************************************\n";
}

// ★ 所有合法的参数名（包括 flag 和带值项）
set<string> valid_params = {
    "--gwas-summary", "--dbsnp", "--out",
    "--chr", "--pos", "--A1", "--A2", "--pval",
    "--dbchr", "--dbpos", "--dbA1", "--dbA2", "--dbrsid",
    "--format", "--freq", "--beta", "--se", "--n",
    "--threads", "--maf", "--log",
    "--remove-dup-snp"
};

Params parse_args(int argc, char* argv[]) {
    map<string,string> args;
    //  ----------- FLAG 参数（无需 value 的开关）-----------
    set<string> flag_params = {"--remove-dup-snp"};
    
    cout << "******************************************************************\n";
    cout << "* rsidImpu                                                       *\n";
    cout << "* Add correct RSID to GWAS summary statistics                    *\n";
    cout << "* Authors: Loren Shi                                             *\n";
    cout << "* MIT License                                                    *\n";
    cout << "******************************************************************\n\n";

    for (int i=1; i<argc; i++){
        string a = argv[i];
        if (a == "--help" || a == "-h"){
            print_help();
            exit(0);
        }
    }
    
    // ----------- 参数解析（支持 flag）-------------
    int i = 1;
    while (i < argc){
        string key = argv[i];

        // 检查参数
        if (!valid_params.count(key)) {
            LOG_ERROR("Unknown parameter: " + key);
            exit(1);
        }

        // ★ flag 型参数（如 --remove-dup-snp）
        if (flag_params.count(key)){
            args[key] = "1";
            i += 1;
            continue;
        }

        // ★ 普通参数必须有值
        if (i+1 >= argc) {
            LOG_ERROR("Error: missing value for " + key);
            exit(1);
        }

        string val = argv[i+1];
        args[key] = val;
        i += 2;
    }

    if (!args.count("--gwas-summary") || !args.count("--dbsnp") || !args.count("--out")){
        LOG_ERROR("Error: required arguments missing.");
        print_help();
        exit(1);
    }

    Params P;
    // ---------- Required files ----------
    P.gwas_file = args["--gwas-summary"];
    P.dbsnp_file = args["--dbsnp"];
    P.out_file = args["--out"];

    // --------------------- GWAS columns -------------------
    P.g_chr = args.count("--chr") ? args["--chr"] : "CHR";
    P.g_pos = args.count("--pos") ? args["--pos"] : "POS";
    P.g_A1  = args.count("--A1")  ? args["--A1"]  : "A1";
    P.g_A2  = args.count("--A2")  ? args["--A2"]  : "A2";
    P.g_p   = args.count("--pval")  ? args["--pval"]  : "P";

    // ---------- QC options ----------
    P.remove_dup_snp = args.count("--remove-dup-snp");
    P.maf_threshold  = args.count("--maf") ? stod(args["--maf"]) : 0.01;

    // ---------- Threads ----------
    P.threads = args.count("--threads") ? stoi(args["--threads"]) : 1;

    // log 
    P.log_enabled = args.count("--log");
    P.log_file    = args.count("--log") ? args["--log"] : "";

    // ----------------- output format columns -----------------
    P.format   = args.count("--format") ? args["--format"] : "gwas";
    P.col_freq = args.count("--freq")   ? args["--freq"]   : "freq";
    P.col_beta = args.count("--beta")   ? args["--beta"]   : "beta";
    P.col_se   = args.count("--se")     ? args["--se"]     : "se";
    P.col_n    = args.count("--n")      ? args["--n"]      : "N";

    vector<string> required = {"--dbchr","--dbpos","--dbA1","--dbA2","--dbrsid"};
    for (auto &k : required) {
        if (!args.count(k)){
            LOG_ERROR("Error: missing dnSNP column " + k);
            exit(1);
        }
    }

    P.d_chr  = args["--dbchr"];
    P.d_pos  = args["--dbpos"];
    P.d_A1   = args["--dbA1"];
    P.d_A2   = args["--dbA2"];
    P.d_rsid = args["--dbrsid"];

    return P;
}