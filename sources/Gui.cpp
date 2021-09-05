#include "Vram.h"
#include "Kbd.h"
#include "Mouse.h"
#include "Gui.h"

static unsigned char font_SPACE[16]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//32(0x20)
static unsigned char font_PLUS[16]   = {0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xfe,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00};//43
static unsigned char font_COMMA[16]   = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x78,0x18,0x20,0x40,0x00};//44
static unsigned char font_DOT[16]    = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00};//46
static unsigned char font_LEFT_PAR[16] = {0x00,0x08,0x10,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x20,0x10,0x08,0x00,0x00};//46
static unsigned char font_RIGHT_PAR[16]    = {0x00,0x10,0x08,0x04,0x04,0x02,0x02,0x02,0x02,0x02,0x04,0x04,0x08,0x10,0x00,0x00};//46
static unsigned char font_EQUAL[16]  = {0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x7E,0x00,0x7E,0x7E,0x00,0x00,0x00,0x00,0x00};
static unsigned char font_0[16]      = {0x00,0x7f,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00};//48
static unsigned char font_1[16]      = {0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00};//49
static unsigned char font_2[16]      = {0x00,0x3c,0x42,0x41,0x41,0x01,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x7f,0x00,0x00};//50
static unsigned char font_3[16]      = {0x00,0x3c,0x42,0x01,0x01,0x01,0x01,0x3e,0x02,0x01,0x01,0x01,0x42,0x3c,0x00,0x00};//51
static unsigned char font_4[16]      = {0x00,0x10,0x20,0x48,0x88,0x88,0xff,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00};//52
static unsigned char font_5[16]      = {0x00,0x7f,0x40,0x40,0x40,0x40,0x7f,0x01,0x01,0x01,0x01,0x01,0x01,0x7e,0x00,0x00};//53
static unsigned char font_6[16]      = {0x00,0x1f,0x20,0x40,0x40,0x40,0x40,0x7f,0x41,0x41,0x41,0x41,0x41,0x7e,0x00,0x00};//54
static unsigned char font_7[16]      = {0x00,0x7f,0x41,0x41,0x41,0x41,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00};//55
static unsigned char font_8[16]      = {0x00,0x7f,0x41,0x41,0x41,0x41,0x7f,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,0x00};//56
static unsigned char font_9[16]      = {0x00,0x7f,0x41,0x41,0x41,0x41,0x7f,0x01,0x01,0x01,0x01,0x01,0x01,0x7f,0x00,0x00};//57
static unsigned char font_COLON[16]  = {0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00};//58
static unsigned char font_A[16]      = {0x00,0x18,0x18,0x18,0x18,0x24,0x24,0x24,0x24,0x7e,0x42,0x42,0x42,0xe7,0x00,0x00};//65に用意予定
static unsigned char font_B[16]      = {0x00,0x7e,0x41,0x41,0x41,0x42,0x7e,0x41,0x41,0x41,0x41,0x41,0x42,0x7e,0x00,0x00};//66に用意予定
static unsigned char font_C[16]      = {0x00,0x0c,0x12,0x21,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x21,0x12,0x0c,0x00,0x00};//67に用意予定
static unsigned char font_D[16]      = {0x00,0x78,0x44,0x42,0x41,0x41,0x41,0x41,0x41,0x41,0x42,0x42,0x44,0x78,0x00,0x00};//68
static unsigned char font_E[16]      = {0x00,0x7f,0x40,0x40,0x40,0x40,0x40,0x7f,0x40,0x40,0x40,0x40,0x40,0x7f,0x00,0x00};//69
static unsigned char font_F[16]      = {0x00,0x7f,0x40,0x40,0x40,0x40,0x7f,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00};//70
static unsigned char font_G[16]      = {0x00,0x0c,0x12,0x21,0x40,0x40,0x40,0x4f,0x41,0x41,0x41,0x21,0x13,0x0d,0x00,0x00};//71
static unsigned char font_H[16]      = {0x00,0x41,0x41,0x41,0x41,0x41,0x41,0x7f,0x41,0x41,0x41,0x41,0x41,0x41,0x00,0x00};//72
static unsigned char font_I[16]      = {0x00,0x7c,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7c,0x00,0x00};//73
static unsigned char font_J[16]      = {0x00,0xff,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x44,0x44,0x38,0x00,0x00};//74
static unsigned char font_K[16]      = {0x00,0x40,0x41,0x42,0x44,0x48,0x50,0x60,0x60,0x50,0x48,0x44,0x42,0x41,0x00,0x00};//75
static unsigned char font_L[16]      = {0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7f,0x00,0x00};//76
static unsigned char font_M[16]      = {0x00,0x42,0x66,0x66,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A,0x00,0x00};//77
static unsigned char font_N[16]      = {0x00,0x81,0xc1,0xa1,0xa1,0xa1,0x91,0x89,0x89,0x85,0x85,0x85,0x83,0x81,0x00,0x00};//78
static unsigned char font_O[16]      = {0x00,0x18,0x24,0x42,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x42,0x3c,0x00,0x00};//79
static unsigned char font_P[16]      = {0x00,0xf8,0x84,0x82,0x82,0x82,0x82,0xfc,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00};//80
static unsigned char font_Q[16]      = {0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x5A,0x66,0x66,0x24,0x26,0x1A,0x00,0x00};
static unsigned char font_R[16]      = {0x00,0xf8,0x84,0x82,0x82,0x82,0x82,0xfe,0xc0,0xa0,0x90,0x88,0x84,0x82,0x00,0x00};//82
static unsigned char font_S[16]      = {0x00,0x7e,0x41,0x41,0x41,0x41,0x40,0x7c,0x03,0x01,0x41,0x41,0x42,0x7c,0x00,0x00};//83
static unsigned char font_T[16]      = {0x00,0xfe,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00};
static unsigned char font_U[16]      = {0x00,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00};//85 
static unsigned char font_V[16]      = {0x00,0xfe,0x44,0x44,0x44,0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x10,0x10,0x00,0x00};//86
static unsigned char font_W[16]      = {0x00,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x66,0x66,0x00,0x00};//87
static unsigned char font_X[16]      = {0x00,0x82,0x82,0x64,0x68,0x38,0x38,0x38,0x38,0x38,0x3,0x6c,0xc6,0xc6,0xc6,0x00};
static unsigned char font_Y[16]      = {0x00,0x41,0x22,0x22,0x14,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00};//89
static unsigned char font_Z[16]      = {0x00,0x7E,0x02,0x02,0x04,0x04,0x08,0x08,0x10,0x10,0x20,0x20,0x40,0x7E,0x00,0x00};//89
static unsigned char font_HYPHEN[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//0x5f  ハイフン
static unsigned char font_a[16]      = {0x00,0x00,0x00,0x00,0x00,0x02,0x3A,0x42,0x42,0x42,0x46,0x46,0x46,0x39,0x00,0x00};//65に用意予定


Gui::Gui(Vram* vram, Kbd* kbd, Mouse* mouse){
    this->vram = vram;
    this->kbd  = kbd;
    this->mouse = mouse;
    assert(this->vram!=NULL);
    assert(this->kbd!=NULL);
    assert(this->mouse!=NULL);
    this->InitFontAscii();
    this->now_col = 0;
    this->now_row = 0;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        cerr << "SDLの初期化に失敗しました" << endl;
        exit(0);
    }

    this->window = SDL_CreateWindow("EMULATOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->SCREEN_WIDTH*2, this->SCREEN_HEIGHT*2, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(this->window==NULL){
        cout << SDL_GetError() << endl;
        this->Error("at Gui::Gui");
    }
    this->renderer      = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    //this->texture       = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, this->SCREEN_WIDTH, this->SCREEN_HEIGHT); 
    this->texture       = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, this->SCREEN_WIDTH, this->SCREEN_HEIGHT); 
    if(SDL_RenderSetLogicalSize(this->renderer, WIDTH, HEIGHT)<0){
        cout << SDL_GetError() << endl;
        this->Error("at Gui::Gui");
    }
    this->image = (Pixel*)malloc(this->SCREEN_WIDTH*this->SCREEN_HEIGHT*sizeof(Pixel));
}

Gui::~Gui(){
    //SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Gui::InitFontAscii(){
    int i;

    for(i=0; i<16; i++){
        this->font_ascii['\n'][i] = 0x00;
    }
    for(i=0; i<16; i++){
        this->font_ascii['('][i] = font_RIGHT_PAR[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[32][i] = font_SPACE[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[43][i] = font_PLUS[i];
    }
    for(i=0; i<16; i++){
        this->font_ascii[','][i] = font_COMMA[i];
    }
    for(i=0; i < 16; i++){
        font_ascii['='][i] = font_EQUAL[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[45][i] = font_HYPHEN[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[46][i] = font_DOT[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[48][i] = font_0[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[49][i] = font_1[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[50][i] = font_2[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[51][i] = font_3[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[52][i] = font_4[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[53][i] = font_5[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[54][i] = font_6[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[55][i] = font_7[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[56][i] = font_8[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[57][i] = font_9[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[58][i] = font_COLON[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[65][i] = font_A[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[66][i] = font_B[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[67][i] = font_C[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[68][i] = font_D[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[69][i] = font_E[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[70][i] = font_F[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[71][i] = font_G[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[72][i] = font_H[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[73][i] = font_I[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[74][i] = font_J[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[75][i] = font_K[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[76][i] = font_L[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[77][i] = font_M[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[78][i] = font_N[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[79][i] = font_O[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[80][i] = font_P[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii['Q'][i] = font_Q[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[82][i] = font_R[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[83][i] = font_S[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[84][i] = font_T[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[85][i] = font_U[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[86][i] = font_V[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii[87][i] = font_W[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii['X'][i] = font_X[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii['Y'][i] = font_Y[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii['Z'][i] = font_Y[i];
    }
    for(i=0; i < 16; i++){
        this->font_ascii['a'][i] = font_Y[i];
    }
}

void Gui::UpdateSurface(){
    SDL_UpdateTexture(this->texture, NULL, this->image, this->SCREEN_WIDTH * sizeof(Pixel));
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    SDL_RenderPresent(this->renderer);
}

void Gui::Update(){
    SDL_RenderPresent(this->renderer);
}

void Gui::HideCursor(){
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);//http://sdl2referencejp.osdn.jp/SDL_SetRelativeMouseMode.html
}

void Gui::ShowCursor(){
    SDL_ShowCursor(SDL_ENABLE);
}

void Gui::DrawPerPixel(int x, int y){
    if(x < 0 || y < 0 || x>=this->SCREEN_WIDTH || y>=this->SCREEN_HEIGHT){
        cerr << "範囲外の座標にアクセスしようとしてます" << endl;
        cerr << "x : " << x << " y : " << y << endl;
        exit(0);
    }
    unsigned int* vram = ((unsigned int*)this->screenSurface->pixels);
    vram = vram + this->screenSurface->w*y+x;
    *vram = SDL_MapRGB(this->screenSurface->format, 0xFF, 0xFF, 0xFF);
    //SDL_RenderClear(this->renderer);
    //SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 255);
    //SDL_RenderDrawPoint(this->renderer, x, y);
}

void Gui::SetFont(unsigned char ascii_code){
    int x, y;
    unsigned char data;
    x = 8*this->now_col;
    y = 16*this->now_row;
    this->now_col++;
    if(ascii_code=='\n'){
        this->now_col = 0;
        this->now_row++;
        return;
    }
    if(x < 0 || y < 0 || x>=this->SCREEN_WIDTH || y>=this->SCREEN_HEIGHT){
        cerr << "範囲外の座標にアクセスしようとしてます" << endl;
        cerr << "x : " << x << " y : " << y << endl;
        exit(1);
    }

    for(int i=0; i < 16; i++){
        data = this->font_ascii[ascii_code][i];
		if((data & 0x80) != 0){
            this->DrawPerPixel(x+0, y+i);       
		}
		if((data & 0x40) != 0){
            this->DrawPerPixel(x+1, y+i);       
		}
		if((data & 0x20) != 0){
            this->DrawPerPixel(x+2, y+i);        
		}
		if((data & 0x10) != 0){
            this->DrawPerPixel(x+3, y+i);       
		}
		if((data & 0x08) != 0){
            this->DrawPerPixel(x+4, y+i);        
		}
		if((data & 0x04) != 0){
            this->DrawPerPixel(x+5, y+i);       
		}
		if((data & 0x02) != 0){
            this->DrawPerPixel(x+6, y+i);       
		}
		if((data & 0x01) != 0){
            this->DrawPerPixel(x+7, y+i);       
		}
    }
    SDL_UpdateWindowSurface(this->window);
}

void Gui::DrawPerPixel(int x, int y, Pixel pixel){
    /***
    if(x < 0 || y < 0 || x>=this->SCREEN_WIDTH || y>=this->SCREEN_HEIGHT){
        cerr << "範囲外の座標にアクセスしようとしてます" << endl;
        cerr << "x : " << x << " y : " << y << endl;
        exit(0);
    }
    ***/
    unsigned int* vram = ((unsigned int*)this->screenSurface->pixels);
    vram = vram + this->screenSurface->w*y+x;
    *vram = SDL_MapRGB(this->screenSurface->format, pixel.r, pixel.g, pixel.b);
    //*vram = SDL_MapRGB(this->screenSurface->format, 0xFF, 0xFb, 0x00);
    //SDL_RenderClear(this->renderer);
    //SDL_SetRenderDrawColor(this->renderer, pixel.r, pixel.g, pixel.b, 255);
    //SDL_RenderDrawPoint(this->renderer, x, y);
}

void Gui::Display(){
    SDL_Event e;
    bool quit = false;
    Pixel pixel;
    unsigned int key_code;
    //this->HideCursor();
    unsigned int start;
    unsigned int end;

    while (!quit){
        start = SDL_GetTicks();
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if(e.type==SDL_KEYDOWN){
                this->HandleKeyDown(&e);
                break;
            }
            if(e.type==SDL_KEYUP){
                this->HandleKeyUp(&e);
                break;
            }
            if(e.type==SDL_MOUSEMOTION){
                if(!this->mouse->IsEnable()){
                    break;
                }
                this->HandleMouseMotion(&e);
                break;
            }
            if(e.type==SDL_MOUSEBUTTONDOWN){
                if(!this->mouse->IsEnable()){
                    break;
                }
                this->HideCursor();
                this->HandleMouseButton(&e);
            }
        }
        for(int y=0; y<this->SCREEN_HEIGHT; y++){
            for(int x=0; x<this->SCREEN_WIDTH; x++){
                pixel = this->vram->GetPixel(x, y);
                this->image[x+y*this->SCREEN_WIDTH] = pixel;
            }
        }
        end = SDL_GetTicks();
        end = end - start;
        if(16>end){
            SDL_Delay(16-end);
        }   
        this->UpdateSurface();
    }
}

unsigned char Gui::SdlScancode2KeyCode(SDL_Event *e){
    unsigned char key_code;
    switch (e->key.keysym.sym){
        case SDLK_0:
            key_code = 0x0B;
            break;
        case SDLK_1:
            key_code = 0x02;
            break;
        case SDLK_2:
            key_code = 0x03;
            break;
        case SDLK_3:
            key_code = 0x04;
            break;
        case SDLK_4:
            key_code = 0x05;
            break;
        case SDLK_5:
            key_code = 0x06;
            break;
        case SDLK_6:
            key_code = 0x07;
            break;
        case SDLK_7:
            key_code = 0x08;
            break;
        case SDLK_8:
            key_code = 0x09;
            break;
        case SDLK_9:
            key_code = 0x0A;
            break;
        case SDLK_a:
            key_code = 0x1E;
            break;
        case SDLK_b:
            key_code = 0x30;
            break;
        case SDLK_c:
            key_code = 0x2E;
            break;
        case SDLK_d:
            key_code = 0x20;
            break;
        case SDLK_e:
            key_code = 0x12;
            break;
        case SDLK_f:
            key_code = 0x21;
            break;
        case SDLK_s:
            key_code = 0x1F;
            break;
        case SDLK_g:
            key_code = 0x22;
            break;
        case SDLK_q:
            key_code = 0x10;
            break;
        case SDLK_h:
            key_code = 0x23;
            break;
        case SDLK_j:
            key_code = 0x24;
            break;
        case SDLK_k:
            key_code = 0x25;
            break;
        case SDLK_l:
            key_code = 0x26;
            break;
        case SDLK_m:
            key_code = 0x32;
            break;
        case SDLK_n:
            key_code = 0x31;
            break;
        case SDLK_o:
            key_code = 0x18;
            break;
        case SDLK_p:
            key_code = 0x19;
            break;
        case SDLK_r:
            key_code = 0x13;
            break;
        case SDLK_t:
            key_code = 0x14;
            break;
        case SDLK_w:
            key_code = 0x11;
            break;
        case SDLK_x:
            key_code = 0x2D;
            break;
        case SDLK_z:
            key_code = 0x2C;
            break;
        case SDLK_y:
            key_code = 0x15;
            break;
        case SDLK_u:
            key_code = 0x16;
            break;
        case SDLK_i:
            key_code = 0x17;
            break;
        case SDLK_v:
            key_code = 0x2F;
            break;
        case SDLK_SEMICOLON:
            key_code = 0x27;
            break;
        case SDLK_COLON:
            key_code = 0x28;
            break;
        case SDLK_SPACE:
            key_code = 0x39;
            break;
        case SDLK_RETURN:
            key_code = 0x1C;
            break;
        case SDLK_PERIOD:
            key_code = 0x34;
            break;
        case SDLK_COMMA:
            key_code = 0x33;
            break;
        case SDLK_LEFTBRACKET:
            key_code = 0x1B;
            break;
        case SDLK_RIGHTBRACKET:
            key_code = 0x2B;
            break;
        case SDLK_BACKSPACE:
            key_code = 0x0E;
            break;
        case SDLK_LSHIFT:
            key_code = 0x2A;
            break;
        case SDLK_RSHIFT:
            key_code = 0x36;
            break;
        case SDLK_TAB:
            key_code = 0x0F;
            break;
        default:
            this->Error("SDL_Keycode = %08X(http://sdl2referencejp.osdn.jp/SDLKeycodeLookup.html) is not implemented at Gui::HandleKeyDown", e->key.keysym.sym, SDLK_8);
    }
    return key_code;
}
char keytable[0x54] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};

void Gui::HandleKeyDown(SDL_Event *e){
    /***
    if(e->key.repeat){
        return;
    }
    ***/
    SDL_Keymod mod = SDL_GetModState();

    unsigned char key_code;
    key_code = this->SdlScancode2KeyCode(e);
    this->kbd->Receive(key_code);
    /***
    if(mod&KMOD_LSHIFT){//KMOD_LSHIFT = 1
        if(key_code<0x54){
            fprintf(stderr, "LSHIFT + %c\n", keytable[key_code]);
        }
        //this->Error("LSHIFT is pressed at Gui::HandleKeyDown");
    }
    if(mod&KMOD_RSHIFT){
        if(key_code<0x54){
            fprintf(stderr, "RSHIFT + %c\n", keytable[key_code]);
        }
        //this->Error("LSHIFT is pressed at Gui::HandleKeyDown");
    }
    ***/
    //fprintf(stderr, "RSHIFT + %c\n", keytable[key_code]);
}

void Gui::HandleKeyUp(SDL_Event *e){
    /***
    if(e->key.repeat){
        return;
    }
    ***/
    unsigned char key_code;
    key_code = 0x80 | this->SdlScancode2KeyCode(e);
    this->kbd->Receive(key_code);
}

void Gui::HandleMouseMotion(SDL_Event *e){
    this->mouse_x = e->motion.x;
    this->mouse_y = e->motion.y;
    unsigned char data0, data1, data2;
    int rel_x, rel_y;
    
    rel_x = e->motion.xrel / 5;
    rel_y = e->motion.yrel / 5;
    if(rel_x>127){
        rel_x = 127;
    }else if(rel_x < -127){
        rel_x = -127;
    }
    if(rel_y>127){
        rel_y = 127;
    }else if(rel_y < -127){
        rel_y = -127;
    }

    rel_y *= -1;
    data0 = 0x08;
    if(rel_x < 0){
        data0 = data0 | 0x10;
    }
    if(rel_y < 0){
        data0 = data0 | 0x20;
    }
    this->mouse->Receive(data0);
    this_thread::sleep_for(std::chrono::milliseconds(20));
    this->mouse->Receive((unsigned char)rel_x);
    this_thread::sleep_for(std::chrono::milliseconds(20));
    this->mouse->Receive((unsigned char)rel_y);
    this_thread::sleep_for(std::chrono::milliseconds(20));

}

void Gui::HandleMouseButton(SDL_Event *e){
    unsigned char data0, data1, data2;
    int rel_x, rel_y;
    
    rel_x = (e->motion.x-this->mouse_x);
    rel_y = (e->motion.y-this->mouse_y);
    if(rel_x>127){
        rel_x = 127;
    }else if(rel_x < -127){
        rel_x = -127;
    }
    if(rel_y>127){
        rel_y = 127;
    }else if(rel_y < -127){
        rel_y = -127;
    }

    rel_y *= -1;
    data0 = 0x09;
    if(rel_x < 0){
        data0 = data0 | 0x10;
    }
    if(rel_y < 0){
        data0 = data0 | 0x20;
    }
    this->mouse->Receive(data0);
    this_thread::sleep_for(std::chrono::milliseconds(10));
    this->mouse->Receive((unsigned char)rel_x);
    this_thread::sleep_for(std::chrono::milliseconds(10));
    this->mouse->Receive((unsigned char)rel_y);
    this_thread::sleep_for(std::chrono::milliseconds(10));
    //this->Error("Not implemented at Gui::HandleMouseButton");
}