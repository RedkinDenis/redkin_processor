#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include "input_output.h"
#include "stack.h"
#include "encoding.h"
#include "err_codes.h"

#define PROC_DUMP(proc) proc_dump(proc, __LINE__, GET_VARIABLE_NAME(proc));

struct processor
{
    int* data = {};
    int ip = 0;
    int ax  = 0, bx = 0, cx = 0, dx = 0;
    struct Stack stk = {};
};

void fill_proc(struct processor* proc, FILE* read, int fsize);

enum err executor(struct processor* proc);

void proc_dump(struct processor* proc, int LINE, const char* proc_name);

enum err proc(struct processor* stk);

void proc_free(struct processor* proc);

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

    int fsize = GetFileSize(read) / sizeof(int);

    struct processor processor = {};

    fill_proc(&processor, read, fsize);

    proc(&processor);

    proc_free(&processor);
}

enum err executor(struct processor* proc)
{
    #define POP(reg) res = stack_pop(&(proc->stk), &reg); \
                     if(res != SUCCESS) \
                        return res;
    #define PUSH(reg) res = stack_push(&(proc->stk), &reg); \
                      if(res != SUCCESS) \
                        return res;

    int x1 = 0, x2 = 0;
    int x = 0;
    err res;

    int flag = 1;

    while(flag != 0)
    {
        switch((int)(proc->data[proc->ip]))
        {
            case PUSH:
                PUSH(proc->data[proc->ip + 1])
                proc->ip += 2;
                break;

            #define MATH_COMM(enum, operand) \
            case enum:                       \
                POP(x1)                      \
                POP(x2)                      \
                x = x2 operand x1;           \
                proc->ip += 1;               \
                PUSH(x)                      \
                break;
            #include "math_comm.h"
            #undef MATH_COMM

            case OUT:
                POP(x)
                proc->ip += 1;
                printf("\nresult - %d\n", x);
                break;
            case HET:
                stack_dtor(&(proc->stk));
                proc->ip += 1;
                flag = 0;
                break;
            case IN:
                printf("enter push element");
                scanf("%d", &x);
                stack_push(&(proc->stk), &x);
                proc->ip += 1;
                break;
            case RPUSH:
                proc->ip += 1;
                switch(proc->data[proc->ip])
                {
                    #define Define_Command(str, enum) \
                    case enum:              \
                        stack_push(&(proc->stk), &(proc->enum)); \
                        break;
                    #include "registers.h"
                    #undef Define_Command
                    default:
                        return REG_NAME_ERR;
                        break;
                }
                proc->ip += 1;
                break;
            case POP:
                proc->ip += 1;
                switch(proc->data[proc->ip])
                {
                    #define Define_Command(str, enum) \
                    case enum:              \
                        POP(proc->enum)  \
                        break;
                    #include "registers.h"
                    #undef Define_Command
                    default:
                        return REG_NAME_ERR;
                        break;
                }
                proc->ip += 1;
                break;
            case JMP:
                proc->ip = proc->data[proc->ip + 1];
                break;
            #define Define_Jumps(rub, enum, operand) \
            case enum:                        \
                stack_pop(&(proc->stk), &x1); \
                stack_pop(&(proc->stk), &x2); \
                if(x1 operand x2)             \
                    proc->ip = proc->data[proc->ip + 1];\
                break;
            #include "jumps.h"
            #undef Define_Jumps
            default:
                return COMM_NAME_ERR;
                break;
        }

        //STACK_DUMP(proc->stk)
        //PROC_DUMP(proc)
    }
    return SUCCESS;
}

enum err proc(struct processor* proc)
{
    enum err res = executor(proc);
}

void fill_proc(struct processor* proc, FILE* read, int fsize)
{
    proc->data = (int*)calloc(fsize + 1, sizeof(int));

    stack_ctor(&proc->stk, 20);

    int x = fread(proc->data, sizeof(int), fsize, read);

    //PROC_DUMP(proc)
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

    printf("ax - %d \n"                                                                  // так же как в стеке
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

    printf("\n-------DUMP_END-------\n\n");
}

void proc_free(struct processor* proc)
{
    free(proc->data);
    proc->ip = 0;
    proc->ax = 0;
    proc->bx = 0;
    proc->cx = 0;
    proc->dx = 0;
}

