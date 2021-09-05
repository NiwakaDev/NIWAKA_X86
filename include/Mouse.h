#pragma once
#include "common.h"
#include "Device.h"

class Pic;

class Mouse:public Device{
    private:
        Pic* pic;
        bool enable_flg;
        unsigned char data;
    public:
        Mouse(Pic* pic);
        void Out8(unsigned int addr, unsigned char data);
        unsigned char In8(unsigned int addr);
        bool IsEnable();
        void SetEnable();
        void Receive(unsigned char data);
        void HandleMouseMotion(unsigned char state, unsigned char dx, unsigned char dy);
};