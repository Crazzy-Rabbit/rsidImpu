//
//  rsidImpu.cpp
//
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//


#include "args.hpp"
#include <iostream>

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

    << "Notes:\n"
    << "  * Matching allows allele flipping (A1/A2 <-> A2/A1)\n"
    << "  * Matching allows strand complementarity (A<->T, C<->G)\n"
    << "  * GWAS alleles are NEVER modified — only RSID is added\n"
    << "  * Only matched GWAS rows are kept in output\n"
    << "  * Supports gzipped input (.gz)\n\n"

    << "Example:\n"
    << "  rsidImpu \\\n"
    << "    --gwas-summary gwas.txt --dbsnp dbsnp.txt --out gwas_rsid.txt \\\n"
    << "    --chr CHR --pos POS --A1 EA --A2 OA --pval PVAL \\\n"
    << "    --dbchr CHROM --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID\n\n"
    << "******************************************************************\n";
}

Params parse_args(int argc, char* argv[]) {
    map<string,string> args;
    
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
    
    for (int i=1; i<argc; i+=2){
        if (i+1 >= argc){
            cerr << "Error: missing value for " << argv[i] << endl;
            exit(1);
        }
        args[argv[i]] = argv[i + 1];
    }

    if (!args.count("--gwas-summary") || !args.count("--dbsnp") || !args.count("--out")){
        cerr << "Error: required arguments missing. \n";
        print_help();
        exit(1);
    }

    Params P;
    P.gwas_file = args["--gwas-summary"];
    P.dbsnp_file = args["--dbsnp"];
    P.out_file = args["--out"];

    P.g_chr = args.count("--chr") ? args["--chr"] : "CHR";
    P.g_pos = args.count("--pos") ? args["--pos"] : "POS";
    P.g_A1  = args.count("--A1")  ? args["--A1"]  : "A1";
    P.g_A2  = args.count("--A2")  ? args["--A2"]  : "A2";
    P.g_p   = args.count("--pval") ? args["--pval"] : "P";

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