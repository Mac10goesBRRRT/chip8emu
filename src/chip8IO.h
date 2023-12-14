#ifndef CHIP_8IO_H
#define CHIP_8IO_H

#include <SDL_render.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define KEYNOTSET 0xFF;

typedef struct graphics {
    SDL_Window* window;
	SDL_Surface* surface;
    SDL_Renderer* renderer;
    SDL_Texture* screen;
	SDL_Event event;
} graphics_t;

typedef struct module{
	Chip8* chip8;
	graphics_t* graphics;
} module_t;

//Initializes the Window
graphics_t* initSDL(char* romName);
//Links chip and window in a struct
module_t* initMOD(Chip8* chip8, graphics_t* graphics);
//Key Conversion
int SDLK_to_hex(SDL_KeyCode key);

#endif