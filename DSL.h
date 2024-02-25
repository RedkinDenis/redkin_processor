#define POP(stk_type, reg)                     \
    res = stack_pop(&(proc->stk_type), &buf);  \
    if(res != SUCCESS)                         \
       return res;                             \
    memcpy(&reg, &buf, sizeof(elem_t));

#define PUSH(stk_type, reg)                    \
    temp = (elem_t)reg;                        \
    memcpy(&buf, &temp, sizeof(elem_t));       \
    res = stack_push(&(proc->stk_type), &buf); \
    if(res != SUCCESS)                         \
        return res;

#define check_result(func)                          \
    if(res != SUCCESS)                              \
    {                                               \
        printf("\nerror in " #func " %d \n", res);  \
        return res;                                 \
    }

#define CHECK_MARK(cmd)                                       \
    if(strchr(data[i].str + strlen(cmd) + 1, ':') != NULL)        \
    {                                                             \
        num = 0;                                                  \
        for(int k = strlen(cmd); data[i].str[k - 1] != ':'; k++)  \
            num += data[i].str[k];                                \
                                                                  \
        num1 = num;                                               \
                                                                  \
        for(int j = 0; j < marks_len; j++)                        \
            if(num == marks[j].num)                               \
                num = marks[j].adress;                            \
                                                                  \
        if(num1 == num)                                           \
            return MARK_NOT_FOUND;                                \
    }                                                             \
    else                                                          \
        sscanf(data[i].str + strlen(cmd) + 1, "%d", &num);

#define PROC_DUMP(proc, func_name) proc_dump(proc, __LINE__, GET_VARIABLE_NAME(proc), __FILE__, GET_VARIABLE_NAME(func_name));

#define GET_VARIABLE_NAME(variable) #variable

#define STACK_DUMP(stk, func_name) stack_dump(&stk, __LINE__, GET_VARIABLE_NAME(stk), __FILE__, GET_VARIABLE_NAME(func_name));

#define PRINT_(START)                                                                                   \
    printf("-------------------------------------------------------------------------------------\n");  \
    printf("-----------------------------------START_OF_PROGRAMM---------------------------------\n");  \
    printf("-------------------------------------------------------------------------------------\n");
