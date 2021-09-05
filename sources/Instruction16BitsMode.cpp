#include "Cpu.h"
#include "Sib.h"
#include "Instruction.h"

CallRel16::CallRel16(string code_name):Instruction(code_name){

}

void CallRel16::ExecuteSelf(Cpu* cpu){
    unsigned short diff16;
    unsigned int diff32;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("32bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
        return;
    }else{
        diff16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        this->Push16(cpu, cpu->GetEipValue()+2);
        cpu->AddEip(diff16+2);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

Ret16Near::Ret16Near(string code_name):Instruction(code_name){

}

void Ret16Near::ExecuteSelf(Cpu* cpu){
    unsigned int addr;
    cpu->AddEip(1);

    if(cpu->IsPrefixOp()){
        addr = (unsigned int)this->Pop32(cpu);
        cpu->SetEip(addr);
        //this->Error("32bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
        return;
    }else{
        addr = (unsigned int)this->Pop16(cpu);
        cpu->SetEip(addr);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

PushR16::PushR16(string code_name):Instruction(code_name){

}

void PushR16::ExecuteSelf(Cpu* cpu){
    unsigned short r16;
    unsigned int r32;
    unsigned int addr;
    GENERAL_PURPOSE_REGISTER register_type;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x50);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        r32 = cpu->GetR32(register_type);
        this->Push32(cpu, r32);
        return;
    }else{
        r16 = cpu->GetR16(register_type);
        this->Push16(cpu, r16);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

MovR16Imm16::MovR16Imm16(string code_name):Instruction(code_name){

}

void MovR16Imm16::ExecuteSelf(Cpu* cpu){
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned short imm16;
    unsigned int imm32;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0xB8);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        cpu->SetR32(register_type, imm32);
        return;
    }else{
        imm16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(2);
        cpu->SetR16(register_type, imm16);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

MovRm16Imm16::MovRm16Imm16(string code_name):Instruction(code_name){

}

void MovRm16Imm16::ExecuteSelf(Cpu* cpu){
    unsigned short imm16;
    unsigned int imm32;

    cpu->AddEip(1);

    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }

    if(cpu->IsPrefixOp()){
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        this->SetRM32(cpu, imm32);
        return;
    }else{
        imm16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(2);
        this->SetRM16(cpu, imm16);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

JmpRel16::JmpRel16(string code_name):Instruction(code_name){

}

void JmpRel16::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("32bits mode is implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }
    cpu->AddEip(1);
    int diff = (int)((short)cpu->Read16(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(diff+2);
    return;
}

MovRm16R16::MovRm16R16(string code_name):Instruction(code_name){

}

void MovRm16R16::ExecuteSelf(Cpu* cpu){
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned short rm16;
    unsigned int rm32;
    unsigned int r32;
    unsigned short r16;
    cpu->AddEip(1);

    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;

    if(cpu->IsPrefixOp()){
        r32 = cpu->GetR32(register_type);
        this->SetRM32(cpu, r32);
        return;
    }
    if(!cpu->IsPrefixOp()){
        r16 = cpu->GetR16(register_type);
        this->Error("not op prefix mode is implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }

    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
    return;
}

MovR16Rm16::MovR16Rm16(string code_name):Instruction(code_name){

}

void MovR16Rm16::ExecuteSelf(Cpu* cpu){
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned short rm16;
    unsigned int rm32;
    unsigned int r32;
    unsigned short r16;
    cpu->AddEip(1);

    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;

    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        cpu->SetR32(register_type, rm32);
        return;
    }
    if(!cpu->IsPrefixOp()){
        r16 = cpu->GetR16(register_type);
        this->Error("not op prefix mode is implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }

    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
    return;
}

AddRm8Imm8::AddRm8Imm8(string code_name):Instruction(code_name){

}

void AddRm8Imm8::ExecuteSelf(Cpu* cpu){
    unsigned char rm8;
    unsigned char imm8;
    GENERAL_PURPOSE_REGISTER register_type;
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    rm8  = this->GetRM8(cpu);
    this->SetRM8(cpu, rm8+imm8);
    return;
}

MovRm8Imm8::MovRm8Imm8(string code_name):Instruction(code_name){

}

void MovRm8Imm8::ExecuteSelf(Cpu* cpu){
    unsigned char rm8;
    unsigned char imm8;
    GENERAL_PURPOSE_REGISTER register_type;
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    this->SetRM8(cpu, imm8);
    return;
}

Code0F_16bitsMode::Code0F_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0x01] = new Code0F01_16bitsMode("Code0F01_16bitsMode");
    this->instructions[0x20] = new MovR32CRX("MovR32CRX");
    this->instructions[0x22] = new MovCRXR32("MovCRXR32");
}

void Code0F_16bitsMode::ExecuteSelf(Cpu* cpu){   
    unsigned char op_code; 
    cpu->AddEip(1);
    op_code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    if(this->instructions[op_code]==NULL){
            this->Error("code 0F %02X is not implemented %s::ExecuteSelf", op_code, this->code_name.c_str());
    }
    this->instructions[op_code]->ExecuteSelf(cpu);
    return;
}

Code0F01_16bitsMode::Code0F01_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[2] = new Lgdt("Lgdt");
    //this->instructions[0] = new AddRm8Imm8("AddRm8Imm8");
    //this->instructions[7] = new CmpRm8Imm8("CmpRm8Imm8");

}

void Code0F01_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->GetDigit(cpu);

    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("op_codecode : 0F 01 /%02X is not implemented at %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

Code80_16bitsMode::Code80_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new AddRm8Imm8("AddRm8Imm8");
    this->instructions[7] = new CmpRm8Imm8("CmpRm8Imm8");
    //this->instructions[5] = new SubRm16Imm8("SubRm16Imm8");
}

void Code80_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);

    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 80 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

SubRm16Imm16::SubRm16Imm16(string code_name):Instruction(code_name){

}

void SubRm16Imm16::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int rm32;
    unsigned int imm32;
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        result = (unsigned long long)rm32 - (unsigned long long)imm32;
        this->SetRM32(cpu, rm32-imm32);
        cpu->UpdateEflagsForSub(result, rm32, imm32);
        return;
    }
    this->Error("Stopped at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

Code81_16bitsMode::Code81_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[5] = new SubRm16Imm16("SubRm16Imm16");
}

void Code81_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 81 %02X(Real Mode) is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

Code83_16bitsMode::Code83_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new AddRm16Imm8("AddRm16Imm8");
    this->instructions[1] = new OrRm16Imm8("OrRm16Imm8");
    this->instructions[5] = new SubRm16Imm8("SubRm16Imm8");
}

void Code83_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 83 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

AddRm16Imm8::AddRm16Imm8(string code_name):Instruction(code_name){

}

void AddRm16Imm8::ExecuteSelf(Cpu* cpu){  
    unsigned int imm8;
    unsigned short imm8_16bits;
    unsigned short rm16;
    unsigned int rm32;  
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        this->SetRM32(cpu, rm32+imm8);
        cpu->AddEip(1);
        return;
    }else{
        rm16 = this->GetRM16(cpu);
        imm8_16bits = (short)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        this->SetRM16(cpu, rm16+imm8_16bits);
        cpu->AddEip(1);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

SubRm16Imm8::SubRm16Imm8(string code_name):Instruction(code_name){

}

void SubRm16Imm8::ExecuteSelf(Cpu* cpu){  
    unsigned int imm8;
    unsigned short rm16;
    unsigned int rm32;  
    unsigned long long result;
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        this->SetRM32(cpu, rm32-imm8);
        cpu->AddEip(1);
        result = (unsigned long long)rm32 - (unsigned long long)imm8;
        cpu->UpdateEflagsForSub(result, rm32, imm8);
        return;
    }else{
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

AddRm16R16::AddRm16R16(string code_name):Instruction(code_name){

}

void AddRm16R16::ExecuteSelf(Cpu* cpu){  
    unsigned int r16;
    unsigned int r32;
    unsigned short rm16;
    unsigned int rm32;  
    GENERAL_PURPOSE_REGISTER register_type;
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        r32  = cpu->GetR32(register_type);
        this->SetRM32(cpu, rm32+r32);
        return;
    }else{
        rm16 = this->GetRM16(cpu);
        r16  = cpu->GetR16(register_type);
        this->SetRM16(cpu, rm16+r16);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

IncRm16::IncRm16(string code_name):Instruction(code_name){

}

void IncRm16::ExecuteSelf(Cpu* cpu){  
    unsigned short rm16;
    unsigned int rm32;  
    
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        this->SetRM32(cpu, rm32+1);
        return;
    }else{
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

ShrRm16Imm8::ShrRm16Imm8(string code_name):Instruction(code_name){

}

void ShrRm16Imm8::ExecuteSelf(Cpu* cpu){  
    unsigned short rm16;
    unsigned int rm32;  
    unsigned int imm8;
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm8 = (unsigned int)((unsigned char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        cpu->AddEip(1);
        if(imm8==1){
            this->Error("imm8==1 is not implemented at %s::ExecuteSelf", this->code_name.c_str());
        }
        for(unsigned int i=0; i<imm8; i++){
            if(rm32&0x00000001){
                cpu->SetCF();
            }else{
                cpu->ClearCF();
            }
            rm32 = rm32 / 2;
        }
        this->SetRM32(cpu, rm32);
        cpu->UpdateEflagsForShr(rm32);
        return;
    }else{
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

CodeC1_16bitsMode::CodeC1_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[5] = new ShrRm16Imm8("ShrRm16Imm8");
    //this->instructions[0] = new MovRm8Imm8("MovRm8Imm8");
}

void CodeC1_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code C1 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

CodeC6_16bitsMode::CodeC6_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new MovRm8Imm8("MovRm8Imm8");
}

void CodeC6_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code C6 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}


CodeFF_16bitsMode::CodeFF_16bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new IncRm16("IncRm16");
}

void CodeFF_16bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    if(cpu->IsPrefixAddr()){
        this->ParseModRM_32bitsMode(cpu);
    }else{
        this->ParseModRM_16bitsMode(cpu);
    }
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 83 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

AndAxImm16::AndAxImm16(string code_name):Instruction(code_name){

}

void AndAxImm16::ExecuteSelf(Cpu* cpu){
    unsigned short imm16;
    unsigned int imm32;
    unsigned int eax;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        eax   = cpu->GetR32(EAX);
        cpu->SetR32(EAX, imm32&eax);
        cpu->UpdateEflagsForAnd(imm32&eax);
        return;
    }
    this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    /***
    imm16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
    cpu->SetR16(EAX, imm16);
    cpu->AddEip(2);
    ***/
    return;
}

OrRm16Imm8::OrRm16Imm8(string code_name):Instruction(code_name){

}

void OrRm16Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int imm32;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm32 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        cpu->AddEip(1);
        this->SetRM32(cpu, rm32 | imm32);
        cpu->UpdateEflagsForOr(rm32 | imm32);
        return;
    }
    this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

JmpPtr16M16::JmpPtr16M16(string code_name):Instruction(code_name){

}

void JmpPtr16M16::ExecuteSelf(Cpu* cpu){
    unsigned int m32;
    unsigned short selector;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        m32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        selector = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->Set(CS, selector);
        cpu->SetEip(m32);
        return;
    }
    this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    return;
}
