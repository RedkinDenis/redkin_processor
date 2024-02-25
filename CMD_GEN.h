CMD_GEN("add", ADD,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(ADD, +)
})

CMD_GEN("sub", SUB,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(SUB, -)
})

CMD_GEN("div", DIV,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(DIV, /)
})

CMD_GEN("mul", MUL,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(MUL, *)
})

CMD_GEN("jb", JB,
{
    ASSM_JUMPS("jb", JB)
},
{
    PROC_JUMPS(JB, >)
})

CMD_GEN("jbe", JBE,
{
    ASSM_JUMPS("jbe", JBE)
},
{
    PROC_JUMPS(JBE, >=)
})

CMD_GEN("ja", JA,
{
    ASSM_JUMPS("ja", JA)
},
{
    PROC_JUMPS(JA, <)
})

CMD_GEN("jae", JAE,
{
    ASSM_JUMPS("jae", JAE)
},
{
    PROC_JUMPS(JAE, <=)
})

CMD_GEN("je", JE,
{
    ASSM_JUMPS("je", JE)
},
{
    PROC_JUMPS(JE, ==)
})

CMD_GEN("jne", JNE,
{
    ASSM_JUMPS("jne", JNE)
},
{
    PROC_JUMPS(JNE, !=)
})


CMD_GEN("push", PUSH,
{
    if(isdigit(*(data[i].str + strlen("push "))) || ((*(data[i].str + strlen("push ")) == '-') && isdigit(*(data[i].str + strlen("push -")))) )
    {
        sscanf(data[i].str + strlen("push "), "%d", &num);
        buffer[ptr] = PUSH;
    }

    else if(*(data[i].str + strlen("push ")) == '[' && strchr(data[i].str + strlen("push "), ']') != NULL)
        if(isdigit(*(data[i].str + strlen("push ["))))
        {
            sscanf(data[i].str + strlen("push "), "[%d]", &num);
            buffer[ptr] = RAMPUSH;
        }
        else if(*(data[i].str + strlen("push [a")) == 'x')
        {
            buffer[ptr] = RAMPUSHR;

            num = reg_det(data[i].str + strlen("push "));

            ptr += command;
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
            ptr += reg;
            break;
        }
    if(*(data[i].str + strlen("push a")) == 'x')
        {
            buffer[ptr] = RPUSH;

            num = reg_det(data[i].str + strlen("push "));

            ptr += command;
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
            ptr += reg;
            break;
        }

    ptr += command;
    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    if((cmd & 0xE0) == 0)
        {
            PUSH(cmd_stk, proc->data[proc->ip + command])
            proc->ip += (command + number);
        }

        else if((cmd & 0xE0) == 0x40)
        {
            if(proc->data[proc->ip + command] > 4)
                return UNKNOWN_REGISTER_NAME;

            PUSH(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
            proc->ip += (command + reg);
        }

        else if((cmd & 0x20) == 0x20)
        {
            if((cmd & 0x80) == 0x80)
            {
                if(proc->data[proc->ip + command] > 4)
                    return UNKNOWN_REGISTER_NAME;

                PUSH(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]])
                proc->ip += (command + reg);
            }
            else
            {
                PUSH(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
                proc->ip += (command + number);
            }
        }
})

CMD_GEN("pop", (POP & 0x1F),
{
    if(*(data[i].str + strlen("pop ")) == '[' && strchr(data[i].str + strlen("push "), ']') != NULL)
        if(isdigit(*(data[i].str + strlen("pop ["))))
        {
            sscanf(data[i].str + strlen("pop "), "[%d]", &num);
            buffer[ptr] = RAMPOP;
        }
        else if(*(data[i].str + strlen("pop [a")) == 'x')
        {
            buffer[ptr] = RAMRPOP;

            num = reg_det(data[i].str + strlen("pop "));

            ptr += command;
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
            ptr += reg;
            break;
        }
    if(*(data[i].str + strlen("pop a")) == 'x')
        {
            buffer[ptr] = POP;

            num = reg_det(data[i].str + strlen("pop "));

            ptr += command;
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
            ptr += reg;
            break;
        }
    ptr += command;
    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
    ptr += number;
},
{
    if((cmd & 0x40) == 0x40)
        {

            POP(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
            proc->ip += (command + reg);
        }
        else if((cmd & 0x20) == 0x20)
        {

            POP(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
            proc->ip += (command + number);
        }
        else if((cmd & 0xA0) == 0xA0)
        {

            POP(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]]);
            proc->ip += (command + reg);
        }
})

CMD_GEN("out", OUT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    POP(cmd_stk, x)
    proc->ip += command;
    printf("\nOUT - %f\n", x);
})

CMD_GEN("sqrt", SQRT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    POP(cmd_stk, x)
    x = (elem_t)sqrt(x);
    proc->ip += command;
    PUSH(cmd_stk, x)
})

CMD_GEN("in", IN,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    printf("\nenter push element ");
    scanf("%d", &x);
    PUSH(cmd_stk, x)
    proc->ip += command;
})

CMD_GEN("hlt", HLT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    stack_dtor(&(proc->cmd_stk));
    proc->ip += command;
    running = 0;
})

CMD_GEN("outc", OUTC,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    POP(cmd_stk, temp)
    liter = (int)temp;
    proc->ip += command;
    printf("%c", liter);
})

CMD_GEN("call", CALL,
{
    buffer[ptr] = CALL;
    ptr += command;

    CHECK_MARK("call")

    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    x = proc->ip + command + number;
    PUSH(call_stk, x)
    memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
})

CMD_GEN("ret", RET,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    POP(call_stk, temp)
    proc->ip = (int)temp;
})

CMD_GEN("jump", JMP,
{
    buffer[ptr] = JMP;
    ptr += command;

    CHECK_MARK("call")

    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
})

