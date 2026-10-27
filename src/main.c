#include <windows.h>
#include <stdio.h>
#include "cpu.h"


extern char *MEMORY;

int main(void) {
    read_asm_file(L"test.txt");

    puts(MEMORY);

    VirtualFree(MEMORY, 0, MEM_FREE);
}