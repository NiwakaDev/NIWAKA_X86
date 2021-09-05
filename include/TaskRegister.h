#pragma once
#include<string>
#include "DescData.h"
#include "Register.h"
using namespace std;
class Cpu;

class TaskRegister:public Register{
    private:
        unsigned short value;
        Cache cache;
        void SetCache(Cpu* cpu, unsigned int desc_addr);
    public: 
        void ShowSelf();
        TaskRegister(string name, unsigned short value);
        void Set(unsigned short value, Cpu* cpu);
        unsigned short GetValue();
};