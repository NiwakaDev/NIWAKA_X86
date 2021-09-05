#pragma once
#include "common.h"
#include "DescData.h"
class Memory;
class SegmentRegister;
class IoManager;
class TaskRegister;
class Bios;
class Idtr;
class Gdtr;
class Instruction;
class ControlRegister;

#define SIGN_FLG 0x80000000
class Cpu:public Object{
    private:
        bool exception;
        SEGMENT_REGISTER default_code_selector;
        SEGMENT_REGISTER default_data_selector;
        SEGMENT_REGISTER default_stack_selector;
        vector<bool> prefix_flgs;
        Memory* memory;
        uint32_t gprs[GENERAL_PURPOSE_REGISTERS_COUNT];
        uint32_t eip;
        union{
            uint32_t raw;
            struct{
                unsigned CF : 1;
                unsigned RESERVED0 : 1;
                unsigned PF : 1;
                unsigned RESERVED1 : 1;
                unsigned AF : 1;
                unsigned RESERVED2 : 1;
                unsigned ZF : 1;
                unsigned SF : 1;
                unsigned TF : 1;
                unsigned IF : 1;
                unsigned DF : 1;
                unsigned OF : 1;
                unsigned IOPL : 2;
                unsigned NT : 1;
                unsigned RESERVED3 : 1;
                unsigned RF : 1;
                unsigned VM : 1;
                unsigned AC : 1;
                unsigned VIF : 1;
                unsigned VIP : 1;
                unsigned ID : 1;
            }flgs;
        }eflags;

        Tss now_tss;
        Tss buff_tss;
    
        SegmentRegister* segment_registers[SEGMENT_REGISTERS_COUNT];
        ControlRegister* control_registers[CONTROL_REGISTERS_COUNT];
        Instruction* instructions_32bits_mode[INSTRUCTION_CNT];
        Instruction* instructions_16bits_mode[INSTRUCTION_CNT];
        vector<string> instruction_log;
        //Eip* eip;
        //Eflags* eflags;
        Gdtr* gdtr;
        Idtr* idtr;
        TaskRegister* task_register = NULL;
        Bios* bios;
        map<unsigned char, bool> is_prefix_table;
        void CheckPrefixCode();
        void ResetPrefixFlg();
        bool Is67Flg();
        bool Is66Flg();

        void SetCF(uint64_t result);
        void SetZF(uint64_t result);
        void SetOF(uint32_t result, uint32_t d1, uint32_t d2);
        void SetSF(uint32_t result);
        void SetPF(uint32_t result);    
        void SetSF8(uint8_t result);

    public:
        void LogInstruction(string op_name);//命令実行履歴
        IoManager* io_manager;
        void PrintInstuctionLog();
        void PrintInstructionLog(unsigned int start_idx, unsigned int cnt);
        void CallFunction(unsigned char bios_number);
        bool IsPrefixAddr();
        bool IsPrefixOp();
        bool Is32bitsMode();
        bool Is16bitsMode();
        void Out8(unsigned int addr, unsigned char data);
        Cpu(Memory* memory, Bios* bios);
        void ExecuteSelf();
        void ShowSelf();
        bool IsProtectedMode();
        bool IsRealMode();
        void SetR8L(GENERAL_PURPOSE_REGISTER register_type, uint8_t data);
        void SetR8H(GENERAL_PURPOSE_REGISTER register_type, uint8_t data);
        void SetR8(GENERAL_PURPOSE_REGISTER register_type, uint8_t data);
        void SetR16(GENERAL_PURPOSE_REGISTER register_type, uint16_t data);
        void SetR32(GENERAL_PURPOSE_REGISTER register_type, uint32_t data);
        void Set(SEGMENT_REGISTER register_type, unsigned short data);
        void SetR32(CONTROL_REGISTER register_type, unsigned int data);
        void SetGdtr(unsigned short limit, unsigned int base);
        void SetIdtr(unsigned short limit, unsigned int base);
        void SetTr(unsigned short value);
        uint8_t GetR8H(GENERAL_PURPOSE_REGISTER register_type);
        uint8_t GetR8L(GENERAL_PURPOSE_REGISTER register_type);
        uint16_t GetR16(GENERAL_PURPOSE_REGISTER register_type);
        unsigned short GetR16(SEGMENT_REGISTER register_type);
        uint32_t   GetR32(GENERAL_PURPOSE_REGISTER register_type);
        unsigned int  GetR32(CONTROL_REGISTER register_type);
        uint32_t GetGPRValue(GENERAL_PURPOSE_REGISTER register_type);
        uint32_t GetEipValue();
        unsigned int GetEflagsValue();
        unsigned int GetGdtBase();
        unsigned int GetIdtBase();
        unsigned int GetLinearAddrForCodeAccess();//機械語命令解釈時の物理アドレス
        unsigned int GetLinearAddrForDataAccess(unsigned int offset);//データを読み取る際の物理アドレス
        unsigned int GetLinearStackAddr();//スタック領域のリニアアドレスを取得する
        void AddEip(uint32_t data);
        void SetEip(uint32_t addr);

        void SetEflags(uint32_t data);

        void Write8(unsigned int addr, unsigned char data);
        void Write16(unsigned int addr, unsigned short data);
        void Write32(unsigned int addr, unsigned int data);
    
        unsigned char Read8(unsigned int linear_addr);
        unsigned short Read16(unsigned int linear_addr);
        unsigned int Read32(unsigned int linear_addr);

        void UpdateEflagsForSub(uint64_t result, uint32_t d1, uint32_t d2);
        void UpdateEflagsForAnd(uint32_t result);
        void UpdateEflagsForAnd8(unsigned char result);
        void UpdateEflagsForOr(unsigned int result);
        void UpdateEflagsForOr8(unsigned char result);
        void UpdateEflagsForXor(unsigned int result);

        void UpdateEflagsForShr(unsigned int result);
        void UpdateEflagsForShr8(unsigned char result);
        void UpdateEflagsForInc(unsigned int result, unsigned int d1, unsigned int d2);
        void UpdateEflagsForDec(unsigned int result, unsigned int d1, unsigned int d2);
        
        void InitSelector();
        void SetDataSelector(SEGMENT_REGISTER register_type);
        void SetCodeSelector(SEGMENT_REGISTER register_type);
        void SetStackSelector(SEGMENT_REGISTER register_type);

        bool IsCF();
        void ClearCF();
        void SetCF();
        bool IsZF();
        void ClearIF();
        void SetIF();
        void ClearOF();
        bool IsIF();
        bool IsOF();
        bool IsSF();
    
        unsigned int GetGdtGate(unsigned short select_number);

        void SaveTask(unsigned short next_selector);
        void SwitchTask();

        bool IsSystemSegment(GdtGate gdt_gate);
        bool IsCodeSegment(GdtGate gdt_gate);
        bool IsTss(GdtGate gdt_gate);
};