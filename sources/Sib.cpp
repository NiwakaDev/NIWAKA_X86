#include "Cpu.h"
#include "Sib.h"

void Sib::ParseSib(unsigned char sib_byte, unsigned char mod){
    assert(this!=NULL);
    this->scale = ((sib_byte & 0xc0)>>6);
    this->index = ((sib_byte & 0x38)>>3);
    this->base  = sib_byte & 0x07;
    this->mod   = mod;
}

unsigned int Sib::GetAddress(Cpu* cpu){
    assert(this!=NULL);
    unsigned int addr = 0;
    if(this->base==4){
        cpu->SetDataSelector(SS);
    }
    if(this->base==5&&(this->mod==1 || this->mod==2)){
        addr = cpu->GetR32(EBP);
        cpu->SetDataSelector(SS);

    }
    else if(this->base==5 && this->mod==0x00){
        addr = this->disp32;
    }

    if(this->scale==0){
        if(this->base!=5){
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->base);
        }
        if(this->index!=4){
            addr = addr + cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->index);
        }
    }
    if(this->scale==1){
        if(this->base!=5){
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->base);
        }
        if(this->index!=4){
            addr = addr + cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->index)*2;
        }
    }
    if(this->scale==2){
        if(this->base!=5){
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->base);
        }
        if(this->index!=4){
            addr = addr + cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->index)*4;
        }
    }
    if(this->scale==3){
        if(this->base!=5){
            addr = cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->base);
        }
        if(this->index!=4){
            addr = addr + cpu->GetR32((GENERAL_PURPOSE_REGISTER)this->index)*8;
        }
    }
    return addr;
}

void Sib::ShowSelf(){
    assert(this!=NULL);
    printf("sib scale : %d\n", this->scale);
    printf("sib index : %d\n", this->index);
    printf("sib base  : %d\n", this->base);
}

unsigned int Sib::GetBase(){
    return this->base;
}

unsigned int Sib::GetScale(){
    return this->scale;
}

unsigned int Sib::GetIndex(){
    return this->index;
}

void Sib::SetDisp32(unsigned int disp32){
    this->disp32 = disp32;
}