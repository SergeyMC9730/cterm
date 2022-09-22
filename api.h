#pragma once

#include <stdbool.h>
#include "cJSON/cJSON.h"
#include <curses.h>

#pragma pack(1)
typedef struct {
    bool (* callback)(void *custom);
    char *command;
    char *helpdesc;
    bool helpHide;
} cterm_command_t;
typedef struct {
    bool (* callback)(void *custom);
    char *command;
    char *helpdesc;
    bool *helpHide;
} cterm_command_reference_t;
typedef struct {
    void (*e_cJSON_Delete)(cJSON *item);
    int (*e_printw)(const char *format, ...);
    int (*e_refresh)(void);
    int (*e_move)(int y, int x);
    int (*e_endwin)(void);
    int (*e_getchar)(void);
    int (*e_clear)(void);
    void *(*dlsym)(void *handler, const char *name);
    int *e_LINES;
    int *e_COLS;
} cterm_embed_t;
typedef struct {
    void *handler;
    char *error;
} cterm_module_t;
typedef struct {
    char *version;
    void (* register_command)(char *command, char *helpdesc, bool helpHide, bool (* callback)(void *custom));
    void (* load_module)(const char *file, const char *init_function);
    cterm_command_reference_t (* find)(char *command);
    int *terminal_y;
    cterm_command_t *commands;
    int *command_size;
    cJSON *config_instance;
    cterm_embed_t embedded;
} cterm_t;
#pragma pop()