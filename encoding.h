#ifndef ENCODING_H
#define ENCODING_H

enum byte_codes
{
    ERR = 100,
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    OUT = 4,
    HET = -1,
    DIV = 5,
    MUL = 6,
    IN = 7,
    RPUSH = 16,
    POP = 22,
    JMP = 33,
    JB = 35,
    JBE = 37,
    JA = 39,
    JAE = 41,
    JE = 43,
    JNE = 45,
    CALL = 47,
    RET = 49,
    ax = 1,
    bx = 2,
    cx = 3,
    dx = 4
};

#endif //ENCODING_H
