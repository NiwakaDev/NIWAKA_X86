#pragma once
#include "common.h"

class Cpu;
class Pic;

class IntHandler:public Object{
    private:
        Cpu* cpu;
        Pic* pic;
        void Push32(unsigned int data);
        void Push16(unsigned short data);
        unsigned int Pop32();
        unsigned short Pop16();
        IdtGate GetIdtGate(unsigned int idt_base_addr, unsigned int irq_num);
    public:
        IntHandler(Cpu* cpu, Pic* pic);
        void Handle(int irq_num);
};