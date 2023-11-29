#ifndef STACK_H
#define STACK_H

#define GET_VARIABLE_NAME(variable) #variable

#define STACK_DUMP(stk) stack_dump(&stk, __LINE__, GET_VARIABLE_NAME(stk));

typedef char elem_t;

static const elem_t poison = 0;

struct Stack
{
    elem_t* data = NULL;
    size_t capacity = 0;
    size_t size = 0;
};

void stack_pop(struct Stack* stk, elem_t* pop_el);

void stack_dump(struct Stack* stk, int LINE, const char* stk_name);

void stack_ctor(struct Stack* stk, size_t capacity);

void stack_push(struct Stack* stk, const elem_t* x);

void stack_dtor(struct Stack* stk);

void capacity_down(struct Stack* stk);

void capacity_up(struct Stack* stk);

#endif //STACK_H
