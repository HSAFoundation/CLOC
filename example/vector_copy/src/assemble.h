#pragma once
#include "hsail_c.h"
#include "hsa.h"
#include "hsa_ext_finalize.h"
#include <string>

enum SymbolType {
    GLOBAL_READ_SYMBOLS=0,
    KERNEL_SYMBOLS
};

bool FindSymbolOffset(hsa_ext_brig_module_t* brig_module, 
    std::string symbol_name, SymbolType symbol_type,hsa_ext_brig_code_section_offset32_t& offset);

