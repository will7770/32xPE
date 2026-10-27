#pragma once

#include <stdint.h>


typedef struct CPU {
    uint32_t PC;
    uint32_t X1;
    uint32_t X2;
    uint32_t X3;
    uint32_t X4;
} CPU;


void read_asm_file(const wchar_t *filename);