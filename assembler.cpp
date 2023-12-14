#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"

int assembler(FILE* out, struct line* data, int nLines);

enum byte_codes comm_det(const char* comm);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"word_code.txt";
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
    {
        printf("read file open error");
        return 0;
    }

    int fsize = GetFileSize(read);

    struct file data = {};

    InputData(&data, read, fsize);
    //printf("out - %s", outName);

    FILE* out = fopen(outName, "wb");
    if(out == NULL)
    {
        printf("out file open error");
        return 0;
    }

    fclose(read);

    //PrintData(data, n);

    assembler(out, data.lines, data.nLines);

    DataFree(&data);

    fclose(out);
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

int assembler(FILE* out, struct line* data, int nLines)
{
    int comm = 0;
    int num = 0;
    int len = nLines;

    int ptr = 0;

    for(int i = 0; i < nLines; i++)
    {
        for(int k = 0; k < data[i].len; k++)
            if(data[i].str[k] == ' ')
            {
                data[i].str[k] = '\0';
                len++;
            }
    }

    printf("\nlen - %d\n", len);

    int* buffer = (int*)calloc(len, sizeof(int));

    for(int i = 0; i < nLines; i++)
    {
        comm = comm_det(data[i].str);
        switch(comm)
        {
            case PUSH:
                buffer[ptr] = PUSH;
                ptr++;
                sscanf(data[i].str + strlen("push "), "%d", &num);
                buffer[ptr] = num;
                ptr++;
                break;
            case POP:
                buffer[ptr] = POP;
                ptr++;
                buffer[ptr] = comm_det(data[i].str + strlen("pop "));
                ptr++;
                break;
            case RPUSH:
                buffer[ptr] = RPUSH;
                ptr++;
                buffer[ptr] = comm_det(data[i].str + strlen("rpush "));
                ptr++;
                break;
            case JMP:
                buffer[ptr] = JMP;
                ptr++;
                sscanf(data[i].str + strlen("jump") + 1, "%d", &num);
                buffer[ptr] = num;
                ptr++;
                break;
            #define Define_Jumps(string, enum, rub)                     \
            case enum:                                                  \
                buffer[ptr] = enum;                                     \
                ptr++;                                                  \
                sscanf(data[i].str + strlen(string) + 1, "%d", &num);   \
                buffer[ptr] = num;                                      \
                ptr++;                                                  \
                break;
            #include "jumps.h"
            #undef Define_Jumps

            case ERR:
                return ERR;
            default:
                buffer[ptr] = comm;
                ptr++;
                break;
        }
    }

    for(int i = 0; i < len; i++)
        printf(" %d", buffer[i]);

    fwrite(buffer, sizeof(int), len, out);
}
