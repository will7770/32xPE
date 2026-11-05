#pragma once
#include <stdint.h>


typedef enum {
    ADDI,
    ADD,

    INSTR_END,
} INSTR;


typedef void (*exec_func)(uint32_t PC);