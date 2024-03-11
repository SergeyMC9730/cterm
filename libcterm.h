#pragma once

#include "applications/api.h"

extern int _cterm_init();
extern void cterm_register_command(char *command, char *helpdesc, bool helpHide, bool (*callback)(void *args));
extern cterm_command_reference_t cterm_find_command(char *command);

#include <stdbool.h>

extern bool _cterm_closed;
extern int _cterm_result;
extern bool _cterm_ready;
