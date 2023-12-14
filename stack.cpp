#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>                                                                  // подабовлять больше проверок и ассертов

#include "stack.h"


/*int main()
{
    size_t cpc = 5;
    struct Stack stk = {};

    stack_ctor(&stk, cpc);

    elem_t push_elem = 10;
    stack_push(&stk, &push_elem);
    printf("data[0] - %d\ncapacity - %d" ,stk.data[0], stk.capacity);

    elem_t pop_elem = *stack_pop(&stk);
    printf("\ndata[0] - %d\ncapacity - %d" ,stk.data[0], stk.capacity);
}*/

void stack_dump(struct Stack* stk, int LINE, const char* stk_name)
{
    printf("--------STACK---------");
    printf("\n------DUMP_BEGIN------\n");
    printf("\nstack name: %s\n", stk_name);
    printf("\nDUMP was called from line: %d\n", LINE);
    printf("\nstack adress: %d\n", stk);                                        // выводить функцию и файл откуда был вызов
    printf("stack->data adress: %d\n", stk->data);
    printf("capacity - %d\nsize - %d\nstack:\n", stk->capacity, stk->size);
    for(int i = 0; i < stk->size; i++)
        printf("%d ", stk->data[i]);
    printf("* ");
    for(int i = stk->size; i < stk->capacity; i++)
        printf("%d ", stk->data[i]);
                                                // неиспользованные элементы - poison, вывод формата *[i] = ...
    printf("\n-------DUMP_END-------\n\n"); // TODO
}

enum err stack_ctor(struct Stack* stk, size_t capacity)
{
    if(stk->capacity > 0) // data
    {
        return ALRCR; //ТТКЖ - более развернутые имена енамов
    }

    // capacity > 0 verify
    stk->data = (elem_t*)calloc(capacity, sizeof(elem_t)); // verify
    stk->capacity = capacity;
    stk->size = 0;
    return SUCCESS;
}

enum err stack_push(struct Stack* stk, const elem_t* x)
{
    enum err res = capacity_up(stk);
    if(res != SUCCESS)
        return res;

    stk->data[stk->size] = *x;
    stk->size++;
    return SUCCESS;
}

enum err stack_pop(struct Stack* stk, elem_t* pop_el)
{
    if(stk->size <= 0)
    {
        //printf("--pop error: stack is empty");
        return STEMP;
    }

    stk->size--;

    if(pop_el != NULL)
        *pop_el = *(stk->data + stk->size);

    *(stk->data + stk->size) = poison;

    enum err res = capacity_down(stk);
    if(res != SUCCESS)
        return res;

    return SUCCESS;
}

void stack_dtor(struct Stack* stk)
{
    free(stk->data);
    stk->data = NULL;
    stk->size = (size_t)-1;                    // > 0 значит пр и попытке повторно создать этот стек будет ощибка что он уже созданг
    stk->capacity = (size_t)-1;
}
enum err capacity_down(struct Stack* stk)
{
    if(stk->capacity < 2 ||
       stk->capacity / 4 < stk->size)
        return SUCCESS;

    void* temp = realloc(stk->data, stk->capacity / 2);

    if(temp == NULL)
    {
        return REALLERR;
    }

    stk->capacity /= 2;

    stk->data = (elem_t*)temp;

    return SUCCESS;
}

enum err capacity_up(struct Stack* stk)
{
    assert(stk != NULL);
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
        return REALLERR;
    }

    stk->data = (elem_t*)temp;
    stk->capacity += expander;
    return SUCCESS;
}
