#include "chip8IO.h"

graphics_t* initSDL(char* romName){
    graphics_t* gra = malloc(sizeof(graphics_t));
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

module_t* initMOD(Chip8* chip8, graphics_t* graphics){
    module_t* module = malloc(sizeof(module_t));
    module->chip8 = chip8;
    module->graphics = graphics;
    return module;
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