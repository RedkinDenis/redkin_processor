#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"

int GetFileSize(FILE* fp)
{
    int startPos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, startPos, SEEK_SET);
    return fsize;
}

void DataFree(struct file* data)
{
    free(data->buffer);
    free(data->lines);
    data->nLines = 0;
}

struct line* InputData(struct file* Data, FILE* fp, int fsize)
{
    char* buffer = (char*)calloc(fsize + 1, sizeof(char));
    int nLines = 0;
    Data->buffer = buffer;

    int rsize = fread(buffer, sizeof(char), fsize, fp);
    assert(rsize == fsize);

    if(buffer[rsize - 1] != '\n')
        buffer[rsize] = '\n';
    else
        buffer[rsize] = '\0';

    for(int i = 0; i < rsize + 1; i++)
    {
        if(buffer[i] == '\r')
        {
            buffer[i] = '\0';
            i += 1;
            buffer[i] = '\0';
            nLines++;
        }
        else if(buffer[i] == '\n')
        {
            nLines++;
            buffer[i] = '\0';
        }
    }

/*    printf("nLines - %d\n", nLines);*/
    struct line* data = (struct line*)calloc(nLines, sizeof(struct line));

    int j = 0;
    int oldind = 0;

    data[j].str = buffer;
    j++;
    for(int i = 0; i < rsize + 1; i++)
    {
        if(buffer[i] == '\0')
        {
            if(j <= nLines)
            {
                data[j].str = buffer + i + 1;
                data[j - 1].len = i - oldind;
                oldind = i + 1;

                if(buffer[i + 1] == '\0')
                {
                    data[j].str++;
                    i++;
                    oldind++;
                }
                j++;
            }
        }
    }
    /*for(int i = 0; i < nLines; i++)
    {
        printf("%d - %d\n", i, data[i].len);
    }*/

    Data->nLines = nLines;
    Data->lines = data;
}

void clear_tabs(struct line* data, int nLines)
{
    for(int i = 0; i < nLines; i++)
        while(data[i].str[0] == ' ')
        {
            data[i].str = '\0';
            data[i].str++;
        }
}

void PrintData(struct line* data, int data_size)
{
    printf("----Print_Data----");
    for(int i = 0; i < data_size; i++)
        puts(data[i].str);
    printf("--------end-------");
}

void DataOut(FILE* fp, struct line* data, int nLines)
{
    for(int i = 0; i < nLines; i++)
        fprintf(fp, "%s\n", data[i].str);
}
