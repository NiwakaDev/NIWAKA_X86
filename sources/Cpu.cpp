#include "Bios.h"
#include "Memory.h"
#include "TaskRegister.h"
#include "IoManager.h"
#include "Instruction.h"
#include "ControlRegister.h"
#include "Gdtr.h"
#include "Idtr.h"
#include "SegmentRegister.h"
#include "Cpu.h"

Cpu::Cpu(Memory* memory, Bios* bios){
    this->InitSelector();
    this->memory = memory;
    this->bios   = bios;
    this->io_manager = new IoManager(memory);
    assert(this->bios!=NULL);
    assert(this->memory!=NULL);
    assert(this->io_manager!=NULL);
    this->gdtr   = new Gdtr("Gdtr", 0, 0);
    this->idtr   = new Idtr("Idtr", 0, 0);
    this->task_register  = new TaskRegister("TaskRegister", 0x0000);
    assert(this->gdtr!=NULL);
    assert(this->idtr!=NULL);
    assert(this->task_register!=NULL);
    //汎用レジスタ
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->gprs[i] = 0x00000000;
    }
    this->gprs[ESP] = PRG_START_ADDR;

    //this->eflags = 0x00000002;
    this->eflags.raw = 0x00000002;
    //fprintf(stderr, "eflags_size : %d\n", sizeof(this->eflags.raw));
    this->eip = PRG_START_ADDR;

    //enum SEGMENT_REGISTER {ES, CS, SS, DS, FS, GS, SEGMENT_REGISTERS_COUNT};
    this->segment_registers[ES] = new SegmentRegister("ES", 0x0000);
    this->segment_registers[CS] = new SegmentRegister("CS", 0x0000);
    this->segment_registers[SS] = new SegmentRegister("SS", 0x0000);
    this->segment_registers[DS] = new SegmentRegister("DS", 0x0000);
    this->segment_registers[FS] = new SegmentRegister("FS", 0x0000);
    this->segment_registers[GS] = new SegmentRegister("GS", 0x0000);

    this->control_registers[CR0] = new ControlRegister("CR0",0x00000000);
    this->control_registers[CR1] = new ControlRegister("CR1",0x00000000);
    this->control_registers[CR2] = new ControlRegister("CR2",0x00000000);
    this->control_registers[CR3] = new ControlRegister("CR3",0x00000000);
    this->control_registers[CR4] = new ControlRegister("CR4",0x00000000);

    for(int i=0; i<INSTRUCTION_CNT; i++){
        this->instructions_16bits_mode[i] = NULL;
        this->instructions_32bits_mode[i] = NULL;
    }
    
    
    this->instructions_32bits_mode[0x00] = new AddRm8R8("AddRm8R8");
    this->instructions_32bits_mode[0x01] = new AddRm32R32("AddRm32R32");
    this->instructions_32bits_mode[0x05] = new AddEaxImm32("AddEaxImm32");
    this->instructions_32bits_mode[0x06] = new PushEs("PushEs");
    this->instructions_32bits_mode[0x07] = new PopEs("PopEs");
    this->instructions_32bits_mode[0x09] = new OrRm32R32("OrRm32R32");
    this->instructions_32bits_mode[0x0C] = new OrAlImm8("OrAlImm8");
    this->instructions_32bits_mode[0x0D] = new OrEaxImm32("OrEaxImm32");
    this->instructions_32bits_mode[0x0E] = new PushCs("PushCs");
    this->instructions_32bits_mode[0x10] = new AdcRm8R8("AdcRm8R8");
    this->instructions_32bits_mode[0x1E] = new PushDs("PushDs");
    this->instructions_32bits_mode[0x1F] = new PopDs("PopDs");
    this->instructions_32bits_mode[0x04] = new AddAlImm8("AddAlImm8");
    this->instructions_32bits_mode[0x0F] = new Code0F_32bitsMode("Code0F_32bitsMode");
    this->instructions_32bits_mode[0x25] = new AndEaxImm32("AndEaxImm32");
    this->instructions_32bits_mode[0x29] = new SubRm32R32("SubRm32R32");
    this->instructions_32bits_mode[0x2B] = new SubR32Rm32("SubR32Rm32");
    this->instructions_32bits_mode[0x2D] = new SubEaxImm32("SubEaxImm32");
    this->instructions_32bits_mode[0x35] = new XorEaxImm32("XorEaxImm32");
    this->instructions_32bits_mode[0x38] = new CmpRm8R8("CmpRm8R8");
    this->instructions_32bits_mode[0x39] = new CmpRm32R32("CmpRm32R32");
    this->instructions_32bits_mode[0x3B] = new CmpR32Rm32("CmpR32Rm32");
    this->instructions_32bits_mode[0x3C] = new CmpAlImm8("CmpAlImm8");
    this->instructions_32bits_mode[0x3D] = new CmpEaxImm32("CmpEaxImm32");
    this->instructions_32bits_mode[0x60] = new PushAd("PushAd");
    this->instructions_32bits_mode[0x61] = new PopAd("PopAd");
    this->instructions_32bits_mode[0x72] = new JcRel8("JcRel8");
    this->instructions_32bits_mode[0x73] = new JaeRel8("JaeRel8");
    this->instructions_32bits_mode[0x74] = new JzRel8("JzRel8");
    this->instructions_32bits_mode[0x75] = new JnzRel8("JnzRel8");
    this->instructions_32bits_mode[0x76] = new JbeRel8("JbeRel8");
    this->instructions_32bits_mode[0x77] = new JaRel8("JaRel8");
    this->instructions_32bits_mode[0x78] = new JsRel8("JsRel8");
    this->instructions_32bits_mode[0x79] = new JnsRel8("JnsRel8");
    this->instructions_32bits_mode[0x7C] = new JlRel8("JlRel8");
    this->instructions_32bits_mode[0x7D] = new JgeRel8("JgeRel8");
    this->instructions_32bits_mode[0x7E] = new JleRel8("JleRel8");
    this->instructions_32bits_mode[0x7F] = new JgRel8("JgRel8");
    this->instructions_32bits_mode[0x80] = new Code80_32bitsMode("Code80_32bitsMode");
    this->instructions_32bits_mode[0x81] = new Code81_32bitsMode("Code81_32bitsMode");
    this->instructions_32bits_mode[0x83] = new Code83_32bitsMode("Code83_32bitsMode");
    this->instructions_32bits_mode[0x85] = new TestRm32R32("TestRm32R32");
    this->instructions_32bits_mode[0x89] = new MovRm32R32("MovRm32R32");
    this->instructions_32bits_mode[0x8D] = new LeaR32M("LeaR32M");
    this->instructions_32bits_mode[0xE4] = new InAlImm8("InAlImm8("); 
    this->instructions_32bits_mode[0xEB] = new JmpRel8("JmpRel8");
    this->instructions_32bits_mode[0xEC] = new InAlDx("InAlDx");
    this->instructions_32bits_mode[0xE8] = new CallRel32("CallRel32");
    this->instructions_32bits_mode[0xE9] = new JmpRel32("JmpRel32");
    this->instructions_32bits_mode[0xEE] = new OutDxAl("OutDxAl");
    this->instructions_32bits_mode[0xC0] = new CodeC0_32bitsMode("CodeC0_32bitsMode");
    this->instructions_32bits_mode[0xC6] = new CodeC6_32bitsMode("CodeC6_32bitsMode");
    this->instructions_32bits_mode[0xFF] = new CodeFF_32bitsMode("CodeFF_32bitsMode");
    this->instructions_32bits_mode[0xC1] = new CodeC1_32bitsMode("CodeC1_32bitsMode");
    this->instructions_32bits_mode[0xC3] = new Ret32Near("Ret32Near");
    this->instructions_32bits_mode[0xC7] = new MovRm32Imm32("MovRm32Imm32");
    this->instructions_32bits_mode[0xC9] = new Leave("Leave");
    this->instructions_32bits_mode[0xCD] = new IntImm8("IntImm8");
    this->instructions_32bits_mode[0xCF] = new Iretd("Iretd");
    this->instructions_32bits_mode[0x6A] = new PushImm8("PushImm8");
    this->instructions_32bits_mode[0x68] = new PushImm32("PushImm32");
    this->instructions_32bits_mode[0x84] = new TestRm8R8("TestRm8R8");
    this->instructions_32bits_mode[0x8A] = new MovR8Rm8("MovR8Rm8");
    this->instructions_32bits_mode[0x8B] = new MovR32Rm32("MovR32Rm32");
    this->instructions_32bits_mode[0x8C] = new MovRm16Sreg("MovRm16Sreg");
    this->instructions_32bits_mode[0x8E] = new MovSregRm16("MovSregRm16");
    this->instructions_32bits_mode[0x98] = new Cwde("Cwde");
    this->instructions_32bits_mode[0x99] = new Cdq("Cdq");
    this->instructions_32bits_mode[0x9A] = new CallPtr1632("CallPtr1632");
    this->instructions_32bits_mode[0x9C] = new PushFd("PushFd");
    this->instructions_32bits_mode[0x9D] = new PopFd("PopFd");
    this->instructions_32bits_mode[0xA1] = new MovEaxMoffs32("MovEaxMoffs32");
    this->instructions_32bits_mode[0xA2] = new MovMoffs8Al("MovMoffs8Al");
    this->instructions_32bits_mode[0xA3] = new MovMoffs32Eax("MovMoffs32Eax");
    this->instructions_32bits_mode[0xCB] = new Ret32Far("Ret32Far");
    this->instructions_32bits_mode[0xEA] = new JmpPtr1632("JmpPtr1632");
    this->instructions_32bits_mode[0xFA] = new Cli("Cli");
    this->instructions_32bits_mode[0xF7] = new CodeF7_32bitsMode("CodeF7_32bitsMode");
    this->instructions_32bits_mode[0xFB] = new Sti("Sti");
    this->instructions_32bits_mode[0xFE] = new CodeFE_32bitsMode("CodeFE_32bitsMode");
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0x48+i] = new DecR32("DecR32");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0x58+i] = new PopR32("PopR32");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0x50+i] = new PushR32("PushR32");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0xB8+i] = new MovR32Imm32("MovR32Imm32");
    }
    this->instructions_32bits_mode[0xA0] = new MovAlMoffs8("MovAlMoffs8");
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0x40+i] = new IncR32("IncR32");
    }
    this->instructions_32bits_mode[0xF4] = new Hlt("Hlt");
    this->instructions_32bits_mode[0x88] = new MovRm8R8("MovRm8R8");
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0x90+i] = new XchgEaxR32("XchgEaxR32");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_32bits_mode[0xB0+i] = new MovR8Imm8("MovR8Imm8");
    }
    
    this->instructions_16bits_mode[0x01] = new AddRm16R16("AddRm16R16");
    this->instructions_16bits_mode[0x25] = new AndAxImm16("AndAxImm16");
    this->instructions_16bits_mode[0x72] = new JbRel8("JbRel8");
    this->instructions_16bits_mode[0x73] = new JncRel8("JncRel8");
    this->instructions_16bits_mode[0x75] = new JnzRel8("JnzRel8");
    this->instructions_16bits_mode[0x74] = new JzRel8("JzRel8");
    this->instructions_16bits_mode[0x76] = new JbeRel8("JbeRel8");
    this->instructions_16bits_mode[0x8A] = new MovR8Rm8("MovR8Rm8");
    this->instructions_16bits_mode[0xE6] = new OutImm8("OutImm8");
    this->instructions_16bits_mode[0xE8] = new CallRel16("CallRel16");
    this->instructions_16bits_mode[0xEB] = new JmpRel8("JmpRel8");
    this->instructions_16bits_mode[0xE9] = new JmpRel16("JmpRel16");
    this->instructions_16bits_mode[0xEA] = new JmpPtr16M16("JmpPtr16M16");
    this->instructions_16bits_mode[0x81] = new Code81_16bitsMode("Code81_16bitsMode");
    this->instructions_16bits_mode[0x88] = new MovRm8R8("MovRm8R8");
    this->instructions_16bits_mode[0x89] = new MovRm16R16("MovRm16R16");
    this->instructions_16bits_mode[0x8B] = new MovR16Rm16("MovR16Rm16");
    this->instructions_16bits_mode[0x8C] = new MovRm16Sreg("MovRm16Sreg");
    this->instructions_16bits_mode[0x98] = new Cwde("Cwde");
    this->instructions_16bits_mode[0xC1] = new CodeC1_16bitsMode("CodeC1_16bitsMode");
    this->instructions_16bits_mode[0xC3] = new Ret16Near("Ret16Near");
    this->instructions_16bits_mode[0xC6] = new CodeC6_16bitsMode("CodeC6_16bitsMode");
    this->instructions_16bits_mode[0xC7] = new MovRm16Imm16("MovRm16Imm16");
    this->instructions_16bits_mode[0xC9] = new Leave("Leave");
    this->instructions_16bits_mode[0xCD] = new IntImm8("IntImm8");
    this->instructions_16bits_mode[0x69] = new ImulR16Rm16Imm16("ImulR16Rm16Imm16");
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_16bits_mode[0xB8+i] = new MovR16Imm16("MovR16Imm16");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_16bits_mode[0x50+i] = new PushR16("PushR16");
    }
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        this->instructions_16bits_mode[0xB0+i] = new MovR8Imm8("MovR8Imm8");
    }

    this->instructions_16bits_mode[0x80] = new Code80_16bitsMode("Code80_16bitsMode");
    this->instructions_16bits_mode[0x83] = new Code83_16bitsMode("Code83_16bitsMode");
    this->instructions_16bits_mode[0x0F] = new Code0F_16bitsMode("Code0F_16bitsMode");
    this->instructions_16bits_mode[0x8E] = new MovSregRm16("MovSregRm16");
    this->instructions_16bits_mode[0xFA] = new Cli("Cli");
    this->instructions_16bits_mode[0xFF] = new CodeFF_16bitsMode("Code83_16bitsMode");
    this->instructions_16bits_mode[0xA2] = new MovMoffs8Al("MovMoffs8Al");
    this->is_prefix_table[0xF0] = true;
    this->is_prefix_table[0xF2] = true;
    this->is_prefix_table[0xF3] = true;
    this->is_prefix_table[0x26] = true;
    this->is_prefix_table[0x2E] = true;
    this->is_prefix_table[0x36] = true;
    this->is_prefix_table[0x3E] = true;
    this->is_prefix_table[0x64] = true;
    this->is_prefix_table[0x65] = true;
    this->is_prefix_table[0x66] = true;
    this->is_prefix_table[0x67] = true;

    for(int i=0; i<PREFIX_FLG_KIND_COUNT; i++){
        this->prefix_flgs.push_back(false);
    }
}

void Cpu::ShowSelf(){
    if(this->IsProtectedMode()){
        fprintf(stdout, "\nMODE : PROTECTED_MODE\n");
    }else{
        fprintf(stdout, "\nMODE : REAL_MODE\n");
    }
    fprintf(stdout, "\nGENERAL_PURPOSE_REGISTER : \n");
    for(int i=0; i<GENERAL_PURPOSE_REGISTERS_COUNT; i++){
        //this->general_purpose_registers[i]->ShowSelf();
        fprintf(stderr, "%08X\n", this->gprs[i]);
    }

    fprintf(stderr, "EIP = %08X\n", this->eip);

    fprintf(stderr, "EFLAGS = %08X\n", this->eflags.raw);

    fprintf(stdout, "\nSEGMENT_REGISTER : \n");
    for(int i=0; i<SEGMENT_REGISTERS_COUNT; i++){
        this->segment_registers[i]->ShowSelf();
    }

    fprintf(stdout, "\n");
    this->task_register->ShowSelf();
    fprintf(stdout, "\n");
    fprintf(stdout, "\nCONTROL_REGISTER : \n");
    for(int i=0; i<CONTROL_REGISTERS_COUNT; i++){
        this->control_registers[i]->ShowSelf();
    }

    fprintf(stdout, "\nDESCRIPTRO_TABLE_REGISTER : \n");
    this->gdtr->ShowSelf();
    this->idtr->ShowSelf();
    fflush(stdout);
}

bool Cpu::IsProtectedMode(){
    unsigned int cr0 = this->control_registers[CR0]->GetValue();
    return (cr0&PE)==PE;
}

bool Cpu::IsRealMode(){
    unsigned int cr0 = this->control_registers[CR0]->GetValue();
    return (cr0&PE)==0x00000000;
}

unsigned int Cpu::GetGPRValue(GENERAL_PURPOSE_REGISTER register_type){
    return this->gprs[register_type];
}

unsigned int Cpu::GetEipValue(){
    return this->eip;
}

unsigned int Cpu::GetEflagsValue(){
    return this->eflags.raw;
}

void Cpu::ExecuteSelf(){
    this->InitSelector();
    unsigned char op_code;
    this->CheckPrefixCode();
    op_code = this->memory->Read8(this->GetLinearAddrForCodeAccess());
    if(this->Is32bitsMode()){
        if(this->instructions_32bits_mode[op_code]==NULL){
            //this->ShowSelf();
            this->PrintInstructionLog(0, 200);
            this->Error("op_code : %02X(protected_mode) is not implemented at Cpu::ExecuteSelf", op_code);
        }
        //this->LogInstruction(this->instructions_32bits_mode[op_code]->GetName());
        this->instructions_32bits_mode[op_code]->ExecuteSelf(this);
    }else{
        if(this->instructions_16bits_mode[op_code]==NULL){
            this->PrintInstructionLog(0, 200);
            this->Error("op_code : %02X(real_mode) is not implemented at Cpu::ExecuteSelf", op_code);
        }
        //this->LogInstruction(this->instructions_16bits_mode[op_code]->GetName());
        this->instructions_16bits_mode[op_code]->ExecuteSelf(this);
    }
}

bool Cpu::Is66Flg(){
    return this->prefix_flgs[FLG_66];
}

bool Cpu::Is67Flg(){
    return this->prefix_flgs[FLG_67];
}

bool Cpu::IsPrefixAddr(){
    return this->Is67Flg();
}

bool Cpu::IsPrefixOp(){
    return this->Is66Flg();
}

bool Cpu::Is32bitsMode(){
    if(this->segment_registers[CS]->Is32bitsMode()){
        return true;
    }
    return false;
}

bool Cpu::Is16bitsMode(){
    if(!this->segment_registers[CS]->Is32bitsMode()){
        return true;
    }
    return false;
}

void Cpu::CheckPrefixCode(){
    this->ResetPrefixFlg();
    unsigned char op_code=this->memory->Read8(this->GetLinearAddrForCodeAccess());
    while(this->is_prefix_table[op_code]){      
        switch (op_code){
            case 0x66:
                this->prefix_flgs[FLG_66] = true;
                break;
            case 0x67:
                this->prefix_flgs[FLG_67] = true;
                break;
            case 0x2E:
                this->prefix_flgs[FLG_2E] = true;
                this->default_data_selector = CS;
                break;
            default:
                if(this->is_prefix_table[op_code]){
                    this->Error("prefix_code : %02X is not implemented at Cpu::CheckPrefixCode\n", op_code);
                }else{
                    return;
                }
        }
        this->eip++;
        op_code = this->memory->Read8(this->GetLinearAddrForCodeAccess());
    }
}

void Cpu::ResetPrefixFlg(){
    for(int i=0; i<PREFIX_FLG_KIND_COUNT; i++){
        this->prefix_flgs[i] = false;
    }
}

unsigned int Cpu::GetLinearAddrForCodeAccess(){
    unsigned int base_addr;
    unsigned int offset;
    GdtGate* gdt_gate;
    
    if(this->IsProtectedMode()){
        base_addr = this->segment_registers[this->default_code_selector]->GetBaseAddr();
        return base_addr + this->GetEipValue();
    }
    base_addr = ((unsigned int)this->segment_registers[CS]->GetValue())*16;
    offset    = 0x0000FFFF & this->GetEipValue();
    return base_addr + offset;
}

void Cpu::PrintInstuctionLog(){

    fprintf(stdout, "INSTRUCTION_LOG : \n");
    for(auto name:this->instruction_log){
        fprintf(stdout, "%s\n", name.c_str());
    }

}

unsigned int Cpu::GetLinearAddrForDataAccess(unsigned int offset){
    unsigned int base_addr;
    if(this->IsProtectedMode()){
        base_addr = this->segment_registers[this->default_data_selector]->GetBaseAddr();
        return base_addr + offset;
    }
    base_addr = ((unsigned int)this->segment_registers[this->default_data_selector]->GetValue())*16;
    offset    = 0x0000FFFF & offset;
    return base_addr + offset;
}

void Cpu::LogInstruction(string op_name){
    this->instruction_log.push_back(op_name);
}

unsigned int Cpu::GetLinearStackAddr(){
    unsigned int base_addr;
    unsigned int offset;
    if(this->IsProtectedMode()){
        offset    = this->GetR32(ESP);
        base_addr = this->segment_registers[this->default_stack_selector]->GetBaseAddr();
        return offset + base_addr;
    }else if(this->IsRealMode()){
        if(this->Is32bitsMode() ^ this->IsPrefixAddr()){
            offset = this->GetR32(ESP);
        }else{
            offset = (unsigned int)this->GetR16(ESP);
            offset    = 0x0000FFFF & offset;
        }
        base_addr = ((unsigned int)this->segment_registers[this->default_stack_selector]->GetValue())*16;
        offset    = 0x0000FFFF & offset;
    }
    return base_addr + offset;
}

void Cpu::SetR8L(GENERAL_PURPOSE_REGISTER register_type, uint8_t data){
    //this->general_purpose_registers[register_type]->SetR8L(data);
    this->gprs[register_type] = (this->gprs[register_type]&0xffffff00)|(uint32_t)data;
}

void Cpu::SetR8H(GENERAL_PURPOSE_REGISTER register_type, uint8_t data){
    //this->general_purpose_registers[register_type]->SetR8H(data);
    this->gprs[register_type] = (this->gprs[register_type]&0xffff00FF)|((uint32_t)data<<8);
}

void Cpu::SetR8(GENERAL_PURPOSE_REGISTER register_type, uint8_t data){
    //this->general_purpose_registers[register_type]->Set8(data);
    this->gprs[register_type] = (this->gprs[register_type]&0xffffff00)|(uint32_t)data;
}


void Cpu::SetR16(GENERAL_PURPOSE_REGISTER register_type, uint16_t data){
    //this->general_purpose_registers[register_type]->Set16(data);
    this->gprs[register_type] = (this->gprs[register_type]&0xffff0000)|(uint32_t)data;
}

void Cpu::SetR32(GENERAL_PURPOSE_REGISTER register_type, uint32_t data){
    //this->general_purpose_registers[register_type]->Set32(data);
    this->gprs[register_type] = data;
}


void Cpu::SetR32(CONTROL_REGISTER register_type, unsigned int data){
    this->control_registers[register_type]->Set(data);
}

uint8_t Cpu::GetR8H(GENERAL_PURPOSE_REGISTER register_type){
    uint8_t data;
    data = (this->gprs[register_type]>>8)&0xff;
    return data;
}

uint8_t Cpu::GetR8L(GENERAL_PURPOSE_REGISTER register_type){
    uint8_t data;
    data = this->gprs[register_type]&0x000000ff;
    return data;
}

uint16_t Cpu::GetR16(GENERAL_PURPOSE_REGISTER register_type){
    return this->gprs[register_type]&0x0000FFFF;
    //return this->general_purpose_registers[register_type]->GetValue()&0x0000FFFF;
}

uint32_t Cpu::GetR32(GENERAL_PURPOSE_REGISTER register_type){
    return this->gprs[register_type];
    //return this->general_purpose_registers[register_type]->GetValue();
}

unsigned int Cpu::GetR32(CONTROL_REGISTER register_type){
    return this->control_registers[register_type]->GetValue();
}

unsigned short Cpu::GetR16(SEGMENT_REGISTER register_type){
    return this->segment_registers[register_type]->GetValue();

}

void Cpu::AddEip(uint32_t data){
    this->eip = data + this->eip;
}

void Cpu::SetEip(unsigned int addr){
    this->eip = addr;
}

void Cpu::Write8(unsigned int addr, unsigned char data){
    this->memory->Write8(addr, data);
}

void Cpu::Write16(unsigned int addr, unsigned short data){
    this->memory->Write16(addr, data);
}

void Cpu::Write32(unsigned int addr, unsigned int data){
    this->memory->Write32(addr, data);
}


unsigned char Cpu::Read8(unsigned int linear_addr){
    return this->memory->Read8(linear_addr);
}

unsigned short Cpu::Read16(unsigned int linear_addr){
    return this->memory->Read16(linear_addr);
}

unsigned int Cpu::Read32(unsigned int linear_addr){
    return this->memory->Read32(linear_addr);
}

void Cpu::Set(SEGMENT_REGISTER register_type, unsigned short data){
    this->segment_registers[register_type]->Set(data, this);
}

void Cpu::CallFunction(unsigned char bios_number){
    this->bios->CallFunction(this, bios_number);
}

bool Cpu::IsCF(){
    return this->eflags.flgs.CF;
}

void Cpu::ClearCF(){
    this->eflags.flgs.CF = 0;
}

void Cpu::ClearIF(){
    this->eflags.flgs.IF = 0;
}

void Cpu::SetIF(){
    this->eflags.flgs.IF = 1;
}

void Cpu::ClearOF(){
    this->eflags.flgs.OF = 0;
}


bool Cpu::IsZF(){
    return this->eflags.flgs.ZF;
}

bool Cpu::IsOF(){
    return this->eflags.flgs.OF;
}

bool Cpu::IsSF(){
    return this->eflags.flgs.SF;
}

bool Cpu::IsIF(){
    return this->eflags.flgs.IF;
}

/***
bool Cpu::IsCF(){
    return (this->eflags&CF)==CF;
}

void Cpu::ClearCF(){
    this->eflags = this->eflags & ~CF;
}

void Cpu::ClearIF(){
    this->eflags = this->eflags & ~IF;
}

void Cpu::SetIF(){
    this->eflags = this->eflags | IF;
}

void Cpu::ClearOF(){
    this->eflags = this->eflags & ~OF;
}


bool Cpu::IsZF(){
    return (this->eflags&ZF)==ZF;
}

bool Cpu::IsOF(){
    return (this->eflags&OF)==OF;
}

bool Cpu::IsSF(){
    return (this->eflags&SF)==SF;
}

bool Cpu::IsIF(){
    return (this->eflags&IF)==IF;
}
***/

void Cpu::UpdateEflagsForSub(uint64_t result, uint32_t d1, uint32_t d2){
    this->SetCF(result);
    this->SetZF((unsigned int)result);
    this->SetOF((unsigned int)result, d1, d2);
    this->SetSF((unsigned int)result);
}

void Cpu::UpdateEflagsForAnd(uint32_t result){
    this->ClearCF();
    this->ClearOF();
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForOr(unsigned int result){
    this->ClearCF();
    this->ClearOF();
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForXor(unsigned int result){
    this->ClearCF();
    this->ClearOF();
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForShr(unsigned int result){
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForShr8(unsigned char result){
    this->SetZF(result);
    this->SetSF8(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForAnd8(unsigned char result){
    this->ClearCF();
    this->ClearOF();
    this->SetZF(result);
    this->SetSF8(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForOr8(unsigned char result){
    this->ClearCF();
    this->ClearOF();
    this->SetZF(result);
    this->SetSF8(result);
    this->SetPF(result);
}

void Cpu::UpdateEflagsForInc(unsigned int result, unsigned int d1, unsigned int d2){
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
    this->SetOF((unsigned int)result, d1, d2);
}

void Cpu::UpdateEflagsForDec(unsigned int result, unsigned int d1, unsigned int d2){
    this->SetZF(result);
    this->SetSF(result);
    this->SetPF(result);
    this->SetOF((unsigned int)result, d1, d2);
}

void Cpu::SetGdtr(unsigned short limit, unsigned int base){
    this->gdtr->Set(limit, base);
}

void Cpu::SetIdtr(unsigned short limit, unsigned int base){
    this->idtr->Set(limit, base);
}

unsigned int Cpu::GetGdtGate(unsigned short select_number){
    unsigned int desc_addr;
    unsigned int offset = 0;
    offset = select_number;
    desc_addr = this->gdtr->GetBase()+offset;
    return desc_addr;
}

void Cpu::SetCF(){
    this->eflags.flgs.CF = 1;
}

void Cpu::Out8(unsigned int addr, unsigned char data){
    this->io_manager->Out8(this, addr, data);
}

void Cpu::SetEflags(uint32_t data){
    this->eflags.raw = data;
}

void Cpu::PrintInstructionLog(unsigned int start_idx, unsigned int cnt){
    while(cnt > 0){
        fprintf(stdout, "%s\n", this->instruction_log[this->instruction_log.size()-cnt].c_str());
        cnt--;
    }
}

void Cpu::InitSelector(){
    this->default_code_selector = CS;
    this->default_data_selector = DS;
    this->default_stack_selector = SS;
}

void Cpu::SetDataSelector(SEGMENT_REGISTER register_type){
    this->default_data_selector = register_type;
}

void Cpu::SetCodeSelector(SEGMENT_REGISTER register_type){
    this->default_code_selector = register_type;
}

void Cpu::SetStackSelector(SEGMENT_REGISTER register_type){
    this->default_stack_selector = register_type;
}

unsigned int Cpu::GetGdtBase(){
    return this->gdtr->GetBase();
}

unsigned int Cpu::GetIdtBase(){
    return this->idtr->GetBase();
}

void Cpu::SetTr(unsigned short value){
    this->task_register->Set(value, this);
}

void Cpu::SaveTask(unsigned short next_selector){
    unsigned int task_addr;
    unsigned short selector;
    unsigned int desc_addr;
    GdtGate gdt_gate;
    selector = this->task_register->GetValue();
    this->SetTr(next_selector);
    desc_addr = this->GetGdtGate(selector);
    gdt_gate.limit_low = this->Read16(desc_addr);
    gdt_gate.base_low = this->Read16(desc_addr+2);
    gdt_gate.base_mid = this->Read8(desc_addr+4);
    gdt_gate.access_right = this->Read8(desc_addr+5);
    gdt_gate.limit_high = this->Read8(desc_addr+6);
    gdt_gate.base_high = this->Read8(desc_addr+7);

    task_addr = (((unsigned int)gdt_gate.base_high)<<24) | (((unsigned int)gdt_gate.base_mid)<<16) | (unsigned int)gdt_gate.base_low;

    this->Write32(task_addr+8*4, this->GetEipValue());
    this->Write32(task_addr+9*4, this->GetEflagsValue());
    this->Write32(task_addr+10*4, this->GetR32(EAX));
    this->Write32(task_addr+11*4, this->GetR32(ECX));
    this->Write32(task_addr+12*4, this->GetR32(EDX));
    this->Write32(task_addr+13*4, this->GetR32(EBX));
    this->Write32(task_addr+14*4, this->GetR32(ESP));
    this->Write32(task_addr+15*4, this->GetR32(EBP));
    this->Write32(task_addr+16*4, this->GetR32(ESI));
    this->Write32(task_addr+17*4, this->GetR32(EDI));
    this->Write32(task_addr+18*4, this->GetR16(ES));
    this->Write32(task_addr+19*4, this->GetR16(CS));
    this->Write32(task_addr+20*4, this->GetR16(SS));
    this->Write32(task_addr+21*4, this->GetR16(DS));
    this->Write32(task_addr+22*4, this->GetR16(FS));
    this->Write32(task_addr+23*4, this->GetR16(GS));
}

void Cpu::SwitchTask(){
    unsigned int task_addr;
    unsigned int desc_addr;
    GdtGate gdt_gate;
    desc_addr = this->GetGdtGate(this->task_register->GetValue());
    gdt_gate.limit_low = this->Read16(desc_addr);
    gdt_gate.base_low = this->Read16(desc_addr+2);
    gdt_gate.base_mid = this->Read8(desc_addr+4);
    gdt_gate.access_right = this->Read8(desc_addr+5);
    gdt_gate.limit_high = this->Read8(desc_addr+6);
    gdt_gate.base_high = this->Read8(desc_addr+7);

    task_addr = (((unsigned int)gdt_gate.base_high)<<24) | (((unsigned int)gdt_gate.base_mid)<<16) | (unsigned int)gdt_gate.base_low;

    this->SetEip(this->Read32(task_addr+8*4));
    this->SetEflags(this->Read32(task_addr+9*4));
    this->SetR32(EAX, this->Read32(task_addr+10*4));
    this->SetR32(ECX, this->Read32(task_addr+11*4));
    this->SetR32(EDX, this->Read32(task_addr+12*4));
    this->SetR32(EBX, this->Read32(task_addr+13*4));
    this->SetR32(ESP, this->Read32(task_addr+14*4));
    this->SetR32(EBP, this->Read32(task_addr+15*4));
    this->SetR32(ESI, this->Read32(task_addr+16*4)); 
    this->SetR32(EDI, this->Read32(task_addr+17*4)); 
    this->Set(ES, this->Read16(task_addr+18*4));
    this->Set(CS, this->Read16(task_addr+19*4));
    this->Set(SS, this->Read16(task_addr+20*4));
    this->Set(DS, this->Read16(task_addr+21*4));
    this->Set(FS, this->Read16(task_addr+22*4));
    this->Set(GS, this->Read16(task_addr+23*4));
}

bool Cpu::IsSystemSegment(GdtGate gdt_gate){
    this->Error("Stopped at Cpu::IsSystemSegment");
}

bool Cpu::IsCodeSegment(GdtGate gdt_gate){
    if((gdt_gate.access_right&SEGMENT_DESC_TYPE_FLG)!=0){
        return true;
    }else{
        return false;
    }
}

bool Cpu::IsTss(GdtGate gdt_gate){
    if((gdt_gate.access_right&0x1D)==TSS_TYPE){
        return true;
    }
    return false;
}

void Cpu::SetCF(uint64_t result){
    bool flg;
    flg = (bool)((result>>32)&1);

    if(flg){
        this->eflags.flgs.CF = 1;
    }else{
        this->eflags.flgs.CF = 0;
    }
}

void Cpu::SetZF(uint64_t result){
    bool flg;
    flg = (bool)(result==0);

    if(flg){
        this->eflags.flgs.ZF = 1;
    }else{
        this->eflags.flgs.ZF = 0;
    }
}

void Cpu::SetOF(uint32_t result, uint32_t d1, uint32_t d2){
    bool flg;

    flg = ((d1&SIGN_FLG)!=(d2&SIGN_FLG)) && ((result&SIGN_FLG)!=(d1&SIGN_FLG));

    if(flg){
        this->eflags.flgs.OF = 1;
    }else{
        this->eflags.flgs.OF = 0;
    }
}

void Cpu::SetSF(uint32_t result){
    bool flg;

    flg = (result&SIGN_FLG)==SIGN_FLG;
    if(flg){
        this->eflags.flgs.SF = 1;
    }else{
        this->eflags.flgs.SF = 0;
    }
}

void Cpu::SetPF(uint32_t result){
    bool flg;

    int pf_cnt=0;
    for(int i=0; i<8; i++){
        if(((result>>i)&0x01)==0x01){
            pf_cnt++;
        }
    }
    if(pf_cnt%2==0){
        this->eflags.flgs.PF = 1;
    }else{
        this->eflags.flgs.PF = 0;
    }
}

void Cpu::SetSF8(uint8_t result){
    bool flg;

    flg = (result&0x80)==0x80;
    if(flg){
        this->eflags.flgs.SF = 1;
    }else{
        this->eflags.flgs.SF = 0;
    }
}