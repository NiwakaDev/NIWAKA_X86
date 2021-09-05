#include "ControlRegister.h"

ControlRegister::ControlRegister(string name, unsigned int value):Register(name){
    this->value = value;
}

void ControlRegister::ShowSelf(){
    fprintf(stdout, "%s = %08X\n", this->name.c_str(), this->value);
    fflush(stdout);
}

unsigned int ControlRegister::GetValue(){
    return this->value;
}

void ControlRegister::Set(unsigned int value){
    this->value = value;
}
