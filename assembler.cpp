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
    char* inpName = (char*)"qadr.txt";
    char* outName = (char*)"qadr.bin";

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
    //printf("out - %s", outName);

    FILE* out = fopen(outName, "wb");
    if(out == NULL)
        return OPEN_ERROR;

    fclose(read);

    //PrintData(data, n);

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
                //data[i].str[k + 1] = '$';

                while(k < data[i].len)
                {
                    data[i].str[k] = '\0';
                    k++;
                }
            }
        }
    }

    //printf("\nlen - %d\n", len);

    struct mark* marks = (struct mark*)calloc(MARK_QUANTITY, sizeof(struct mark));
    int marks_len = 0;
    int n = 0;

    char* buffer = (char*)calloc(len * 4, sizeof(char));
    //printf("len - %d\n", len);


    for(int i = 0; i < nLines; i++)
    {

        comm = comm_det(data[i].str);
        //printf("cmd - %d    ", comm);
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
            /*for(int j = 0; j < data[i].len; j++)
                printf("%c", data[i].str[j]);
            printf("\n");*/
            //printf("\nhere\n");

            if((strstr(data[i].str + strlen(data[i].str) + 1, "ax") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "bx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "cx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "dx") != NULL))
            //if(strchr(data[i].str + strlen(data[i].str) + 1, 'x') != NULL)
            {
                //printf("\nhere\n");
                ptr += (command + reg);
            }
            else
                ptr += (command + number);
        }
        //printf("ptr - %d\n", ptr);
    }

    //for(int i = 0; i < marks_len; i++)
    //    printf("\n\nmarks[%d]: \nnum - %d\nadress - %d\n", i, marks[i].num, marks[i].adress);

    ptr = 0;
    for(int i = 0; i < nLines; i++)
    {
        //printf("i - %d, ", i);

        comm = comm_det(data[i].str);

        //printf("cmd - %d \n", comm);

        switch(comm)
        {
            case PUSH:
                //printf("%d ", isdigit(*(data[i].str + strlen("push "))));
                if(isdigit(*(data[i].str + strlen("push "))))
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
                        //printf("num - %d\n", num);
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

    #define CHECK_MARK(cmd)                                  \
    if(strchr(data[i].str + strlen(cmd) + 1, ':') != NULL)                  \
    {                                                       \
        num = 0;                                             \
        for(int k = strlen(cmd); data[i].str[k - 1] != ':'; k++)       \
            num += data[i].str[k];                           \
                                                             \
        num1 = num;                                          \
                                                             \
        for(int j = 0; j < marks_len; j++)                   \
            if(num == marks[j].num)                          \
                num = marks[j].adress;                       \
                                                             \
        if(num1 == num)                                      \
            return MARK_NOT_FOUND;                           \
    }                                                        \
    else                                                     \
        sscanf(data[i].str + strlen(cmd) + 1, "%d", &num);

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

    #undef CHAECK_MARK

            case ERR:
                if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL || strlen(data[i].str) == 0)
                    break;
                return UNKNOWN_COMMAND_NAME;

            default:
                /*if(comm == HLT)
                    printf("%d\n", data[i].len);*/
                buffer[ptr] = (char)comm;
                ptr += command;
                break;
        }
    }

    //printf("ptr - %d\n", ptr);

    for(int i = 0; i < ptr; i++)
        printf(" %d", (uint8_t)buffer[i]);
    printf("\n");
    fwrite(buffer, sizeof(char), ptr, out);
}
