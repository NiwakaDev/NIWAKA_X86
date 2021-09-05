#pragma once
#include "common.h"

class Memory;
class Device;
class Cpu;
 
class IoManager:public Object{
    private:
        Memory* mem;
    public:
        Device* device_list[DEVICE_KIND_CNT];
        IoManager(Memory* mem);
        void Out8(Cpu* cpu, unsigned int addr, unsigned char data);
        unsigned char In8(Cpu* cpu, unsigned int addr);
};
