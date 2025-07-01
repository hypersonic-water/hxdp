#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/argsparse.h"
#include "../include/term.h"


void usage() {
    fprintf(stderr, "Usage: ./hxdp [-c|--cache-size <cache_size>] [-b|--bytes-per-line <bytes>] [--safe] [--light] <filename>\n");
    exit(EXIT_FAILURE);
}

size_t parse_int(const char* arg) {
    if (arg[0] == '-') {
        fprintf(stderr, RED"[ERROR] Negative Value '%s' not allowed\n"COLOR_RESET, arg);
        exit(EXIT_FAILURE);
    }

    char* endptr = NULL;

    size_t result = strtoul(arg, &endptr, 10);

    if (errno == ERANGE || result > SIZE_MAX) {
        fprintf(stderr, RED"[ERROR] Integer %s out of range\n"COLOR_RESET, arg);
        exit(EXIT_FAILURE);
    }

    if (endptr == arg || *endptr != '\0') {
        fprintf(stderr, RED"[ERROR] %s is not a valid integer\n"COLOR_RESET, arg);
        exit(EXIT_FAILURE);
    }

    return result;

}

void print_help() {
    printf(
        "hxdp — Hexadecimal File Viewer\n\n"
        "Usage:\n"
        "  hxdp [OPTIONS] <filename>\n\n"
        "Options:\n"
        "  -c, --cache-size <value>       Set the number of pages to cache (minimum: 5, default: 25)\n"
        "  -b, --bytes-per-line <value>   Set the number of bytes displayed per line (maximum: 24, default: 16)\n"
        "  --safe                         Enable safe mode to enforce resource limits:\n"
        "                                   Max cache size: 10,000 pages\n"
        "                                   Max bytes per line: 24\n"
        "  --light                        Enable light mode for low-resource usage:\n"
        "                                   Max cache size: 8 pages\n"
        "                                   Max bytes per line: 16\n"
        "  -h, --help                     Show this help message and exit\n\n");
}


void parseargs(int argc, char** argv, Args* args) {
    if (argc == 1) {
        usage();
    }

    args->bytes_per_line = 0;
    args->cache_size = 0;
    args->f_name = NULL;
    args->safe = 0;
    args->light = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cache-size") == 0) {
            if (i + 1 >= argc) usage();
            args->cache_size = parse_int(argv[++i]);
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bytes-per-line") == 0) {
            if (i + 1 >= argc) usage();
            args->bytes_per_line = parse_int(argv[++i]);
        }
        else if (argv[i][0] != '-') {
            if (args->f_name != NULL) {
                fprintf(stderr, RED "[ERROR] Multiple arguments provided: %s\n" COLOR_RESET, argv[i]);
                usage();
            }
            args->f_name = argv[i];
        }
        else if (strcmp(argv[i], "--safe") == 0) {
            args->safe = 1;
        }
        else if (strcmp(argv[i], "--light") == 0) {
            args->light = 1;
        }
        else {
            fprintf(stderr, RED "[ERROR] Unknown option: %s\n" COLOR_RESET, argv[i]);
            usage();
        }
    }

    if (args->safe) {
       if (args->bytes_per_line > 24) {
           args->bytes_per_line = 24;
           args->flags |= 1<<1;
       }
        if (args->cache_size > 10000) {
           args->cache_size = 10000;
            args->flags |= 1<<2;
       }
    }

    if (args->cache_size > 0 && args->cache_size < 5) {
        args->flags |= 1<<3;
        args->cache_size = 5;

    }

    if (args->light) {
        if (args->cache_size > 8 || args->cache_size == 0) {
            args->cache_size = 8;
            args->flags |= 1<<4;
        }
        if (args->bytes_per_line > 16 || args->bytes_per_line == 0) {
            args->bytes_per_line = 16;
            args->flags |= 1<<5;
        }
    }


    if (args->f_name == NULL) {fprintf(stderr, RED"[ERROR] No Input file provided\n"COLOR_RESET); exit(EXIT_FAILURE);}

}
