#ifndef ERR_CODES_H
#define ERR_CODES_H

enum err
{
    SUCCESS = 1,           // OPERATION IS SUCCESFUL
    STK_EMPTY = 2,            // STACK IS EMPTY
    REALLOC_ERROR = 3,
    STACK_ALREDY_CREATED = 4,
    UNKNOWN_REGISTER_NAME = 5,
    UNKNOWN_COMMAND_NAME = 6,
    NULL_INSTEAD_PTR = 7,
    CALLOC_ERROR = 8,
    OPEN_ERROR = 9
};

#endif //ERR_CODES_H
