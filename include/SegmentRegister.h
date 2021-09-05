#pragma once
#include "common.h"
#include "Register.h"
class Cpu;

class SegmentRegister:public Register{
    private:
        unsigned short value;
        Cache cache;
        void SetCache(Cpu* cpu, unsigned int desc_addr);
    public:
        void ShowSelf();
        SegmentRegister(string name, unsigned short value);
        unsigned int GetBaseAddr();
        unsigned int GetLimit();
        bool Is32bitsMode();
        unsigned short GetValue();
        void Set(unsigned short data, Cpu* cpu);
};