#include "chip8.h"

void initSprite(uint8_t* mem);

Chip8* initChip8(){
	Chip8* chip8 = (Chip8*)malloc(sizeof(Chip8));
	chip8->stackPointer = 0;
	memset(chip8->stack, 0, sizeof(uint16_t)*STACKSIZE);
	chip8->programCounter = 0x200;
	chip8->delayTimer = 0;
	chip8->soundTimer = 0;
	initSprite(chip8->mem);
	for(int i = 0; i < MEMSIZE; i++ )
		chip8->mem[i]=0;
	chip8->draw = false;
	srand(time(NULL)); // For RNG
	return chip8;
}

//This will need work
void initSprite(uint8_t* mem){
	//Each byte may be up to 8x15 in size
	int sprite[] = {
	/*0*/ 0xF0, 0x90, 0x90, 0x90, 0xF0,
	/*1*/ 0x20, 0x60, 0x20, 0x20, 0x70,
	/*2*/ 0xF0, 0x10, 0xF0, 0x80, 0xF0,
	/*3*/ 0xF0, 0x10, 0xF0, 0x10, 0xF0,
	/*4*/ 0x90, 0x90, 0xF0, 0x10, 0x10,
	/*5*/ 0xF0, 0x80, 0xF0, 0x10, 0xF0,
	/*6*/ 0xF0, 0x80, 0xF0, 0x90, 0xF0,
	/*7*/ 0xF0, 0x10, 0x20, 0x40, 0x40,
	/*8*/ 0xF0, 0x90, 0xF0, 0x90, 0xF0,
	/*9*/ 0xF0, 0x90, 0xF0, 0x10, 0xF0,
	/*A*/ 0xF0, 0x90, 0xF0, 0x90, 0x90,
	/*B*/ 0xE0, 0x90, 0xE0, 0x90, 0xE0,
	/*C*/ 0xF0, 0x80, 0x80, 0x80, 0xF0,
	/*D*/ 0xE0, 0x90, 0x90, 0x90, 0xE0,
	/*E*/ 0xF0, 0x80, 0xF0, 0x80, 0xF0,
	/*F*/ 0xF0, 0x80, 0xF0, 0x80, 0x80
	};
	memcpy(mem+0x50,sprite,sizeof(sprite));
}


void closeChip8(Chip8* chip8){
	free(chip8);
}

//00E0 clear display
void clearScreen(Chip8* chip8){
	memset(chip8->display,0,sizeof(uint8_t)*DISP_ROW*DISP_COL);
}

//00EE - RET return from subroutine
void returnFunc(Chip8* chip8){
	if(chip8->stackPointer > 0){
		chip8->programCounter = chip8->stack[chip8->stackPointer];
		chip8->stackPointer -= 1;
	} else {
		fprintf(stderr, "Stackunderflow\n");
		return EXIT_FAILURE;
	}
}

//1nnn - jump to address
void jumpPC(Chip8* chip8, uint16_t opcode){
	opcode = opcode & 0x0FFF;
	chip8->programCounter = opcode;
}

//2nnn - CALL addr Call subroutine at nnn.
void callFunc(Chip8* chip8, uint16_t opcode){
	opcode &= 0x0FFF;
	if(chip8->stackPointer < STACKSIZE){
		chip8->stackPointer += 1;
		chip8->stack[chip8->stackPointer] = chip8->programCounter;
		chip8->programCounter = opcode;
	} else {
		fprintf(stderr, "Stackoverflow\n");
		return EXIT_FAILURE;
	}

}

//3xnn - Skip next instruction if Vx = kk.
void isEqual(Chip8* chip8, uint16_t opcode){
	uint8_t addr = (opcode & 0x0F00)>>8;
	uint16_t value = opcode & 0x00FF;
	if(chip8->reg[addr] == value)
		chip8->programCounter += 2;
}

//4xnn - Skip next instruction if Vx != kk.
void isNotEqual(Chip8* chip8, uint16_t opcode){
	uint8_t addr = (opcode & 0x0F00)>>8;
	uint16_t value = opcode & 0x00FF;
	if(chip8->reg[addr] != value)
		chip8->programCounter += 2;
}

//5xy0 - SE Vx, Vy Skip next instruction if Vx == Vy.
void regEqual(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	if(chip8->reg[addrX] == chip8->reg[addrY])
		chip8->programCounter += 2;
}

//6xnn - Load Register Immediate
void loadRegisterIm(Chip8* chip8, uint16_t opcode){
	uint8_t addr = (opcode & 0x0F00)>>8;
	uint16_t value = opcode & 0x00FF;
	chip8->reg[addr] = value;
}

//7xkk - Adds the value kk to the value of register Vx, then stores the result in Vx. 
void addValueToReg(Chip8* chip8, uint16_t opcode){
	uint8_t addr = (opcode & 0x0F00)>>8;
	uint16_t value = opcode & 0x00FF;
	chip8->reg[addr] += value;
}

//8xy0 - LD Vx, Vy Vx = Vy
void loadRegXRegY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	chip8->reg[addrX] = chip8->reg[addrY];
}
//8xy1 - OR Vx, Vy
void logORRegXReY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	chip8->reg[addrX] |= chip8->reg[addrY];
	//CHIP 8 ONLY
	chip8->reg[0xF] = 0;
}

//8xy2 - AND Vx, Vy
void logANDRegXReY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	chip8->reg[addrX] &= chip8->reg[addrY];
	//CHIP 8 ONLY
	chip8->reg[0xF] = 0;
}

//8xy3 - XOR Vx, Vy
void logXORRegXReY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	chip8->reg[addrX] ^= chip8->reg[addrY];
	//CHIP 8 ONLY
	chip8->reg[0xF] = 0;
}

//8xy4 - ADD Vx, Vy Set Vx = Vx + Vy, set VF = carry
void regXADDregY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	uint16_t res = (chip8->reg[addrX] + chip8->reg[addrY]) & 0xFFFF;
	chip8->reg[addrX] = res;
	chip8->reg[0xF] = (res > 255) ? 1 : 0;
}

//8xy5 - SUB Vx, Vy Set Vx = Vx - Vy, set VF = NOT borrow.
void regXSUBregY(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	//This is what Octo does. It seems to work with Timendus Test Suite
	uint8_t flag = chip8->reg[addrX] >= chip8->reg[addrY];
	chip8->reg[addrX] -= chip8->reg[addrY];
	chip8->reg[0xF] = flag; 
}

//8xy6 - SHR Vx {, Vy}
void bitshiftRightRegX(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	uint8_t flag = (chip8->reg[addrY] & 0x1) == 1;
	chip8->reg[addrX] = chip8->reg[addrY] >> 1; 
	chip8->reg[0xF] = flag;
}

//8xy7 - SUBN Vx, Vy
void regYNEGSUBregX(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	uint8_t flag = (chip8->reg[addrY] >= chip8->reg[addrX]) ? 1 : 0;
	chip8->reg[addrX] = chip8->reg[addrY] - chip8->reg[addrX];
	chip8->reg[0xF] = flag;
}

//8xyE - SHL Vx {, Vy}
void bitshiftLeftRegX(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	uint8_t flag = ((chip8->reg[addrY] & 0x80) == 128);
	chip8->reg[addrX] = chip8->reg[addrY] << 1; 
	chip8->reg[0xF] = flag; 
}

//9xy0 - SNE Vx, Vy Skip next instruction if Vx != Vy.
void regNotEqual(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t addrY = (opcode & 0x00F0)>>4;
	if(chip8->reg[addrX] != chip8->reg[addrY])
		chip8->programCounter += 2;
}

//Annn - Load index Register with imm. Value
void loadIndexRegister(Chip8* chip8, uint16_t opcode){
	uint16_t value = opcode & 0x0FFF;
	chip8->indexRegister = value;
}

//Bnnn - JP V0, addr, Jump to location nnn + V0.
void jumpToLoc(Chip8* chip8, uint16_t opcode){
	uint16_t addr = opcode & 0xFFF;
	chip8->programCounter = addr + chip8->reg[0x0];
}

//Cxnn RND Vx, byte
void randomNum(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint32_t r = rand() % 256;
	chip8->reg[addrX] = r & opcode;
}

//Dxyn - Display n-byte sprite starting at memory location I at
void drawSprite(Chip8* chip8, uint16_t opcode){
	chip8->draw = true;
	chip8->reg[0xF] = 0; //Collision set false (Bit 0)
	uint16_t x = (opcode & 0x0F00)>>8;
	uint16_t y = (opcode & 0x00F0)>>4;
	uint16_t n = opcode & 0x000F;
	x = (chip8->reg[x]) % DISP_COL;
	y = (chip8->reg[y]) % DISP_ROW;
	for(int yVal = 0; yVal < n; yVal++){
		uint8_t line = chip8->mem[chip8->indexRegister + yVal];
		for(int xVal = 0; xVal < 8; xVal++){
			if((x+xVal >= DISP_COL) || (y+yVal >=DISP_ROW))
				break;
			if(chip8->display[y+yVal][x+xVal] == 1)
				chip8->reg[0xF] = 1; //if there is a Collision, Flag Register is set to 1
			chip8->display[y+yVal][x+xVal] ^= (line & 0x80)>>7;
			line = line << 1;
		}
	}
}

//Ex9E - SKP Vx
void skipOnKey(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	if(chip8->keyboard[chip8->reg[addrX]] == true){
		//fprintf(stdout, "Key Pressed\n");
		chip8->programCounter +=2;
	}
}

//ExA1 - SKNP Vx
void skipOnNotKey(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	if(chip8->keyboard[chip8->reg[addrX]] == false){
		chip8->programCounter +=2;
	}
}

//Fx07 - LD Vx, DT
void getDelayReg(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	chip8->reg[addrX] = chip8->delayTimer;
}

//Fx0A - LD Vx, K - Stop execution until a Key is pressed
void getKey(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	for(int i = 0; i < 0x10; i++){
		if(chip8->keyboard[i]){
			chip8->mem[addrX] = chip8->keyboard[i];
			return;
		}
	}
	chip8->programCounter -= 2;
}

//Fx15 - LD Vx, DT load delay register
void setDelayReg(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0xF00)>>8;
	chip8->delayTimer = chip8->reg[addrX];
}

//Fx18 - LD Vx, ST load sound register
void setSoundReg(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0xF00)>>8;
	chip8->soundTimer = chip8->reg[addrX];
}

//Fx1E
void indexRegAddVx(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	chip8->indexRegister += chip8->reg[addrX];
}

//Fx29 - LD F, Vx
void loadSprite(Chip8* chip8, uint16_t opcode){
	uint8_t addrx = (opcode & 0x0F00)>>8;
	chip8->indexRegister = 0x50+chip8->mem[addrx]*5;
}

//Fx33 - LD B, Vx Vx to BCD
void regToBCD(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	uint8_t res = chip8->reg[addrX];
	chip8->mem[chip8->indexRegister] = res/100;
	chip8->mem[chip8->indexRegister+1] = (res%100)/10;
	chip8->mem[chip8->indexRegister+2] = (res%10);
}

//Fx65 - LD Vx, [I]
void loadV0toVX(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	for(int i = 0; i <= addrX; i++){
		//FOR CHIP 8
		chip8->reg[i] = chip8->mem[chip8->indexRegister];
		chip8->indexRegister += 1; 
	}
}

//Fx55 LD [I], VX
void storeV0toVX(Chip8* chip8, uint16_t opcode){
	uint8_t addrX = (opcode & 0x0F00)>>8;
	for(int i = 0; i <= addrX; i++){
		//FOR CHIP 8
		chip8->mem[chip8->indexRegister] = chip8->reg[i];
		chip8->indexRegister += 1; 
	}
}

//Decremnt delay and sound by 1 if greater than  0
void decrementCounters(Chip8* chip8){
	if(chip8->delayTimer > 0)
		chip8->delayTimer -= 1;
	if(chip8->soundTimer > 0)
		chip8->soundTimer -= 1;
}

//Emulation logic
int emulate(Chip8* chip8){
	uint16_t opcode = (chip8->mem[chip8->programCounter] <<8) | (chip8->mem[chip8->programCounter+1]);
	//printf("Opcode: 0x%x, PC: 0x%x\n", opcode, chip8->programCounter);
	chip8->programCounter += 2;
	switch (opcode & 0xF000){
		case 0x0000:
			switch(opcode & 0x0FFF){
				case 0x00E0: clearScreen(chip8); break;
				case 0x00EE: returnFunc(chip8); break;
				default: fprintf(stderr, "Failure on 0x%x\n", opcode); return EXIT_FAILURE;
			}
			break;
		case 0x1000:
			jumpPC(chip8, opcode); break;
		case 0x2000:
			callFunc(chip8,opcode); break;
		case 0x3000:
			isEqual(chip8, opcode); break;
		case 0x4000:
			isNotEqual(chip8, opcode); break;
		case 0x5000:
			regEqual(chip8, opcode); break;
		case 0x6000:
			loadRegisterIm(chip8, opcode); break;
		case 0x7000:
			addValueToReg(chip8, opcode); break;
		case 0x8000:
			switch(opcode & 0xF){
				case 0x0:	loadRegXRegY(chip8, opcode); break;
				case 0x1:	logORRegXReY(chip8, opcode); break;
				case 0x2:	logANDRegXReY(chip8, opcode); break;
				case 0x3:	logXORRegXReY(chip8, opcode); break;
				case 0x4:	regXADDregY(chip8, opcode); break;
				case 0x5:	regXSUBregY(chip8, opcode); break;
				case 0x6:	bitshiftRightRegX(chip8, opcode); break;
				case 0x7:	regYNEGSUBregX(chip8, opcode); break;
				case 0xE:	bitshiftLeftRegX(chip8, opcode); break;
				default: fprintf(stderr, "Failure on 0x%x\n", opcode); return EXIT_FAILURE;
			} break;
		case 0x9000:
			regNotEqual(chip8, opcode); break;
		case 0xA000:
			loadIndexRegister(chip8, opcode); break;
		case 0xB000:
			jumpToLoc(chip8, opcode); break;
		case 0xC000:
			randomNum(chip8, opcode); break;
		case 0xD000:
			drawSprite(chip8, opcode); break;
		case 0xE000:
			switch(opcode & 0xFF){
				case 0x9E:	skipOnKey(chip8, opcode); break;
				case 0xA1:	skipOnNotKey(chip8, opcode); break;
				default: fprintf(stderr, "Failure on 0x%x\n", opcode); return EXIT_FAILURE;
			} break;
		case 0xF000:
			switch(opcode & 0xFF){
				case 0x07:	getDelayReg(chip8, opcode); break;
				case 0x0A:	getKey(chip8, opcode); break;
				case 0x15:	setDelayReg(chip8, opcode); break;
				case 0x18:	setSoundReg(chip8, opcode); break;
				case 0x1E:	indexRegAddVx(chip8, opcode); break;
				case 0x29:	loadSprite(chip8, opcode); break;
				case 0x33:	regToBCD(chip8, opcode); break;
				case 0x55:	storeV0toVX(chip8, opcode); break;
				case 0x65:	loadV0toVX(chip8, opcode); break;
				default: fprintf(stderr, "Failure on 0x%x\n", opcode); return EXIT_FAILURE;
			} break;
		default: fprintf(stderr, "Failure on 0x%x\n", opcode); return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


//FOR DEBUGGING ONLY, NOT NEEDED FOR EMULATION
void printDisp(Chip8* chip8){
	//Getting the Display Output
	for(int y = 0; y < DISP_ROW; y++){
		for(int x = 0; x < DISP_COL; x++){
			if((chip8->display[y][x])==1)
				printf("#");
			else
				printf(" ");
		}
		printf("\n");
	}
}