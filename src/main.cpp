//
//  main.cpp
//  rsidImpu
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//

#include "args.hpp"
#include "dbsnp.hpp"
#include "gwas.hpp"
#include "gadgets.hpp"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    Params P = parse_args(argc, argv);

    Gadget::Timer timer;
    timer.setTime();
    cout << "\nAnalysis started: " << timer.getDate() << endl;

    auto db = load_dbsnp(P);
    process_gwas(P, db);

    timer.getTime();
    
    cout << "\nAnalysis finished: " << timer.getDate() << endl;
    cout << "Computational time: "  << timer.format(timer.getElapse()) << endl;

    return 0;
}