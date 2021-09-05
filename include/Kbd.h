#pragma once
#include "common.h"
#include "Device.h"

class Memory;
class Mouse;
class Pic;

class Kbd:public Device{
    private:
        //unsigned char status_register;
        union{
            uint8_t raw;
            struct{
                unsigned obf : 1;
                unsigned ibf : 1;
                unsigned reserved  : 6;
            }flgs;
        }status_register;
        Memory* mem;
        Mouse* mouse;
        Pic* pic;
        uint8_t output_buf;
        uint8_t input_buf;
        mutex kbd_mutex;
    public:
        Kbd(Memory* mem, Pic* pic, Mouse* mouse);
        void Receive(unsigned char data);
        void Out8(unsigned int addr, unsigned char data);
        unsigned char In8(unsigned int addr);
        void ProcessCommand(unsigned char data);
};