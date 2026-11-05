#pragma once

#include <stdint.h>


typedef struct {
    uint32_t PC;
    uint32_t reg_file[8];
} CPU;


typedef struct {
    CPU cpu;
    uint8_t *memory;
} Emulator;