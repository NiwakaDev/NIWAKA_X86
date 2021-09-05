#include "Cpu.h"
#include "Pic.h"
#include "IntHandler.h"

IntHandler::IntHandler(Cpu* cpu, Pic* pic){
    this->cpu = cpu;
    this->pic = pic;
    assert(this->cpu!=NULL);
    assert(this->pic!=NULL);
}

void IntHandler::Handle(int irq_num){
    irq_num = irq_num + 0x20;
    unsigned int new_cs;
    unsigned int new_eip;
    unsigned int idt_base_addr;
    unsigned int eflags;
    unsigned int cs;
    unsigned int eip;
    eflags = this->cpu->GetEflagsValue();//0x00000283
    eip    = this->cpu->GetEipValue();//0x00000f80
    cs     = (unsigned int)((unsigned short)this->cpu->GetR16(CS));//0x00000010
    this->Push32(eflags);
    this->Push32(eip);
    this->Push32(cs);
    idt_base_addr = this->cpu->GetIdtBase();
    this->GetIdtGate(idt_base_addr, irq_num);
}

void IntHandler::Push32(unsigned int data){
    unsigned int addr;
    this->cpu->SetR32(ESP, this->cpu->GetR32(ESP)-4);
    addr = this->cpu->GetLinearStackAddr();
    this->cpu->Write32(addr, data);
}

void IntHandler::Push16(unsigned short data){
    unsigned int addr;
    this->cpu->SetR16(ESP, this->cpu->GetR16(ESP)-2);
    addr = this->cpu->GetLinearStackAddr();
    this->cpu->Write16(addr, data);
}

unsigned int IntHandler::Pop32(){
    unsigned int addr;
    unsigned int data;
    addr = this->cpu->GetLinearStackAddr();
    data = this->cpu->Read32(addr);
    this->cpu->SetR32(ESP, this->cpu->GetR32(ESP)+4);
    return data;
}

unsigned short IntHandler::Pop16(){
    unsigned int addr;
    unsigned short data;
    addr = this->cpu->GetLinearStackAddr();
    data = this->cpu->Read16(addr);
    this->cpu->SetR16(ESP, this->cpu->GetR16(ESP)+2);
    return data;
}

IdtGate IntHandler::GetIdtGate(unsigned int idt_base_addr, unsigned int irq_num){
    IdtGate idt_gate;
    unsigned int offset;
    offset = this->cpu->GetLinearAddrForDataAccess(idt_base_addr+irq_num*8);
    idt_gate.offset_low = this->cpu->Read16(offset);
    idt_gate.selector   = this->cpu->Read16(offset+2);
    idt_gate.dw_cnt     = this->cpu->Read8(offset+4);
    idt_gate.access_right = this->cpu->Read8(offset+5);
    idt_gate.offset_high = this->cpu->Read16(offset+6);
    offset              = (((unsigned int)idt_gate.offset_high)<<16) | ((unsigned int)idt_gate.offset_low);
    this->cpu->SetEip(offset);
    this->cpu->Set(CS, idt_gate.selector);
    this->cpu->ClearIF();
}