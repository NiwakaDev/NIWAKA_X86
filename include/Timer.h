#pragma once
#include "common.h"
#include "Device.h"
class Pic;

class Timer:public Device{
    private:
        Pic* pic;
        thread* timer_thread;
        unsigned int cycle;
        unsigned int clock;
    public:
        Timer(Pic* pic);
        void Out8(unsigned int addr, unsigned char data);
        unsigned char In8(unsigned int addr);
        void Sleep();
};