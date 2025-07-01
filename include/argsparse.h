#ifndef ARGSPARSE_H
#define ARGSPARSE_H
#include <sys/types.h>

typedef struct {
    const char* f_name;
    size_t cache_size;
    size_t bytes_per_line;
    unsigned char safe;
    unsigned char light;
    unsigned char flags;
}Args;


void parseargs(int argc, char** argv, Args* args);


extern Args args;

#endif //ARGSPARSE_H
