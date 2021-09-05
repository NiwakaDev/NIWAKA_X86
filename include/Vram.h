#pragma once
#include "common.h"
#include "Device.h"

class Memory;

class Vram:public Device{
    public:
        void Execute();
        void GetVramMode();
        Pixel GetPixel(int x, int y);
        Vram(Memory *mem);
        VIDEO_DA_CONVERTER_MODE video_da_converter_mode;
        unsigned char now_palette_number;
        void Out8(unsigned int addr, unsigned char data);
        unsigned char In8(unsigned int addr);
    private:
        thread* vram_thread;
        unsigned char palette[16][3];
        Memory* mem;
        VRAM_MODE vram_mode;
        unsigned char* vram_buff;
        int height;
        int width;
        int vram_mem_size;
        int vram_start_addr = VRAM_START_ADDR;
        void InitPalette();
        void SetColor(int idx, unsigned int color);
        void VramTransfer(unsigned int addr, unsigned char palette_num);
        void VramTransferCordinate(int x, int y, unsigned char palette_num);
        Pixel GetPlatteNum2ColorValue(unsigned char palennte_num);
        Pixel GetPaletteNum2Pixel(unsigned char palette_num);
};