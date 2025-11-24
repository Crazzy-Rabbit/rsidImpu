//
//  dbsnp.cpp
//  rsidImpu
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//

#include "dbsnp.hpp"
#include "linereader.hpp"
#include "allele.hpp"
#include "util.hpp"
#include <iostream>

using namespace std;

unordered_map<string,string> load_dbsnp(const Params& P){
    unordered_map<string,string> mapdb;
    mapdb.reserve(50000000);

    LineReader reader(P.dbsnp_file);
    string line;
    bool is_bim = ends_with(P.dbsnp_file, ".bim") ||
                    ends_with(P.dbsnp_file, ".bim.gz");
    
    int dCHR, dPOS, dA1, dA2, dRS;

    if (!is_bim){
        reader.getline(line);
        auto hdr = split(line);
        dCHR = find_col(hdr, P.d_chr);
        dPOS = find_col(hdr, P.d_pos);
        dA1  = find_col(hdr, P.d_A1);
        dA2  = find_col(hdr, P.d_A2);
        dRS  = find_col(hdr, P.d_rsid);

        if (dCHR<0||dPOS<0||dA1<0||dA2<0||dRS<0){
            cerr << "dbSNP header incomplete.\n"; exit(1);
        }
    } else {
        dCHR=0; dRS=1; dPOS=3; dA1=4; dA2=5;
    }

    while (reader.getline(line)){
        if (line.empty()) continue;
        auto f = split(line);
        string key = make_key(f[dCHR], f[dPOS], f[dA1], f[dA2]);
        if (!mapdb.count(key)){
            mapdb[key] = f[dRS];
        }
    }

    cerr << "dbSNP entries loaded: " << mapdb.size() << endl;
    return mapdb;
}