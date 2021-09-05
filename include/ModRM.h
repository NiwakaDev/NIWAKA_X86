#pragma once
typedef struct _ModRM{
    unsigned char mod;
    union{
        unsigned char op_code;
        unsigned char reg_index;
    };
    unsigned char rm;
    unsigned char sib;
    union{
        char disp8;
        unsigned short disp16;
        unsigned int disp32;
    };
}ModRM;