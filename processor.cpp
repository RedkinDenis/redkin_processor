#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include "input_output.h"
#include "stack.h"
#include "encoding.h"

#define PROC_DUMP(proc) proc_dump(proc, __LINE__, GET_VARIABLE_NAME(proc));

struct processor
{
    char* data = {};
    int ip = 0;
    char ax  = 0, bx = 0, cx = 0, dx = 0;
};

void fill_proc(struct processor* proc, FILE* read, int fsize);

void proc_dump(struct processor* proc, int LINE, const char* proc_name);

void proc(struct processor* stk);

int main(int argc, char* argv[])
{
    system("ass.exe");

    char* inpName = (char*)"byte_code1.bin";

    if(argc == 2)
        inpName = argv[1];

    FILE* read = fopen(inpName, "rb");

    if(read == NULL)
    {
        printf("read file open error");
        return 0;
    }

    int fsize = GetFileSize(read);

    //printf("\nfsize - %d\n", fsize);

    struct processor processor = {};

    fill_proc(&processor, read, fsize);

    //proc_dump(&processor);

    proc(&processor);
}

void proc(struct processor* proc)
{
    struct Stack stk = {};
    stack_ctor(&stk, 20);

    char x1 = 0, x2 = 0;
    char x = 0;

    int flag = 1;

    while(flag != 0)   // флаг на конец
    {
        switch((int)(proc->data[proc->ip]))
        {
            case PUSH:
                stack_push(&stk, &(proc->data[proc->ip + 1]));
                proc->ip += 2;
                break;
            case ADD:
                stack_pop(&stk, &x1);
                stack_pop(&stk, &x2);
                proc->ip += 1;
                x = x1 + x2;
                stack_push(&stk, &x);
                break;
            case SUB:
                stack_pop(&stk, &x1);
                stack_pop(&stk, &x2);
                x = x2 - x1;
                proc->ip += 1;
                stack_push(&stk, &x);
                break;
            case DIV:
                stack_pop(&stk, &x1);
                stack_pop(&stk, &x2);
                proc->ip += 1;
                x = x2 / x1;
                stack_push(&stk, &x);
                break;
            case MUL:
                stack_pop(&stk, &x1);
                stack_pop(&stk, &x2);
                proc->ip += 1;
                x = x2 * x1;
                stack_push(&stk, &x);
                break;
            case OUT:
                stack_pop(&stk, &x);
                proc->ip += 1;
                printf("\nresult - %d\n", x);
                break;
            case HET:
                stack_dtor(&stk);
                proc->ip += 1;
                flag = 0;
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
                    #define REG_NAME(reg) \
                    case reg:              \
                        stack_push(&stk, &(proc->reg)); \
                        proc->reg = 0;                  \
                        break;
                    #include "registers.h"
                    #undef REG_NAME
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
                    #define REG_NAME(reg) \
                    case reg:              \
                        stack_pop(&stk, &(proc->reg));  \
                        break;
                    #include "registers.h"
                    #undef REG_NAME
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

        //STACK_DUMP(stk)
        //PROC_DUMP(proc)
    }
}

void fill_proc(struct processor* proc, FILE* read, int fsize)
{
    proc->data = (char*)calloc(fsize + 1, sizeof(char));

    int x = fread(proc->data, sizeof(char), fsize, read);

    assert(x == fsize);
}

void proc_dump(struct processor* proc, int LINE, const char* proc_name)
{
    printf("\n------PROCESSOR-------");
    printf("\n------DUMP_BEGIN------\n");

    printf("\nprocessor name: %s\n", proc_name);
    printf("\nDUMP was called from line: %d\n", LINE);
    printf("\nprocessor adress: %d\n", proc);
    printf("processor->data adress: %d\n", proc->data);

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

    printf("ip = %d", proc->ip);

    printf("\n-------DUMP_END-------\n");
}

