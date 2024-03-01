#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "input_output.h"
#include "encoding.h"
#include "C:\Users\vp717\Desktop\ilab\err_codes.h"
#include "DSL.h"
#include "stack.h"

struct processor
{
    char* data = {};
    int data_size;
    int ip = 0;
    elem_t* reg = 0;   //registers ax, bx, cx, dx
    elem_t* RAM = 0;
    struct Stack cmd_stk = {};
    struct Stack call_stk = {};
};



err fill_proc(struct processor* proc, FILE* read, int fsize);

err executor(struct processor* proc);

err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name);

err proc(struct processor* cmd_stk);

err proc_free(struct processor* proc);

int main(int argc, char* argv[])
{
    err res;
   /* res = (err)system("ass.exe qadr.txt qadr.bin");

    if(res != SUCCESS)
    {
        printf("\nerror number %d in ass.exe\n", res);
        return res;
    }
*/

    PRINT_(START)


    char* inpName = (char*)"qadr.bin";

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

    res = fill_proc(&processor, read, fsize);
    check_result(fill_proc)

    res = proc(&processor);
    check_result(proc)

    proc_free(&processor);
    check_result(proc_free)

    return SUCCESS;
}

err executor(struct processor* proc)
{
    elem_t x = 0, x1 = 0, x2 = 0;
    char cmd = 0;
    int liter;
    elem_t buf = 0;
    err res;

    elem_t temp = 0;

    int running = 1;

    while(running)
    {
        //printf("\n=========================================================================\n");
        //PROC_DUMP(proc, executor)

        cmd = proc->data[proc->ip];
        switch(cmd & 0x1F)
        {
            #define CMD_GEN(str, NAME, assm_code, proc_code, dis_code) \
            case NAME:                                                 \
                proc_code                                              \
                break;
            #include "CMD_GEN.h"

            default:
                return UNKNOWN_COMMAND_NAME;
                break;
        }

        //STACK_DUMP(proc->cmd_stk, executor)
        //printf("\n=========================================================================\n");
    }
    return SUCCESS;
}

err proc(struct processor* proc)
{
    if(proc == NULL)
        return NULL_INSTEAD_PTR;

    return executor(proc);
}

err fill_proc(struct processor* proc, FILE* read, int fsize)
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

    err res = stack_ctor(&proc->cmd_stk, 5);
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

    assert(x == fsize);
    return SUCCESS;
}

err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name)
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

        printf("ax - %f \n"
               "bx - %f \n"
               "cx - %f \n"
               "dx - %f \n", proc->reg[0], proc->reg[1], proc->reg[2], proc->reg[3]);


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
        return PROC_NOT_INIT;

    printf("ip = %d", proc->ip);

    printf("\n-------DUMP_END-------\n\n");
    return SUCCESS;
}

err proc_free(struct processor* proc)
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
