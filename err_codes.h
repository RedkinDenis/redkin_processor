#ifndef ERR_CODES_H
#define ERR_CODES_H

enum err
{
    SUCCESS = 1,           // OPERATION IS SUCCESFUL
    STEMP = 2,            // STACK IS EMPTY
    REALLERR = 3,         // REALLOC ERROR
    ALRCR = 4,             // STACK IS ALREDY CREATED
    REG_NAME_ERR = 5,
    COMM_NAME_ERR = 6
};

#endif //ERR_CODES_H
