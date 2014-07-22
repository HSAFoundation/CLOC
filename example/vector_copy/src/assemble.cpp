#include <iostream>
#include "assemble.h"
#include "HSAILItems.h"
#include <assert.h>
using namespace Brig;
using namespace HSAIL_ASM;

int alignUp (int x, int number) {
    return x + (number - x%number);
}

void print_brig(hsa_ext_brig_module_t* brig_module){
    std::cout<<"Number of sections:"<<brig_module->section_count<<std::endl;
    for (int i=0; i<brig_module->section_count;i++) {
        hsa_ext_brig_section_header_t* section_header = brig_module->section[i];
        std::cout<<"Name:"<<(char*)section_header->name<<std::endl;
        std::cout<<"Header size:"<<section_header->header_byte_count<<std::endl;
        std::cout<<"Total size:"<<section_header->byte_count<<std::endl;
    }
}
bool CreateBrigModule(const char* kernel_source, hsa_ext_brig_module_t** brig_module_t){
    brig_container_t c = brig_container_create_empty();
    if (brig_container_assemble_from_memory(c, kernel_source, strlen(kernel_source))) { // or use brig_container_assemble_from_file
        printf("error assembling:%s\n", brig_container_get_error_text(c)); 
        brig_container_destroy(c);
        return false;
    }
    // \todo 1.0p: allow brig_container_t to manage the memory and just use brig_container_get_brig_module(c).
    uint32_t number_of_sections = brig_container_get_section_count(c);
    hsa_ext_brig_module_t* brig_module;
    brig_module = (hsa_ext_brig_module_t*)
                (malloc (sizeof(hsa_ext_brig_module_t) + sizeof(void*)*number_of_sections));
    brig_module->section_count = number_of_sections;
    for(int i=0; i < number_of_sections; ++i) {
        //create new section header
        uint64_t size_section_bytes = brig_container_get_section_size(c, i);
        void* section_copy = malloc(size_section_bytes);
        //copy the section data
        memcpy ((char*)section_copy,
            brig_container_get_section_bytes(c, i),
            size_section_bytes);
        brig_module->section[i] = (hsa_ext_brig_section_header_t*) section_copy;
    }
    //print_brig(brig_module);
    *brig_module_t = brig_module;
    brig_container_destroy(c);
    return true;
}

bool DestroyBrigModule(hsa_ext_brig_module_t* brig_module) {
     for (int i=0; i<brig_module->section_count;i++) {
        hsa_ext_brig_section_header_t* section_header = brig_module->section[i];
        free(section_header);
     }
     free (brig_module);
     return true;
}

char* GetSectionAndSize(hsa_ext_brig_module_t* brig_module, 
    int section_id, int* size) {
    hsa_ext_brig_section_header_t* section_header =
        brig_module->section[section_id];
    char* section_data = (char*)section_header + section_header->header_byte_count;
    int section_data_size = section_header->byte_count - 
        section_header->header_byte_count;
    *size = section_data_size;
    return section_data;
}

bool FindSymbolOffset(hsa_ext_brig_module_t* brig_module, 
    std::string symbol_name,SymbolType symbol_type, hsa_ext_brig_code_section_offset32_t& offset) {
        //Create a BRIG container
        BrigContainer c((Brig::BrigModule*) brig_module);
        Code first_d = c.code().begin();
        Code last_d = c.code().end();

        for (;first_d != last_d;first_d = first_d.next()) {
            switch (symbol_type) {
            case GLOBAL_READ_SYMBOLS :

                if (DirectiveVariable sym = first_d) {
                    if ((sym.segment() == BRIG_SEGMENT_GLOBAL) ||
                        (sym.segment() == BRIG_SEGMENT_READONLY)) {
                            std::string variable_name = (SRef)sym.name();
                            if (variable_name == symbol_name) {
                                offset = sym.brigOffset();
                                return true;
                            }
                    }
                }
                break;
            case KERNEL_SYMBOLS :
                if (DirectiveExecutable de = first_d) {
                    if (symbol_name == de.name()) {
                        offset = de.brigOffset();
                        return true;
                    }
                }
                break;
            default:
                return false;
            }
        }
        return false;
}

