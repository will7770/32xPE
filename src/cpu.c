#include "cpu.h"
#include <windows.h>



uint8_t *MEMORY;


void read_asm_file(const wchar_t *filename) {
    HANDLE fd = CreateFileW(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    LARGE_INTEGER filesize;
    GetFileSizeEx(fd, &filesize);

    MEMORY = VirtualAlloc(NULL, filesize.QuadPart + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    int cursor = 0;

    char buf[4096];
    DWORD read;

    do {
        ReadFile(fd, buf, sizeof(buf), &read, NULL);
        
        for(int i = 0; i < read; i++, cursor++) {
            *(MEMORY+cursor) = buf[i];
        }

    } while(read > 0);

    *(MEMORY+cursor+1) = '\0';

    CloseHandle(fd);
}