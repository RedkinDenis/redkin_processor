#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"

struct line* dis_assembler(FILE* out, struct line* data, int nLines);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"byte_code.txt";
    char* outName = (char*)"word_code.txt";

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

    int n = 0;
    struct line* data = InputData(read, fsize, &n);

    //printf("out - %s", outName);

    FILE* out = fopen(outName, "w");
    if(out == NULL)
    {
        printf("out file open error");
        return 0;
    }

    fclose(read);

    //PrintData(data, n);

    dis_assembler(out, data, n);

    fclose(out);
}

struct line* dis_assembler(FILE* out, struct line* data, int nLines)
{
    int j = 0;
    int num = 0;
    int num1 = 0;

    for(int i = 0; i < nLines; i++)
    {
        sscanf(data[i].str, "%d %d", &num, &num1);

        switch(num)
        {
            case PUSH:
                fprintf(out, "%s %d\n", "push", num1);                     // перенести свитчи в функцию
                break;
            case ADD:
                fprintf(out, "%s\n", "add");
                break;
            case SUB:
                fprintf(out, "%s\n", "sub");
                break;
            case OUT:
                fprintf(out, "%s\n", "out");
                break;
            case HET:
                fprintf(out, "%s\n", "het");
                break;
            case DIV:
                fprintf(out, "%s\n", "div");
                break;
            case MUL:
                fprintf(out, "%s\n", "mul");
                break;
            case IN:
                fprintf(out, "%s\n", "in");
                break;
            case RPUSH:
                switch(num1)
                    case ax:
                        fprintf(out, "%s\n", "rpush ax");
                        break;
                    case bx:
                        fprintf(out, "%s\n", "rpush bx");
                        break;
                    case cx:
                        fprintf(out, "%s\n", "rpush cx");
                        break;
                    case dx:
                        fprintf(out, "%s\n", "rpush dx");
                        break;
                    default:
                        printf("unknown command");
                        return;
                        break;
                break;
            case POP:
                switch(num1)
                    case ax:
                        fprintf(out, "%s\n", "pop ax");
                        break;
                    case bx:
                        fprintf(out, "%s\n", "pop bx");
                        break;
                    case cx:
                        fprintf(out, "%s\n", "pop cx");
                        break;
                    case dx:
                        fprintf(out, "%s\n", "pop dx");
                        break;
                    default:
                        printf("unknown command");
                        return;
                        break;
                break;
            default:
                printf("unknown command");
                return;
                break;
        }
    }
}

