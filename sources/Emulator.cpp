#include "Memory.h"
#include "Cpu.h"
#include "IoManager.h"
#include "Bios.h"
#include "IntHandler.h"
#include "Device.h"
#include "Pic.h"
#include "Emulator.h"
Emulator::Emulator(Memory* memory, Bios* bios, Cpu* cpu, IoManager* io_manager){
    this->mem = memory;
    this->bios = bios;
    this->cpu = cpu;
    this->io_manager = io_manager;
    this->int_handler = new IntHandler(cpu, (Pic*)this->io_manager->device_list[PIC]);
    this->emu_thread = new thread(&Emulator::Run, this);
    //this->Run();
}

void Emulator::ShowSelf(){
    cout << "Emulator Info" << endl;
    this->bios->ShowSelf();
    this->mem->PrintMem(0x7c00, 512);
    this->mem->ShowSelf();
}

void Emulator::Run(){

    this->bios->LoadIpl(this->mem);
    int irq_num;
    while(1){
        if(this->cpu->IsIF()){
            if((irq_num=this->io_manager->device_list[PIC]->IsIrq())!=-1){
                this->int_handler->Handle(irq_num);
            }
        }
        this->cpu->ExecuteSelf();
    }
    this->ShowSelf();
    fprintf(stdout, "stopped at Emulator::Run\n");
}