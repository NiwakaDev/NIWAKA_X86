#include "Vram.h"
#include "Memory.h"

Memory::Memory(){
    this->size = MEM_SIZE;
    this->memory_content = (unsigned char*)malloc(this->size);
}

void Memory::Write8(long long addr, unsigned char data){
    static int cnt=0;
    if(addr>=MEM_SIZE){
        return;
    }
    if(addr>=0 && addr<MEM_SIZE){
        this->memory_content[addr] = data;
        return;
    }
    this->Error("%dはメモリーの範囲外です。", addr);
}

void Memory::Write16(long long addr, unsigned short data){
    if(addr>=MEM_SIZE){
        return;
    }
    unsigned char* p = (unsigned char*)&data;
    if(addr<0 || addr>=MEM_SIZE){
        this->Error("%dはメモリーの範囲外です。", addr);
        return;
    }
    for(int i=0; i<2; i++){
        //this->memory_content[addr + i] = *(p+i);
        this->Write8(addr+i, *(p+i));
    }
}

void Memory::Write32(long long addr, unsigned int data){
    if(addr>=MEM_SIZE){
        return;
    }
    unsigned char* p = (unsigned char*)&data;
    if(addr<0 || addr>=MEM_SIZE){
        this->Error("%dはメモリーの範囲外です。", addr);
        return;
    }
    for(int i=0; i<4; i++){
        //this->memory_content[addr + i] = *(p+i);
        this->Write8(addr+i, *(p+i));
    }
}


unsigned char Memory::Read8(long long addr){
    if(addr>=MEM_SIZE){
        return 0;
    }
    if(addr>=0 && addr<MEM_SIZE){
        return this->memory_content[addr];
    }
    this->Error("%dはメモリーの範囲外です。", addr);
}

unsigned short Memory::Read16(long long addr){
    if(addr>=MEM_SIZE){
        return 0;
    }
    unsigned short data;
    unsigned char* p = (unsigned char*)&data;
    
    for(int i=0; i<2; i++){
        *(p+i) = this->Read8(addr + i);
    }

    return *((unsigned int*)p);
}

unsigned int Memory::Read32(long long addr){
    if(addr>=MEM_SIZE){
        return 0;
    }
    unsigned int data;
    unsigned char* p = (unsigned char*)&data;
    
    for(int i=0; i<4; i++){
        *(p+i) = this->Read8(addr + i);
    }
    return *((unsigned int*)p);
}



void Memory::ShowSelf(){
    fprintf(stdout, "MEM_SIZE = %lld\n", this->size);
    fflush(stdout);
}

void Memory::PrintMem(long long start_addr, long long cnt){
    for(int i=0; i<cnt; i++){
        fprintf(stdout, "%02X ", this->memory_content[i+start_addr]);
        if((i+1)%16==0){
            fprintf(stdout, "\n");
        }
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}