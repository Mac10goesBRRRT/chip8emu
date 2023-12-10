#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define MEMSIZE 4096

typedef struct chip8{
	uint8_t mem[MEMSIZE];
	uint8_t V[16];
	uint16_t I;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint16_t programCounter;
	uint16_t stackPointer; //Points to top of stack
	stack_t stack;
} Chip8;


Chip8* initChip8();

void closeChip8(Chip8* chip8);

#endif
