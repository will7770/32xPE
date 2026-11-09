#include "files.h"
#include <stdio.h>

void get_file_map(const wchar_t *filename, FileMap *fm, bool open_w_nullterm) {
    DWORD file_flags, mapping_flags, view_flags;

    file_flags = GENERIC_READ;
    if (open_w_nullterm) { file_flags |= GENERIC_WRITE; }

    HANDLE fd = CreateFileW(
        filename,
        file_flags,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (fd == INVALID_HANDLE_VALUE) {
        DWORD dwError = GetLastError();
        wprintf(L"Could not open file. Error code: 0x%X\n", dwError);
        return;
    }
    
    LARGE_INTEGER filesize;
    GetFileSizeEx(fd, &filesize);

    if (open_w_nullterm) {
        mapping_flags = PAGE_READWRITE;
        view_flags = FILE_MAP_COPY;
    }
    else {
        mapping_flags = PAGE_READONLY;
        view_flags = FILE_MAP_READ;
    }

    HANDLE filemap = CreateFileMappingW(
        fd,
        NULL,
        mapping_flags,
        filesize.HighPart,
        filesize.LowPart,
        NULL
    );

    if (filemap == NULL) {
        DWORD dwError = GetLastError();
        wprintf(L"File could not be mapped. Error code: 0x%X\n", dwError);
        CloseHandle(fd);
        return;
    }

    // will fail horribly if file size is divisible by page size. lets hope it isnt
    char* map = (char*)MapViewOfFile(filemap, view_flags, 0, 0, 0);
    if (open_w_nullterm) {
        *(map+filesize.QuadPart) = '\0';
    }

    fm->fd = fd; fm->filemap = filemap; fm->map = map; fm->mapsize = filesize.QuadPart;
}


void close_file_map(FileMap *fm) {
    UnmapViewOfFile(fm->map);
    CloseHandle(fm->filemap);
    CloseHandle(fm->fd);
}