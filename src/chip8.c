#include "chip8.h"
#include "stack.h"


Chip8* initChip8(){
	Chip8* chip8 = (Chip8*)malloc(sizeof(Chip8));
	stack_t* stack = (stack_t*)malloc(sizeof(stack_t));
	//set mem
	chip8->programCounter = 0x200;
	for(int i = 0; i < MEMSIZE; i++ )
		chip8->mem[i]=0;
	chip8->draw = false;
	return chip8;
}

void initSprite(uint8_t* mem){
	//Each byte may be up to 8x15 in size
	//0
	mem[0] = 0xF0;
	mem[1] = 0x90;
	mem[2] = 0x90;
	mem[3] = 0x90;
	mem[4] = 0xF0;
	//1
	mem[16] = 0x20;
	//2
	mem[32] = 0xF0;
	//3
	mem[48] = 0xF0;
	//4
	mem[64] = 0x90;
	//5
	mem[80] = 0xF0;
	//6
	mem[96] = 0xF0;
	//7
	mem[112] = 0xF0;
	//8
	mem[128] = 0xF0;
	//9
	mem[144] = 0xF0;
	//A
	mem[160] = 0xF0;
	//B
	mem[176] = 0xE0;
	//C
	mem[192] = 0xF0;
	//D
	mem[208] = 0xE0;
	//E
	mem[224] = 0xF0;
	//F
	mem[240] = 0xF0;
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
	chip8->reg[addr] = value;
}

//Annn Load index Register with imm. Value
void loadIndexRegister(Chip8* chip8, uint16_t opcode){
	uint16_t value = opcode & 0x0FFF;
	chip8->indexRegister = value;
}

//Dxyn Display n-byte sprite starting at memory location I at
void drawSprite(Chip8* chip8, uint16_t opcode){
	chip8->draw = true;
	uint16_t x = (opcode & 0x0F00)>>8;
	uint16_t y = (opcode & 0x00F0)>>4;
	uint16_t n = opcode & 0x000F;
	for(int yVal = 0; yVal < n; yVal++){
		uint8_t line = chip8->mem[chip8->indexRegister + yVal];
		for(int xVal = 0; xVal < 8; xVal++){
			chip8->display[(chip8->reg[y])+yVal][(chip8->reg[x])+xVal] = (line & 0x80)>>7;
			line = line << 1;
		}
	}
}

//1nnn - jump to address
void jumpPC(Chip8* chip8, uint16_t opcode){
	opcode = opcode & 0x0FFF;
	chip8->programCounter = opcode;
}

int emulate(Chip8* chip8){
	uint16_t opcode = (chip8->mem[chip8->programCounter] <<8) | (chip8->mem[chip8->programCounter+1]);
	printf("Opcode: 0x%x, PC: 0x%x", opcode, chip8->programCounter);
	chip8->programCounter += 2;
	switch (opcode & 0xF000){
		case 0x0000:
			switch(opcode & 0x0FFF){
				case 0x00E0: clearScreen(chip8); break;
			}
			break;
		case 0x1000:
			jumpPC(chip8, opcode); break;
		case 0x6000:
			loadRegisterIm(chip8, opcode); break;
		case 0xA000:
			loadIndexRegister(chip8, opcode); break;
		case 0xD000:
			drawSprite(chip8, opcode); break;
		default: sprintf(stderr, "Failure on 0x%x", opcode); return EXIT_FAILURE;
	}
	//usleep(5);
	return EXIT_SUCCESS;
}
