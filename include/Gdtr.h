#pragma once
#include "common.h"
#include "DescriptorTableRegister.h"

class Gdtr:public DescriptorTableRegister{
    public:
        Gdtr(string name, unsigned short limit, unsigned int base);
};
