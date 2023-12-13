#include <SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>


#include "chip8.h"

int loadRom(Chip8* chip, char* file);

int main (int argc, char** argv){
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	bool is_running = true;
	SDL_Event event;
	int clockperiod = 1000/CLOCK_HZ;
	clock_t start_time, end_time;
    double elapsed_time;
	Chip8* chip8 = initChip8();
	char romName[70] = "../rom/4-flags.ch8";
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
	while(is_running) {
		start_time = clock();
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				is_running = false;
			}
		}
		if(cycle % 4 == 0){
			decrementCounters(chip8);
		}
		emulate(chip8);
		//printf(" %d\n", cycle);
		cycle++;
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
		end_time = clock();
		elapsed_time =((double) (end_time-start_time))/CLOCKS_PER_SEC*1000;
		if(elapsed_time>2)
			printf("could not keep up\n");
		double time = (clockperiod-elapsed_time)<0 ? 0 : clockperiod;
		//printf("Elapsed time: %.2f milliseconds\n", clockperiod-elapsed_time);
		SDL_Delay(time);
	}
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
	return EXIT_SUCCESS;
}
