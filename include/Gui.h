#pragma once
#include "common.h"
class Vram;
class Kbd;
class Mouse;

class Gui:public Object{
    public:
        Gui(Vram* vram, Kbd* kbd, Mouse* mouse);
        ~Gui();
        void Display();
        void DrawPerPixel(int x, int y);
        void DrawPerPixel(int x, int y, Pixel pixel);
        void SetFont(unsigned char ascii_code, int row, int col);
        void SetFont(unsigned char ascii_code);
        void ResizeWindow(int w, int h);
        void UpdateSurface();
        void Update();
        unsigned char font_ascii[128][16];   
    private:    
        int mouse_x, mouse_y;
        Vram* vram;
        Kbd* kbd;
        Mouse* mouse;
        Pixel* vram_mem = NULL;
        int now_col;
        int now_row;
        void InitFontAscii();
        int SCREEN_WIDTH = WIDTH;
        int SCREEN_HEIGHT = HEIGHT;
        SDL_Window* window = NULL;
        SDL_Surface* screenSurface = NULL;
        SDL_Renderer *renderer = NULL;
        SDL_Texture *texture   = NULL;
        SDL_DisplayMode display_mode;
        Pixel* image = NULL;
        unsigned char SdlScancode2KeyCode(SDL_Event *e);
        void HideCursor();
        void ShowCursor();
        void HandleKeyDown(SDL_Event *e);
        void HandleKeyUp(SDL_Event *e);
        void HandleMouseMotion(SDL_Event *e);
        void HandleMouseButton(SDL_Event *e);
};

