#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "stack.h"
#include "encoding.h"

struct processor
{
    int* data = {};
    int ip = 0;
    int ax  = 0, bx = 0, cx = 0, dx = 0;
};

void fill_proc(struct processor* stk, struct line* data, int nLines);

void proc_dump(struct processor* proc);

void proc(struct processor* stk);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"byte_code.txt";

    if(argc == 2)
        inpName = argv[1];

    FILE* read = fopen(inpName, "rb");

    if(read == NULL)
    {
        printf("read file open error");
        return 0;
    }

    int fsize = GetFileSize(read);

    struct file data = {};

    InputData(&data, read, fsize);

    struct processor processor = {};

    fill_proc(&processor, data.lines, data.nLines);

    //PrintData(data, nLines);

    proc_dump(&processor);

    proc(&processor);

    //elem_t ans = proc(&stk);

    //printf("\nRESULT: %d", ans);
}

void proc(struct processor* proc)
{
    struct Stack stk = {};
    stack_ctor(&stk, 20);

    int x1 = 0, x2 = 0, x = 0;

    while(proc->data[proc->ip] != 0)   // флаг на конец
    {
        switch(proc->data[proc->ip])
        {
            case PUSH:
                stack_push(&stk, &(proc->data[proc->ip + 1]));
                proc->ip += 2;
                /*printf("input\n");*/
                break;
            case ADD:
                x1 = *stack_pop(&stk);
                x2 = *stack_pop(&stk);
                proc->ip += 1;
                x = x1 + x2;
                stack_push(&stk, &x);
                /*printf("add: %d\n", x);*/
                break;
            case SUB:
                x1 = *stack_pop(&stk);
                x2 = *stack_pop(&stk);
                x = x2 - x1;
                proc->ip += 1;
                stack_push(&stk, &x);
                /*printf("sub: %d\n", x);*/
                break;
            case DIV:
                x1 = *stack_pop(&stk);
                x2 = *stack_pop(&stk);
                proc->ip += 1;
                x = x2 / x1;
                stack_push(&stk, &x);
                /*printf("div: %d\n", x);*/
                break;
            case OUT:
                x = *stack_pop(&stk);
                proc->ip += 1;
                printf("\nresult - %d\n", x);
                /*printf("return\n");*/
                break;
            case HET:
                /*printf("break\n");*/
                stack_dtor(&stk);
                proc->ip += 1;
                break;
            case MUL:
                x1 = *stack_pop(&stk);
                x2 = *stack_pop(&stk);
                proc->ip += 1;
                x = x2 * x1;
                stack_push(&stk, &x);
                break;
            case IN:
                printf("enter push element");
                scanf("%d", &x);
                stack_push(&stk, &x);
                proc->ip += 1;
                break;
            case RPUSH:
                proc->ip += 1;
                switch(proc->data[proc->ip])
                {
                    case ax:
                        stack_push(&stk, &(proc->ax));
                        proc->ax = 0;
                        break;
                    case bx:
                        stack_push(&stk, &(proc->bx));
                        proc->bx = 0;
                        break;
                    case cx:
                        stack_push(&stk, &(proc->cx));
                        proc->cx = 0;
                        break;
                    case dx:
                        stack_push(&stk, &(proc->dx));
                        proc->dx = 0;
                        break;
                    default:
                        printf("unknown register name");
                        return;
                        break;
                }
                proc->ip += 1;
                break;
            case POP:
                proc->ip += 1;
                switch(proc->data[proc->ip])
                {
                    case ax:
                        proc->ax = *stack_pop(&stk);
                        break;
                    case bx:
                        proc->bx = *stack_pop(&stk);
                        break;
                    case cx:
                        proc->cx = *stack_pop(&stk);
                        break;
                    case dx:
                        proc->dx = *stack_pop(&stk);
                        break;
                    default:
                        printf("unknown register name");
                        return;
                        break;
                }
                proc->ip += 1;
                break;
            default:
                printf("unknown command");
                return;
                break;
        }
    }
}

void fill_proc(struct processor* proc, struct line* data, int nLines)
{
    proc->data = (int*)calloc(nLines * 2 + 1, sizeof(int));
    int ptr = 0;

    for(int i = 0; i < nLines; i++)
    {
        if(data[i].len > 2)
        {
            sscanf(data[i].str, "%d %d", &(proc->data[ptr]), &(proc->data[ptr + 1]));
            //printf("%d %d \n", proc->data[ptr], proc->data[ptr + 1]);
            ptr += 2;
        }
        else if(data[i].len > 0)
        {
            sscanf(data[i].str, "%d", &(proc->data[ptr]));
            //printf("%d \n", proc->data[ptr]);
            ptr += 1;
        }
    }
}

void proc_dump(struct processor* proc)
{
    printf("\n------PROCESSOR-------");
    printf("\n------DUMP_BEGIN------\n");

    printf("ax - %d \n"
           "bx - %d \n"
           "cx - %d \n"
           "dx - %d \n", proc->ax, proc->bx, proc->cx, proc->dx);
    int i = 0;

    while(proc->data[i] != 0)
    {
        printf("%d ", proc->data[i]);
        i++;
    }
    printf("\n");

    /*for(int i = 0; i < proc->ip * 2; i++)
        printf(" ");
    printf("|\n");

    for(int i = 0; i < proc->ip * 2 - 3; i++)
        printf(" ");*/
    printf("ip = %d", proc->ip);

    printf("\n-------DUMP_END-------");
}

