#include "common.h"
#include "Memory.h"
#include "Bios.h"
#include "Gui.h"
#include "Vram.h"
#include "Kbd.h"
#include "Mouse.h"
#include "Emulator.h"
#include "Cpu.h"
#include "IoManager.h"

int main(){
    int x= 0;
    int y=1;
    Memory* memory = new Memory();
    Bios* bios = new Bios();
    Cpu* cpu = new Cpu(memory, bios);

    Emulator* emu = new Emulator(memory, bios, cpu, cpu->io_manager);
    memory->vram = (Vram*)cpu->io_manager->device_list[VRAM];
    Gui* gui = new Gui((Vram*)cpu->io_manager->device_list[VRAM], (Kbd*)cpu->io_manager->device_list[KBD], (Mouse*)cpu->io_manager->device_list[MOUSE]);
    gui->Display();
    return 0;
}