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

void stack_dump(struct Stack* stk, int LINE, const char* stk_name)
{
    printf("\n------DUMP_BEGIN------\n");
    printf("\nstack name: %s\n", stk_name);
    printf("\nDUMP was called from line: %d\n", LINE);
    printf("\nstack adress: %d\n", stk);
    printf("stack->data adress: %d\n", stk->data);
    printf("capacity - %d\nsize - %d\nstack:\n", stk->capacity, stk->size);
    for(int i = 0; i < stk->size; i++)
        printf("%d ", stk->data[i]);
    printf("* ");
    for(int i = stk->size; i < stk->capacity; i++)
        printf("%d ", stk->data[i]);

    printf("\n-------DUMP_END-------");
}

void stack_ctor(struct Stack* stk, size_t capacity)
{
    if(stk->capacity > 0)
    {
        printf("--Error: Stack is already created");                  //���������� ��� ������ � �� �������� �� �������
        return;
    }
    stk->data = (elem_t*)calloc(capacity, sizeof(elem_t));
    stk->capacity = capacity;
    stk->size = 0;
}

void stack_push(struct Stack* stk, const elem_t* x)
{
    capacity_up(stk);               //���������� ������ ������ � ��������
    stk->data[stk->size] = *x;
    stk->size++;
}

void stack_pop(struct Stack* stk, elem_t* pop_el)
{
    if(stk->size <= 0)
    {
        printf("--pop error: stack is empty");
        return ;
    }
    stk->size--;
    *pop_el = *(stk->data + stk->size);

    *(stk->data + stk->size) = poison;

    capacity_down(stk);
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
        return;                                   //��� ������
    }

    stk->data = (elem_t*)temp;
}

void capacity_up(struct Stack* stk)
{
    if(stk->size != stk->capacity)
        return;                       // ���� ������ ������� �������, �� ����������� �� �������

    stk->capacity *= 2;
    void* temp = realloc(stk->data, stk->capacity);

    if(temp == NULL)
    {
        printf("\n--realloc error\n");
        return;
    }
    else
        stk->data = (elem_t*)temp;
}
