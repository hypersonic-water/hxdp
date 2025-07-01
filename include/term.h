#ifndef TERM_H
#define TERM_H

#include <stddef.h>

#define ANSI_INVERT  "\x1b[7m"
#define ANSI_RESET   "\x1b[0m"
#define COLOR_RESET   "\033[0m"
#define GREEN   "\033[32m" /* Printable ascii */
#define GREY "\033[90m" /* Non-Printable ascii */
#define CYAN "\033[36m" /* Offset */
#define YELLOW "\033[33m" /* Status */
#define RED  "\033[31m"  /* For errors */
#define WHITE "\033[37m" /* Selected byte */

#define HEX_COL 11
#define ASC_COL 53

/* ASCII codes for arrow keys */
#define UP_ARROW 65
#define DOWN_ARROW 66
#define LEFT_ARROW 68
#define RIGHT_ARROW 67

struct winsiz_inf {
    int row;
    int col;
};


void config_terminal();
void restore_terminal();
int get_termwinsize(struct winsiz_inf* wsf);
void print_term_header_footer(const char* headerinfo);
void cur_pos(int row, int col);
void draw_message_box(const char* message);
void clrscr();

extern struct winsiz_inf wsf;


#endif //TERM_H
