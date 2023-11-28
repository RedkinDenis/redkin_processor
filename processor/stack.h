#ifndef STACK_H
#define STACK_H

typedef int elem_t;

static const elem_t poison = 0;

struct Stack
{
    elem_t* data = NULL;
    size_t capacity = 0;
    size_t size = 0;
};

elem_t* stack_pop(struct Stack* stk);

void stack_dump(struct Stack* stk);

void stack_ctor(struct Stack* stk, size_t capacity);

void stack_push(struct Stack* stk, elem_t* x);

void stack_dtor(struct Stack* stk);

void capacity_down(struct Stack* stk);

void capacity_up(struct Stack* stk);

#endif //STACK_H
