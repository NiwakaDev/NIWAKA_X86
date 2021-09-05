#include "Pic.h"
#include "Mouse.h"

Mouse::Mouse(Pic* pic){
    this->pic = pic;
    assert(this->pic!=NULL);
    this->enable_flg = false;
}

void Mouse::Out8(unsigned int addr, unsigned char data){
    this->Error("Stopped at Mouse::Out8");
}

unsigned char Mouse::In8(unsigned int addr){
    switch(addr){
        case 0x60:
            return this->data;
        default:
            this->Error("addr(0x%02X) is not implemented at Mouse::In8", addr);
    }
}

bool Mouse::IsEnable(){
    return this->enable_flg;
}

void Mouse::SetEnable(){
    this->enable_flg = true;
}

void Mouse::Receive(unsigned char data){
    this->pic->Set(12);
    this->data = data;
}

void Mouse::HandleMouseMotion(unsigned char state, unsigned char dx, unsigned char dy){
    
}