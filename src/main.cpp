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
#include "log.hpp"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    Params P = parse_args(argc, argv);

    // ------------------ 设置 LOG ------------------
    std::ofstream log_ofs;
    if (P.log_enabled) {
        log_ofs.open(P.log_file);
        if (!log_ofs) {
            LOG_ERROR("Cannot open log file: " + P.log_file);
            return 1;
            }
        g_log = &log_ofs;      // 给日志文件指针
    } else {
        g_log = nullptr;       // 没有日志文件
    }

    g_log_to_console = true;    // ★ 永远打印到终端

    Gadget::Timer timer;
    timer.setTime();
    
    LOG_INFO(string("Analysis started: ") + timer.getDate());

    // ------------- 加载 dbSNP (chr-bucket) -------------
    auto db = load_dbsnp(P);
    // ------------- 运行 GWAS 处理 ----------------------
    process_gwas(P, db);

    timer.getTime();
    
    LOG_INFO(string("Analysis finished: ") + timer.getDate());
    LOG_INFO(string("Computational time: ") + timer.format(timer.getElapse()));
    return 0;
}