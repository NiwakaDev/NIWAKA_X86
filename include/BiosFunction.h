#pragma once
#include<string>
#include <queue>
#include "common.h"
class Cpu;


class BiosFunction:public Object{
    protected:
        string function_name;
    public:
        BiosFunction();
        void ShowSelf();
        virtual void ExecuteSelf(Cpu *cpu) = 0;
};

class KeyFunction:public BiosFunction{
    private:
        queue<unsigned char> q;
    public:
        KeyFunction();
        void ExecuteSelf(Cpu *cpu);
};

class VideoFunction:public BiosFunction{
    private:
        unsigned char color_table[8] = {30, 34, 32, 36, 31, 35, 33, 37};
        void out8(unsigned short addr, unsigned char value);
    public:
        VideoFunction();
        void ExecuteSelf(Cpu *cpu);
};

class FloppyFunction:public BiosFunction{
    private:
        int processed_sector_number = 0;
        int sector_number   = 0;
        int cylinder_number = 0;
        int head_number = 0;
        int drive_number = 0;
        int es=0;
        int bx=0;
        int floppy_size = 1474560;
        char* buff_region=NULL;
        FILE* disk_img_stream;
        void Read(Cpu *cpu);
        void Init();
    public:
        void ExecuteSelf(Cpu *cpu);
        FloppyFunction();
};  