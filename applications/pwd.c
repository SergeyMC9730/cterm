#include "./api.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

cterm_t *cterm;

bool cmd_pwd(void *args) {
    char *buffer = (char *)malloc(1024);

    getcwd(buffer, 1024);
    printf("%s\n", (const char *)buffer);

    free(buffer);
    
    return true;
}

void init(cterm_t *info) {
    cterm = info;

    cterm->register_command("pwd", "Gets current directory", false, cmd_pwd);

    return;
}

SET_INFORMATION("pwd", "Gets current directory", "1.33")