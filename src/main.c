#include <windows.h>
#include <stdio.h>
#include "cpu.h"
#include "assembler.h"


extern char *MEMORY;

int main(void) {
    assemble(L"test.txt");
}