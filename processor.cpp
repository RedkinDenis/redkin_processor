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
    char* data = {};
    int data_size;
    int ip = 0;
    elem_t* reg = 0;
    /*int ax  = 0, bx = 0, cx = 0, dx = 0;*/
    elem_t* RAM = 0;
    struct Stack cmd_stk = {};
    struct Stack call_stk = {};
};

enum step
{
    command = 1,
    number = 4,
    reg = 1
};

enum err fill_proc(struct processor* proc, FILE* read, int fsize);

enum err executor(struct processor* proc);

enum err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name);

enum err proc(struct processor* cmd_stk);

enum err proc_free(struct processor* proc);

int main(int argc, char* argv[])
{
    enum err res = (enum err)system("ass.exe fib.txt fib.bin");

    /*if(res != SUCCESS)
    {
        printf("\nerror number %d in ass.exe\n", res);
        return res;
    }*/

    printf("-------------------------------------------------------------------------------------\n");
    printf("-----------------------------------START_OF_PROGRAMM---------------------------------\n");
    printf("-------------------------------------------------------------------------------------\n");


    char* inpName = (char*)"fib.bin";

    if(argc == 2)
        inpName = argv[1];

    FILE* read = fopen(inpName, "rb");

    if(read == NULL)
    {
        printf("\nread file open error\n");
        return 0;
    }

    int fsize = GetFileSize(read) / sizeof(char);

    struct processor processor = {};

    #define check_result(func)                  \
    if(res != SUCCESS)                          \
    {                                           \
        printf("\nerror in " #func " %d \n", res);     \
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
    #define POP(stk_type, reg)                                 \
                    res = stack_pop(&(proc->stk_type), &buf);  \
                    if(res != SUCCESS)                         \
                       return res;                             \
                    memcpy(&reg, &buf, sizeof(elem_t));

    #define PUSH(stk_type, reg)                                \
                    memcpy(&buf, &reg, sizeof(elem_t));           \
                    res = stack_push(&(proc->stk_type), &buf); \
                    if(res != SUCCESS)                         \
                        return res;

    elem_t x = 0, x1 = 0, x2 = 0;
    char cmd = 0;
    elem_t buf = 0;
    err res;

    int running = 1;

    while(running)
    {
        //printf("\n=========================================================================\n");
        //PROC_DUMP(proc, executor)

        cmd = proc->data[proc->ip];

        //printf("\nip - %d, cmd - %d", proc->ip, cmd);

        switch(cmd & 0x1F)
        {
            case PUSH:

                if((cmd & 0xE0) == 0)
                {

                    PUSH(cmd_stk, proc->data[proc->ip + command])
                    proc->ip += (command + number);
                }

                else if((cmd & 0xE0) == 0x40)
                {
                    if(proc->data[proc->ip + command] > 4)
                        return UNKNOWN_REGISTER_NAME;

                    PUSH(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
                    proc->ip += (command + reg);
                }

                else if((cmd & 0x20) == 0x20)
                {
                    if((cmd & 0x80) == 0x80)
                    {
                        if(proc->data[proc->ip + command] > 4)
                            return UNKNOWN_REGISTER_NAME;

                        PUSH(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]])
                        proc->ip += (command + reg);
                    }
                    else
                    {

                        PUSH(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
                        proc->ip += (command + number);
                    }
                }
                break;

            case (POP & 0x1F):

                if((cmd & 0x40) == 0x40)
                {

                    POP(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
                    proc->ip += (command + reg);
                }
                else if((cmd & 0x20) == 0x20)
                {

                    POP(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
                    proc->ip += (command + number);
                }
                else if((cmd & 0xA0) == 0xA0)
                {

                    POP(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]]);
                    proc->ip += (command + reg);
                }
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
            case JMP:
                memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
                break;
            #define Define_Jumps(rub, enum, operand) \
            case enum:                        \
                POP(cmd_stk, x1) \
                POP(cmd_stk, x2) \
                if(x1 operand x2)             \
                    memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));\
                else                                          \
                    proc->ip += (command + number);            \
                break;
            #include "jumps.h"
            #undef Define_Jumps

            case CALL:
                x = proc->ip + command + number;
                PUSH(call_stk, x)
                memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
                //proc->ip = proc->data[proc->ip + command];
                break;
            case RET:
                POP(call_stk, proc->ip)
                break;

            default:
                printf("\nerr in cmd %d", cmd);
                printf("\nip - %d, cmd - %d, cmd & 0x1F - %d", proc->ip, cmd, (cmd & 0x1F));
                return UNKNOWN_COMMAND_NAME;
                break;
        }

        //STACK_DUMP(proc->cmd_stk, executor)
        //printf("\n=========================================================================\n");
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

    char* temp = (char*)calloc(fsize + 1, sizeof(char));
    if(temp == NULL)
        return CALLOC_ERROR;
    proc->data = temp;

    elem_t* temp1 = (elem_t*)calloc(100, sizeof(elem_t));
    if(temp1 == NULL)
        return CALLOC_ERROR;
    proc->RAM = temp1;

    enum err res = stack_ctor(&proc->cmd_stk, 5);
    if(res != SUCCESS)
        return res;

    res = stack_ctor(&proc->call_stk, 1);
    if(res != SUCCESS)
        return res;

    temp1 = (elem_t*)calloc(4, sizeof(elem_t));
    if(temp1 == NULL)
        return CALLOC_ERROR;
    proc->reg = temp1;

    int x = fread(proc->data, sizeof(char), fsize, read);

    proc->data_size = fsize;

   // printf("\nfsize - %d\n", fsize);
    //for(int i = 0; i < fsize; i++)
      //  printf(" %d", proc->data[i]);

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
               "dx - %d \n", proc->reg[0], proc->reg[1], proc->reg[2], proc->reg[3]);


        int i = 0;
        while(i < proc->data_size)
        {
            if(i == proc->ip)
                printf("[%d] ", proc->data[i]);

            else
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
    proc->data_size = 0;

    free(proc->reg);

    return SUCCESS;
}
