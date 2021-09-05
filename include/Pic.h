#pragma once
#include "common.h"
#include "Device.h"

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021 
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

class Memory;
class Fifo;

class Pic:public Device{
    public:
        void Execute();
        Pic(Memory *mem);
        void Out8(unsigned int addr, unsigned char data);
        unsigned char In8(unsigned int addr);
        void ShowSelf();
        void Set(unsigned int irq_number);
        int GetNowIrq();
        int  IsIrq();
    private:
        bool irq_list[16];//割り込み許可
        Memory* mem;
        Fifo* fifo;
        int now_irq;
};