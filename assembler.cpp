#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"
#include "err_codes.h"

enum err assembler(FILE* out, struct line* data, int nLines);

enum byte_codes comm_det(const char* comm);

enum step
{
    command = 1,
    number = 1
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
    char* outName = (char*)"byte_code1.bin";

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
        for(int k = 0; k < data[i].len; k++)
        {
            if(data[i].str[k] == ' ')
            {
                data[i].str[k] = '\0';
                len++;
                k = data[i].len;
            }
            if(data[i].str[k] == '$')    // символ начала комментария
            {
                data[i].str[k] = '\0';
                k = data[i].len;
            }
        }
    }

    //printf("\nlen - %d\n", len);

    struct mark* marks = (struct mark*)calloc(MARK_QUANTITY, sizeof(struct mark));
    int marks_len = 0;
    int temp = 1;
    int n = 0;

    int* buffer = (int*)calloc(len, sizeof(int));


    for(int i = 0; i < nLines; i++)
    {
        comm = comm_det(data[i].str);

        if(comm == ADD || comm == SUB || comm == MUL || comm == DIV || comm == HET || comm == OUT || comm == RET)
            temp++;

        else if(comm == ERR)
        {
            if(data[i].str[data[i].len - 1] == ':')
            {
                for(int j = 0; j < data[i].len; j++)
                    marks[marks_len].num += data[i].str[j];

                marks[marks_len].adress = temp;
                marks_len++;

                //printf("\nmark %d\n", temp);
            }
        }

        else
            temp += 2;
    }

    //for(int i = 0; i < marks_len; i++)
    //    printf("\n\nmarks[%d]: \nnum - %d\nadress - %d\n", i, marks[i].num, marks[i].adress);


    for(int i = 0; i < nLines; i++)
    {
        comm = comm_det(data[i].str);
        switch(comm)
        {
            case PUSH:
                buffer[ptr] = PUSH;
                ptr += command;
                sscanf(data[i].str + strlen("push "), "%d", &num);
                buffer[ptr] = num;
                ptr += number;
                break;
            case POP:
                buffer[ptr] = POP;
                ptr += command;
                buffer[ptr] = comm_det(data[i].str + strlen("pop "));
                ptr += number;
                break;
            case RPUSH:
                buffer[ptr] = RPUSH;
                ptr += command;
                buffer[ptr] = comm_det(data[i].str + strlen("rpush "));
                ptr += number;
                break;
            case JMP:
                buffer[ptr] = JMP;
                ptr += command;
                sscanf(data[i].str + strlen("jump") + 1, "%d", &num);
                buffer[ptr] = num;
                ptr += number;
                break;
            case CALL:
                buffer[ptr] = CALL;
                ptr += command;

                if(data[i].str[data[i].len - 1] == ':')
                {
                    num = 0;
                    for(int k = strlen("call"); k < data[i].len; k++)
                        num += data[i].str[k];

                    num1 = num;

                    for(int j = 0; j < marks_len; j++)
                        if(num == marks[j].num)
                            num = marks[j].adress;

                    if(num1 == num)
                        return MARK_NOT_FOUND;
                }
                else
                    sscanf(data[i].str + strlen("call "), "%d", &num);

                buffer[ptr] = num;
                ptr += number;
                break;
            #define Define_Jumps(string, enum, rub)                     \
            case enum:                                                  \
                buffer[ptr] = enum;                                     \
                ptr += command;                                         \
                sscanf(data[i].str + strlen(string) + 1, "%d", &num);   \
                buffer[ptr] = num;                                      \
                ptr += number;                                          \
                break;
            #include "jumps.h"
            #undef Define_Jumps

            case ERR:
                if(data[i].str[data[i].len - 1] == ':')
                    break;
                return UNKNOWN_COMMAND_NAME;

            default:
                buffer[ptr] = comm;
                ptr += command;
                break;
        }
    }

    for(int i = 0; i < len; i++)
        printf(" %d", buffer[i]);

    fwrite(buffer, sizeof(int), len, out);
}


