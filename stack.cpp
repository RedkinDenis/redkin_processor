#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

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

void stack_dump(struct Stack* stk)
{
    printf("\n------DUMP_BEGIN------\n");
    printf("capacity - %d\nsize - %d\nstack:\n", stk->capacity, stk->size);
    for(int i = 0; i < stk->capacity; i++)                                       // печать от нуля то сайза и от сайза до капасити
        printf("%d ", stk->data[i]);                                             // печатать адрес, закидывать позицию откуда вызвана и все это через дефайн.
    printf("\n-------DUMP_END-------");
}

void stack_ctor(struct Stack* stk, size_t capacity)
{
    if(stk->capacity > 0)
    {
        printf("--Error: Stack is already created");                  //возвращать код ошибки а не печатать из функции
        return;
    }
    stk->data = (elem_t*)calloc(capacity, sizeof(elem_t));
    stk->capacity = capacity;
    stk->size = 0;
}

void stack_push(struct Stack* stk, elem_t* x)  // const
{
    capacity_up(stk);               //обработать случай ошибки в реаллоке
    stk->data[stk->size] = *x;
    stk->size++;
}

elem_t* stack_pop(struct Stack* stk) // писать по указателю
{
    if(stk->size > 0)
    {
        stk->size--;
        elem_t* s = stk->data + stk->size;

        capacity_down(stk);

        return s;
    }                      //поменять ифы местами
    else
        printf("--pop error: stack is empty");
    return 0;
}

void stack_dtor(struct Stack* stk)
{
    free(stk->data);
    stk->data = NULL;
    stk->size = (size_t)-1;
    stk->capacity = (size_t)-1;
}

void capacity_down(struct Stack* stk)
{
    if(stk->size > stk->capacity / 4)
        return;

    if(stk->capacity / 2 != 0)
        stk->capacity /= 2;

    void* temp = realloc(stk->data, stk->capacity);

    if(temp == NULL)
    {
        printf("\n--realloc error\n");
        return;                                   //код ошибки
    }

    stk->data = (elem_t*)temp;
}

void capacity_up(struct Stack* stk)
{
    if(stk->size == stk->capacity)
        stk->capacity *= 2;               //ифы // если размер слишком большой, то увеличивать на меньшее
    else
        return;

    void* temp = realloc(stk->data, stk->capacity);

    if(temp == NULL)
    {
        printf("\n--realloc error\n");
        return;
    }
    else
        stk->data = (elem_t*)temp;
}
