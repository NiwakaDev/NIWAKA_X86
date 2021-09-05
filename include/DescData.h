#pragma once
#pragma pack(1)
#define DB 0x40 //32bits flg
#define SEGMENT_DESC_TYPE_FLG 1<<4//TSS or Code Segment
#define TSS_TYPE 0x09

typedef struct _GdtGate{
    uint16_t limit_low, base_low;
    uint8_t base_mid, access_right;
    uint8_t limit_high, base_high;
}GdtGate;

typedef struct _IdtGate{
    uint16_t offset_low, selector;
    uint8_t dw_cnt, access_right;
    uint16_t offset_high;
}IdtGate;  

typedef struct _Tss{
    uint32_t backlink;//0
    uint32_t esp0;//1
    uint32_t ss0;//2
    uint32_t esp1;//3
    uint32_t ss1;//4
    uint32_t esp2;//5
    uint32_t ss2;//6
    uint32_t cr3;//7
    uint32_t eip;//8
    uint32_t eflags;//9
    uint32_t eax;//10
    uint32_t ecx;//11
    uint32_t edx;//12
    uint32_t ebx;//13
    uint32_t esp;//14
    uint32_t ebp;//15
    uint32_t esi;//16
    uint32_t edi;//17
    uint32_t es;//18
    uint32_t cs;//19
    uint32_t ss;//20
    uint32_t ds;//21
    uint32_t fs;//22
    uint32_t gs;//23
    uint32_t ldtr;
    uint32_t io_map_base_addr;
}Tss;

typedef struct _Cache{
    uint32_t base_addr;
    uint32_t limit;
    bool bit32_mode;
}Cache;
