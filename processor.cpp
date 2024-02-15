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

#define PROC_DUMP(proc, func_name) proc_dump(proc, __LINE__, GET_VARIABLE_NAME(proc), __FILE__, GET_VARIABLE_NAME(func_name));

struct processor
{
    int* data = {};
    int ip = 0;
    int ax  = 0, bx = 0, cx = 0, dx = 0;
    int* RAM = 0;
    struct Stack cmd_stk = {};
    struct Stack call_stk = {};
};

enum step
{
    command = 1,
    number = 1
};

enum err fill_proc(struct processor* proc, FILE* read, int fsize);

enum err executor(struct processor* proc);

enum err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name);

enum err proc(struct processor* cmd_stk);

enum err proc_free(struct processor* proc);

int main(int argc, char* argv[])
{
    enum err res = (enum err)system("ass.exe");

    if(res != SUCCESS)
    {
        printf("error number %d in ass.exe", res);
        return res;
    }

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

    #define check_result(func)                  \
    if(res != SUCCESS)                          \
    {                                           \
        printf("error in " #func " %d ", res);     \
        return res;                             \
    }


    res = fill_proc(&processor, read, fsize);
    check_result(fill_proc)

    res = proc(&processor);
    check_result(proc)

    proc_free(&processor);
    check_result(proc_free)

    #undef check_result
}

enum err executor(struct processor* proc)
{
    #define POP(stk_type, reg) res = stack_pop(&(proc->stk_type), &reg); \
                     if(res != SUCCESS) \
                        return res;                                                      // גûםוסעט ג ץוהונ
    #define PUSH(stk_type, reg) res = stack_push(&(proc->stk_type), &reg); \
                      if(res != SUCCESS) \
                        return res;

    int x1 = 0, x2 = 0;
    int x = 0;
    int cmd = 0;
    err res;

    int running = 1;

    while(running)
    {
        cmd = proc->data[proc->ip];
        switch(cmd)
        {
            case PUSH:
                PUSH(cmd_stk, proc->data[proc->ip + command])
                proc->ip += (command + number);
                break;

            #define MATH_COMM(enum, operand) \
            case enum:                       \
                POP(cmd_stk, x1)             \
                POP(cmd_stk, x2)             \
                x = x2 operand x1;           \
                proc->ip += command;         \
                PUSH(cmd_stk, x)             \
                break;
            #include "math_comm.h"
            #undef MATH_COMM

            case OUT:
                POP(cmd_stk, x)
                proc->ip += command;
                printf("\nresult - %d\n", x);
                break;
            case HET:
                stack_dtor(&(proc->cmd_stk));
                proc->ip += command;
                running = 0;
                break;
            case IN:
                printf("\nenter push element ");
                scanf("%d", &x);
                PUSH(cmd_stk, x)
                proc->ip += command;
                break;
            case RPUSH:
                proc->ip += command;
                switch(proc->data[proc->ip])
                {
                    #define Define_Command(str, enum) \
                    case enum:              \
                        PUSH(cmd_stk, proc->enum); \
                        break;
                    #include "registers.h"
                    #undef Define_Command
                    default:
                        return UNKNOWN_REGISTER_NAME;
                        break;
                }
                proc->ip += number;
                break;
            case POP:
                proc->ip += command;
                switch(proc->data[proc->ip])
                {
                    #define Define_Command(str, enum) \
                    case enum:              \
                        POP(cmd_stk, proc->enum)  \
                        break;
                    #include "registers.h"
                    #undef Define_Command
                    default:
                        return UNKNOWN_REGISTER_NAME;
                        break;
                }
                proc->ip += number;
                break;
            case JMP:
                proc->ip = proc->data[proc->ip + command];
                break;
            #define Define_Jumps(rub, enum, operand) \
            case enum:                        \
                POP(cmd_stk, x1) \
                POP(cmd_stk, x2) \
                if(x1 operand x2)             \
                    proc->ip = proc->data[proc->ip + command];\
                break;
            #include "jumps.h"
            #undef Define_Jumps

            case CALL:
                PUSH(call_stk, proc->ip + command + number)
                proc->ip = proc->data[proc->ip + command];
                break;
            case RET:
                POP(call_stk, proc->ip)
                break;

            default:
                return UNKNOWN_COMMAND_NAME;
                break;
        }
        //STACK_DUMP(proc->cmd_stk, executor)
    }
    return SUCCESS;
}

enum err proc(struct processor* proc)
{
    if(proc == NULL)
        return NULL_INSTEAD_PTR;

    return executor(proc);
}

enum err fill_proc(struct processor* proc, FILE* read, int fsize)
{
    if(proc == NULL)
        return NULL_INSTEAD_PTR;

    int* temp = (int*)calloc(fsize + 1, sizeof(int));
    if(temp == NULL)
        return CALLOC_ERROR;
    proc->data = temp;

    temp = (int*)calloc(100, sizeof(int));
    if(temp == NULL)
        return CALLOC_ERROR;
    proc->RAM = temp;

    enum err res = stack_ctor(&proc->cmd_stk, 10);
    if(res != SUCCESS)
        return res;

    res = stack_ctor(&proc->call_stk, 1);
    if(res != SUCCESS)
        return res;

    int x = fread(proc->data, sizeof(int), fsize, read);

    //PROC_DUMP(proc, fill_proc)
    assert(x == fsize);
    return SUCCESS;
}

enum err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name)
{
    if(proc == NULL)
        return NULL_INSTEAD_PTR;

    printf("\n------PROCESSOR-------");
    printf("\n------DUMP_BEGIN------\n");

    printf("\nprocessor name: %s\n", proc_name);
    printf("\nDUMP was called from function: %s \n"
                             "from file: %s \n"
                             "from line: %d \n", func_name, file_name, LINE);
    printf("\nprocessor adress: %d\n", proc);
    if(proc->data != NULL)
    {
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
    }
    else
        printf("processor is not initialized \n");

    printf("ip = %d", proc->ip);

    printf("\n-------DUMP_END-------\n\n");
    return SUCCESS;
}

enum err proc_free(struct processor* proc)
{
    if(proc == NULL)
        return NULL_INSTEAD_PTR;

    free(proc->data);
    free(proc->RAM);
    stack_dtor(&proc->cmd_stk);
    proc->ip = 0;

    proc->ax = 0;
    proc->bx = 0;
    proc->cx = 0;
    proc->dx = 0;
    return SUCCESS;
}

