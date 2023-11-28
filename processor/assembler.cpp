#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"

void assembler(FILE* out, struct line* data, int nLines);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"word_code.txt";
    char* outName = (char*)"byte_code1.txt";

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

    FILE* out = fopen(outName, "w");
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

void assembler(FILE* out, struct line* data, int nLines)
{
    int j = 0;

    int num = 55;
    int len = nLines;

    int ptr = 0;

    for(int i = 0; i < nLines; i++)
    {
        for(int k = 0; k < data[i].len; k++)
            if(data[i].str[k] == ' ')
                len++;
    }

    //printf("\nlen - %d\n", len);

    char* buffer = (char*)calloc(len, sizeof(char));

    for(int i = 0; i < nLines; i++)
    {
        switch(data[i].str[0])
        {
            case 'p':
                switch(data[i].str[1])
                {
                    case 'u':
                        buffer[ptr] = PUSH;
                        ptr++;
                        sscanf(data[i].str + strlen("push"), "%d", &num);
                        buffer[ptr] = num;
                        ptr++;
                        break;
                    case 'o':
                        buffer[ptr] = POP;
                        ptr++;

                        switch(data[i].str[4])
                        {
                            case 'a':
                                buffer[ptr] = ax;
                                ptr++;
                                break;
                            case 'b':
                                buffer[ptr] = bx;
                                ptr++;
                                break;
                            case 'c':
                                buffer[ptr] = cx;
                                ptr++;
                                break;
                            case 'd':
                                buffer[ptr] = dx;
                                ptr++;
                                break;
                        }
                        break;
                }
                break;
            case 'a':
                buffer[ptr] = ADD;
                ptr++;
                break;
            case 's':
                buffer[ptr] = SUB;
                ptr++;
                break;
            case 'o':
                buffer[ptr] = OUT;
                ptr++;
                break;
            case 'h':
                buffer[ptr] = HET;
                ptr++;
                break;
            case 'd':
                buffer[ptr] = DIV;
                ptr++;
                break;
            case 'm':
                buffer[ptr] = MUL;
                ptr++;
                break;
            case 'i':
                buffer[ptr] = IN;
                ptr++;
                break;
            case 'r':
                buffer[ptr] = RPUSH;
                ptr++;
                switch(data[i].str[6])
                {
                    case 'a':
                        buffer[ptr] = ax;
                        ptr++;
                        break;
                    case 'b':
                        buffer[ptr] = bx;
                        ptr++;
                        break;
                    case 'c':
                        buffer[ptr] = cx;
                        ptr++;
                        break;
                    case 'd':
                        buffer[ptr] = dx;
                        ptr++;
                        break;
                }
                break;
            default:
                printf("unknown command");
                return;
                break;
        }
    }

    for(int i = 0; i < len; i++)
        printf(" %d", buffer[i]);

    fputs(buffer, out);

}
