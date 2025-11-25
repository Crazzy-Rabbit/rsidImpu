#include "writer.hpp"
#include "util.hpp"   // 用里面的 ends_with
#include <iostream>

Writer::Writer(const std::string &filename, const std::string &format)
    : use_gz_(false), ok_(false), gzfp_(nullptr), format_(format)
{
    // 判断是否以 .gz 结尾
    if (ends_with(filename, ".gz")) {
        use_gz_ = true;
        gzfp_ = gzopen(filename.c_str(), "wb");
        if (!gzfp_) {
            std::cerr << "Error: cannot open gzip file for writing: "
                        << filename << std::endl;
            return;
        }
    } else {
        ofs_.open(filename);
        if (!ofs_) {
            std::cerr << "Error: cannot open text file for writing: "
                        << filename << std::endl;
            return;
        }
    }
    ok_ = true;
}

Writer::~Writer() {
    if (use_gz_) {
        if (gzfp_) gzclose(gzfp_);
    } else {
        if (ofs_.is_open()) ofs_.close();
    }
}

void Writer::write_line(const std::string &line) {
    if (!ok_) return;

    if (use_gz_) {
        gzputs(gzfp_, (line + "\n").c_str());
    } else {
        ofs_ << line << '\n';
    }
}

void Writer::write_smr_header(){
    if (format_ == "smr") {
        write_line("SNP\tA1\tA2\tfreq\tbeta\tse\tP\tN");
    } 
}