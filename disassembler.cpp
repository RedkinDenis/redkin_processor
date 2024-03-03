#include "disass.h"

int main(int argc, char* argv[])
{
    char* inpName = (char*)"qadr.bin";
    char* outName = (char*)"out.txt";

    check_arguments(argc, argv, inpName, outName);

    FOPEN(read, inpName, "rb")

    int fsize = GetFileSize(read);

    char* data = 0;
    fill_data(&data, read, fsize);

    FOPEN(out, outName, "wb")

    fclose(read);

    dis_assembler(out, fsize, data);

    fclose(out);

    free(data);
}

void check_arguments(int argc, char* argv[], char* inpName, char* outName)
{
    if(argc == 3)
    {
        inpName = argv[1];
        outName = argv[2];
    }
    else if(argc == 2)
    {
        inpName = argv[1];
    }
}

err fill_data(char** data, FILE* read, int fsize)
{
    CHECK_PTR(data);
    CHECK_PTR(read);

    CALLOC(*data, char, fsize + 1);

    int x = fread(*data, sizeof(char), fsize, read);

    assert(x == fsize);

    return SUCCESS;
}

err dis_assembler(FILE* out, int fsize, char* data)
{
    CHECK_PTR(out);
    CHECK_PTR(data);

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


