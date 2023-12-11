#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stack.h"

#define MEMSIZE 4096
#define DISP_ROW 32
#define DISP_COL 64
#define DISP_ZOOM 16

typedef struct chip8{
	uint8_t mem[MEMSIZE];
	uint8_t reg[16]; //Register
	uint16_t indexRegister; //Memory-Adress-Storage
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint16_t programCounter;
	uint16_t stackPointer; //Points to top of stack
	stack_t stack;
	uint8_t display[DISP_ROW][DISP_COL];
	bool draw;
} Chip8;


Chip8* initChip8();

void closeChip8(Chip8* chip8);

#endif
