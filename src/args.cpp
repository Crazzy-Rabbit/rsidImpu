//
//  rsidImpu.cpp
//
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//


#include "args.hpp"
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
    << "                                 smr   : SMR input format (SNP,A1,A2,Freq,Beta,SE,P,N)\n\n"
    << "  --freq             COL      Frequency column in GWAS file   (default: freq)\n"
    << "  --beta             COL      Beta/Effect column              (default: beta)\n"
    << "  --se               COL      Standard error column           (default: se)\n"
    << "  --n                COL      Sample size column              (default: N)\n\n"
    << "------------------------------------------------------------------\n"
    << "Quality Control (QC) options:\n"
    << "  --remove-dup-snp            Remove duplicate SNPs (keep lowest P)\n"
    << "  --maf              VAL      MAF threshold, default = 0.01\n\n"
    << "------------------------------------------------------------------\n"
    << "Example:\n"
    << "  rsidImpu \\\n"
    << "    --gwas-summary gwas.txt --dbsnp dbsnp.txt --out out.txt \\\n"
    << "    --chr CHR --pos POS --A1 A1 --A2 A2 --pval P \\\n"
    << "    --dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID \\\n"
    << "    --remove-dup-snp --maf 0.01 --format smr --freq Freq --beta Beta --se SE --n N\n\n"
    << "******************************************************************\n";
}

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

        if (flag_params.count(key)){
            args[key] = "1";
            i += 1;
            continue;
        }

        if (i+1 >= argc) {
            cerr << "Error: missing value for " << key << "\n";
            exit(1);
        }

        string val = argv[i+1];
        args[key] = val;
        i += 2;
    }

    if (!args.count("--gwas-summary") || !args.count("--dbsnp") || !args.count("--out")){
        cerr << "Error: required arguments missing. \n";
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
    
    // ----------------- output format columns -----------------
    P.format   = args.count("--format") ? args["--format"] : "gwas";
    P.col_freq = args.count("--freq")   ? args["--freq"]   : "freq";
    P.col_beta = args.count("--beta")   ? args["--beta"]   : "beta";
    P.col_se   = args.count("--se")     ? args["--se"]     : "se";
    P.col_n    = args.count("--n")      ? args["--n"]      : "N";

    vector<string> required = {"--dbchr","--dbpos","--dbA1","--dbA2","--dbrsid"};
    for (auto &k : required) {
        if (!args.count(k)){
            cerr << "Error: missing dnSNP column " << k << endl;
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