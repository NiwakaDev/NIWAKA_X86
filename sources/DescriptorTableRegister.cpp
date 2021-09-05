#include "DescriptorTableRegister.h"
#include "Gdtr.h"
#include "Idtr.h"

DescriptorTableRegister::DescriptorTableRegister(string name, unsigned short limit, unsigned int base):Register(name){
    this->limit = limit;
    this->base  = base;
}   

void DescriptorTableRegister::Set(unsigned short limit, unsigned int base){
    this->limit = limit;
    this->base  = base;
}

void DescriptorTableRegister::ShowSelf(){
    fprintf(stdout, "%s.base  = 0x%08X\n", this->name.c_str(), this->base);
    fprintf(stdout, "%s.limit = 0x%04X\n", this->name.c_str(), this->limit);
    fflush(stdout);
}

Gdtr::Gdtr(string name, unsigned short limit, unsigned int base):DescriptorTableRegister(name, limit, base){

}

Idtr::Idtr(string name, unsigned short limit, unsigned int base):DescriptorTableRegister(name, limit, base){

}

unsigned int DescriptorTableRegister::GetBase(){
    return this->base;
}
