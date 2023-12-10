#include "chip8.h"
#include "stack.h"


Chip8* initChip8(){
	Chip8* chip8 = (Chip8*)malloc(sizeof(Chip8));
	stack_t* stack = (stack_t*)malloc(sizeof(stack_t));
	//set mem
	for(int i = 0; i < MEMSIZE; i++ )
		chip8->mem[i]=0;
	return chip8;
}

void closeChip8(Chip8* chip8){
	free(chip8);
}

//1nnn - jump to address
void jumpPC(Chip8* chip8, uint16_t opcode){
	opcode = opcode & 0x0FFF;
	chip8->programCounter = opcode;
}
