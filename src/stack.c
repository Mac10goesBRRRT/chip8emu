#include "stack.h"
#include <stdint.h>
#include <stdlib.h>

int push (stack_t* stack, uint16_t value){
	if(stack->len < STACK_MAX_DEPTH){
		*(stack->stackpointer + 1) = value;
		stack->stackpointer += 1;
		stack->len += 1;
		return 0;
	} else {
		return -1;
	}
}

uint16_t pop(stack_t* stack){
	assert(stack->len > 0);
	uint16_t pointerPop = *(stack->stackpointer);
	stack->stackpointer -= 1;
	stack->len -= 1;
	return pointerPop;
}

stack_t* stackInit(){
	stack_t* stackPtr = malloc(sizeof(stack_t));
	stackPtr->len = 0;
	stackPtr->buffer = malloc(sizeof(uint16_t)*stackPtr->len);
	stackPtr->stackpointer = stackPtr->buffer;
	return stackPtr;
}

void stackFree(stack_t* stack){
	free(stack->buffer);
	free(stack);
}
