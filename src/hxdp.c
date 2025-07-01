#include "../include/hxdp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>


Cache cache = {0};
Args args;
FILE* f = NULL;

void cleanup() {
    if (f) fclose(f);
    free_buffer(&cache);
    restore_terminal();
}

int main(int argc, char* argv[]) {
    parseargs(argc, argv, &args);

    atexit(cleanup);
    config_terminal();

    f = fopen(args.f_name, "rb");
    print_term_header_footer(args.f_name);

    if (f == NULL) {
        display_error(strerror(errno));
        return 1;
    }

    if (args.flags != 0x0) {
        display_config_messages(&args);
    }

    init_globals(&args);
    init_cache(&cache);
    load_buffer(f, &cache);
    print_hexdump(&cache, 0, 0);


    char c[3];

    while (1) {
        if (read(STDIN_FILENO, &c, 3) >= 1) {
            switch (c[0]) {
                case 'w': page_up(&cache, f); break;
                case 's': page_down(&cache, f); break;
                case 'q': goto _end;
                case '\033':
                    if (c[1] == '[') {
                        switch (c[2]) {
                            case 'A':
                                arrow_control(UP_ARROW, &cache); break;
                            case 'B':
                                arrow_control(DOWN_ARROW, &cache); break;
                            case 'C':
                                arrow_control(RIGHT_ARROW, &cache); break;
                            case 'D':
                                arrow_control(LEFT_ARROW, &cache); break;
                        }
                    }
            }
        }
        else break;
    }

_end:
    return 0;

}
