#include "Cpu.h"
#include "IoManager.h"
#include "Sib.h"
#include "Instruction.h"

void Instruction::ShowSelf(){
    fprintf(stdout, "%s\n", this->code_name.c_str());
}

void Instruction::ExecuteSelf(Cpu* cpu){
    this->Error("stopped at ");
}

string Instruction::GetName(){
    return this->code_name;
}

Instruction::Instruction(string code_name){
    this->code_name = code_name;
    this->sib       = NULL;
}

void Instruction::Push32(Cpu* cpu, unsigned int data){
    unsigned int addr;
    cpu->SetR32(ESP, cpu->GetR32(ESP)-4);
    addr = cpu->GetLinearStackAddr();
    cpu->Write32(addr, data);
}

void Instruction::Push16(Cpu* cpu, unsigned short data){
    unsigned int addr;
    cpu->SetR16(ESP, cpu->GetR16(ESP)-2);
    addr = cpu->GetLinearStackAddr();
    cpu->Write16(addr, data);
}

unsigned int Instruction::Pop32(Cpu* cpu){
    unsigned int addr;
    unsigned int data;
    addr = cpu->GetLinearStackAddr();
    data = cpu->Read32(addr);
    cpu->SetR32(ESP, cpu->GetR32(ESP)+4);
    return data;
}

unsigned short Instruction::Pop16(Cpu* cpu){
    unsigned int addr;
    unsigned short data;
    addr = cpu->GetLinearStackAddr();
    data = cpu->Read16(addr);
    cpu->SetR16(ESP, cpu->GetR16(ESP)+2);
    return data;
}

void Instruction::ParseModRM_32bitsMode(Cpu* cpu){
    unsigned int addr = 0;
    unsigned char code;
    code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    this->modrm.mod = ((code&0xC0)>>6);
    this->modrm.op_code = ((code&0x38) >> 3);
    this->modrm.rm = code & 0x07;
    cpu->AddEip(1);//ModRMの内容を読み込んだので、次の番地へ
    if((this->modrm.mod==0x01&&this->modrm.rm==0x05) || (this->modrm.mod==0x10&&this->modrm.rm==0x05)){
        cpu->SetDataSelector(SS);
    }
    //SIB判定
    if(this->modrm.mod!=3 && this->modrm.rm==4){
        this->modrm.sib = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(1);
        this->sib = new Sib();
        this->sib->ParseSib(this->modrm.sib, this->modrm.mod);
        if((this->sib->GetBase()==5 && this->modrm.mod==0x00)){
            this->modrm.disp32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
            cpu->AddEip(4);
            this->sib->SetDisp32(this->modrm.disp32);
        }
    }

    //disp取得disp32は
    if((this->modrm.mod==0 && this->modrm.rm==5) || this->modrm.mod==2){
        this->modrm.disp32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
    }else if(this->modrm.mod==1){
        this->modrm.disp8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(1);
    }
}

void Instruction::ParseModRM_16bitsMode(Cpu* cpu){
    unsigned char code;
    code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    this->modrm.mod = ((code&0xC0)>>6);
    this->modrm.op_code = ((code&0x38) >> 3);
    this->modrm.rm = code & 0x07;
    cpu->AddEip(1);//ModRMの内容を読み込んだので、次の番地へ

    //disp取得disp32は
    if((this->modrm.mod==0 && this->modrm.rm==6) || this->modrm.mod==2){
        this->modrm.disp16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(2);
    }else if(this->modrm.mod==1){
        this->modrm.disp8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(1);
    }    
}
void Instruction::SetModRM(ModRM modrm, Sib* sib){
    if(sib!=NULL){
        this->sib = sib;
    }
    this->modrm = modrm;
}

void Instruction::ParseModRM(Cpu *cpu){
    if(cpu->Is32bitsMode()){
        if(cpu->IsPrefixAddr()){//32bitmodeで16bitアドレスサイズ
            this->ParseModRM_16bitsMode(cpu);
            return;
        }
        this->ParseModRM_32bitsMode(cpu);
        return;
    }else{
        if(cpu->IsPrefixAddr()){//16bitmodeで32bitアドレスサイズ
            this->ParseModRM_32bitsMode(cpu);
            return;
        }
        this->ParseModRM_16bitsMode(cpu);
        return;
    }
}

void Instruction::GetDigit(Cpu* cpu){
    unsigned char code;
    code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    this->modrm.mod = ((code&0xC0)>>6);
    this->modrm.op_code = ((code&0x38) >> 3);
    this->modrm.rm = code & 0x07;
    cpu->AddEip(1);//ModRMの内容を読み込んだので、次の番地へ
}

void Instruction::GetRegIndex(Cpu* cpu){
    unsigned char code;
    code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    this->modrm.mod = ((code&0xC0)>>6);
    this->modrm.op_code = ((code&0x38) >> 3);
    this->modrm.rm = code & 0x07;
    cpu->AddEip(1);//ModRMの内容を読み込んだので、次の番地へ
}

unsigned short Instruction::GetR16ForEffectiveAddr(Cpu* cpu){
    unsigned short data=0;
    unsigned short r1;
    unsigned short r2;
    switch (this->modrm.rm){
        case 0:
            r1   = (unsigned short)cpu->GetR32(EBX);
            r2   = (unsigned short)cpu->GetR32(ESI);
            data = r1+r2;
            return data;
        case 1:
            r1   = (unsigned short)cpu->GetR32(EBX);
            r2   = (unsigned short)cpu->GetR32(EDI);
            data = r1+r2;
            return data;
        case 2:
            r1   = (unsigned short)cpu->GetR32(EBP);
            r2   = (unsigned short)cpu->GetR32(ESI);
            data = r1+r2;
            return data;
        case 3:
            r1   = (unsigned short)cpu->GetR32(EBP);
            r2   = (unsigned short)cpu->GetR32(EDI);
            data = r1+r2;
            return data;
        case 4:
            r1   = (unsigned short)cpu->GetR32(ESI);
            data = r1;
            return data;
        case 5:
            r1   = (unsigned short)cpu->GetR32(EDI);
            data = r1;
            return data;
        case 6:
            r1   = (unsigned short)cpu->GetR32(EBP);
            data = r1;
            return data;
        case 7:
            r1   = (unsigned short)cpu->GetR32(EBX);
            data = r1;
            return data;
    }
    return data;
}

void Instruction::SetRM8(Cpu* cpu, unsigned char data){
    unsigned int addr = 0;
    unsigned int disp8 = 0;
    unsigned int disp16 = 0;
    unsigned int disp32 = 0;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write8(addr, data);
                return;
            }
            if(this->modrm.rm==4){
                cpu->Write8(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp8;
                cpu->Write8(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp32;
                cpu->Write8(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm<4){
            cpu->SetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
        }else{
            cpu->SetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm-4), data);
        }
    }else{
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write8(this->modrm.disp16, data);
                return;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            disp8 = (short)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            disp16 = (short)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write8(addr, data);
            return;
        }
        if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm<4){
            cpu->SetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
        }else{
            cpu->SetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm-4), data);
        }
    }
}

void Instruction::SetRM16(Cpu* cpu, unsigned short data){
    unsigned int addr = 0;
    unsigned int disp8 = 0;
    unsigned int disp16 = 0;
    unsigned int disp32 = 0;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
            //this->Error("Sib is not implemented at Instruction::SetRM16", this->code_name.c_str());
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write16(addr, data);
                return;
            }
            if(this->modrm.rm==4){
                this->Error("Stopped at Instruction::SetRM16");
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        if(this->modrm.mod==3){
            cpu->SetR16((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
            return;
        }
    }else{
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write16(this->modrm.disp16, data);
                return;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            disp8 = (short)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            disp16 = (short)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write16(addr, data);
            return;
        }
        cpu->SetR16((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
    }
}

void Instruction::SetRM32(Cpu* cpu, unsigned int data){
    unsigned int addr = 0;
    unsigned int disp8 = 0;
    unsigned int disp16 = 0;
    unsigned int disp32 = 0;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){//32bitアドレスサイズ
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write32(addr, data);
                return;
            }
            if(this->modrm.rm==4){
                cpu->Write32(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp8;
                cpu->Write32(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp32;
                cpu->Write32(cpu->GetLinearAddrForDataAccess(addr), data);
                return;
            }
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        if(this->modrm.mod==3){
            cpu->SetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
            return;
        }
    }else{//16bitアドレスサイズ
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                cpu->Write32(this->modrm.disp16, data);
                return;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        if(this->modrm.mod==1){
            disp8 = (short)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        if(this->modrm.mod==2){
            disp16 = (short)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            cpu->Write32(addr, data);
            return;
        }
        cpu->SetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm, data);
        return;
    }
}

unsigned int Instruction::GetEffectiveAddr(Cpu* cpu){
    unsigned int addr = 0;
    unsigned int rm32;
    unsigned int disp8;
    unsigned int disp16;
    unsigned int disp32;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                return addr;
            }
            if(this->modrm.rm==0x04){
                return addr;
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            return addr;
        }
        if(this->modrm.mod==1){
            if(this->modrm.rm==4){
                disp8 = (int)this->modrm.disp8;
                addr = addr + disp8;
                return addr;
            }
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            return addr;
        }
        if(this->modrm.mod==2){
            if(this->modrm.rm==4){
                disp32 = (int)this->modrm.disp32;
                addr = addr+disp32;
                return addr;
            }
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            return addr;
        }
        if(this->modrm.mod==3){
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            return addr;
        }
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
}

unsigned char Instruction::GetRM8(Cpu* cpu){
    unsigned int addr = 0;
    unsigned short rm8;
    unsigned int disp8;
    unsigned int disp16;
    unsigned int disp32;

    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){//32bitアドレスサイズ
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
            //this->Error("Sib is not implemented at Instruction::GetRM8", this->code_name.c_str());
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm8 = cpu->Read8(addr);
                return rm8;
            }
            if(this->modrm.rm==4){
                rm8 = cpu->Read8(cpu->GetLinearAddrForDataAccess(addr));
                return rm8;
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8 = cpu->Read8(addr);
            return rm8;
        }
        if(this->modrm.mod==1){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp8;
                rm8 = cpu->Read8(cpu->GetLinearAddrForDataAccess(addr));
                return rm8;
            }
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8 = cpu->Read8(addr);
            return rm8;
        }
        if(this->modrm.mod==2){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp32;
                rm8 = cpu->Read8(cpu->GetLinearAddrForDataAccess(addr));
                return rm8;
            }
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8 = cpu->Read8(addr);
            return rm8;
        }
        if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm<4){
            rm8 = cpu->GetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
        }else{
            rm8 = cpu->GetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm-4));
        }
        return rm8;
    }else{//16bitアドレス
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm8   = cpu->Read8(this->modrm.disp16);
                return rm8;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8 = cpu->Read8(addr);
            return rm8;
        }
        if(this->modrm.mod==1){
            disp8 = (int)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8  = cpu->Read8(addr);
            return rm8;
        }
        if(this->modrm.mod==2){
            disp16 = (int)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm8  = cpu->Read8(addr);
            return rm8;
        }
        if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm<4){
            rm8 = cpu->GetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
        }else{
            rm8 = cpu->GetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.rm-4));
        }
        return rm8;
    }
    this->Error("Not implemented at Instruction::GetRM8");
}

unsigned short Instruction::GetRM16(Cpu* cpu){
    unsigned int addr = 0;
    unsigned short rm16;
    unsigned int disp8;
    unsigned int disp16;
    unsigned int disp32;

    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){//32bitアドレスサイズ
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
            //this->Error("Sib is not implemented at Instruction::GetRM16", this->code_name.c_str());
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm16 = cpu->Read16(addr);
                return rm16;
            }
            if(this->modrm.rm==4){
                this->Error("Stopped at Instruction::GetRM16");
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16 = cpu->Read16(addr);
            return rm16;
        }
        if(this->modrm.mod==1){
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16 = cpu->Read16(addr);
            return rm16;
        }
        if(this->modrm.mod==2){
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16 = cpu->Read16(addr);
            return rm16;
        }
        if(this->modrm.mod==3){
            rm16 = cpu->GetR16((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            return rm16;
        }
    }else{//16bitアドレス
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm16   = cpu->Read16(this->modrm.disp16);
                return rm16;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16 = cpu->Read16(addr);
            return rm16;
        }
        if(this->modrm.mod==1){
            disp8 = (int)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16  = cpu->Read16(addr);
            return rm16;
        }
        if(this->modrm.mod==2){
            disp16 = (int)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm16  = cpu->Read16(addr);
            return rm16;
        }
        rm16 = cpu->GetR16((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
        return rm16;
    }
    this->Error("Not implemented at Instruction::GetRM16");
}

unsigned int Instruction::GetRM32(Cpu *cpu){
    unsigned int addr;
    unsigned int rm32;
    unsigned int disp8;
    unsigned int disp16;
    unsigned int disp32;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){//32bitアドレスサイズ
        if(this->modrm.mod!=3 && this->modrm.rm==4){
            addr = this->sib->GetAddress(cpu);
            //this->Error("Sib is not implemented at Instruction::GetRM32", this->code_name.c_str());
        }
        if(this->modrm.mod==0){
            if(this->modrm.rm==5){
                addr = this->modrm.disp32;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm32 = cpu->Read32(addr);
                return rm32;
            }
            if(this->modrm.rm==4){
                rm32 = cpu->Read32(cpu->GetLinearAddrForDataAccess(addr));
                return rm32;
            }
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32 = cpu->Read32(addr);
            return rm32;
        }
        if(this->modrm.mod==1){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp8;
                rm32 = cpu->Read32(cpu->GetLinearAddrForDataAccess(addr));
                return rm32;
            }
            disp8 = (int)this->modrm.disp8;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp8;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32 = cpu->Read32(addr);
            return rm32;
        }
        if(this->modrm.mod==2){
            if(this->modrm.rm==4){
                addr = addr + (int)this->modrm.disp32;
                rm32 = cpu->Read32(cpu->GetLinearAddrForDataAccess(addr));
                return rm32;
            }
            disp32 = (int)this->modrm.disp32;
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm)+disp32;
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32 = cpu->Read32(addr);
            return rm32;
        }
        if(this->modrm.mod==3){
            rm32 = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
            return rm32;
        }
    }else{//16bitアドレス
        if(this->modrm.mod==0){
            if(this->modrm.rm==6){
                addr = this->modrm.disp16;
                addr = cpu->GetLinearAddrForDataAccess(addr);
                addr = cpu->GetLinearAddrForDataAccess(addr);
                rm32   = cpu->Read32(this->modrm.disp16);
                return rm32;
            }
            addr = this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32 = cpu->Read32(addr);
            return rm32;
        }
        if(this->modrm.mod==1){
            disp8 = (int)this->modrm.disp8;
            addr  = disp8 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32  = cpu->Read32(addr);
            return rm32;
        }
        if(this->modrm.mod==2){
            disp16 = (int)this->modrm.disp16;
            addr  = disp16 + this->GetR16ForEffectiveAddr(cpu);
            addr = cpu->GetLinearAddrForDataAccess(addr);
            rm32  = cpu->Read32(addr);
            return rm32;
        }
        rm32 = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.rm);
        return rm32;
    }
    this->Error("Not implemented at Instruction::GetRM32");
}

JmpPtr1632::JmpPtr1632(string code_name):Instruction(code_name){

}

void JmpPtr1632::ExecuteSelf(Cpu* cpu){
    unsigned short selector;
    unsigned int imm32;
    GdtGate gdt_gate;
    unsigned int desc_addr;
    cpu->AddEip(1);
    imm32    = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    selector = cpu->Read16(cpu->GetLinearAddrForCodeAccess()+4);
    desc_addr = cpu->GetGdtGate(selector);
    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);
    cpu->AddEip(6);
    static int cnt=0;
    if(cpu->IsCodeSegment(gdt_gate)){
        this->Error("CodeSegment is not implemented at %s::ExecuteSelf",this->code_name.c_str());
    }else if(cpu->IsTss(gdt_gate)){
        cnt++;
        cpu->SaveTask(selector);
        cpu->SwitchTask();
    }
    //printf("task_b_base_addr = %08X\n", (((unsigned int)gdt_gate.base_high)<<24) | (((unsigned int)gdt_gate.base_mid)<<16) | (unsigned int)gdt_gate.base_low);
    //this->Error("Stopped at %s", this->code_name.c_str());
    /***
    m32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    selector = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
    cpu->Set(CS, selector);
    cpu->SetEip(m32);data0x00001a0b, addr0x00313620
    ***/
    return;
}

JmpRel8::JmpRel8(string code_name):Instruction(code_name){

}

void JmpRel8::ExecuteSelf(Cpu* cpu){
    int diff;
    cpu->AddEip(1);
    diff = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(diff+1);
}

Leave::Leave(string code_name):Instruction(code_name){

}

void Leave::ExecuteSelf(Cpu* cpu){
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        cpu->SetR32(ESP, cpu->GetR32(EBP));
        cpu->SetR32(EBP, this->Pop32(cpu));
    }else{
        cpu->SetR16(ESP, cpu->GetR16(EBP));
        cpu->SetR16(EBP, this->Pop16(cpu));
    }
    cpu->AddEip(1);
}

MovSregRm16::MovSregRm16(string code_name):Instruction(code_name){

}

void MovSregRm16::ExecuteSelf(Cpu* cpu){
    unsigned short rm16;
    SEGMENT_REGISTER register_type;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    register_type = (SEGMENT_REGISTER)this->modrm.reg_index;
    rm16 = this->GetRM16(cpu);
    cpu->Set(register_type, rm16);
    return;
}

MovR8Imm8::MovR8Imm8(string code_name):Instruction(code_name){

}

void MovR8Imm8::ExecuteSelf(Cpu* cpu){
    unsigned char imm8;
    GENERAL_PURPOSE_REGISTER register_type;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0xB0);
    cpu->AddEip(1);
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    
    if((int)register_type<4){
        cpu->SetR8L(register_type, imm8);
    }else{
        cpu->SetR8H((GENERAL_PURPOSE_REGISTER)((int)register_type-4), imm8);
    }

    cpu->AddEip(1);
    return;
}

IntImm8::IntImm8(string code_name):Instruction(code_name){

}

void IntImm8::ExecuteSelf(Cpu* cpu){
    unsigned int selector;
    unsigned int eflags;
    unsigned int cs;
    unsigned int eip;
    unsigned int idt_base_addr;
    unsigned int idt_desc_addr;
    unsigned int offset_addr;
    IdtGate idt_gate;

    cpu->AddEip(1);
    selector = (unsigned int )((unsigned char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(cpu->IsProtectedMode()){
        idt_base_addr = cpu->GetIdtBase();
        eflags = cpu->GetEflagsValue();
        eip    = cpu->GetEipValue();
        cs     = (unsigned int)((unsigned short)cpu->GetR16(CS));
        this->Push32(cpu, eflags);
        this->Push32(cpu, eip);
        this->Push32(cpu, cs);
        idt_desc_addr = cpu->GetLinearAddrForDataAccess(idt_base_addr+selector*8);
        idt_gate.offset_low = cpu->Read16(idt_desc_addr);
        idt_gate.selector   = cpu->Read16(idt_desc_addr+2);
        idt_gate.dw_cnt     = cpu->Read8(idt_desc_addr+4);
        idt_gate.access_right = cpu->Read8(idt_desc_addr+5);
        idt_gate.offset_high = cpu->Read16(idt_desc_addr+6);
        offset_addr              = (((unsigned int)idt_gate.offset_high)<<16) | ((unsigned int)idt_gate.offset_low);
        cpu->SetEip(offset_addr);
        cpu->Set(CS, idt_gate.selector);
        cpu->ClearIF();//割り込みゲートではClearする
        return;
    }else{
        cpu->CallFunction(selector);

    }
    return;
}

JnzRel8::JnzRel8(string code_name):Instruction(code_name){

}

void JnzRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(!cpu->IsZF()){
        cpu->AddEip(rel8);
    }
    return;
}

JncRel8::JncRel8(string code_name):Instruction(code_name){

}

void JncRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(cpu->IsProtectedMode()){
        this->Error("Protected mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }

    if(!cpu->IsCF()){
        cpu->AddEip(rel8);
    }

    return;
}

JnsRel8::JnsRel8(string code_name):Instruction(code_name){

}

void JnsRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    
    if(!cpu->IsSF()){
        cpu->AddEip(rel8);
    }

    return;
}


JleRel8::JleRel8(string code_name):Instruction(code_name){

}

void JleRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(cpu->IsZF() || (cpu->IsOF()!=cpu->IsSF())){
        cpu->AddEip(rel8);
    }

    return;
}

JgRel8::JgRel8(string code_name):Instruction(code_name){

}

void JgRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(!cpu->IsZF() && (cpu->IsSF()==cpu->IsOF())){
        cpu->AddEip(rel8);
    }

    return;
}

JaRel8::JaRel8(string code_name):Instruction(code_name){

}

void JaRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(!cpu->IsZF() && !cpu->IsCF()){
        cpu->AddEip(rel8);
    }

    return;
}

JaeRel8::JaeRel8(string code_name):Instruction(code_name){

}

void JaeRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(!cpu->IsCF()){
        cpu->AddEip(rel8);
    }

    return;
}

JcRel8::JcRel8(string code_name):Instruction(code_name){

}

void JcRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(cpu->IsCF()){
        cpu->AddEip(rel8);
    }
    return;
}

JgeRel8::JgeRel8(string code_name):Instruction(code_name){

}

void JgeRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(cpu->IsSF()==cpu->IsOF()){
        cpu->AddEip(rel8);
    }

    return;
}

JsRel8::JsRel8(string code_name):Instruction(code_name){

}

void JsRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(cpu->IsSF()){
        cpu->AddEip(rel8);
    }

    return;
}

JlRel8::JlRel8(string code_name):Instruction(code_name){

}

void JlRel8::ExecuteSelf(Cpu* cpu){
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);

    if(cpu->IsSF()!=cpu->IsOF()){
        cpu->AddEip(rel8);
    }

    return;
}

MovRm16Sreg::MovRm16Sreg(string code_name):Instruction(code_name){

}

void MovRm16Sreg::ExecuteSelf(Cpu* cpu){
    unsigned short register_value;
    SEGMENT_REGISTER register_type;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    register_type = (SEGMENT_REGISTER)this->modrm.reg_index;
    register_value = cpu->GetR16(register_type);
    this->SetRM16(cpu, register_value);
    return;
}

CmpRm8Imm8::CmpRm8Imm8(string code_name):Instruction(code_name){

}

void CmpRm8Imm8::ExecuteSelf(Cpu* cpu){
    unsigned char imm8;
    unsigned char rm8;
    unsigned long long result;
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    rm8  = this->GetRM8(cpu);
    result = (unsigned long long)rm8 - (unsigned long long)imm8;
    cpu->UpdateEflagsForSub(result, rm8, imm8);
    return;
}

CmpRm8R8::CmpRm8R8(string code_name):Instruction(code_name){

}

void CmpRm8R8::ExecuteSelf(Cpu* cpu){
    unsigned char r8;
    unsigned char rm8;
    unsigned long long result;

    cpu->AddEip(1);
    this->ParseModRM(cpu);
    rm8  = this->GetRM8(cpu);
    r8   = this->GetR8(cpu);
    result = (unsigned long long)rm8 - (unsigned long long)r8;
    cpu->UpdateEflagsForSub(result, rm8, r8);
    return;
}

JzRel8::JzRel8(string code_name):Instruction(code_name){

}

void JzRel8::ExecuteSelf(Cpu* cpu){    
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(cpu->IsZF()){
        cpu->AddEip(rel8);
    }
    return;
}

JbeRel8::JbeRel8(string code_name):Instruction(code_name){

}

void JbeRel8::ExecuteSelf(Cpu* cpu){    
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(cpu->IsCF()||cpu->IsZF()){
        cpu->AddEip(rel8);
    }
    return;
}

JbRel8::JbRel8(string code_name):Instruction(code_name){

}

void JbRel8::ExecuteSelf(Cpu* cpu){    
    unsigned int rel8;
    cpu->AddEip(1);
    rel8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(cpu->IsProtectedMode()){
        this->Error("Protected mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    if(cpu->IsCF()){
        cpu->AddEip(rel8);
    }
    return;
}

MovRm8R8::MovRm8R8(string code_name):Instruction(code_name){

}

void MovRm8R8::ExecuteSelf(Cpu* cpu){    
    unsigned char r8;
    unsigned char rm8;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    r8  = this->GetR8(cpu); 
    this->SetRM8(cpu, r8);
    return;
}

unsigned char Instruction::GetR8(Cpu* cpu){
    unsigned char r8;
    if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.reg_index<4){
        r8 = cpu->GetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    }else{
        r8 = cpu->GetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.reg_index-4));
    }
    return r8;
}

void Instruction::SetR8(Cpu* cpu, unsigned char data){
    if((int)(GENERAL_PURPOSE_REGISTER)this->modrm.reg_index<4){
        cpu->SetR8L((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index, data);
    }else{
        cpu->SetR8H((GENERAL_PURPOSE_REGISTER)((int)(GENERAL_PURPOSE_REGISTER)this->modrm.reg_index-4), data);
    }
}

MovMoffs8Al::MovMoffs8Al(string code_name):Instruction(code_name){

}

void MovMoffs8Al::ExecuteSelf(Cpu* cpu){    
    unsigned short offset16;
    unsigned int   offset32;
    unsigned char al;
    unsigned int addr;
    cpu->AddEip(1);
    al = cpu->GetR8L(EAX);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        offset32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        addr = cpu->GetLinearAddrForDataAccess(offset32);
        cpu->Write8(addr, al);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
        offset16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        addr = cpu->GetLinearAddrForDataAccess((unsigned int)offset16);
        cpu->AddEip(2);
        cpu->Write8(addr, al);
    }
    return;
}

OutImm8::OutImm8(string code_name):Instruction(code_name){

}

void OutImm8::ExecuteSelf(Cpu* cpu){    
    if(cpu->Is32bitsMode()){
        this->Error("32bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    //fprintf(stderr, "OutImm8は何もしません。");
    return;
}

AdcRm8R8::AdcRm8R8(string code_name):Instruction(code_name){

}

void AdcRm8R8::ExecuteSelf(Cpu* cpu){    

    unsigned char rm8;
    unsigned char r8;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    rm8 = this->GetRM8(cpu);
    r8  = this->GetR8(cpu);
    if(cpu->Is32bitsMode()){
        this->SetRM8(cpu, rm8+r8);
    }else{
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

Cli::Cli(string code_name):Instruction(code_name){

}

void Cli::ExecuteSelf(Cpu* cpu){    
    cpu->AddEip(1);
    cpu->ClearIF();
    return;
}

Sti::Sti(string code_name):Instruction(code_name){

}

void Sti::ExecuteSelf(Cpu* cpu){    
    cpu->AddEip(1);
    cpu->SetIF();
    return;
}

Lgdt::Lgdt(string code_name):Instruction(code_name){

}

void Lgdt::ExecuteSelf(Cpu* cpu){    
    unsigned short m16;
    unsigned int base16;
    unsigned short limit16;
    unsigned int m32, base32;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        //m32  = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        m32    = this->GetEffectiveAddr(cpu);
        limit16 = cpu->Read16(cpu->GetLinearAddrForDataAccess(m32));
        base32  = cpu->Read32(cpu->GetLinearAddrForDataAccess(m32+2));
        cpu->SetGdtr(limit16, base32);
    }else{
        m16   = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        limit16 = cpu->Read16(cpu->GetLinearAddrForDataAccess(m16));
        base16  = cpu->Read16(cpu->GetLinearAddrForDataAccess(m16+2));
        cpu->AddEip(2);
        cpu->SetGdtr(limit16, base16);
    }
    return;
}

Lidt::Lidt(string code_name):Instruction(code_name){

}

void Lidt::ExecuteSelf(Cpu* cpu){    //この後にRET命令が実行されない

    unsigned short m16;
    unsigned int base16;
    unsigned short limit16;
    unsigned int m32, base32;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        //m32  = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        m32    = this->GetEffectiveAddr(cpu);
        limit16 = cpu->Read16(cpu->GetLinearAddrForDataAccess(m32));
        base32  = cpu->Read32(cpu->GetLinearAddrForDataAccess(m32+2));
        cpu->SetIdtr(limit16, base32);
    }else{
        m16   = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        limit16 = cpu->Read16(cpu->GetLinearAddrForDataAccess(m16));
        base16  = cpu->Read16(cpu->GetLinearAddrForDataAccess(m16+2));
        cpu->AddEip(2);
        cpu->SetIdtr(limit16, base16);
    }
    return;
}

MovR32CRX::MovR32CRX(string code_name):Instruction(code_name){

}

void MovR32CRX::ExecuteSelf(Cpu* cpu){   
    unsigned int cr_value;
    cpu->AddEip(1); 
    this->GetRegIndex(cpu);
    GENERAL_PURPOSE_REGISTER general_purpose_register_type;
    CONTROL_REGISTER control_register_type;

    general_purpose_register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.rm;
    control_register_type         = (CONTROL_REGISTER)this->modrm.reg_index;

    cr_value = cpu->GetR32(control_register_type);
    cpu->SetR32(general_purpose_register_type, cr_value);
    return;
}

MovCRXR32::MovCRXR32(string code_name):Instruction(code_name){

}

void MovCRXR32::ExecuteSelf(Cpu* cpu){   
    unsigned int r_value;
    cpu->AddEip(1); 
    this->GetRegIndex(cpu);
    GENERAL_PURPOSE_REGISTER general_purpose_register_type;
    CONTROL_REGISTER control_register_type;

    general_purpose_register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.rm;
    control_register_type         = (CONTROL_REGISTER)this->modrm.reg_index;

    r_value = cpu->GetR32(general_purpose_register_type);
    cpu->SetR32(control_register_type, r_value);
    return;
}

JmpRel32::JmpRel32(string code_name):Instruction(code_name){

}

void JmpRel32::ExecuteSelf(Cpu* cpu){   
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }
    cpu->AddEip(1);
    int diff = (int)((short)cpu->Read32(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(diff+4);
    return;
}

PushR32::PushR32(string code_name):Instruction(code_name){

}

void PushR32::ExecuteSelf(Cpu* cpu){   
    unsigned short r16;
    unsigned int r32;
    unsigned int addr;
    GENERAL_PURPOSE_REGISTER register_type;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x50);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        r16 = cpu->GetR16(register_type);
        this->Push16(cpu, r16);
        return;
    }
    r32 = cpu->GetR32(register_type);
    this->Push32(cpu, r32);
    return;
}

MovRm32R32::MovRm32R32(string code_name):Instruction(code_name){

}

void MovRm32R32::ExecuteSelf(Cpu* cpu){   
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned short rm16;
    unsigned int rm32;
    unsigned int r32;
    unsigned short r16;
    cpu->AddEip(1);

    this->ParseModRM(cpu);
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;

    if(cpu->IsPrefixOp()){
        r16 = cpu->GetR16(register_type);
        this->SetRM16(cpu, r16);
        return;
    }
    r32 = cpu->GetR32(register_type);
    this->SetRM32(cpu, r32);
    return;
}

AndEaxImm32::AndEaxImm32(string code_name):Instruction(code_name){

}

void AndEaxImm32::ExecuteSelf(Cpu* cpu){   
    unsigned int imm32;
    unsigned int eax;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode()  ^ cpu->IsPrefixOp()){
        eax   = cpu->GetR32(EAX);
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        cpu->SetR32(EAX, eax&imm32);
        cpu->UpdateEflagsForAnd(imm32&eax);
    }else{
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

AndRm32Imm8::AndRm32Imm8(string code_name):Instruction(code_name){

}

void AndRm32Imm8::ExecuteSelf(Cpu* cpu){   

    unsigned int imm8;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    rm32  = this->GetRM32(cpu);
    this->SetRM32(cpu, imm8&rm32);
    cpu->UpdateEflagsForAnd(imm8&rm32);
    return;
}

AndRm32Imm32::AndRm32Imm32(string code_name):Instruction(code_name){

}

void AndRm32Imm32::ExecuteSelf(Cpu* cpu){   

    unsigned int imm32;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    rm32  = this->GetRM32(cpu);
    this->SetRM32(cpu, imm32&rm32);
    cpu->UpdateEflagsForAnd(imm32&rm32);
    return;
}

PushImm32::PushImm32(string code_name):Instruction(code_name){

}

void PushImm32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("Not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int imm32;
    cpu->AddEip(1);
    imm32 = (unsigned int)cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    this->Push32(cpu, imm32);
    cpu->AddEip(4);
    return;
}

PushImm8::PushImm8(string code_name):Instruction(code_name){

}

void PushImm8::ExecuteSelf(Cpu* cpu){
    unsigned int imm8;
    cpu->AddEip(1);
    imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    this->Push32(cpu, imm8);
    cpu->AddEip(1);
    return;
}

SubRm32Imm8::SubRm32Imm8(string code_name):Instruction(code_name){

}

void SubRm32Imm8::ExecuteSelf(Cpu* cpu){   
    unsigned long long result;
    unsigned int imm8;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    rm32  = this->GetRM32(cpu);
    //cpu->SetR32(EAX, rm32-imm8);
    this->SetRM32(cpu, rm32-imm8);
    result = (unsigned long long)rm32 - (unsigned long long)imm8;
    cpu->UpdateEflagsForSub(result, rm32, imm8);
    return;
}

CmpRm32Imm8::CmpRm32Imm8(string code_name):Instruction(code_name){

}

void CmpRm32Imm8::ExecuteSelf(Cpu* cpu){   
    unsigned long long result;
    unsigned int imm8;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    rm32  = this->GetRM32(cpu);
    result = (unsigned long long)rm32 - (unsigned long long)imm8;
    cpu->UpdateEflagsForSub(result, rm32, imm8);
    return;
}

JleRel32::JleRel32(string code_name):Instruction(code_name){

}

void JleRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsZF() || (cpu->IsOF()!=cpu->IsSF())){
        cpu->AddEip(rel32);
    }

    return;
}

JgeRel32::JgeRel32(string code_name):Instruction(code_name){

}

void JgeRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsOF()==cpu->IsSF()){
        cpu->AddEip(rel32);
    }

    return;
}

JneRel32::JneRel32(string code_name):Instruction(code_name){

}

void JneRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(!cpu->IsZF()){
        cpu->AddEip(rel32);
    }
    return;
}

JgRel32::JgRel32(string code_name):Instruction(code_name){

}

void JgRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(!cpu->IsZF() && (cpu->IsSF()==cpu->IsOF())){
        cpu->AddEip(rel32);
    }
    return;
}

JsRel32::JsRel32(string code_name):Instruction(code_name){

}

void JsRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsSF()){
        cpu->AddEip(rel32);
    }
    return;
}


JeRel32::JeRel32(string code_name):Instruction(code_name){

}

void JeRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsZF()){
        cpu->AddEip(rel32);
    }
    return;
}

JcRel32::JcRel32(string code_name):Instruction(code_name){

}

void JcRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsCF()){
        cpu->AddEip(rel32);
    }
    return;
}

JnaRel32::JnaRel32(string code_name):Instruction(code_name){

}

void JnaRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsCF() || cpu->IsZF()){
        cpu->AddEip(rel32);
    }
    return;
}

JlRel32::JlRel32(string code_name):Instruction(code_name){

}

void JlRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(cpu->IsSF() != cpu->IsOF()){
        cpu->AddEip(rel32);
    }
    return;
}

JaRel32::JaRel32(string code_name):Instruction(code_name){

}

void JaRel32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rel32;
    cpu->AddEip(1);
    rel32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);

    if(!cpu->IsCF() && !cpu->IsZF()){
        cpu->AddEip(rel32);
    }
    return;
}

MovzxR32Rm8::MovzxR32Rm8(string code_name):Instruction(code_name){

}

void MovzxR32Rm8::ExecuteSelf(Cpu* cpu){
    if(cpu->Is16bitsMode()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    if(cpu->IsPrefixOp()){
        this->Error("prefix mode is not implemented at %s::ExecuteSelf");
    }
    unsigned int rm8;//ゼロ拡張されるので、4bytes
    GENERAL_PURPOSE_REGISTER register_type;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    rm8 = (unsigned int)((unsigned char)this->GetRM8(cpu));
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    cpu->SetR32(register_type, rm8);
    return;
}

ImulR16Rm16Imm16::ImulR16Rm16Imm16(string code_name):Instruction(code_name){

}

void ImulR16Rm16Imm16::ExecuteSelf(Cpu* cpu){
    unsigned short rm16;
    unsigned short imm16;
    unsigned int rm32;
    unsigned int imm32;

    GENERAL_PURPOSE_REGISTER register_type;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    if(cpu->Is32bitsMode()){
        this->Error("32 bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    if(cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        cpu->SetR32(register_type, rm32*imm32);
    }else{
        rm16 = this->GetRM16(cpu);
        imm16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(2);
        cpu->SetR16(register_type, rm16*imm16);
    }
    return;
}

ImulR32Rm32::ImulR32Rm32(string code_name):Instruction(code_name){

}


void ImulR32Rm32::ExecuteSelf(Cpu* cpu){
    unsigned int r32;
    unsigned int rm32;
    GENERAL_PURPOSE_REGISTER register_type;
    if(cpu->Is16bitsMode()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    rm32 = this->GetRM32(cpu);
    r32  = cpu->GetR32(register_type);
    cpu->SetR32(register_type, rm32*r32);
    return;
}

MovzxR32Rm16::MovzxR32Rm16(string code_name):Instruction(code_name){

}


void MovzxR32Rm16::ExecuteSelf(Cpu* cpu){
    unsigned int rm16;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    rm16 = (unsigned int)((unsigned short)this->GetRM16(cpu));
    cpu->SetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index, rm16);
    return;
}

MovsxR32Rm8::MovsxR32Rm8(string code_name):Instruction(code_name){

}

void MovsxR32Rm8::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    int rm8;
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    rm8 = (int)((char)this->GetRM8(cpu));

    cpu->SetR32(register_type, rm8);
    return;
}

Ltr::Ltr(string code_name):Instruction(code_name){

}

void Ltr::ExecuteSelf(Cpu* cpu){
    GdtGate gdt_gate;
    unsigned short rm16;
    unsigned int desc_addr;
    rm16 = this->GetRM16(cpu);
    //そのセグメントはTSSか?
    desc_addr = cpu->GetGdtGate(rm16);
    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);
    if(cpu->IsTss(gdt_gate)){
        cpu->SetTr(rm16);
    }else{
        this->Error("You should set up Initial TSS before executing %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

Code0F00_32bitsMode::Code0F00_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0x03] = new Ltr("Ltr");
}

void Code0F00_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    //this->GetDigit(cpu);
    this->ParseModRM(cpu);
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("op_codecode : 0F 00 /%02X is not implemented at %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

Code0F01_32bitsMode::Code0F01_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[2] = new Lgdt("Lgdt");
    this->instructions[3] = new Lidt("Lidt");
    //this->instructions[0] = new AddRm8Imm8("AddRm8Imm8");
    //this->instructions[7] = new CmpRm8Imm8("CmpRm8Imm8");

}

void Code0F01_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    //this->GetDigit(cpu);
    this->ParseModRM(cpu);
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("op_codecode : 0F 01 /%02X is not implemented at %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

Code0F_32bitsMode::Code0F_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0x00] = new Code0F00_32bitsMode("Code0F00_32bitsMode");
    this->instructions[0x01] = new Code0F01_32bitsMode("Code0F01_32bitsMode");
    this->instructions[0x20] = new MovR32CRX("MovR32CRX");
    this->instructions[0x22] = new MovCRXR32("MovCRXR32");
    this->instructions[0xB6] = new MovzxR32Rm8("MovzxR32Rm8");
    this->instructions[0xB7] = new MovzxR32Rm16("MovzxR32Rm16");
    this->instructions[0x82] = new JcRel32("JcRel32");
    this->instructions[0x84] = new JeRel32("JeRel32");
    this->instructions[0x85] = new JneRel32("JneRel32");
    this->instructions[0x86] = new JnaRel32("JnaRel32");
    this->instructions[0x87] = new JaRel32("JaRel32");
    this->instructions[0x88] = new JsRel32("JsRel32");
    this->instructions[0x8C] = new JlRel32("JlRel32");
    this->instructions[0x8D] = new JgeRel32("JgeRel32");
    this->instructions[0x8E] = new JleRel32("JleRel32");
    this->instructions[0x8F] = new JgRel32("JgRel32");
    this->instructions[0xAF] = new ImulR32Rm32("ImulR32Rm32");
    this->instructions[0xBE] = new MovsxR32Rm8("MovsxR32Rm8");
}

void Code0F_32bitsMode::ExecuteSelf(Cpu* cpu){   
    unsigned char op_code; 
    cpu->AddEip(1);
    op_code = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    if(this->instructions[op_code]==NULL){
            cpu->PrintInstructionLog(0, 200);
            this->Error("code 0F %02X is not implemented %s::ExecuteSelf", op_code, this->code_name.c_str());
    }
    this->instructions[op_code]->ExecuteSelf(cpu);
    cpu->LogInstruction(this->instructions[op_code]->GetName());
    return;
}

AddRm32Imm32::AddRm32Imm32(string code_name):Instruction(code_name){

}


void AddRm32Imm32::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int imm32;
    unsigned int result;
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    rm32  = this->GetRM32(cpu);
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    this->SetRM32(cpu, rm32+imm32);
    return;
}

CmpRm32Imm32::CmpRm32Imm32(string code_name):Instruction(code_name){

}


void CmpRm32Imm32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int imm32;
    unsigned int rm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    rm32  = this->GetRM32(cpu);
    result = (unsigned long long)rm32 - (unsigned long long)imm32;
    cpu->UpdateEflagsForSub(result, rm32, imm32);
    return;
}

OrAlImm8::OrAlImm8(string code_name):Instruction(code_name){

}


void OrAlImm8::ExecuteSelf(Cpu* cpu){
    unsigned char imm8;
    unsigned char al;
    unsigned char result;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        al = cpu->GetR8L(EAX);
        imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(1);
        cpu->SetR8L(EAX, al | imm8);
        cpu->UpdateEflagsForOr8(al | imm8);  
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

OrRm32Imm32::OrRm32Imm32(string code_name):Instruction(code_name){

}


void OrRm32Imm32::ExecuteSelf(Cpu* cpu){
    unsigned int imm32;
    unsigned int rm32;

    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        this->SetRM32(cpu, imm32|rm32);
        cpu->UpdateEflagsForOr(imm32|rm32);  
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

OrEaxImm32::OrEaxImm32(string code_name):Instruction(code_name){

}


void OrEaxImm32::ExecuteSelf(Cpu* cpu){
    unsigned int imm32;
    unsigned int eax;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        eax   = cpu->GetR32(EAX);
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        cpu->SetR32(EAX, imm32|eax);
        cpu->UpdateEflagsForOr(imm32|eax);  
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

OrRm32Imm8::OrRm32Imm8(string code_name):Instruction(code_name){

}


void OrRm32Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int imm8;

    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        rm32 = this->GetRM32(cpu);
        imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        cpu->AddEip(1);
        this->SetRM32(cpu, imm8|rm32);
        cpu->UpdateEflagsForOr(imm8|rm32);  
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

Code81_32bitsMode::Code81_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0x00] = new AddRm32Imm32("AddRm32Imm32");
    this->instructions[0x01] = new OrRm32Imm32("OrRm32Imm32");
    this->instructions[0x04] = new AndRm32Imm32("AndRm32Imm32");
    this->instructions[0x05] = new SubRm32Imm32("SubRm32Imm32");
    this->instructions[0x07] = new CmpRm32Imm32("CmpRm32Imm32");
}

void Code81_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);

    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 81 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

XorRm32Imm8::XorRm32Imm8(string code_name):Instruction(code_name){

}

void XorRm32Imm8::ExecuteSelf(Cpu* cpu){   
    unsigned int imm8;
    unsigned int rm32;
    if(cpu->IsProtectedMode() ^ cpu->IsPrefixOp()){
        imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        cpu->AddEip(1);
        rm32 = this->GetRM32(cpu);
        this->SetRM32(cpu, rm32^imm8);
        cpu->UpdateEflagsForXor(rm32^imm8);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

XorEaxImm32::XorEaxImm32(string code_name):Instruction(code_name){

}

void XorEaxImm32::ExecuteSelf(Cpu* cpu){   
    unsigned int imm32;
    unsigned int eax;
    cpu->AddEip(1);
    if(cpu->IsProtectedMode() ^ cpu->IsPrefixOp()){
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        eax = cpu->GetR32(EAX);
        cpu->SetR32(EAX, eax^imm32);
        cpu->UpdateEflagsForXor(eax^imm32);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}


Code83_32bitsMode::Code83_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new AddRm32Imm8("AddRm32Imm8");
    this->instructions[1] = new OrRm32Imm8("OrRm32Imm8");
    this->instructions[6] = new XorRm32Imm8("XorRm32Imm8");
    this->instructions[4] = new AndRm32Imm8("AndRm32Imm8");
    this->instructions[5] = new SubRm32Imm8("SubRm32Imm8");
    this->instructions[7] = new CmpRm32Imm8("CmpRm32Imm8");
}

void Code83_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);

    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 83 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

MovR32Imm32::MovR32Imm32(string code_name):Instruction(code_name){

}

void MovR32Imm32::ExecuteSelf(Cpu* cpu){   
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned short imm16;
    unsigned int imm32;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0xB8);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        imm16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(2);
        cpu->SetR16(register_type, imm16);
        return;
    }

    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    cpu->SetR32(register_type, imm32);
    return;
}

CodeC6_32bitsMode::CodeC6_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new MovRm8Imm8("MovRm8Imm8");
}

void CodeC6_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code C6 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

IdivRm32::IdivRm32(string code_name):Instruction(code_name){

}

void IdivRm32::ExecuteSelf(Cpu* cpu){   
    int rm32;
    long long r64;
    int edx, eax;
    edx = cpu->GetR32(EDX);
    eax = cpu->GetR32(EAX);
    rm32 = this->GetRM32(cpu);

    r64  = (((long long)edx)<<((long long)32))| ((long long)eax);
    cpu->SetR32(EAX, r64/((long long)rm32));
    cpu->SetR32(EDX, r64%(long long)rm32);
}

ImulRm32::ImulRm32(string code_name):Instruction(code_name){

}

void ImulRm32::ExecuteSelf(Cpu* cpu){   
    long long eax;
    long long rm32;
    rm32 = (long long)((int)this->GetRM32(cpu));
    eax  = (long long)((int)cpu->GetR32(EAX));
    cpu->SetR32(EAX, 0x00000000FFFFFFFF&(rm32*eax));
    cpu->SetR32(EDX,(0xFFFFFFFF00000000&(rm32*eax))>>32);
}


CodeF7_32bitsMode::CodeF7_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0x03]  = new NegRm32("NegRm32");
    this->instructions[0x07] = new IdivRm32("IdivRm32");
    this->instructions[0x05] = new ImulRm32("ImulRm32");
}

void CodeF7_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code F7 /%X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}


IncR32::IncR32(string code_name):Instruction(code_name){

}

void IncR32::ExecuteSelf(Cpu* cpu){   
    GENERAL_PURPOSE_REGISTER register_type;
    unsigned int r32;
    register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x40);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemeted at %s::ExecuteSelf\n", this->code_name.c_str());
    }
    r32 = cpu->GetR32(register_type);
    cpu->SetR32(register_type, r32+1);
    cpu->UpdateEflagsForInc(r32+1, r32, 1);
}

CmpEaxImm32::CmpEaxImm32(string code_name):Instruction(code_name){

}

void CmpEaxImm32::ExecuteSelf(Cpu* cpu){   
    unsigned int imm32;
    unsigned int eax;
    unsigned long long result;
    cpu->AddEip(1);
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    eax   = cpu->GetR32(EAX);
    result = (unsigned long long)eax - (unsigned long long)imm32;
    cpu->UpdateEflagsForSub(result, eax, imm32);
}

CallRel32::CallRel32(string code_name):Instruction(code_name){

}

void CallRel32::ExecuteSelf(Cpu* cpu){
    unsigned short diff16;
    unsigned int diff32;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        diff16 = cpu->Read16(cpu->GetLinearAddrForCodeAccess());
        this->Push16(cpu, cpu->GetEipValue()+2);
        cpu->AddEip(diff16+2);
        return;
    }else{
        diff32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        this->Push32(cpu, cpu->GetEipValue()+4);
        cpu->AddEip(diff32+4);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

Hlt::Hlt(string code_name):Instruction(code_name){

}

void Hlt::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this_thread::sleep_for(std::chrono::milliseconds(10));
}

LeaR32M::LeaR32M(string code_name):Instruction(code_name){

}

void LeaR32M::ExecuteSelf(Cpu* cpu){
    unsigned int r32;
    unsigned int addr;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        addr = GetEffectiveAddr(cpu);
        cpu->SetR32(register_type, addr);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
}

AddAlImm8::AddAlImm8(string code_name):Instruction(code_name){

}

void AddAlImm8::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    unsigned char al   = cpu->GetR8L(EAX);
    unsigned char imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    cpu->SetR8(EAX, al+imm8);
}

MovRm32Imm32::MovRm32Imm32(string code_name):Instruction(code_name){

}

void MovRm32Imm32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits operand size is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int imm32;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    this->SetRM32(cpu, imm32);
    return;
}

InAlImm8::InAlImm8(string code_name):Instruction(code_name){

}

void InAlImm8::ExecuteSelf(Cpu* cpu){
    //cpu->PrintInstuctionLog();
    unsigned char imm8;
    cpu->AddEip(1);
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    this->Error("imm8 = %02X at InAlImm8s\n", imm8);
    return;
}

InAlDx::InAlDx(string code_name):Instruction(code_name){

}

void InAlDx::ExecuteSelf(Cpu* cpu){
    //cpu->PrintInstuctionLog();
    unsigned char al;
    unsigned int dx;
    cpu->AddEip(1);
    dx = (unsigned int)((unsigned short)cpu->GetR16(EDX));
    al = cpu->io_manager->In8(cpu, dx);
    cpu->SetR8(EAX, al);
    return;
}


PushRm32::PushRm32(string code_name):Instruction(code_name){

}

void PushRm32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16 bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    unsigned int rm32;
    rm32 = this->GetRM32(cpu);
    this->Push32(cpu, rm32);
    return;
}

DecRm32::DecRm32(string code_name):Instruction(code_name){

}

void DecRm32::ExecuteSelf(Cpu* cpu){
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    unsigned int rm32;
    unsigned int result;
    rm32 = this->GetRM32(cpu);
    result = rm32 - 1;
    this->SetRM32(cpu, result);
    cpu->UpdateEflagsForDec(result, rm32, -1);
    return;
}

Code80_32bitsMode::Code80_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[7] = new CmpRm8Imm8("CmpRm8Imm8");
}

void Code80_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code 80 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

JmpM1632::JmpM1632(string code_name):Instruction(code_name){

}

void JmpM1632::ExecuteSelf(Cpu* cpu){
    unsigned int addr;
    unsigned int desc_addr;
    unsigned int offset_addr;
    GdtGate gdt_gate;
    unsigned short selector;
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    addr = this->GetEffectiveAddr(cpu);
    offset_addr = cpu->Read32(cpu->GetLinearAddrForDataAccess(addr));
    selector = cpu->Read16(cpu->GetLinearAddrForDataAccess(addr+4));
    desc_addr = cpu->GetGdtGate(selector);
    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);
    if(cpu->IsCodeSegment(gdt_gate)){
        cpu->Set(CS, selector);
        cpu->SetEip(offset_addr);
        //this->Error("CodeSegment is not implemented at %s::ExecuteSelf",this->code_name.c_str());
    }else{
        cpu->SaveTask(selector);
        cpu->SwitchTask();
    }
    return;
}

CodeFF_32bitsMode::CodeFF_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[0] = new IncRm32("IncRm32");
    this->instructions[1] = new DecRm32("DecRm32");
    this->instructions[3] = new CallM1632("CallM1632");
    this->instructions[5] = new JmpM1632("JmpM1632");
    this->instructions[6] = new PushRm32("PushRm32");
}

void CodeFF_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code FF %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    //this->instructions[this->modrm.reg_index]->ShowSelf();
    return;
}

OrRm32R32::OrRm32R32(string code_name):Instruction(code_name){

}

void OrRm32R32::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int r32;
    unsigned int result;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    rm32 = this->GetRM32(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    result = rm32 | r32;
    this->SetRM32(cpu, result);
    cpu->UpdateEflagsForOr(result);
    return;
}

ShrRm32Imm8::ShrRm32Imm8(string code_name):Instruction(code_name){

}

void ShrRm32Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned char imm8;
    rm32 = this->GetRM32(cpu);
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    if(imm8==1){
        this->Error("imm8==1 is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    for(unsigned int i=0; i<imm8; i++){
        if(rm32&0x80000000){
            cpu->SetCF();
        }else{
            cpu->ClearCF();
        }
        rm32 = rm32 / 2;
    }
    this->SetRM32(cpu, rm32);
    cpu->UpdateEflagsForShr(rm32);
    return;
}

CodeFE_32bitsMode::CodeFE_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
}

void CodeFE_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            cpu->PrintInstructionLog(0, 200);
            this->Error("code FE %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

CodeC1_32bitsMode::CodeC1_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    //this->instructions[6] = new PushRm32("PushRm32");
    this->instructions[4] = new SalRm32Imm8("SalRm32Imm8");
    this->instructions[5] = new ShrRm32Imm8("ShrRm32Imm8");
    this->instructions[7] = new SarRm32Imm8("SarRm32Imm8");
}

void CodeC1_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code C1 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

MovR32Rm32::MovR32Rm32(string code_name):Instruction(code_name){

}

void MovR32Rm32::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int r32;
    unsigned short rm16;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    if(cpu->IsPrefixOp()){
        rm16 = this->GetRM16(cpu);
        cpu->SetR16((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index, rm16);
        return;
    }
    rm32 = this->GetRM32(cpu);
    cpu->SetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index, rm32);
    return;
}

MovR8Rm8::MovR8Rm8(string code_name):Instruction(code_name){

}

void MovR8Rm8::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    unsigned char r8;
    unsigned char rm8;

    rm8 = this->GetRM8(cpu);
    this->SetR8(cpu, rm8);
    return;
}

CmpR32Rm32::CmpR32Rm32(string code_name):Instruction(code_name){

}

void CmpR32Rm32::ExecuteSelf(Cpu* cpu){
    unsigned int r32;
    unsigned int rm32;
    unsigned long long result;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    rm32 = this->GetRM32(cpu);
    result = (unsigned long long)r32 - (unsigned long long)rm32;
    cpu->UpdateEflagsForSub(result, r32, rm32);
    return;
}

CmpRm32R32::CmpRm32R32(string code_name):Instruction(code_name){

}

void CmpRm32R32::ExecuteSelf(Cpu* cpu){
    unsigned int r32;
    unsigned int rm32;
    unsigned long long result;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    rm32 = this->GetRM32(cpu);
    result = (unsigned long long)rm32 - (unsigned long long)r32;
    cpu->UpdateEflagsForSub(result, rm32, r32);
    return;
}

SalRm32Imm8::SalRm32Imm8(string code_name):Instruction(code_name){

}

void SalRm32Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int imm8;

    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    rm32 = this->GetRM32(cpu);
    imm8 = (unsigned int)((unsigned char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(imm8==1){
        this->Error("imm8==1 is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    for(unsigned int i=0; i<imm8; i++){
        if(rm32&0x80000000){
            cpu->SetCF();
        }else{
            cpu->ClearCF();
        }
        rm32 = rm32 * 2;
    }
    this->SetRM32(cpu, rm32);
    return;
}

SubEaxImm32::SubEaxImm32(string code_name):Instruction(code_name){

}

void SubEaxImm32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int eax;
    unsigned int imm32;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    eax = cpu->GetR32(EAX);
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    cpu->SetR32(EAX, eax-imm32);
    result = (unsigned long long)eax - (unsigned long long)imm32;
    cpu->UpdateEflagsForSub(result, eax, imm32);
    return;
}

SubRm32R32::SubRm32R32(string code_name):Instruction(code_name){

}

void SubRm32R32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int rm32;
    unsigned int r32;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    
    rm32 = this->GetRM32(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    result = (unsigned long long)rm32 - (unsigned long long)r32;
    this->SetRM32(cpu, rm32-r32);
    cpu->UpdateEflagsForSub(result, rm32, r32);
    return;
}

SubR32Rm32::SubR32Rm32(string code_name):Instruction(code_name){

}

void SubR32Rm32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int rm32;
    unsigned int r32;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    
    rm32 = this->GetRM32(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    result = (unsigned long long)r32 - (unsigned long long)rm32;
    this->SetRM32(cpu, r32-rm32);
    cpu->UpdateEflagsForSub(result, r32, rm32);
    return;
}

SubRm32Imm32::SubRm32Imm32(string code_name):Instruction(code_name){

}

void SubRm32Imm32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int rm32;
    unsigned int imm32;
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    
    rm32 = this->GetRM32(cpu);
    imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(4);
    result = (unsigned long long)rm32 - (unsigned long long)imm32;
    this->SetRM32(cpu, rm32-imm32);
    cpu->UpdateEflagsForSub(result, rm32, imm32);
    return;
}

NegRm32::NegRm32(string code_name):Instruction(code_name){

}

void NegRm32::ExecuteSelf(Cpu* cpu){
    unsigned long long result;
    unsigned int rm32;
    unsigned int zero = 0;
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    
    rm32 = this->GetRM32(cpu);
    if(rm32==0){
        cpu->ClearCF();
    }else{
        cpu->SetCF();
    }
    result = (unsigned long long)zero - (unsigned long long)rm32;
    this->SetRM32(cpu, zero-rm32);
    cpu->UpdateEflagsForSub(result, zero, rm32);
    return;
}

AddRm32R32::AddRm32R32(string code_name):Instruction(code_name){

}

void AddRm32R32::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int r32;
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    
    rm32 = this->GetRM32(cpu);
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    this->SetRM32(cpu, rm32+r32);
    return;
}

IncRm32::IncRm32(string code_name):Instruction(code_name){

}

void IncRm32::ExecuteSelf(Cpu* cpu){  
    unsigned short rm16;
    unsigned int rm32;  
    
    if(cpu->IsPrefixOp()){
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }else{
        rm32 = this->GetRM32(cpu);
        this->SetRM32(cpu, rm32+1);
        cpu->UpdateEflagsForInc(rm32+1, rm32, 1);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

XchgEaxR32::XchgEaxR32(string code_name):Instruction(code_name){

}

void XchgEaxR32::ExecuteSelf(Cpu* cpu){ 
    unsigned int eax;
    unsigned int r32;

    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x90);
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    }else{
        r32 = cpu->GetR32(register_type);
        eax = cpu->GetR32(EAX);
        cpu->SetR32(register_type, eax);
        cpu->SetR32(EAX, r32);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

Ret32Far::Ret32Far(string code_name):Instruction(code_name){

}

void Ret32Far::ExecuteSelf(Cpu* cpu){
    unsigned int addr;
    unsigned short selector;
    cpu->AddEip(1);

    if(cpu->IsPrefixOp()){
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
        return;
    }else{
        addr = (unsigned int)this->Pop32(cpu);
        selector = (unsigned short)this->Pop32(cpu);
        cpu->SetEip(addr);
        cpu->Set(CS, selector);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

Ret32Near::Ret32Near(string code_name):Instruction(code_name){

}

void Ret32Near::ExecuteSelf(Cpu* cpu){
    unsigned int addr;
    cpu->AddEip(1);

    if(cpu->IsPrefixOp()){
        this->Error("16bits op Mode is not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
        return;
    }else{
        addr = (unsigned int)this->Pop32(cpu);
        cpu->SetEip(addr);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str(), this->code_name.c_str());
}

AddRm32Imm8::AddRm32Imm8(string code_name):Instruction(code_name){

}

void AddRm32Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int imm8;
    unsigned short imm8_16bits;
    unsigned short rm16;
    unsigned int rm32;  
    if(cpu->IsPrefixOp()){
        rm16 = this->GetRM16(cpu);
        imm8_16bits = (short)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        this->SetRM16(cpu, rm16+imm8_16bits);
        cpu->AddEip(1);
        return;
    }else{
        rm32 = this->GetRM32(cpu);
        imm8 = (int)((char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
        this->SetRM32(cpu, rm32+imm8);
        cpu->AddEip(1);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

AddEaxImm32::AddEaxImm32(string code_name):Instruction(code_name){

}

void AddEaxImm32::ExecuteSelf(Cpu* cpu){
    unsigned int imm32;
    unsigned int eax;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }else{
        imm32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        eax   = cpu->GetR32(EAX);
        cpu->SetR32(EAX, eax+imm32);
        return;
    }
    this->Error("Not implemeneted at %s::ExecuteSelf", this->code_name.c_str());
    return;
}

CmpAlImm8::CmpAlImm8(string code_name):Instruction(code_name){

}

void CmpAlImm8::ExecuteSelf(Cpu* cpu){
    unsigned char imm8;
    unsigned char al;
    unsigned long long result;
    cpu->AddEip(1);
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    al  = cpu->GetR8L(EAX);
    result = (unsigned long long)al - (unsigned long long)imm8;
    cpu->UpdateEflagsForSub(result, al, imm8);
    return;
}

MovAlMoffs8::MovAlMoffs8(string code_name):Instruction(code_name){

}

void MovAlMoffs8::ExecuteSelf(Cpu* cpu){
    unsigned int offset32;
    unsigned char data;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixAddr()){
        offset32 = (unsigned int)cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        data     = (unsigned char)cpu->Read8(cpu->GetLinearAddrForDataAccess(offset32));
        cpu->SetR8L(EAX, data);
        return;
    }else{
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

MovEaxMoffs32::MovEaxMoffs32(string code_name):Instruction(code_name){

}

void MovEaxMoffs32::ExecuteSelf(Cpu* cpu){
    unsigned int addr = 0;
    unsigned int offset32;
    unsigned int data;

    cpu->AddEip(1);
    if(cpu->IsPrefixOp() | cpu->IsPrefixAddr()){
        this->Error("16bits op or addr is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    if(cpu->Is32bitsMode()){
        offset32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        data     = cpu->Read32(cpu->GetLinearAddrForDataAccess(offset32));
        cpu->SetR32(EAX, data);
        return;
    }else{
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf");
    }
    return;
}

MovMoffs32Eax::MovMoffs32Eax(string code_name):Instruction(code_name){

}

void MovMoffs32Eax::ExecuteSelf(Cpu* cpu){
    unsigned int offset32;
    unsigned int eax;
    unsigned int addr;
    cpu->AddEip(1);
    if(cpu->IsPrefixOp() | cpu->IsPrefixAddr()){
        this->Error("16bits op or addr is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    if(cpu->Is32bitsMode()){
        offset32 = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
        cpu->AddEip(4);
        eax = cpu->GetR32(EAX);
        addr = cpu->GetLinearAddrForDataAccess(offset32);
        cpu->Write32(addr, eax);
        return;
    }else{
        this->Error("16bits Mode is not implemented at %s::ExecuteSelf");
    }
    return;
}

ShrRm8Imm8::ShrRm8Imm8(string code_name):Instruction(code_name){

}

void ShrRm8Imm8::ExecuteSelf(Cpu* cpu){
    unsigned char rm8;
    unsigned char imm8;
    rm8 = this->GetRM8(cpu);
    imm8 = cpu->Read8(cpu->GetLinearAddrForCodeAccess());
    cpu->AddEip(1);
    if(imm8==1){
        this->Error("imm8==1 is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    for(unsigned int i=0; i<imm8; i++){
        if((unsigned char)rm8&(unsigned char)0x80){
            cpu->SetCF();
        }else{
            cpu->ClearCF();
        }
        rm8 = rm8 / 2;
    }
    this->SetRM8(cpu, rm8);
    cpu->UpdateEflagsForShr8(rm8);
    return;
}


CodeC0_32bitsMode::CodeC0_32bitsMode(string code_name):Instruction(code_name){
    for(int i=0; i<this->instruction_size; i++){
        this->instructions[i] = NULL;
    }
    this->instructions[5] = new ShrRm8Imm8("ShrRm8Imm8");
}

void CodeC0_32bitsMode::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    
    if(this->instructions[this->modrm.reg_index]==NULL){
            this->Error("code C0 %02X is not implemented %s::ExecuteSelf", this->modrm.reg_index, this->code_name.c_str());
    }
    this->instructions[this->modrm.reg_index]->SetModRM(this->modrm, this->sib);
    this->instructions[this->modrm.reg_index]->ExecuteSelf(cpu);
    return;
}

OutDxAl::OutDxAl(string code_name):Instruction(code_name){

}

void OutDxAl::ExecuteSelf(Cpu* cpu){
    cpu->AddEip(1);
    unsigned int addr;
    unsigned char data;

    data = cpu->GetR8L(EAX);
    addr = (unsigned int)((unsigned short)cpu->GetR16(EDX));
    
    cpu->Out8(addr, data);
    return;
}

Cwde::Cwde(string code_name):Instruction(code_name){

}

void Cwde::ExecuteSelf(Cpu* cpu){
    short al;
    int ax;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        ax = (int)((short)cpu->GetR16(EAX));
        cpu->SetR32(EAX, ax);
        return;
    }else{
        al = (short)((char)cpu->GetR8L(EAX));
        cpu->SetR16(EAX, al);
        return;
    }
    return;
}

DecR32::DecR32(string code_name):Instruction(code_name){

}

void DecR32::ExecuteSelf(Cpu* cpu){
    if(cpu->Is16bitsMode() ^ cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x48);
    cpu->AddEip(1);
    unsigned int r32;
    unsigned int result;
    r32 = cpu->GetR32(register_type);
    result = r32 - 1;
    cpu->SetR32(register_type, result);
    cpu->UpdateEflagsForDec(result, r32, -1);
    return;
}

PushFd::PushFd(string code_name):Instruction(code_name){

}

void PushFd::ExecuteSelf(Cpu* cpu){   
    unsigned int eflags;
    unsigned int addr;
    cpu->AddEip(1);
    if(cpu->Is16bitsMode() ^ cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    eflags = cpu->GetEflagsValue();
    this->Push32(cpu, eflags);
    return;
}

PopR32::PopR32(string code_name):Instruction(code_name){

}

void PopR32::ExecuteSelf(Cpu* cpu){   
    unsigned int r32;
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x58);
    cpu->AddEip(1);
    if(cpu->Is16bitsMode() ^ cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    r32 = this->Pop32(cpu);
    cpu->SetR32(register_type, r32);
    return;
}

PopAd::PopAd(string code_name):Instruction(code_name){

}

void PopAd::ExecuteSelf(Cpu* cpu){   
    unsigned int eax, ecx, edx, ebx, esp, ebp, esi, edi;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        edi = this->Pop32(cpu);
        esi = this->Pop32(cpu);
        ebp = this->Pop32(cpu);
        esp = this->Pop32(cpu);
        ebx = this->Pop32(cpu);
        edx = this->Pop32(cpu);
        ecx = this->Pop32(cpu);
        eax = this->Pop32(cpu);

        cpu->SetR32(EAX, eax);
        cpu->SetR32(ECX, ecx);
        cpu->SetR32(EDX, edx);
        cpu->SetR32(EBX, ebx);
        //espは無視する
        cpu->SetR32(EBP, ebp);
        cpu->SetR32(ESI, esi);
        cpu->SetR32(EDI, edi);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PopDs::PopDs(string code_name):Instruction(code_name){

}

void PopDs::ExecuteSelf(Cpu* cpu){   
    unsigned int ds = 0;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        ds = this->Pop32(cpu);
        cpu->Set(DS, ds);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PopEs::PopEs(string code_name):Instruction(code_name){

}

void PopEs::ExecuteSelf(Cpu* cpu){   
    unsigned int es = 0;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        es = this->Pop32(cpu);
        cpu->Set(ES, es);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PopFd::PopFd(string code_name):Instruction(code_name){

}

void PopFd::ExecuteSelf(Cpu* cpu){   
    unsigned int imm32;
    cpu->AddEip(1);
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)((unsigned int)cpu->Read8(cpu->GetLinearAddrForCodeAccess())-(unsigned int)0x58);
    if(cpu->Is16bitsMode() ^ cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    imm32 = this->Pop32(cpu);
    cpu->SetEflags(imm32);
    return;
}

TestRm8R8::TestRm8R8(string code_name):Instruction(code_name){

}

void TestRm8R8::ExecuteSelf(Cpu* cpu){   
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    unsigned char r8;
    unsigned char rm8;
    unsigned char result;
    rm8 = this->GetRM8(cpu);
    r8  = this->GetR8(cpu);
    result = rm8 & r8;
    cpu->UpdateEflagsForAnd8(result);
    return;
}

TestRm32R32::TestRm32R32(string code_name):Instruction(code_name){

}

void TestRm32R32::ExecuteSelf(Cpu* cpu){   
    if(cpu->Is16bitsMode() || cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    unsigned int rm32;
    unsigned int r32;
    unsigned int result;
    r32  = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->modrm.reg_index);
    rm32 = this->GetRM32(cpu);
    result = rm32 & r32;
    cpu->UpdateEflagsForAnd(result);
    return;
}

Cdq::Cdq(string code_name):Instruction(code_name){

}

//EAXレジスタの符号部分をEDXの各ビットに転送する
void Cdq::ExecuteSelf(Cpu* cpu){   
    unsigned int eax;

    cpu->AddEip(1);
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    eax = cpu->GetR32(EAX);

    if((eax&0x80000000)==0x80000000){
        cpu->SetR32(EDX, 0xFFFFFFFF);
    }else{
        cpu->SetR32(EDX, 0x00000000);
    }
    return;
}

SarRm32Imm8::SarRm32Imm8(string code_name):Instruction(code_name){

}

void SarRm32Imm8::ExecuteSelf(Cpu* cpu){
    unsigned int rm32;
    unsigned int imm8;

    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }
    rm32 = this->GetRM32(cpu);
    imm8 = (unsigned int)((unsigned char)cpu->Read8(cpu->GetLinearAddrForCodeAccess()));
    cpu->AddEip(1);
    if(imm8==1){
        this->Error("imm8==1 is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    for(unsigned int i=0; i<imm8; i++){
        if(rm32&0x80000000){
            cpu->SetCF();
        }else{
            cpu->ClearCF();
        }
        rm32 = rm32 / 2;
    }
    this->SetRM32(cpu, rm32);
    return;
}

AddRm8R8::AddRm8R8(string code_name):Instruction(code_name){

}

void AddRm8R8::ExecuteSelf(Cpu* cpu){
    /***
    static int c = 0;
    c++;
    if(c == 5){
        cpu->PrintInstructionLog(0, 200);
        this->Error("stopped at %s::ExecuteSelf\n", this->code_name.c_str());
    }
    ***/
    cpu->AddEip(1);
    this->ParseModRM(cpu);
    unsigned char rm8;
    unsigned char r8;
    GENERAL_PURPOSE_REGISTER register_type = (GENERAL_PURPOSE_REGISTER)this->modrm.reg_index;
    rm8  = this->GetRM8(cpu);
    r8   = this->GetR8(cpu);
    this->SetRM8(cpu, rm8+r8);
    return;
}

PushCs::PushCs(string code_name):Instruction(code_name){

}

void PushCs::ExecuteSelf(Cpu* cpu){
    unsigned int cs = 0;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        cs = (unsigned int)((unsigned short)cpu->GetR16(CS));
        this->Push32(cpu, cs);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PushEs::PushEs(string code_name):Instruction(code_name){

}

void PushEs::ExecuteSelf(Cpu* cpu){
    unsigned int es = 0;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        es = (unsigned int)((unsigned short)cpu->GetR16(ES));
        this->Push32(cpu, es);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PushDs::PushDs(string code_name):Instruction(code_name){

}

void PushDs::ExecuteSelf(Cpu* cpu){
    unsigned int ds = 0;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        ds = (unsigned int)((unsigned short)cpu->GetR16(DS));
        this->Push32(cpu, ds);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

PushAd::PushAd(string code_name):Instruction(code_name){

}

void PushAd::ExecuteSelf(Cpu* cpu){
    unsigned int esp;
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        esp = cpu->GetR32(ESP);
        this->Push32(cpu, cpu->GetR32(EAX));
        this->Push32(cpu, cpu->GetR32(ECX));
        this->Push32(cpu, cpu->GetR32(EDX));
        this->Push32(cpu, cpu->GetR32(EBX));
        this->Push32(cpu, esp);
        this->Push32(cpu, cpu->GetR32(EBP));
        this->Push32(cpu, cpu->GetR32(ESI));
        this->Push32(cpu, cpu->GetR32(EDI));
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

Iretd::Iretd(string code_name):Instruction(code_name){

}

void Iretd::ExecuteSelf(Cpu* cpu){
    unsigned int eflags;
    unsigned int eip;
    unsigned short cs;
    if(cpu->IsRealMode()){
        this->Error("Real mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    cpu->AddEip(1);
    if(cpu->Is32bitsMode() ^ cpu->IsPrefixOp()){
        cs  = this->Pop32(cpu);
        eip = this->Pop32(cpu);
        eflags = this->Pop32(cpu);
        cpu->Set(CS, cs);
        cpu->SetEip(eip);
        cpu->SetEflags(eflags);
    }else{
        this->Error("16bits mode is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

CallPtr1632::CallPtr1632(string code_name):Instruction(code_name){

}

void CallPtr1632::ExecuteSelf(Cpu* cpu){
    unsigned short selector;
    unsigned int imm32;
    unsigned int desc_addr;
    cpu->AddEip(1);
    GdtGate gdt_gate;

    if(cpu->IsPrefixOp()){
        this->Error("Not implemented PrefixOp at %s::ExecuteSelf", this->code_name.c_str());
    }
    imm32    = cpu->Read32(cpu->GetLinearAddrForCodeAccess());
    selector = cpu->Read16(cpu->GetLinearAddrForCodeAccess()+4);
    desc_addr = cpu->GetGdtGate(selector);
    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);
    cpu->AddEip(6);
    if(cpu->IsCodeSegment(gdt_gate)){
        this->Push32(cpu, cpu->GetR16(CS));
        this->Push32(cpu, cpu->GetEipValue());
        cpu->SetEip(imm32);
        cpu->Set(CS, selector);
    }else{
        this->Error("Task switch is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}

CallM1632::CallM1632(string code_name):Instruction(code_name){

}

void CallM1632::ExecuteSelf(Cpu* cpu){
    unsigned int addr;
    unsigned int desc_addr;
    unsigned int offset_addr;
    GdtGate gdt_gate;
    unsigned short selector;
    if(cpu->IsPrefixOp()){
        this->Error("16bits mode is not implemented at %s::ExecuteSelf");
    }

    addr = this->GetEffectiveAddr(cpu);
    offset_addr = cpu->Read32(cpu->GetLinearAddrForDataAccess(addr));
    selector = cpu->Read16(cpu->GetLinearAddrForDataAccess(addr+4));
    desc_addr = cpu->GetGdtGate(selector);
    gdt_gate.limit_low = cpu->Read16(desc_addr);
    gdt_gate.base_low = cpu->Read16(desc_addr+2);
    gdt_gate.base_mid = cpu->Read8(desc_addr+4);
    gdt_gate.access_right = cpu->Read8(desc_addr+5);
    gdt_gate.limit_high = cpu->Read8(desc_addr+6);
    gdt_gate.base_high = cpu->Read8(desc_addr+7);

    if(cpu->IsCodeSegment(gdt_gate)){
        this->Push32(cpu, cpu->GetR16(CS));
        this->Push32(cpu, cpu->GetEipValue());
        cpu->SetEip(offset_addr);
        cpu->Set(CS, selector);
    }else{
        this->Error("Task switch is not implemented at %s::ExecuteSelf", this->code_name.c_str());
    }
    return;
}
