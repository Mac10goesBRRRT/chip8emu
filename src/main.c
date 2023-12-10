#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>


#include "chip8.h"

int main (int argv, char** argc){
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	bool is_running = true;
	SDL_Event event;

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

	//TODO: delayTimer and soundTimer get lower by 1 at 60Hz
	Chip8* chip8 = initChip8();

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
