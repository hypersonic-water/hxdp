#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/hxdp.h"

#include <errno.h>


size_t bytes_per_line;
size_t cache_size;



CursorControl cursor_control = {.cur_row = 0, .cur_col = 0, .prev_row = 0};

void init_globals(Args* args) {
    bytes_per_line = args->bytes_per_line == 0 ? BYTES_PER_LINE : args->bytes_per_line;
    cache_size = args->cache_size == 0 ? CACHE_SIZE : args->cache_size;
}


int get_page_size(struct winsiz_inf* wsf) {
    if (get_termwinsize(wsf) == -1) return -1;

    int pagesize = wsf->row - 2;
    if (pagesize < 1) return -1;

    return pagesize;
}


int load_buffer(FILE* f, Cache* cache) {
    cur_pos(2, 1);
    int page_size = get_page_size(&wsf);
    if (page_size < 1) return page_size;

    const size_t nitems = bytes_per_line * page_size;

    for (int i = 0; i<cache_size; i++) {
        Page* page = malloc(sizeof(Page));
        if (page == NULL) display_error(strerror(errno));

        if (cache->head == NULL) {
            page->next = NULL;
            page->prev = NULL;
            cache->head = cache->tail = cache->curr = page;
        }
        else {
            cache->tail->next = page;
            page->prev = cache->tail;
            page->next = NULL;
            cache->tail = page;
        }

        page->data = malloc(nitems);
        if (page->data == NULL) display_error(strerror(errno));

        page->offset = nitems*i;
        size_t bytes_read = fread(page->data, 1, nitems, f);
        page->data_size = bytes_read;
        page->pagesize = page_size;

        if (bytes_read < nitems) {
            if (feof(f)) return 0;
            return -1;
        }
    }

    return 1;
}


int fetch_page_a(FILE* f, Cache* cache, size_t offset) {
    int page_size = get_page_size(&wsf);
    if (page_size < 1) return page_size;

    const size_t nitems = bytes_per_line * page_size;

    if (fseek(f, offset, SEEK_SET) != 0) {
        return -1;
    }

    size_t bytes_read = fread(cache->head->data, 1, nitems, f);
    if (bytes_read == 0) {
        cache->final_offset = cache->tail->offset;
        return 0;
    }


    Page* temp = cache->head;
    cache->head = temp->next;

    if (cache->head != NULL) {
        cache->head->prev = NULL;
    }

    temp->offset = offset;
    temp->data_size = bytes_read;
    temp->pagesize = page_size;

    temp->prev = cache->tail;
    if (cache->tail != NULL) cache->tail->next = temp;
    cache->tail = temp;
    temp->next = NULL;


    return 1;
}


int fetch_page_b(FILE* f, Cache* cache, size_t offset) {
    int page_size = get_page_size(&wsf);
    if (page_size < 1) return page_size;

    const size_t nitems = bytes_per_line * page_size;

    if (fseek(f, offset, SEEK_SET) != 0) {
        return -1;
    }

    size_t bytes_read = fread(cache->tail->data, 1, nitems, f);
    if (bytes_read == 0) {
        return 0;
    }


    Page* temp = cache->tail;
    cache->tail = temp->prev;

    if (cache->tail != NULL) {
        cache->tail->next = NULL;
    }

    temp->offset = offset;
    temp->data_size = bytes_read;
    temp->pagesize = page_size;

    temp->next = cache->head;
    if (cache->head != NULL) cache->head->prev = temp;
    cache->head = temp;
    temp->prev = NULL;


    return 1;
}

void print_line(uint8* data, size_t offset, size_t size, int col) {
    printf(CYAN "%08lx: ", offset);

    if (col >= size && col >= 0) {
        col = size-1;
        cursor_control.cur_col = col;
    }


    for (int i = 0; i<size; i++) {
        if (i == col) printf(ANSI_INVERT);

        if (data[i] >= 32 && data[i] <= 126) {
            printf(GREEN "%02x" COLOR_RESET, data[i]);
        } else {
            printf(GREY "%02x" COLOR_RESET, data[i]);
        }

        printf(ANSI_RESET);
        if (i % 2 != 0) printf(" ");
    }


    for (size_t i = size; i<bytes_per_line; i++) {
        printf("  ");
        if (i % 2 == 1) printf(" ");
    }

    printf("  ");

    for (int i = 0; i<size; i++) {
        if (i == col) printf(ANSI_INVERT);
        if (data[i] >= 32 && data[i] <= 126) {
            printf(GREEN "%c", data[i]);
        } else {
            printf(GREY ".");
        }
        printf(ANSI_RESET);
    }


    printf("\n\r");
}



int print_hexdump(Cache* cache, int row, int col) {
    cur_pos(2, 1);
    size_t tot_bytes = cache->curr->data_size;
    size_t page_size = cache->curr->pagesize;
    size_t tot_lines = (tot_bytes + bytes_per_line - 1) / bytes_per_line;

    for (size_t i = 0; i<tot_bytes; i+=bytes_per_line) {
        printf("\033[2K");
        if (row == 0) print_line(&cache->curr->data[i], cache->curr->offset+i, i+bytes_per_line <= tot_bytes ? bytes_per_line : tot_bytes - i, col);
        else print_line(&cache->curr->data[i], cache->curr->offset+i, i+bytes_per_line <= tot_bytes ? bytes_per_line : tot_bytes - i, -1);
        row--;
    }

    if (page_size*bytes_per_line > tot_bytes) {
        // int win_page_size = get_page_size(&wsf);
        // if (win_page_size < 1) return win_page_size;

        size_t win_page_size = page_size;
            for (size_t i = tot_lines; i<win_page_size; i++) {
                printf("\033[2K\n\r");
        }
    }

    cur_pos(cursor_control.cur_row+2, cursor_control.cur_col+1);
    return 1;
}


void page_down(Cache* cache, FILE* f) {
    size_t tot_lines = (cache->curr->data_size + bytes_per_line - 1) / bytes_per_line;
    if (cache->curr->next == NULL) {
        display_status("No more pages to display", cache);
        return;
    }
    cache->curr = cache->curr->next;

    cursor_control.cur_row = cursor_control.cur_col = 0;
    cur_pos(2, 1);

    if (cache->curr->next != NULL && cache->curr->next->next == NULL) {
        size_t offset = cache->tail->offset + cache->tail->data_size;
        if (offset < cache->final_offset) {
            fetch_page_a(f, cache, offset);
        }
    }

    print_hexdump(cache, cursor_control.cur_row, cursor_control.cur_col);
    cache->page_number++;
    cur_pos(cursor_control.cur_row+2, cursor_control.cur_col+1);

}


void page_up(Cache* cache, FILE* f) {
    if (cache->curr->prev == NULL) {
        display_status("Already at the first page", cache);
        return;
    }
    cache->curr = cache->curr->prev;

    cursor_control.cur_row = cursor_control.cur_col = 0;
    cur_pos(2, 1);

    if (cache->curr->prev != NULL && cache->curr->prev->prev == NULL && cache->head->offset != 0x0) {
        size_t offset = cache->head->offset - cache->head->data_size;
        fetch_page_b(f, cache, offset);
    }

    print_hexdump(cache, cursor_control.cur_row, cursor_control.cur_col);
    cache->page_number--;
    cur_pos(cursor_control.cur_row+2, cursor_control.cur_col+1);
}


void free_buffer(Cache* cache) {
    Page* temp = cache->head;
    while (temp != NULL) {
        Page* next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
    }
    cache->head = cache->tail = cache->curr = NULL;
}


void arrow_control(int key, Cache* cache) {
    size_t tot_lines = (cache->curr->data_size + bytes_per_line - 1) / bytes_per_line;
    switch (key) {
        case UP_ARROW:
            cursor_control.cur_row--;
            if (cursor_control.cur_row < 0) {cursor_control.cur_row = 0; return;}
            break;
        case DOWN_ARROW:
            cursor_control.cur_row++;
            if (cursor_control.cur_row >= tot_lines) {cursor_control.cur_row = tot_lines-1; return;}
            break;
        case LEFT_ARROW:
            cursor_control.cur_col--;
            if (cursor_control.cur_col < 0) {cursor_control.cur_col = 0; return;}
            break;
        case RIGHT_ARROW:
            cursor_control.cur_col++;
            if (cursor_control.cur_col >= bytes_per_line) {cursor_control.cur_col = bytes_per_line-1; return;}
            break;
        default:
            return;
    }


    int row = cursor_control.cur_row;
    int col = cursor_control.cur_col;


    if (cursor_control.page_number != cache->page_number) {
        cursor_control.prev_row = 0;
        cursor_control.page_number = cache->page_number;
    }

    size_t bytes_left = cache->curr->data_size - (row * bytes_per_line);
    size_t size = bytes_left < bytes_per_line ? bytes_left : bytes_per_line;

    size_t prev_bytes_left = cache->curr->data_size - (cursor_control.prev_row * bytes_per_line);
    size_t prev_size = prev_bytes_left < bytes_per_line ? prev_bytes_left : bytes_per_line;

    cur_pos(cursor_control.prev_row + 2, 1);
    print_line(&cache->curr->data[cursor_control.prev_row*bytes_per_line], cache->curr->offset + cursor_control.prev_row * bytes_per_line, prev_size,-1);
    cur_pos(cursor_control.cur_row+2, 1);
    print_line(&cache->curr->data[row*bytes_per_line], cache->curr->offset + row * bytes_per_line, size,col);
    cursor_control.prev_row = row;
}


void init_cache(Cache* cache) {
    memset(cache, 0, sizeof(Cache));
    cache->final_offset = SIZE_MAX;
}

