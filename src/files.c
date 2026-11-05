#include "files.h"


void get_file_map(const wchar_t *filename, FileMap *fm) {
    HANDLE fd = CreateFileW(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    LARGE_INTEGER tmp;
    GetFileSizeEx(fd, &tmp);

    HANDLE filemap = CreateFileMappingA(
        fd,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL
    );

    char* map = (char*)MapViewOfFile(filemap, FILE_MAP_READ, 0, 0, 0);

    fm->fd = fd; fm->filemap = filemap; fm->map = map; fm->mapsize = tmp.QuadPart;
}


void close_file_map(FileMap *fm) {
    UnmapViewOfFile(fm->map);
    CloseHandle(fm->filemap);
    CloseHandle(fm->fd);
}