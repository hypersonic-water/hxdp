#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "../include/term.h"
#include "../include/message.h"


static struct termios orginal;

struct winsiz_inf  wsf;

void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orginal);
    printf("\x1b[?25h");
    printf("\x1b[?1049l");
}

void config_terminal() {
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &orginal) == -1) {
        display_error(strerror(errno));
    }

    atexit(restore_terminal);

    raw = orginal;
    cfmakeraw(&raw);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        display_error(strerror(errno));
    }

    printf("\x1b[?1049h");
    printf("\033[?25l");

}

int get_termwinsize(struct winsiz_inf* wsf) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        return -1;
    }

    wsf->row = ws.ws_row;
    wsf->col = ws.ws_col;

    return 1;
}


void print_term_header_footer(const char* headerinfo) {
    if (get_termwinsize(&wsf) == -1) {
        display_error(strerror(errno));
    }

    const char* message = (headerinfo == NULL) ? "No Input file provided" : headerinfo;
    int msg_len = strlen(message);

    int start_col = (wsf.col - msg_len) / 2;
    if (start_col < 0) start_col = 0;

    printf("\x1b[%d;%dH", 1, 1);
    printf("%s", ANSI_INVERT);

    for (int i = 0; i < start_col; i++) printf(" ");

    printf("%s", message);

    for (int i = start_col + msg_len; i < wsf.col; i++) printf(" ");

    printf("\x1b[%d;%dH", wsf.row, 0);

    const char* footer = "w - page up  s - page down  q - quit";
    msg_len = strlen(footer);

    start_col = (wsf.col - msg_len) / 2;
    if (start_col < 0) start_col = 0;

    for (int i = 0; i < start_col; i++) printf(" ");

    printf("%s", footer);

    for (int i = start_col + msg_len; i < wsf.col; i++) printf(" ");

    printf("\x1b[%d;%dH", 2, 1);
    printf("%s", ANSI_RESET);
    fflush(stdout);
}

void cur_pos(int row, int col) {
    printf("\x1b[%d;%dH", row, col);
}

void draw_message_box(const char* message) {
    if (get_termwinsize(&wsf) == -1) {
        display_error(strerror(errno));
    }

    int msg_len = strlen(message);

    int start_col = (wsf.col - msg_len) / 2;
    if (start_col < 0) start_col = 0;

    int row = (wsf.row - 2) / 2;

    cur_pos(row, start_col);
    printf(COLOR_RESET"%s\n", message);
    cur_pos(row+1, start_col + (msg_len-2)/2);
    printf(ANSI_INVERT CYAN "[OK]" ANSI_RESET);

}

void clrscr() {
    if (get_termwinsize(&wsf) == -1) {
        display_error(strerror(errno));
    }

    cur_pos(2, 1);

    for (int i = 0; i<wsf.row-2; i++) {
        printf("\033[2K\n\r");
    }

    cur_pos(2, 1);
}
