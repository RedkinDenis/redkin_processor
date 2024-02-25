#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "input_output.h"
#include "encoding.h"
#include "err_codes.h"
#include "stack.h"
#include "DSL.h"

enum err assembler(FILE* out, struct line* data, int nLines);

enum byte_codes comm_det(const char* comm);

enum step
{
    command = sizeof(char),
    number = sizeof(elem_t),
    reg = sizeof(char)
};

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
        if(comm == ADD || comm == SUB || comm == MUL || comm == DIV || comm == HLT || comm == OUT || comm == RET || comm == OUTC || comm == SQRT)
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

        switch(comm)
        {
            case PUSH:
                if(isdigit(*(data[i].str + strlen("push "))) || ((*(data[i].str + strlen("push ")) == '-') && isdigit(*(data[i].str + strlen("push -")))) )
                {
                    sscanf(data[i].str + strlen("push "), "%d", &num);
                    buffer[ptr] = PUSH;
                }

                else if(*(data[i].str + strlen("push ")) == '[' && strchr(data[i].str + strlen("push "), ']') != NULL)
                    if(isdigit(*(data[i].str + strlen("push ["))))
                    {
                        sscanf(data[i].str + strlen("push "), "[%d]", &num);
                        buffer[ptr] = RAMPUSH;
                    }
                    else if(*(data[i].str + strlen("push [a")) == 'x')
                    {
                        buffer[ptr] = RAMPUSHR;
                        switch(*(data[i].str + strlen("push [")))
                        {
                            case 'a':
                                num = ax;
                                break;
                            case 'b':
                                num = bx;
                                break;
                            case 'c':
                                num = cx;
                                break;
                            case 'd':
                                num = dx;
                                break;
                            default:
                                return UNKNOWN_REGISTER_NAME;
                        }
                        ptr += command;
                        memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
                        ptr += reg;
                        break;
                    }
                if(*(data[i].str + strlen("push a")) == 'x')
                    {
                        buffer[ptr] = RPUSH;
                        switch(*(data[i].str + strlen("push ")))
                        {
                            case 'a':
                                num = ax;
                                break;
                            case 'b':
                                num = bx;
                                break;
                            case 'c':
                                num = cx;
                                break;
                            case 'd':
                                num = dx;
                                break;
                            default:
                                return UNKNOWN_REGISTER_NAME;
                        }
                        ptr += command;
                        memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
                        ptr += reg;
                        break;
                    }

                ptr += command;
                memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
                ptr += number;
                break;

            case POP:
                if(*(data[i].str + strlen("pop ")) == '[' && strchr(data[i].str + strlen("push "), ']') != NULL)
                    if(isdigit(*(data[i].str + strlen("pop ["))))
                    {
                        sscanf(data[i].str + strlen("pop "), "[%d]", &num);
                        buffer[ptr] = RAMPOP;
                    }
                    else if(*(data[i].str + strlen("pop [a")) == 'x')
                    {
                        buffer[ptr] = RAMRPOP;
                        switch(*(data[i].str + strlen("pop [")))
                        {
                            case 'a':
                                num = ax;
                                break;
                            case 'b':
                                num = bx;
                                break;
                            case 'c':
                                num = cx;
                                break;
                            case 'd':
                                num = dx;
                                break;
                            default:
                                return UNKNOWN_REGISTER_NAME;
                        }
                        ptr += command;
                        memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
                        ptr += reg;
                        break;
                    }
                if(*(data[i].str + strlen("pop a")) == 'x')
                    {
                        buffer[ptr] = POP;
                        switch(*(data[i].str + strlen("pop ")))
                        {
                            case 'a':
                                num = ax;
                                break;
                            case 'b':
                                num = bx;
                                break;
                            case 'c':
                                num = cx;
                                break;
                            case 'd':
                                num = dx;
                                break;
                            default:
                                return UNKNOWN_REGISTER_NAME;
                        }
                        ptr += command;
                        memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
                        ptr += reg;
                        break;
                    }
                ptr += command;
                memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
                ptr += number;
                break;

            case JMP:
                buffer[ptr] = JMP;
                ptr += command;

                CHECK_MARK("call")

                memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
                ptr += number;
                break;

            case CALL:
                buffer[ptr] = CALL;
                ptr += command;

                CHECK_MARK("call")

                memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
                ptr += number;
                break;

            #define Define_Jumps(string, enum, rub)                             \
            case enum:                                                          \
                buffer[ptr] = enum;                                             \
                ptr += command;                                                 \
                CHECK_MARK(string)                                              \
                memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));    \
                ptr += number;                                                  \
                break;
            #include "jumps.h"
            #undef Define_Jumps

            case ERR:
                if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL || strlen(data[i].str) == 0)
                    break;
                return UNKNOWN_COMMAND_NAME;

            default:
                buffer[ptr] = (char)comm;
                ptr += command;
                break;
        }
    }

    for(int i = 0; i < ptr; i++)
        printf(" %d", (uint8_t)buffer[i]);
    printf("\n");
    fwrite(buffer, sizeof(char), ptr, out);

    return SUCCESS;
}
