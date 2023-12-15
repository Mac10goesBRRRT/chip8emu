#include "chip8IO.h"

chip8IO_t* initSDL(char* romName){
    chip8IO_t* gra = malloc(sizeof(chip8IO_t));
    char taskbar[90];
    sprintf(taskbar, "chip8EMU - %s", romName);
    SDL_Window* window = SDL_CreateWindow(taskbar, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if(window == NULL) {
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
    SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 5, 25, 38, 255);
	SDL_RenderClear(renderer);
	SDL_RenderSetLogicalSize(renderer, DISP_COL*DISP_ZOOM, DISP_ROW*DISP_ZOOM);
	SDL_SetRenderDrawColor(renderer, 5, 25, 38, 255);
    SDL_Texture* screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISP_COL, DISP_ROW);
	SDL_RenderPresent(renderer);

    SDL_Event event;

    gra->surface = surface;
    gra->screen = screen;
    gra->renderer = renderer;
    gra->window = window;
    gra->event = event;
    
    return gra;
}

module_t* initMOD(Chip8* chip8, chip8IO_t* graphics){
    module_t* module = malloc(sizeof(module_t));
    module->chip8 = chip8;
    module->graphics = graphics;
    return module;
}

SDL_AudioDeviceID initAudioOut(){
	SDL_AudioSpec buzz;
	SDL_memset(&buzz, 0, sizeof(buzz));
	buzz.freq = 44100;
	buzz.format = AUDIO_S16SYS;
	buzz.channels = 1;
	buzz.samples = 1024;
	buzz.callback = NULL;
	return SDL_OpenAudioDevice(NULL, 0, &buzz, NULL, 0);
}


Uint32 doTimers(Uint32 interval, module_t* mod){
	Chip8* chip8 = mod->chip8;
	SDL_AudioDeviceID dev = mod->graphics->device;
	if(chip8->delayTimer > 0)
		chip8->delayTimer -= 1;
	if(chip8->soundTimer > 0){
		SDL_PauseAudioDevice(dev, 0);
		chip8->soundTimer -= 1;
	} else {
		SDL_PauseAudioDevice(dev, 1);
	}
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
	return 20;
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

bool SDLK_pressed(Chip8* chip8){
	for(int i = 0; i < 0x10; i++){
		if(chip8->keyboard[i] == true){
			return true;
		}
	}
	return false;
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

void freeSDL(chip8IO_t* chip8IO){
	SDL_RemoveTimer(chip8IO->timerDelay);
	SDL_RemoveTimer(chip8IO->timerCPU);
	SDL_RemoveTimer(chip8IO->timerDisplay);
	SDL_DestroyTexture(chip8IO->screen);
	SDL_DestroyRenderer(chip8IO->renderer);
	SDL_DestroyWindow(chip8IO->window);
	SDL_Quit();
}