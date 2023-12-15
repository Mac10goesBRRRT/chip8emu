#include <SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#include "chip8IO.h"
#include "chip8.h"

void printDisp(Chip8* chip8);

int main (int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Number of Arguments wrong\n Use \"chip8 [path-to-rom]\"\n");
		return EXIT_FAILURE;
	}

	bool is_running = true;
	Chip8* chip8 = initChip8();

	if(loadRom(chip8, argv[1]) == 0)
		printf("ROM: %s successfully loaded\n", argv[1]);

	//Setting up SDL
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }
	
	chip8IO_t* chip8IO = initSDL(argv[1]);	
	module_t* mod = initMOD(chip8,chip8IO);

	chip8IO->device = initAudioOut();
	chip8IO->timerDelay = SDL_AddTimer(16, doTimers, mod);
	chip8IO->timerCPU = SDL_AddTimer(2, doCPU, chip8);
	chip8IO->timerDisplay = SDL_AddTimer(30,doDisplay, mod);
	
	SDL_Event event;

	while(is_running) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				is_running = false;
			}
			if(event.type == SDL_KEYDOWN){
				uint8_t hexKey = SDLK_to_hex(event.key.keysym.sym);
				chip8->keyboard[hexKey] = true;
				//fprintf(stdout, "KEYDOWN: 0x%x\n", hexKey);
			}
			if(event.type == SDL_KEYUP){
				uint8_t hexKey = SDLK_to_hex(event.key.keysym.sym);
				chip8->keyboard[hexKey] = false;
				//fprintf(stdout, "KEYUP: 0x%x\n", hexKey);
			}
		}
		if(!SDL_GetQueuedAudioSize(chip8IO->device)){
			float x = 0;
			for(int i = 0; i < BUZZFREQUENCY*4; i++){
				x += .010f;
				int16_t sample = sin(x*4)*5000;
				const int sample_size = sizeof(int16_t) * 1;
				SDL_QueueAudio(chip8IO->device, &sample, sample_size);
			}
		}
	}
	freeSDL(chip8IO);
	return EXIT_SUCCESS;
}