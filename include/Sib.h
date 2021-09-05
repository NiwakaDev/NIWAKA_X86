#pragma once
#include "common.h"
class Cpu;

class Sib{
    public:
        void ParseSib(unsigned char sib_byte, unsigned char mod);
        unsigned int GetAddress(Cpu* cpu);
        unsigned int GetBase();
        unsigned int GetScale();
        unsigned int GetIndex();
        void SetDisp32(unsigned int disp32);
    private:
        unsigned char scale;
        unsigned char index;
        unsigned char base;
        unsigned char mod;
        unsigned int disp32;
        void ShowSelf();
};