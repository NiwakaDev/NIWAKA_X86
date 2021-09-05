#pragma once
#include "common.h"
#include "ModRM.h"
#include "DescData.h"
class Cpu;
class Sib;

class Instruction:public Object{
    protected:
        string code_name;
        ModRM modrm;
        Sib* sib;
        void ParseModRM(Cpu *cpu);
        void ParseModRM_32bitsMode(Cpu *cpu);
        void ParseModRM_16bitsMode(Cpu *cpu);
        unsigned char GetR8(Cpu* cpu);
        unsigned char GetRM8(Cpu* cpu);
        unsigned short GetRM16(Cpu* cpu);
        unsigned int GetRM32(Cpu* cpu);
        unsigned int GetEffectiveAddr(Cpu* cpu);
        void SetR8(Cpu* cpu, unsigned char data);
        void SetRM8(Cpu* cpu, unsigned char data);
        void SetRM16(Cpu* cpu, unsigned short data);
        void SetRM32(Cpu* cpu, unsigned int data);
        void GetDigit(Cpu* cpu);
        void GetRegIndex(Cpu *cpu);
        unsigned short GetR16ForEffectiveAddr(Cpu *cpu);
    public:
        virtual void ExecuteSelf(Cpu* cpu);
        virtual void ShowSelf();
        Instruction(string code_name);
        void SetModRM(ModRM modrm, Sib* sib);
        void Push32(Cpu* cpu, unsigned int data);
        void Push16(Cpu* cpu, unsigned short data);
        unsigned int Pop32(Cpu* cpu);
        unsigned short Pop16(Cpu* cpu);
        string GetName();
};

class MovRm32Imm32:public Instruction{
    public:
        MovRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR8Rm8:public Instruction{
    public:
        MovR8Rm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR8Imm8:public Instruction{
    public:
        MovR8Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovRm8R8:public Instruction{
    public:
        MovRm8R8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovSregRm16:public Instruction{
    public:
        MovSregRm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Leave:public Instruction{
    public:
        Leave(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class InAlImm8:public Instruction{
    public:
        InAlImm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class InAlDx:public Instruction{
    public:
        InAlDx(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CallRel16:public Instruction{
    public:
        CallRel16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CallRel32:public Instruction{
    public:
        CallRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Ret16Near:public Instruction{
    public:
        Ret16Near(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Ret32Near:public Instruction{
    public:
        Ret32Near(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Ret32Far:public Instruction{
    public:
        Ret32Far(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Iretd:public Instruction{
    public:
        Iretd(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Hlt:public Instruction{
    public:
        Hlt(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpRel8:public Instruction{
    public: 
        JmpRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpRel16:public Instruction{
    public: 
        JmpRel16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpRel32:public Instruction{
    public: 
        JmpRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR16Imm16:public Instruction{
    public:
        MovR16Imm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR32Imm32:public Instruction{
    public:
        MovR32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovRm16Imm16:public Instruction{
    public:
        MovRm16Imm16(string code_name);
        void ExecuteSelf(Cpu* cpu);   
};

class MovRm16R16:public Instruction{
    public:
        MovRm16R16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovRm32R32:public Instruction{
    public:
        MovRm32R32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR16Rm16:public Instruction{
    public:
        MovR16Rm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR32Rm32:public Instruction{
    public:
        MovR32Rm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovRm16Sreg:public Instruction{
    public:
        MovRm16Sreg(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushR16:public Instruction{
    public:
        PushR16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushR32:public Instruction{
    public:
        PushR32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushRm32:public Instruction{
    public:
        PushRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushCs:public Instruction{
    public:
        PushCs(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushEs:public Instruction{
    public:
        PushEs(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushDs:public Instruction{
    public:
        PushDs(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PopDs:public Instruction{
    public:
        PopDs(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PopEs:public Instruction{
    public:
        PopEs(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushAd:public Instruction{
    public:
        PushAd(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PopAd:public Instruction{
    public:
        PopAd(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushImm8:public Instruction{
    public:
        PushImm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushImm32:public Instruction{
    public:
        PushImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubEaxImm32:public Instruction{
    public:
        SubEaxImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubRm32Imm8:public Instruction{
    public:
        SubRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubRm32R32:public Instruction{
    public:
        SubRm32R32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubR32Rm32:public Instruction{
    public:
        SubR32Rm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubRm16Imm16:public Instruction{
    public:
        SubRm16Imm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SubRm32Imm32:public Instruction{
    public:
        SubRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class NegRm32:public Instruction{
    public:
        NegRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm8Imm8:public Instruction{
    public:
        AddRm8Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AdcRm8R8:public Instruction{
    public:
        AdcRm8R8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm8R8:public Instruction{
    public:
        AddRm8R8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};


class CmpAlImm8:public Instruction{
    public:
        CmpAlImm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpRm8Imm8:public Instruction{
    public:
        CmpRm8Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpRm8R8:public Instruction{
    public:
        CmpRm8R8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpEaxImm32:public Instruction{
    public:
        CmpEaxImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpR32Rm32:public Instruction{
    public:
        CmpR32Rm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpRm32R32:public Instruction{
    public:
        CmpRm32R32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpRm32Imm8:public Instruction{
    public:
        CmpRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CmpRm32Imm32:public Instruction{
    public:
        CmpRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code80_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code80_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code80_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code80_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class OrRm32Imm32:public Instruction{
    public:
        OrRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class OrEaxImm32:public Instruction{
    public:
        OrEaxImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code81_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code81_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code81_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code81_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code83_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code83_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code83_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        Code83_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AndRm32Imm8:public Instruction{
    public:
        AndRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AndRm32Imm32:public Instruction{
    public:
        AndRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class TestRm8R8:public Instruction{
    public:
        TestRm8R8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class TestRm32R32:public Instruction{
    public:
        TestRm32R32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class IncR32:public Instruction{
    public:
        IncR32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeC6_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeC6_16bitsMode(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class CodeC6_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeC6_32bitsMode(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class IdivRm32:public Instruction{
    public:
        IdivRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ImulRm32:public Instruction{
    public:
        ImulRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeF7_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeF7_32bitsMode(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class MovRm8Imm8:public Instruction{
    public:
        MovRm8Imm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class CodeFF_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeFF_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeFF_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeFF_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class IncRm16:public Instruction{
    public:
        IncRm16(string code_name);
        void ExecuteSelf(Cpu* cpu);      
};

class CodeC1_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeC1_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeFE_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public:
        CodeFE_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class IncRm32:public Instruction{
    public:
        IncRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);      
};

class SubRm16Imm8:public Instruction{
    public:
        SubRm16Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm16Imm8:public Instruction{
    public:
        AddRm16Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm32Imm32:public Instruction{
    public:
        AddRm32Imm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm32Imm8:public Instruction{
    public:
        AddRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm16R16:public Instruction{
    public:
        AddRm16R16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddEaxImm32:public Instruction{
    public:
        AddEaxImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AddRm32R32:public Instruction{
    public:
        AddRm32R32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class IntImm8:public Instruction{
    public:
        IntImm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JncRel8:public Instruction{
    public:
        JncRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JnsRel8:public Instruction{
    public:
        JnsRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JleRel8:public Instruction{
    public:
        JleRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JleRel32:public Instruction{
    public:
        JleRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JneRel32:public Instruction{
    public:
        JneRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JeRel32:public Instruction{
    public:
        JeRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JcRel32:public Instruction{
    public:
        JcRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JnaRel32:public Instruction{
    public:
        JnaRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JlRel32:public Instruction{
    public:
        JlRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JgRel32:public Instruction{
    public:
        JgRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JsRel32:public Instruction{
    public:
        JsRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};


class JaRel32:public Instruction{
    public:
        JaRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JgRel8:public Instruction{
    public:
        JgRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JaRel8:public Instruction{
    public:
        JaRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JaeRel8:public Instruction{
    public:
        JaeRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JcRel8:public Instruction{
    public:
        JcRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JgeRel8:public Instruction{
    public:
        JgeRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JgeRel32:public Instruction{
    public:
        JgeRel32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JsRel8:public Instruction{
    public:
        JsRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JlRel8:public Instruction{
    public:
        JlRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JzRel8:public Instruction{
    public:
        JzRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JnzRel8:public Instruction{
    public:
        JnzRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JbeRel8:public Instruction{
    public:
        JbeRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JbRel8:public Instruction{
    public:
        JbRel8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpPtr16M16:public Instruction{
    public: 
        JmpPtr16M16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpPtr1632:public Instruction{
    public: 
        JmpPtr1632(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class JmpM1632:public Instruction{
    public: 
        JmpM1632(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovMoffs8Al:public Instruction{
    public:
        MovMoffs8Al(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovAlMoffs8:public Instruction{
    public:
        MovAlMoffs8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovEaxMoffs32:public Instruction{
    public:
        MovEaxMoffs32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovMoffs32Eax:public Instruction{
    public:
        MovMoffs32Eax(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class OutImm8:public Instruction{
    public:
        OutImm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class Cli:public Instruction{
    public:
        Cli(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class Sti:public Instruction{
    public:
        Sti(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovR32CRX:public Instruction{
    public:
        MovR32CRX(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class MovCRXR32:public Instruction{
    public:
        MovCRXR32(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class Code0F_16bitsMode:public Instruction{
    private:
        int instruction_size = 256;
        Instruction* instructions[256];
    public: 
        Code0F_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code0F_32bitsMode:public Instruction{
    private:
        int instruction_size = 256;
        Instruction* instructions[256];
    public: 
        Code0F_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeC0_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public: 
        CodeC0_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CodeC1_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public: 
        CodeC1_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code0F01_16bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public: 
        Code0F01_16bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Ltr:public Instruction{
    public:
        Ltr(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class Code0F00_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public: 
        Code0F00_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Code0F01_32bitsMode:public Instruction{
    private:
        int instruction_size = 8;
        Instruction* instructions[8];
    public: 
        Code0F01_32bitsMode(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SalRm32Imm8:public Instruction{
    public: 
        SalRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class SarRm32Imm8:public Instruction{
    public: 
        SarRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ShrRm32Imm8:public Instruction{
    public:
        ShrRm32Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ShrRm16Imm8:public Instruction{
    public: 
        ShrRm16Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ShrRm8Imm8:public Instruction{
    public: 
        ShrRm8Imm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Lgdt:public Instruction{
    public:
        Lgdt(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Lidt:public Instruction{
    public:
        Lidt(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AndAxImm16:public Instruction{
    public:
        AndAxImm16(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class OrAlImm8:public Instruction{
    public:
        OrAlImm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class OrRm16Imm8:public Instruction{
    public:
        OrRm16Imm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class OrRm32R32:public Instruction{
    public:
        OrRm32R32(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class OrRm32Imm8:public Instruction{
    public:
        OrRm32Imm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class XorRm32Imm8:public Instruction{
    public:
        XorRm32Imm8(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class XorEaxImm32:public Instruction{
    public:
        XorEaxImm32(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class LeaR32M:public Instruction{
    public:
        LeaR32M(string code_name);
        void ExecuteSelf(Cpu *cpu);
};

class AddAlImm8:public Instruction{
    public:
        AddAlImm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class XchgEaxR32:public Instruction{
    public:
        XchgEaxR32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovzxR32Rm8:public Instruction{
    public:
        MovzxR32Rm8(string code_name);
        void ExecuteSelf(Cpu* cpu); 
};

class OutDxAl:public Instruction{
    public:
        OutDxAl(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Cwde:public Instruction{
    public:
        Cwde(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class Cdq:public Instruction{
    public:
        Cdq(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class DecR32:public Instruction{
    public:
        DecR32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class DecRm32:public Instruction{
    public:
        DecRm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ImulR16Rm16Imm16:public Instruction{
    public: 
        ImulR16Rm16Imm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class ImulR32Rm32:public Instruction{
    public:
        ImulR32Rm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PushFd:public Instruction{
    public:
        PushFd(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PopFd:public Instruction{
    public:
        PopFd(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class PopR32:public Instruction{
    public:
        PopR32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovzxR32Rm16:public Instruction{
    public:
        MovzxR32Rm16(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class MovsxR32Rm8:public Instruction{
    public:
        MovsxR32Rm8(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class AndEaxImm32:public Instruction{
    public:
        AndEaxImm32(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CallPtr1632:public Instruction{
    public:
        CallPtr1632(string code_name);
        void ExecuteSelf(Cpu* cpu);
};

class CallM1632:public Instruction{
    public:
        CallM1632(string code_name);
        void ExecuteSelf(Cpu* cpu);
};
