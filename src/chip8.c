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

//00E0 clear display
void clearScreen(Chip8* chip8){
	memset(chip8->display,0,sizeof(uint8_t)*DISP_ROW*DISP_COL);
}

//6xnn Load Register Immediate
void loadRegisterIm(Chip8* chip8, uint16_t opcode){
	uint8_t addr = (opcode & 0X0F00)>>8;
	uint16_t value = opcode & 0x00FF;
	chip8->V[addr] = value;
}

//Annn Load index Register with imm. Value
void loadIndexRegister(Chip8* chip8, uint16_t opcode){
	uint16_t value = opcode & 0x0FFF;
	chip8->I = value;
}

//Dxyn Display n-byte sprite starting at memory location I at
void drawSprite(Chip8* chip8, uint16_t opcode){
	uint8_t n = opcode & 0x000F;
	uint8_t x = (opcode & 0x0F00)>>8;
	uint8_t y = (opcode & 0x00F0)>>4;
}

//1nnn - jump to address
void jumpPC(Chip8* chip8, uint16_t opcode){
	opcode = opcode & 0x0FFF;
	chip8->programCounter = opcode;
}
