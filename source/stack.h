// stack.h
// Header for the stack.
//
// The purpose of the stack is to store objects that must be temporarily
// protected from garbage collection. I got this idea from uLisp:
// http://www.ulisp.com/show?1BD3


#ifndef STACK_H
#define STACK_H


#define STACK_SIZE 1024


LispObject * stack[STACK_SIZE];

long sp;


void push(LispObject * obj);

void pop();

void print_stack();


#endif
