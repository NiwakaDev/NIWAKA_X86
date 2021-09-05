#pragma once
#include "common.h"
class Memory;
class Cpu;
class IoManager;
class Bios;
class IntHandler;

class Emulator:public Object{
    private:
        thread* emu_thread;
        Memory* mem;
        Cpu*    cpu;
        Bios*   bios;
        IoManager* io_manager;
        IntHandler* int_handler;
    public:
        Emulator(Memory* memory, Bios* bios, Cpu* cpu, IoManager* io_manager);
        void ShowSelf();
        void Run();
};