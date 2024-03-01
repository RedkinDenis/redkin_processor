#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"
#include "DSL.h"
#include "C:\Users\vp717\Desktop\ilab\err_codes.h"

err fill_data(char** data, FILE* read, int fsize);

err dis_assembler(FILE* out, int fsize, char* data);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"qadr.bin";
    char* outName = (char*)"out.txt";

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

    char* data = 0;
    fill_data(&data, read, fsize);



    FILE* out = fopen(outName, "wb");
    if(out == NULL)
    {
        printf("out file open error");
        return 0;
    }

    fclose(read);

    dis_assembler(out, fsize, data);

    fclose(out);
}

err fill_data(char** data, FILE* read, int fsize)
{
    if (data == NULL || read == NULL)
        return NULL_INSTEAD_PTR;

    char* data_temp = (char*)calloc(fsize + 1, sizeof(char));
    if (data_temp == NULL)
        return CALLOC_ERROR;

    *data = data_temp;

    int x = fread(*data, sizeof(char), fsize, read);

    assert(x == fsize);

    return SUCCESS;
}

err dis_assembler(FILE* out, int fsize, char* data)
{
    if (data == NULL || data == NULL)
        return NULL_INSTEAD_PTR;

    int ptr = 0;
    char cmd = data[ptr];

    int num = 0;

    while(ptr < fsize)
    {
        switch((data[ptr]) & 0x1F)
        {
            #define CMD_GEN(str, NAME, assm_code, proc_code, dis_code) \
            case NAME:                                                 \
                fprintf(out, str);                                     \
                ptr += command;                                        \
                dis_code                                               \
                fprintf(out, "\n");                                    \
                break;
            #include "CMD_GEN.h"
            default:
                return UNKNOWN_COMMAND_NAME;
        }
        cmd = data[ptr];
    }
    return SUCCESS;
}


