#pragma once
#include "common.h"

class Vram;

class Memory:public Object{
    private:
        long long size;
        unsigned char *memory_content;
    public:
        Vram* vram;
        Memory();
        void ShowSelf();
        void Write8(long long addr, unsigned char data);
        void Write16(long long addr, unsigned short data);
        void Write32(long long addr, unsigned int data);
        unsigned char Read8(long long addr);
        unsigned short Read16(long long addr);
        unsigned int  Read32(long long addr);
        void PrintMem(long long start_addr, long long cnt);
};  