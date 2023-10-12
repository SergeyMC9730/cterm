#pragma once

#include <stdbool.h>

#pragma pack(push, 1)
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
    void *(*dlsym)(void *handler, const char *name);
} cterm_embed_t;
typedef struct {
    void *handler;
    void (*shutdown_handler)();
    char *error;
    char *name;
} cterm_module_t;
typedef struct {
    void (*log)(const char *format, ...);
} cterm_internal_logger_t;
typedef struct {
    char *version;
    void (* register_command)(char *command, char *helpdesc, bool helpHide, bool (* callback)(void *custom));
    void (* load_module)(const char *file, const char *init_function);
    void (* system_shutdown)();
    cterm_command_reference_t (* find)(char *command);
    int *terminal_y;
    cterm_command_t *commands;
    int *command_size;
    void *config_instance;
    cterm_embed_t embedded;
} cterm_t;
#pragma pack(pop)

#define SET_INFORMATION(name, description, version) const char *get_module_name() { return name; } const char *get_module_description() { return description; } const char *get_module_version() { return version; }