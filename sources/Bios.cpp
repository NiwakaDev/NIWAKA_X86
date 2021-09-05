#include "Memory.h"
#include "Cpu.h"
#include "BiosFunction.h"
#include "Bios.h"

Bios::Bios(){
    this->bios_function_size = 256;
    for(int i=0; i<this->bios_function_size; i++){
        this->bios_function_array[i] = NULL;
    }
    this->bios_function_array[0x10] = new VideoFunction();
    this->bios_function_array[0x13] = new FloppyFunction();
    this->bios_function_array[0x16] = new KeyFunction();
}

void Bios::ShowSelf(){
    cout << "Bios type : Legacy Bios" << endl;
}

void Bios::LoadIpl(Memory* mem){
    const char* file_name="./outputs/os.img";
    unsigned char* buff  = (unsigned char*)calloc(IPL_SIZE, sizeof(char));
    unsigned int cnt;
    FILE *fp = fopen(file_name, "rb");
    if(fp==NULL){
        this->Error("can`t open %s", file_name);
    }
    cnt = (unsigned int)fread(buff, 1, IPL_SIZE, fp);

    for(int i=0; i<cnt; i++){
        mem->Write8(PRG_START_ADDR+i, buff[i]);
    }
    cout << endl;
    free(buff);
}

void Bios::CallFunction(Cpu *cpu, unsigned char bios_number){
    if(this->bios_function_array[bios_number]==NULL){
        this->Error("bios_number = 0x%02Xは未実装です。 at Bios::CallFunction", bios_number);
    }
    this->bios_function_array[bios_number]->ExecuteSelf(cpu);
}
