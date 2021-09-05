#pragma once
#include "common.h"

class Device:public Object{
    public:
        Device();
        virtual void Out8(unsigned int addr, unsigned char data) = 0;
        virtual unsigned char In8(unsigned int addr) = 0;
        virtual int  IsIrq();
        virtual bool IsEnable();
        virtual void ShowSelf();
};