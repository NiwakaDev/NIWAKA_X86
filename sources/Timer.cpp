#include "Pic.h"
#include "Device.h"
#include "Timer.h"

Timer::Timer(Pic* pic){
    this->pic = pic;
    this->clock =  119318;
}

void Timer::Out8(unsigned int addr, unsigned char data){
    static int mode;
    switch (addr){
        case 0x43:
            mode = 0;
            break;
        case 0x40:
            if(mode==0){
                this->cycle = 0;
                this->cycle = data;
                mode = 1;
                break;
            }else if(mode==1){
                this->cycle = this->cycle | (((unsigned int)data)<<8);  
                this->cycle = (unsigned int)ceil((double)this->clock / (double)this->cycle);
                mode = 3;
                this->timer_thread =  new thread(&Timer::Sleep, this);
                break;
            }
        default:
            this->Error("io_addr(0x%02X) Stopped at Timer::Out8");
            break;
    }
}

unsigned char Timer::In8(unsigned int addr){
    this->Error("Stopped at Timer::In8");
}

void Timer::Sleep(){
    static int cnt=0;
    while(1){
        this_thread::sleep_for(std::chrono::milliseconds(this->cycle));
        this->pic->Set(0);
    }
}

