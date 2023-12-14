#include <SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "chip8IO.h"
#include "chip8.h"

int loadRom(Chip8* chip, char* file);
int SDLK_to_hex(SDL_KeyCode key);
void printDisp(Chip8* chip8);

Uint32 doTimers(Uint32 interval, Chip8* chip8);
Uint32 doCPU(Uint32 interval, Chip8* chip8);
Uint32 doDisplay(Uint32 interval, module_t* mod);

int main (int argc, char** argv){
	char taskbar[90];
	bool is_running = true;
	//int clockperiod = 1000/CLOCK_HZ; Might be used later
	Chip8* chip8 = initChip8();
	char romName[70] = "../rom/6-keypad.ch8";
	if((loadRom(chip8, romName)) == 0)
		printf("ROM: %s successfully loaded\n", romName);
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not init sdl2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	graphics_t* gra = initSDL(romName);
	//Setting up Timers
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }
	
	module_t* mod = initMOD(chip8,gra);

	SDL_TimerID timerTime = SDL_AddTimer(16, doTimers, (void*) chip8);
	SDL_TimerID timerCPU = SDL_AddTimer(2, doCPU, chip8);
	SDL_TimerID timerDisplay = SDL_AddTimer(30,doDisplay, mod);
	
	SDL_Event event;

	while(is_running) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				is_running = false;
			}
			if(event.type == SDL_KEYDOWN){
				uint8_t hexKey = SDLK_to_hex(event.key.keysym.sym);
				chip8->keyboard[hexKey] = true;
				fprintf(stdout, "KEYDOWN: 0x%x\n", hexKey);
			}
			if(event.type == SDL_KEYUP){
				uint8_t hexKey = SDLK_to_hex(event.key.keysym.sym);
				chip8->keyboard[hexKey] = false;
				fprintf(stdout, "KEYUP: 0x%x\n", hexKey);
			}
		}
	}
	SDL_RemoveTimer(timerTime);
	SDL_RemoveTimer(timerCPU);
	SDL_RemoveTimer(timerDisplay);
	SDL_DestroyWindow(gra->window);
	SDL_Quit();
	return EXIT_SUCCESS;
}

int loadRom(Chip8* chip8, char* file){
	FILE* fp = fopen(file, "rb");
	if(fp == NULL){
		fprintf(stderr, "Cant open the file: %s\n", file);
		return EXIT_FAILURE;
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(size > (4096-0x200)){
		fprintf(stderr, "ROM Size of: %dBytes to big for CHIP8 Mem of %d", size, MEMSIZE);
		return EXIT_FAILURE;
	}
	fread((chip8->mem)+0X200, sizeof(uint8_t), size, fp);
	//Just for ROM 5:
	//chip8->mem[0x1FF]  = 0x1;
	return EXIT_SUCCESS;
}


Uint32 doTimers(Uint32 interval, Chip8* chip8){
	if(chip8->delayTimer > 0)
		chip8->delayTimer -= 1;
	if(chip8->soundTimer > 0)
		chip8->soundTimer -= 1;
	return 16;
}

Uint32 doCPU(Uint32 interval, Chip8* chip8){
	emulate(chip8);
	return 2;
}

Uint32 doDisplay(Uint32 interval, module_t* mod){
	SDL_Renderer* renderer = mod->graphics->renderer;
	Chip8* chip8 = mod->chip8;
	SDL_Texture* screen = mod->graphics->screen;
	uint32_t pixels[DISP_ROW][DISP_COL];
	if (chip8->draw){
		chip8->draw = false;
		for(int y = 0; y < DISP_ROW; y++){
			for(int x = 0; x < DISP_COL; x++){
				if(chip8->display[y][x] == 1)
					pixels[y][x] = 0xFFA50FF;
				else
					pixels[y][x] = 0x051926FF;
				}
			}
		SDL_UpdateTexture(screen, NULL, pixels, DISP_COL * sizeof(uint32_t));
		SDL_Rect rectangle;
		rectangle.x = 0;
		rectangle.y = 0;
		rectangle.w = DISP_COL * DISP_ZOOM;
		rectangle.h = DISP_ROW * DISP_ZOOM;
		SDL_RenderCopy(renderer, screen, NULL, &rectangle);
		SDL_RenderPresent(renderer);
	}
	return 16;
}

void printDisp(Chip8* chip8){
	//Getting the Display Output
	for(int y = 0; y < DISP_ROW; y++){
		for(int x = 0; x < DISP_COL; x++){
			if((chip8->display[y][x])==1)
				printf("#");
			else
				printf(" ");
		}
		printf("\n");
	}
}