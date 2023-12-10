#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_MAX_DEPTH 16 //Can be increased if needed, min is 16;

typedef struct stack{
	uint16_t* buffer;
	uint16_t* stackpointer;
	size_t len;
} stack_t;

int push (stack_t* stack, uint16_t value);
uint16_t pop(stack_t* stack);
stack_t* stackInit();
void stackFree(stack_t* stack);

#endif
