#pragma once
#include "common.h"
#include "Register.h"
class ControlRegister:public Register{
    private:
        unsigned int value;
    public:
        void ShowSelf();
        ControlRegister(string name, unsigned int value);
        unsigned int GetValue();
        void Set(unsigned int value);
};