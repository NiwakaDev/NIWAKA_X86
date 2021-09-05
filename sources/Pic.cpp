#include "Fifo.h"
#include "Memory.h"
#include "Pic.h"

Pic::Pic(Memory* mem){
    this->mem = mem;
    assert(this->mem!=NULL);
    for(int i=0; i<16; i++){
        this->irq_list[i] = false;
    }
    this->fifo = new Fifo<unsigned int, unsigned char>();
}

void Pic::ShowSelf(){
    fprintf(stderr, "PIC_INFO : \n");
    for(int i=0; i<16; i++){
        fprintf(stderr, "IRQ_%d = 1\n", i, this->irq_list[i]);
    }
}

void Pic::Out8(unsigned int addr, unsigned char data){
    static int c=0;
    switch (addr){
        case PIC0_IMR:
            for(int i=0; i<8; i++){
                this->irq_list[i] = !((data & (1<<i))==(1<<i));
            }
            break;
        case PIC1_IMR:
            for(int i=8; i<16; i++){
                this->irq_list[i] = !(((((unsigned int)data)<<8) & (1<<i))==(1<<i));
            }
            break;
    }
}

int Pic::IsIrq(){
    pair<unsigned int, unsigned char> q;
    int irq_num;
    if(this->fifo->IsEmpty()){
        return -1;
    }
    q = this->fifo->Pop();
    this->now_irq = q.first;
    return q.first;
}

void Pic::Set(unsigned int irq_number){
    if(!this->irq_list[irq_number]){
        return;
    }
    this->fifo->Push(irq_number, 0);
}

unsigned char Pic::In8(unsigned int addr){
    this->Error("Not implemented at Pic::In8");
}

int Pic::GetNowIrq(){
    return this->now_irq;
}
