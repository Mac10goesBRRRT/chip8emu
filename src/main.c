#include <SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>


#include "chip8.h"

#define KEYNOTSET 0xFF;

typedef struct module{
	Chip8* chip8;
	SDL_Renderer* renderer;
	SDL_Texture* screen;
} module_t;

int loadRom(Chip8* chip, char* file);
int SDLK_to_hex(SDL_KeyCode key);

Uint32 doTimers(Uint32 interval, Chip8* chip8);
Uint32 doCPU(Uint32 interval, Chip8* chip8);
Uint32 doDisplay(Uint32 interval, module_t* mod);

int main (int argc, char** argv){
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	bool is_running = true;
	SDL_Event event;
	int clockperiod = 1000/CLOCK_HZ;
	clock_t start_time, end_time;
    double elapsed_time;
	Chip8* chip8 = initChip8();
	char romName[70] = "../rom/5-quirks.ch8";
	if((loadRom(chip8, romName))==0)
		printf("ROM: %s successfully loaded\n", romName);


	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not init sdl2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	char taskbar[90];
	sprintf(taskbar, "chip8EMU - %s", romName);
	window = SDL_CreateWindow(taskbar, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

	surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
	SDL_UpdateWindowSurface(window);
	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 5, 25, 38, 255);
	SDL_RenderClear(renderer);
	SDL_RenderSetLogicalSize(renderer, DISP_COL*DISP_ZOOM, DISP_ROW*DISP_ZOOM);
	SDL_SetRenderDrawColor(renderer, 5, 25, 38, 255);
	SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISP_COL, DISP_ROW);
	SDL_RenderPresent(renderer);
	long cycle = 0; //this timer will count to 4 wich equals 62.5Hz on a 500Hz system
	
	
	module_t* mod = (module_t*)malloc(sizeof(module_t));
	mod->renderer = renderer;
	mod->chip8 = chip8;
	mod->screen = screen;

	SDL_TimerID timerTime = SDL_AddTimer(16, doTimers, (void*) chip8);
	SDL_TimerID timerCPU = SDL_AddTimer(2, doCPU, chip8);
	SDL_TimerID timerDisplay = SDL_AddTimer(30,doDisplay, mod);
	
	while(is_running) {
		start_time = clock();
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
		//SDL_Delay(2);
	}
	SDL_RemoveTimer(timerTime);
	SDL_RemoveTimer(timerCPU);
	SDL_RemoveTimer(timerDisplay);
	SDL_DestroyWindow(window);
	SDL_Quit();
	/* Getting the Display Output
	for(int y = 0; y < DISP_ROW; y++){
		for(int x = 0; x < DISP_COL; x++){
			if((chip8->display[y][x])==1)
				printf("#");
			else
				printf(" ");
		}
		printf("\n");
	}
	*/
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
	chip8->mem[0x1FF]  = 0x1;
	return EXIT_SUCCESS;
}


int SDLK_to_hex(SDL_KeyCode key){
	switch (key){
		case SDLK_1: return 0x1; break;
		case SDLK_2: return 0x2; break;
		case SDLK_3: return 0x3; break;
		case SDLK_4: return 0xC; break;

		case SDLK_q: return 0x4; break;
		case SDLK_w: return 0x5; break;
		case SDLK_e: return 0x6; break;
		case SDLK_r: return 0xD; break;

		case SDLK_a: return 0x7; break;
		case SDLK_s: return 0x8; break;
		case SDLK_d: return 0x9; break;
		case SDLK_f: return 0xE; break;

		case SDLK_y: return 0xA; break;
		case SDLK_x: return 0x0; break;
		case SDLK_c: return 0xB; break;
		case SDLK_v: return 0xF; break;
	default: return KEYNOTSET; break;
	}
}

Uint32 doTimers(Uint32 interval, Chip8* chip8){
	//fprintf(stdout, "Timers Called\n");
	if(chip8->delayTimer > 0)
		chip8->delayTimer -= 1;
	if(chip8->soundTimer > 0)
		chip8->soundTimer -= 1;
	return 16;
}

Uint32 doCPU(Uint32 interval, Chip8* chip8){
	emulate(chip8);
	//fprintf(stdout, "CPU Called\n");
	return 2;
}

Uint32 doDisplay(Uint32 interval, module_t* mod){
	SDL_Renderer* renderer = mod->renderer;
	Chip8* chip8 = mod->chip8;
	SDL_Texture* screen = mod->screen;
	uint32_t pixels[DISP_ROW][DISP_COL];
	if (chip8->draw){
		chip8->draw = false;
		//memset(pixels, 0x051926FF, DISP_COL*DISP_ROW*sizeof(uint32_t));
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
	return 30;
}