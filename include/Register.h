#pragma once
#include "common.h"
class Register:public Object{
    protected:
        string name;
    public:
        virtual void ShowSelf();
        Register(string name);
};