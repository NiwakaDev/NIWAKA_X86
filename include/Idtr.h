#pragma once
#include "common.h"
#include "DescriptorTableRegister.h"

class Idtr:public DescriptorTableRegister{
    public:
        Idtr(string name, unsigned short limit, unsigned int base);
};