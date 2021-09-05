#pragma once
#include "common.h"
#include "Register.h"

class DescriptorTableRegister:public Register{
    protected:
        unsigned short limit;
        unsigned int   base;
    public:
        DescriptorTableRegister(string name, unsigned short limit, unsigned int base);
        void Set(unsigned short limit, unsigned int base);
        unsigned int GetBase();
        void ShowSelf();
};