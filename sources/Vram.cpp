#include "Memory.h"
#include "Vram.h"

Vram::Vram(Memory* mem){
    Pixel pixel;
    this->mem = mem;
    this->height = HEIGHT;
    this->width  = WIDTH;
    this->vram_mem_size = this->height*this->width;
    this->vram_mode = X13;
    this->InitPalette();
    this->vram_buff = (unsigned char*)malloc(this->vram_mem_size);
    this->video_da_converter_mode = VIDEO_DA_NOP;
    //this->vram_thread = new thread(&Vram::Execute, this);
}

Pixel Vram::GetPixel(int x, int y){
    if( x < 0 || x>=this->width || y < 0 || y >= this->height){
        cerr << "Vram::GetPixel関数内で範囲外のアクセスがありました" << endl;
        cerr << " x = " << x << endl;
        cerr << " y = " << y << endl;
        this->Error("\n");
    }

    Pixel pixel;
    pixel = this->GetPaletteNum2Pixel(this->mem->Read8(this->vram_start_addr+x+y*this->width));
    //pixel = this->GetPaletteNum2Pixel(this->vram_buff[x+y*this->width]);
    return pixel;
}

Pixel Vram::GetPlatteNum2ColorValue(unsigned char palette_num){
    Pixel color_value;
    return color_value;
}

void Vram::VramTransfer(unsigned int addr, unsigned char palette_num){
    this->vram_buff[addr] = palette_num;
}

void Vram::VramTransferCordinate(int x, int y, unsigned char palette_num){
    this->vram_buff[x+y*this->width] = palette_num;
}

Pixel Vram::GetPaletteNum2Pixel(unsigned char palette_num){
    Pixel pixel;
    unsigned int color_value;
    //color_value = this->GetPlatteNum2ColorValue(palette_num);
    pixel.a = 0xFF;
    pixel.r = this->palette[palette_num][0];
    pixel.g = this->palette[palette_num][1];
    pixel.b = this->palette[palette_num][2];
    return pixel;
}

void Vram::SetColor(int idx, unsigned int color){
    unsigned char r, g, b;
    r = (unsigned char)((color&0x00FF0000) >> 16);
    g = (unsigned char)((color&0x0000FF00) >> 8);
    b = (unsigned char)((color&0x000000FF));   
    this->palette[idx][0] = r;
    this->palette[idx][1] = g;
    this->palette[idx][2] = b;
}

void Vram::InitPalette(){
    this->SetColor(0, 0x00000000);
    this->SetColor(1, 0x000000ff);
    this->SetColor(2, 0x0000ff00);
    this->SetColor(3, 0x0000ffff);
    this->SetColor(4, 0x00ff0000);
    this->SetColor(5, 0x00ff00ff);
    this->SetColor(6, 0x00ffff00);
    this->SetColor(7, 0x00ffffff);
    this->SetColor(8, 0xff000000);
    this->SetColor(9, 0xff0000ff);
    this->SetColor(10,0xff00ff00);
    this->SetColor(11, 0xff00ffff);
    this->SetColor(12,0xffff0000);
    this->SetColor(13, 0xffff00ff);
    this->SetColor(14, 0xffffff00);
    this->SetColor(15, 0xffffffff);
}

void Vram::GetVramMode(){
    switch (this->vram_mode){
        case X13:
            cout << "0x13" << endl;
            break;
        default:
            break;
    }
}

/***
void Vram::Execute(){
    Pixel pixel;
    while(1){
        for(int offset=0; offset<this->vram_mem_size; offset++){
            this->vram_buff[offset] = this->mem->Read8(this->vram_start_addr+offset);
        }
    }
}
***/

void Vram::Out8(unsigned int addr, unsigned char data){
    static int internal_cnt = 0;
    switch(addr){
        case 0x03C8:
            this->video_da_converter_mode = VIDEO_DA_WRITE;
            this->now_palette_number      = data;
            if(this->now_palette_number==16){
                this->Error("now_paletter_number = 16 at Vram::Out8");
            }
            return;
        case 0x03C9:
            if(this->now_palette_number==16){
                this->Error("now_paletter_number = 16 at Vram::Out8");
            }
            if(this->video_da_converter_mode==VIDEO_DA_NOP || this->video_da_converter_mode==VIDEO_DA_READ){
                this->Error("stopped at Vram::Out8");
            }
            if(internal_cnt==3){
                this->now_palette_number++;
                internal_cnt = 0;
            }
            this->palette[this->now_palette_number][internal_cnt] = data;
            internal_cnt++;
            return;
        default:
            this->Error("unknown io_port = 0x%02X at Vram::Out8", addr);
    }
    this->Error("Not implemented at Vram::Out8");
}

unsigned char Vram::In8(unsigned int addr){
    this->Error("Not implemented at Vram::In8");
}