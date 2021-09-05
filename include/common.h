#pragma once
#include<iostream>
#include<vector>
#include<string>
#include <string.h>
#include<map>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <OpenGL/OpenGL.h>
#include <SDL2/SDL_opengl.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <cassert>
#include <stdarg.h>
#include <utility>
#include <condition_variable>
#include <cstdint>
#include <queue>
#include "Object.h"
using namespace std;

enum PREFIX_FLG_KIND {FLG_67, FLG_66, FLG_F0, FLG_F2, FLG_F3, FLG_2E, FLG_36, FLG_26, FLG_64, FLG_65, PREFIX_FLG_KIND_COUNT};
enum VRAM_MODE {X03, X12, X13};
enum VIDEO_DA_CONVERTER_MODE{VIDEO_DA_NOP, VIDEO_DA_WRITE, VIDEO_DA_READ};
enum DEVICE_KIND {VRAM, PIC, KBD, MOUSE, TIMER, DEVICE_KIND_CNT};
enum GENERAL_PURPOSE_REGISTER {EAX , ECX, EDX, EBX, ESP, EBP, ESI, EDI, GENERAL_PURPOSE_REGISTERS_COUNT};//汎用レジスタ
enum SEGMENT_REGISTER {ES, CS, SS, DS, FS, GS, SEGMENT_REGISTERS_COUNT};
enum CONTROL_REGISTER {CR0, CR1, CR2, CR3, CR4, CONTROL_REGISTERS_COUNT};

struct _Pixel{
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
}__attribute__((__packed__));

typedef struct _Pixel Pixel;

#define PRG_START_ADDR 0x7c00
#define PE 0x00000001
//#define MEM_SIZE 4294967296
#define MEM_SIZE 33554432
#define IPL_SIZE 512
#define INSTRUCTION_CNT 256

#define HEIGHT 200
#define WIDTH 320
#define VRAM_START_ADDR 0x000a0000