#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "stack.h"

void stack_dump(struct Stack* stk, int LINE, const char* stk_name, const char* file_name, const char* func_name)
{
    printf("--------STACK---------");
    printf("\n------DUMP_BEGIN------\n");
    printf("\nstack name: %s\n", stk_name);
    printf("\nDUMP was called from function: %s \n"
                             "from file: %s \n"
                             "from line: %d \n", func_name, file_name, LINE);
    printf("\nstack adress: %d\n", stk);
    printf("stack->data adress: %d\n", stk->data);
    printf("capacity - %d\nsize - %d\nstack:\n", stk->capacity, stk->size);
    for(int i = 0; i < stk->size; i++)
        printf("*[%d] = %d  ", i, stk->data[i]);
    for(int i = stk->size; i < stk->capacity; i++)
        printf("*[%d] = poison  ", i);

    printf("\n-------DUMP_END-------\n\n");
}

enum err stack_ctor(struct Stack* stk, size_t capacity)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    if(stk->capacity != 0 && stk->capacity != (size_t)-1)
        return STACK_ALREDY_CREATED;

    assert(capacity > 0);
    elem_t* temp = (elem_t*)calloc(capacity, sizeof(elem_t));
    if(temp == NULL)
        return CALLOC_ERROR;
    stk->data = temp;

    stk->capacity = capacity;
    stk->size = 0;

    return SUCCESS;
}

enum err stack_push(struct Stack* stk, const elem_t* x)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    enum err res = capacity_up(stk);
    if(res != SUCCESS)
        return res;

    stk->data[stk->size] = *x;
    stk->size++;
    return SUCCESS;
}

enum err stack_pop(struct Stack* stk, elem_t* pop_el)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    if(stk->size <= 0)
        return STK_EMPTY;

    stk->size--;

    if(pop_el != NULL)
        *pop_el = *(stk->data + stk->size);

    *(stk->data + stk->size) = poison;

    /*enum err res = capacity_down(stk);
    if(res != SUCCESS)
        return res;*/

    return SUCCESS;
}

enum err stack_dtor(struct Stack* stk)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    free(stk->data);
    stk->data = NULL;
    stk->size = (size_t)-1;
    stk->capacity = (size_t)-1;

    return SUCCESS;
}
enum err capacity_down(struct Stack* stk)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    if(stk->capacity < 2 ||
       stk->capacity / 4 < stk->size)
        return SUCCESS;

    void* temp = realloc(stk->data, stk->capacity / 2);

    if(temp == NULL)
    {
        return REALLOC_ERROR;
    }

    stk->capacity /= 2;

    stk->data = (elem_t*)temp;

    return SUCCESS;
}

enum err capacity_up(struct Stack* stk)
{
    if(stk == NULL)
        return NULL_INSTEAD_PTR;

    if(stk->size != stk->capacity)
        return SUCCESS;

    int expander = 0;
    if(stk->capacity >= 100)
        expander = 50;
    else
        expander = stk->capacity;

    void* temp = realloc(stk->data, stk->capacity + expander);

    if(temp == NULL)
    {
        return REALLOC_ERROR;
    }

    stk->data = (elem_t*)temp;
    stk->capacity += expander;
    return SUCCESS;
}
