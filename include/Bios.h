#pragma once
#include "common.h"

class Cpu;
class Memory;
class Object;
class BiosFunction;

class Bios:public Object{
    private:
        int bios_function_size;
    public:
        BiosFunction *bios_function_array[256];
        Bios();
        void ShowSelf();
        void CallFunction(Cpu* cpu, unsigned char bios_number);
        void LoadIpl(Memory* mem);
};