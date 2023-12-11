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

	Chip8* chip8 = initChip8();
	char romName[80] = "../rom/1-chip8-logo.ch8";
	if((loadRom(chip8, romName))==0)
		printf("ROM: %s successfully loaded", romName);


	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not init sdl2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	window = SDL_CreateWindow("chip8EMU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
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
	SDL_RenderPresent(renderer);

	//TODO: delayTimer and soundTimer get lower by 1 at 60Hz
		while(is_running) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				is_running = false;
			}
		}
		SDL_Delay(100);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}

int loadRom(Chip8* chip8, char* file){
	FILE* fp = fopen(file, "rb");
	if(fp == NULL){
		fprintf(stderr, "Cant open the file: %s", file);
		return EXIT_SUCCESS;
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fread((chip8->mem)+0X200, sizeof(uint8_t), size, fp);
	return EXIT_SUCCESS;
}
