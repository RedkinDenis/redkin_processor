#ifndef STACK_H
#define STACK_H

#define GET_VARIABLE_NAME(variable) #variable

#define STACK_DUMP(stk, func_name) stack_dump(&stk, __LINE__, GET_VARIABLE_NAME(stk), __FILE__, GET_VARIABLE_NAME(func_name));

#include "err_codes.h"

typedef int elem_t;

static const elem_t poison = 0xDEAD;

struct Stack
{
    elem_t* data = NULL;
    size_t capacity = 0;
    size_t size = 0;
};

enum err stack_pop(struct Stack* stk, elem_t* pop_el);

void stack_dump(struct Stack* stk, int LINE, const char* stk_name, const char* file_name, const char* func_name);

enum err stack_ctor(struct Stack* stk, size_t capacity);

enum err stack_push(struct Stack* stk, const elem_t* x);

enum err stack_dtor(struct Stack* stk);

enum err capacity_down(struct Stack* stk);

enum err capacity_up(struct Stack* stk);

#endif //STACK_H
