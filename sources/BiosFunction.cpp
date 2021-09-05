#include "Cpu.h"
#include "BiosFunction.h"

BiosFunction::BiosFunction(){

}

void BiosFunction::ShowSelf(){
    cout << "BiosFunction名 : " << this->function_name << endl;
}


VideoFunction::VideoFunction():BiosFunction(){
    this->function_name = "VideoFunction";
}

void VideoFunction::ExecuteSelf(Cpu *cpu){
    unsigned char mode;
    mode = cpu->GetR8H(EAX);
    switch(mode){
        case 0x00:
            //何もせずリターン。本当の実装
            //http://oswiki.osask.jp/?%28AT%29BIOS
            return;
        default:
            this->Error("Not implemented at VideoFunction::ExecuteSelf");
    }
    /***
    unsigned char color;
    unsigned char ascii_code;
    
    int bright;
    int len;
    static int col = 0;
    static int row = 0;
    unsigned int vram = VRAM_START_ADDR;
    unsigned char data;

    color      = ((unsigned char)cpu->GetRegisterValue(EBX))&0x0F;
    ascii_code = cpu->GetLow(EAX);

    color = this->color_table[color&0x07];

    if(color&0x08){
        bright = 1;
    }else{
        bright = 0;
    }

    if(ascii_code=='\n'){
        col = 0;
        row = row + 16;
        return;
    }
    for(int i=0; i < 16; i++){
        data = cpu->gui->font_ascii[ascii_code][i];
        if((data & 0x80) != 0){   
            cpu->Write8(vram+(col+0+(i+row)*WIDTH), 7);
        }
        if((data & 0x40) != 0){
            cpu->Write8(vram+(col+1+(i+row)*WIDTH), 7);     
        }
        if((data & 0x20) != 0){
            cpu->Write8(vram+(col+2+(i+row)*WIDTH), 7);       
        }
        if((data & 0x10) != 0){
            cpu->Write8(vram+(col+3+(i+row)*WIDTH), 7);   
        }
        if((data & 0x08) != 0){
            cpu->Write8(vram+(col+4+(i+row)*WIDTH), 7);     
        }
        if((data & 0x04) != 0){
            cpu->Write8(vram+(col+5+(i+row)*WIDTH), 7);   
        }
        if((data & 0x02) != 0){
            cpu->Write8(vram+(col+6+(i+row)*WIDTH), 7);    
        }
        if((data & 0x01) != 0){
            cpu->Write8(vram+(col+7+(i+row)*WIDTH), 7);   
        }
    }
    col = col + 8;
    ***/
}

FloppyFunction::FloppyFunction():BiosFunction(){
    this->function_name = "FloppyFunction";
    this->Init();
}

void FloppyFunction::Init(){
    
    char* file_name = "./outputs/os.img";
    this->disk_img_stream = fopen(file_name, "rb");
    if(this->disk_img_stream==NULL){
        fprintf(stderr, "can`t open %s at FloppyFunction::Init\n", file_name);
        exit(1);
    }
    this->buff_region = (char*)malloc(this->floppy_size);
    fprintf(stdout, "file_size = %d\n", fread(this->buff_region, 1, this->floppy_size, this->disk_img_stream));
}

void FloppyFunction::ExecuteSelf(Cpu* cpu){
    unsigned char ah;
    ah = cpu->GetR8H(EAX);

    switch (ah){
        case 0x02:
            this->Read(cpu);
            break;
        default:
            fprintf(stderr, "ah=%02Xは未実装 at FloppyFunction::ExecuteSelf\n", (unsigned int)ah);
            exit(1);
            break;
    }
}

void FloppyFunction::Read(Cpu* cpu){
    static int cnt=0;
    //this->Error("stopped at FloppyFunction::Read");
    int buff;
    unsigned char data;
    this->es = (unsigned int)cpu->GetR16(ES);
    this->bx = cpu->GetR16(EBX);
    this->drive_number = (unsigned int)cpu->GetR8L(EDX);
    this->head_number  = (unsigned int)cpu->GetR8H(EDX);
    this->sector_number = 0x0000003F&((unsigned int)cpu->GetR8L(ECX));
    this->cylinder_number = (unsigned int)cpu->GetR8H(ECX);
    if(this->sector_number==0x00 || this->sector_number>0x12){
        fprintf(stderr, "sector_numberは0x01~0x12の範囲内で指定してください(sector_number=%d) at FloppyFunction::ExecuteSelf\n", this->sector_number);
    }  
    this->processed_sector_number = (unsigned int)cpu->GetR8L(EAX);

    buff = this->es*16 + this->bx;

    //fprintf(stderr, "buff = %X\n", buff);
    /***
    fprintf(stderr, "drive_number = %X\n", this->drive_number);
    fprintf(stderr, "head_number = %X\n", this->head_number);
    fprintf(stderr, "cylinder_number = %X\n", this->cylinder_number);
    fprintf(stderr, "processed_sector_number = %X\n", this->processed_sector_number);
    ***/

    for(int i=0; i<this->processed_sector_number; i++){
        for(int j=0; j<512; j++){
            data = (unsigned char)this->buff_region[this->head_number*18*512+this->cylinder_number*18*2*512+(this->sector_number-1)*512+j];
            cpu->Write8(buff+j+i*512, data);
        }
    }

    cpu->ClearCF();//エラーなし
}

KeyFunction::KeyFunction():BiosFunction(){
    this->function_name = "KeyFunction";
}

void KeyFunction::ExecuteSelf(Cpu* cpu){
    unsigned char ah;
    unsigned char al;
    ah = cpu->GetR8H(EAX);

    switch (ah){
        case 0x02:
            al = 0x00;
            cpu->SetR8L(EAX, al);
            return;
        default:
            this->Error("ah = 0x%02X is not implemented at KeyFunction::ExecuteSelf", ah);
            break;
    }
}