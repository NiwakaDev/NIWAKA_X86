#include "Cpu.h"
#include "TaskRegister.h"

TaskRegister::TaskRegister(string name, unsigned short value):Register(name){
    this->value = value;
    this->cache.base_addr = 0x0;
    this->cache.limit     = 0x0000FFFF;
}

void TaskRegister::ShowSelf(){
    fprintf(stdout, "%s = %04X, CACHE.BASE_ADDR = %08X, CACHE.LIMIT = %08X, ", this->name.c_str(), this->value, this->cache.base_addr, this->cache.limit);
    fflush(stdout);
}

void TaskRegister::Set(unsigned short value, Cpu* cpu){
    this->value = value;
    unsigned int desc_addr;
    if(cpu->IsProtectedMode()){
        desc_addr = cpu->GetGdtGate(value);
        this->SetCache(cpu ,desc_addr);
    }
}

void TaskRegister::SetCache(Cpu* cpu, unsigned int desc_addr){

    unsigned int base_addr;
    unsigned int limit;
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
    /***
    printf("base_addr = %08X\n", base_addr);
    printf("limit     = %08X\n", limit);
    ***/
    this->cache.base_addr = base_addr;
    this->cache.limit     = limit;
}

unsigned short TaskRegister::GetValue(){
    return this->value;
}