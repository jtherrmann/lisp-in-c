// stack.h
// Header for the stack.


#ifndef STACK_H
#define STACK_H


#define STACK_SIZE 1024


LispObject * stack[STACK_SIZE];

int sp;

bool stack_output;


void push(LispObject * obj);

void pop();


#endif
