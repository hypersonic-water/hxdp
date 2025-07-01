#include <stdlib.h>

#include "../include/hxdp.h"


void display_error(const char* msg) {
    draw_message_box(msg);

     do {} while (getchar() != '\r'); exit(EXIT_FAILURE);
}

void display_status(const char* msg, Cache* cache) {
    clrscr();

    draw_message_box(msg);
    do {} while (getchar() != '\r');

    clrscr();
    print_hexdump(cache, cursor_control.cur_row, cursor_control.cur_col);
}

void display_config_messages(Args* args) {
    uint8 flags = args->flags;
    if (flags & 1<<1) {
        printf(CYAN"[Safe Mode] Maximum Bytes Per Line set to 24\n\r"COLOR_RESET);
    }
    if (flags & 1<<2) {
        printf(CYAN"[Safe Mode] Maximum Cache Size set to 10000\n\r"COLOR_RESET);
    }
    if (flags & 1<<3) {
        printf(YELLOW"[Warning] Cache size is below the minimum allowed (5). Cache Size set to 5.\n\r"COLOR_RESET);
    }
    if (flags & 1<<4) {
        printf(CYAN"[Light Mode] Cache size limited to 8 pages\n\r"COLOR_RESET);
    }
    if (flags & 1<<5) {
        printf(CYAN"[Light Mode] Bytes per line limited to 16\n\r"COLOR_RESET);
    }

    printf("Press any key to continue...");
    getchar();
}

