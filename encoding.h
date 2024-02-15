#ifndef ENCODING_H
#define ENCODING_H

enum byte_codes
{
    ERR = 100,
    PUSH = 1,                 // 2
    ADD = 2,                  // 1
    SUB = 3,                  // 1
    OUT = 4,                  // 1
    HET = -1,                 // 1
    DIV = 5,                  // 1
    MUL = 6,                  // 1
    IN = 7,                   // 2
    RPUSH = 16,               // 2
    POP = 22,                 // 2
    JMP = 33,                 // 2
    JB = 35,// 2
    JBE = 37,
    JA = 39,// 2
    JAE = 41,// 2
    JE = 43,// 2
    JNE = 45,// 2
    CALL = 47,// 2
    RET = 49,// 1
    ax = 1,
    bx = 2,
    cx = 3,
    dx = 4
};

#endif //ENCODING_H
