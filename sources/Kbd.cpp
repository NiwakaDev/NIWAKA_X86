#include "Memory.h"
#include "Pic.h"
#include "Mouse.h"
#include "Kbd.h"
#define KBD_OBF 0x01	//output buff
#define KBD_IBF 0x02	//input buff

#define KBD_REPLY_ACK 0xFA	//command ack

Kbd::Kbd(Memory* mem, Pic* pic, Mouse* mouse){
    this->mem = mem;
    this->pic = pic;
    this->mouse = mouse;
    assert(this->mem!=NULL);
    assert(this->pic!=NULL);
    assert(this->mouse!=NULL);
    this->status_register.raw = 0x00;
}

void Kbd::Receive(unsigned char data){
    if(!this->kbd_mutex.try_lock()){
        return;
    }
    //if((this->status_register&KBD_OBF)!=0){//output buff is full
    if(this->status_register.flgs.obf){
        this->kbd_mutex.unlock();
        return;
    }
    this->pic->Set(1);
    this->output_buf = data;
    //this->status_register |= KBD_OBF;
    this->status_register.flgs.obf = 1;
    this->kbd_mutex.unlock();
}

void Kbd::Out8(unsigned int addr, unsigned char data){
    switch (addr){
        case 0x0064:
            break;
        case 0x0060:
                this->ProcessCommand(data);
            break;
        default:
            this->Error("Stopped at Kbd::Out8");
    }
}

void Kbd::ProcessCommand(unsigned char data){
    switch (data){
        case 0xF4:
            this->mouse->SetEnable();
            this->kbd_mutex.lock();
            this->output_buf = KBD_REPLY_ACK;
            this->kbd_mutex.unlock();
            break;
        case 0x47:
            break;
        default:
            this->Error("data = 0x%02X, Stopped at Kbd::ProcessCommand", data);
            break;
    }
}

unsigned char Kbd::In8(unsigned int addr){
    unsigned char data;
    switch(addr){
        case 0x64:
            this->kbd_mutex.lock();
            data = this->status_register.raw;
            this->kbd_mutex.unlock();
            break;
        case 0x60://MOUSE割り込み優先にする
            if(this->pic->GetNowIrq()==0x0C){
                data = this->mouse->In8(addr);
                break;
            }
            this->kbd_mutex.lock();
            //if(this->status_register&KBD_OBF==0){//output buff is empty
            if(this->status_register.flgs.obf==0){
                this->kbd_mutex.unlock();
                return 0;
            }
            data = this->output_buf;
            //this->status_register &= ~KBD_OBF;
            this->status_register.flgs.obf = 0;
            this->kbd_mutex.unlock();
            break;
        default:
            this->Error("io_addr(0x%04X) is not implemented at Kbd::In8", addr);
    }
    return data;
}