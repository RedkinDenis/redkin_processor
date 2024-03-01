#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "input_output.h"
#include "encoding.h"
#include "C:\Users\vp717\Desktop\ilab\err_codes.h"
#include "stack.h"
#include "DSL.h"

enum err assembler(FILE* out, struct line* data, int nLines);

enum byte_codes comm_det(const char* comm);

#define MARK_QUANTITY 20

struct mark
{
    int num = 0;
    int adress = 0;
};

int main(int argc, char* argv[])
{
    char* inpName = (char*)"word_code1.txt";
    char* outName = (char*)"word_code1.bin";

    if(argc == 3)
    {
        inpName = argv[1];
        outName = argv[2];
    }
    else if(argc == 2)
    {
        inpName = argv[1];
    }

    FILE* read = fopen(inpName, "rb");
    if(read == NULL)
        return OPEN_ERROR;

    int fsize = GetFileSize(read);

    struct file data = {};

    InputData(&data, read, fsize);

    FILE* out = fopen(outName, "wb");
    if(out == NULL)
        return OPEN_ERROR;

    fclose(read);

    enum err res = assembler(out, data.lines, data.nLines);
    if(res != SUCCESS)
        return res;

    DataFree(&data);

    fclose(out);

    return SUCCESS;
}

enum byte_codes comm_det(const char* comm)
{
    #define Define_Command(str, enum) \
    if(strcmp(comm, str) == 0)        \
        return enum;
    #include "command.h"
    #undef Define_Command

    return ERR;
}

enum byte_codes reg_det(const char* reg)
{
    #define Define_Command(str, enum) \
    if(strncmp(reg, str, 2) == 0)    \
        return enum;
    #include "registers.h"
    #undef Define_Command

    return ERR;
}

enum err assembler(FILE* out, struct line* data, int nLines)
{
    if(out == NULL || data == NULL)
        return NULL_INSTEAD_PTR;

    int comm = 0;
    int num = 0;
    int num1 = 0;
    int len = nLines;

    int ptr = 0;


    for(int i = 0; i < nLines; i++)
    {
        while(*(data[i].str) == ' ')
        {
            data[i].str++;
            data[i].len -= 1;
        }


        for(int k = 0; k < data[i].len; k++)
        {
            if(data[i].str[k] == ' ')
            {
                data[i].str[k] = '\0';
                len++;
                k = data[i].len;
            }
            if(data[i].str[k] == '$' && (k != 0))
            {
                while(k < data[i].len)
                {
                    data[i].str[k] = '\0';
                    k++;
                }
            }
        }
    }


    struct mark* marks = (struct mark*)calloc(MARK_QUANTITY, sizeof(struct mark));
    int marks_len = 0;
    int n = 0;

    char* buffer = (char*)calloc(len * 4, sizeof(char));

    for(int i = 0; i < nLines; i++)
    {

        comm = comm_det(data[i].str);
        if(comm == ADD || comm == SUB || comm == MUL || comm == DIV || comm == HLT || comm == OUT || comm == RET || comm == OUTC || comm == SQRT || comm == IN)
            ptr += command;

        else if(comm == ERR)
        {
            if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL)
            {
                for(int j = 0; data[i].str[j - 1] != ':'; j++)
                    marks[marks_len].num += data[i].str[j];

                marks[marks_len].adress = ptr;
                marks_len++;
            }
        }
        else
        {
            if((strstr(data[i].str + strlen(data[i].str) + 1, "ax") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "bx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "cx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "dx") != NULL))
                ptr += (command + reg);
            else
                ptr += (command + number);
        }
    }

    ptr = 0;
    for(int i = 0; i < nLines; i++)
    {
        comm = comm_det(data[i].str);

        switch(comm & 0x1F)
        {
            #define CMD_GEN(str, NAME, assm_code, proc_code, dis_code)  \
            case NAME:                                                  \
                assm_code                                               \
                break;
            #include "CMD_GEN.h"

            case ERR:
                if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL || strlen(data[i].str) == 0)
                    break;
                //printf("%d", comm);
                return UNKNOWN_COMMAND_NAME;
        }
    }

    for(int i = 0; i < ptr; i++)
        printf(" %d", (uint8_t)buffer[i]);
    printf("\n");
    fwrite(buffer, sizeof(char), ptr, out);

    return SUCCESS;
}
