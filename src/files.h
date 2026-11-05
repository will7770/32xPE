#include <windows.h>



typedef struct {
    HANDLE fd;
    HANDLE filemap;
    char* map;
    long long mapsize;
} FileMap;


void get_file_map(const wchar_t *filename, FileMap *fm);

void close_file_map(FileMap *fm);