#include <stdint.h>
#include <stdio.h>
#include <time.h>



#include "chip8.h"

int main (int argv, char** argc){
	//TODO: delayTimer and soundTimer get lower by 1 at 60Hz
	Chip8* chip8 = initChip8();
	chip8->mem[0] = 'c';
	printf("%c\n", chip8->mem[0]);
}
