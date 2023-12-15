#ifndef CHIP_8IO_H
#define CHIP_8IO_H

#include <SDL_render.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define KEYNOTSET 0xFF
#define BUZZFREQUENCY 44100

typedef struct chip8IO {
    SDL_Window* window;
	SDL_Surface* surface;
    SDL_Renderer* renderer;
    SDL_Texture* screen;
	SDL_Event event;
	SDL_AudioDeviceID device;
	SDL_TimerID timerDelay;
	SDL_TimerID timerCPU;
	SDL_TimerID	timerDisplay;
} chip8IO_t;

typedef struct module{
	Chip8* chip8;
	chip8IO_t* graphics;
} module_t;

//Initializes the IO
chip8IO_t* initSDL(char* romName);
//Frees the IO
void freeSDL(chip8IO_t* chip8IO);
//Initialize the Audio Output
SDL_AudioDeviceID initAudioOut();
//Links chip and window in a struct
module_t* initMOD(Chip8* chip8, chip8IO_t* graphics);
//Key Conversion
int SDLK_to_hex(SDL_KeyCode key);
//loading ROMs
int loadRom(Chip8* chip8, char* file);
//Threads
Uint32 doTimers(Uint32 interval, module_t* mod);
Uint32 doCPU(Uint32 interval, Chip8* chip8);
Uint32 doDisplay(Uint32 interval, module_t* mod);

#endif