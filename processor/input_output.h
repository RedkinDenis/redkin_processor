#ifndef INPUT_OTPUT_H
#define INPUT_OTPUT_H

struct line
{
    size_t len;
    char* str;
};

struct file
{
    char* buffer;
    struct line* lines;
    int nLines = 0;
};

void DataFree(struct file* data);

int GetFileSize(FILE* fp);

struct line* InputData(struct file* data, FILE* fp, int fsize);

void PrintData(struct line* data, int data_size);

void DataOut(FILE* fp, struct line* data, int nLines);

#endif // INPUT_OTPUT_H
