#ifndef DUMP_H
#define DUMP_H

#define BYTES_PER_LINE 16
#define CACHE_SIZE 25
#define FETCH_AHEAD 5
#define FETCH_BEHIND 2

#include <stdio.h>
#include "argsparse.h"

typedef unsigned char uint8;

typedef struct Page{
    uint8* data;
    size_t offset;
    int pagesize;
    size_t data_size;
    struct Page* next;
    struct Page* prev;
} Page;


typedef struct Cache{
    Page* head;
    Page* tail;
    Page* curr;
    size_t final_offset;
    size_t page_number;
} Cache;

typedef struct {
    int cur_row;
    int cur_col;
    int prev_row;
    size_t page_number;
}CursorControl;


int get_page_size(struct winsiz_inf* wsf);
int load_buffer(FILE* f, Cache* cache);
int print_hexdump(Cache* cache, int row, int col);
void page_down(Cache* cache, FILE* f);
void page_up(Cache* cache, FILE* f);
void free_buffer(Cache* cache);
void init_cache(Cache* cache);
void arrow_control(int key, Cache* cache);
void init_globals(Args* args);

extern CursorControl cursor_control;

#endif //DUMP_H
