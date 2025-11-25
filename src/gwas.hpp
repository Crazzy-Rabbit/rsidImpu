//
//  gwas.hpp
//  rsidImpu
//  Created by Lulu Shi on 24/11/2025.
//  Copyright © 2025 Lulu Shi. All rights reserved.
//

#ifndef RSIDIMPU_GWAS_HPP
#define RSIDIMPU_GWAS_HPP

#include <unordered_map>
#include <string>
#include "args.hpp"
#include "dbsnp.hpp"

void process_gwas(const Params& P,
                const DBMap& mapdb);

#endif

