#include "Cpu.h"
#include "SegmentRegister.h"

SegmentRegister::SegmentRegister(string name, unsigned short value):Register(name){
    this->value = value;
    this->cache.base_addr = 0x0;
    this->cache.limit     = 0x0000FFFF;
    this->cache.bit32_mode = false;
}

void SegmentRegister::ShowSelf(){
    fprintf(stdout, "%s = %04X, CACHE.BASE_ADDR = %08X, CACHE.LIMIT = %08X, ", this->name.c_str(), this->value, this->cache.base_addr, this->cache.limit);
    if(this->cache.bit32_mode){
        fprintf(stdout, "32BIT_MODE\n");
    }else{
        fprintf(stdout, "16BIT_MODE\n");
    }
    fflush(stdout);
}

unsigned int SegmentRegister::GetBaseAddr(){
    return this->cache.base_addr;
}

unsigned int SegmentRegister::GetLimit(){
    return this->cache.limit;
}

bool SegmentRegister::Is32bitsMode(){
    return this->cache.bit32_mode;
}

unsigned short SegmentRegister::GetValue(){
    return this->value;
}

void SegmentRegister::SetCache(Cpu* cpu, unsigned int desc_addr){
    unsigned int base_addr;
    unsigned int limit;
    bool bit32_mode;
    GdtGate gdt_gate;

    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);
    /***
    fprintf(stderr, "desc:\n");
    printf("limit_low = %04X\n", gdt_gate.limit_low);
    printf("base_low = %04X\n", gdt_gate.base_low);
    printf("base_mid = %04X\n", gdt_gate.base_mid);
    printf("access_right = %04X\n", gdt_gate.access_right);
    printf("limit_high = %04X\n", gdt_gate.limit_high);
    printf("base_high = %04X\n", gdt_gate.base_high);
    ***/
    base_addr = 0;
    base_addr = (((unsigned int)gdt_gate.base_high)<<24) | (((unsigned int)gdt_gate.base_mid)<<16) | (unsigned int)gdt_gate.base_low;
    limit     = 0x03FFFFFF & ((((unsigned int)gdt_gate.limit_high)<<16) | (unsigned int)gdt_gate.limit_low);
    //printf("base_addr = %08X\n", base_addr);
    //printf("limit     = %08X\n", limit);
    this->cache.base_addr = base_addr;
    this->cache.limit     = limit;
    this->cache.bit32_mode = (gdt_gate.limit_high&DB)==DB;
}

void SegmentRegister::Set(unsigned short data, Cpu* cpu){
    unsigned int desc_addr;
    if(cpu->IsProtectedMode()){
        desc_addr = cpu->GetGdtGate(data);
        this->value = data;
        this->SetCache(cpu, desc_addr);
        return;
    }
    this->value = data;
}