#include "./api.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

cterm_t *cterm;

bool reset_home(void *args) {
    rmdir("usr/home");
    mkdir("usr/home", 0700);

    return true;
}

int symlink(const char*, const char*);

void init(cterm_t *info) {
    cterm = info;

    struct stat st = {0};

    if (stat("usr", &st) == -1) mkdir("usr", 0700); 
    if (stat("usr/home", &st) == -1) mkdir("usr/home", 0700); 
    if (stat("usr/bin", &st) == -1) {
        char *buffer1 = (char *)malloc(1024);
        char *buffer2 = (char *)malloc(1024);

        getcwd(buffer1, 1024);
        snprintf(buffer2, 1024, "%s/applications", buffer1);

        symlink(buffer2, "usr/bin");

        free(buffer1);
        free(buffer2);
    }
    if (stat("usr/etc", &st) == -1) mkdir("usr/etc", 0700);

    cterm->register_command("resethome", "Resets home directory", false, reset_home);

    return;
}

SET_INFORMATION("cterm_user_environment", "Creates main system directories", "1.33")