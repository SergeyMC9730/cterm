// DIR *application_directory;
//     struct dirent *app_directory;

#include "./api.h"
#include "./cterm_generic.h"
#include <stdlib.h>
#include <dirent.h>

cterm_t *cterm;

char *filter_argument(char *data) {
    if(!data) return data;

    int i = 0;
    int l = strlen(data);

    while(i < l) {
        if(data[i] == '\n') data[i] = 0;
        if(data[i] == ' ' ) data[i] = 0;
        i++;
    }

    return data;
}
bool cmd_ls(void *args) {
    char *path1 = filter_argument(strtok(NULL, " "));
    if(path1 == NULL) path1 = "";

    cterm_command_reference_t paths = cterm->find("CTERM_getdatalocation");
    generic_datalocation pathdata;
    bool is_user_available = false;

    pathdata.home_dir = ".";
    
    if(paths.callback) {
        paths.callback(&pathdata);
        if(!strcmp(pathdata.home_dir, "usr/home")) is_user_available = true;
    }

    char *buffer = (char *)malloc(1024);
    snprintf(buffer, 1024, "%s/%s", pathdata.home_dir, path1);

    DIR *application_directory = opendir(buffer);
    struct dirent *app_directory;

    printf("Listing for %s :\n", buffer);

    while((app_directory = readdir(application_directory)) != NULL) {
        printf(" - %s\n", app_directory->d_name);
    }

    free(buffer);

    return true;
}

void init(cterm_t *info) {
    cterm = info;
    info->register_command("ls", "List directory", true, cmd_ls);
    return;
}

SET_INFORMATION("ls", "List directory", "1.3")