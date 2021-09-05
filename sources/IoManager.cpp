#include "Memory.h"
#include "Device.h"
#include "Pic.h"
#include "Cpu.h"
#include "Timer.h"
#include "Kbd.h"
#include "Vram.h"
#include "Mouse.h"
#include "IoManager.h"

IoManager::IoManager(Memory* mem){
    this->mem = mem;
    assert(this->mem!=NULL);
    this->device_list[VRAM] = new Vram(mem);
    this->device_list[PIC]  = new Pic(mem);
    this->device_list[MOUSE] = new Mouse((Pic*)this->device_list[PIC]);
    this->device_list[TIMER] = new Timer((Pic*)this->device_list[PIC]);
    this->device_list[KBD]  = new Kbd(mem, (Pic*)this->device_list[PIC], (Mouse*)this->device_list[MOUSE]);
}

void IoManager::Out8(Cpu* cpu, unsigned int addr, unsigned char data){
    if(addr>=0x0F8 && addr<=0x0FF){
        this->Error("This port_addr(0x%02X) is reserved. at IoManager::Out8", addr);
    }
    switch(addr){
        case 0x03C8:
            this->device_list[VRAM]->Out8(addr, data);
            break;
        case 0x03C9:
            this->device_list[VRAM]->Out8(addr, data);
            break;
        case PIC0_IMR:
            this->device_list[PIC]->Out8(addr, data);
            break;
        case PIC1_IMR:
            this->device_list[PIC]->Out8(addr, data);
            break; 
        case PIC0_ICW1:
            this->device_list[PIC]->Out8(addr, data);
            break; 
        case PIC1_ICW1:
            this->device_list[PIC]->Out8(addr, data);
            break; 
        case 0x0060:
        case 0x0064:
            this->device_list[KBD]->Out8(addr, data);
            break;
        case 0x0043:
        case 0x0040:
            this->device_list[TIMER]->Out8(addr, data);
            break;
        default:
            this->Error("addr = %04X is not implemented at IoManager::Out8\n data = %02X", addr, data);
    }
}

unsigned char IoManager::In8(Cpu* cpu, unsigned int addr){
    switch(addr){
        case 0x60:
            return this->device_list[KBD]->In8(addr);
        case 0x64:
            return this->device_list[KBD]->In8(addr);
        default:
            this->Error("can`t access io_port(0x%08X) at IoManager::In8", addr);
    }
}