#ifndef __HISTORY_H
#define __HISTORY_H

#define MAXLINE 4096
#define CHAR_BUFFER 128
#define ARGV_BUFFER 1024

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cmd {
        void* pid;
        char* cmd;
        int exitStatus;
        char** argv;
};

struct history {
        unsigned int capacity;
        unsigned int offset;
        struct cmd** entries;
};

struct history* init_history(int capacity);

void add_history(struct history*, char* cmd, int exitStatus, char** argv);

void print_history(struct history*);

void clear_history(struct history*);

void malloc_error_exit();
#endif /* __HISTORY_H */
