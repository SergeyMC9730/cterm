#include "./api.h"
#include "./cterm_generic.h"
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include "../cJSON/cJSON.h"

cterm_t *cterm;
cterm_embed_t efun;

char *buffer;
int line_x = 0;
bool should_exit = false;

bool cmd_hello(void *args) {
    printf("Hello, World!\n");
    return true;
}

void filter_command(char *data) {
    int i = 0;
    int l = strlen(data);

    while(i < l) {
        if(data[i] == '\n') data[i] = 0;
        if(data[i] == ' ' ) data[i] = 0;
        i++;
    }

    return;
}
bool cmd_line(void *args) {
    bool in_stack_mode = false;
    char stackl = 'A' + (rand() % 26);
    uint8_t *runned_by_main = (uint8_t *)args;
    if(!args || runned_by_main[0] != 128) in_stack_mode = true;

    bool execution_error = false;

    if(!in_stack_mode) system("clear");

    start:
    if(buffer) free(buffer);
    buffer = (char *)malloc(2048);

    printf("%c%c%c> ", (in_stack_mode) ? stackl : 0, (execution_error) ? '!' : 0, (in_stack_mode || execution_error) ? ' ' : 0);
    fgets(buffer, 2048, stdin);

    char *requested_command = strtok(buffer, " ");
    filter_command(requested_command);
    cterm_command_reference_t cmd = cterm->find(requested_command);
    
    if(!cmd.callback) {
        printf("Command not found!\n");
        goto start;
    }

    execution_error = !cmd.callback(buffer);
    if(!should_exit) goto start;

    should_exit = false;

    return true;
}

bool cmd_shutdown(void *args) {
    cterm->system_shutdown();
    return true;
}

bool cmd_exit(void *args) {
    should_exit = true;
    return true;
}

bool cmd_help(void *args) {
    int jj = 0;
    int jp = 0;
    while(jj < *cterm->command_size) {
        if(!cterm->commands[jj].helpHide) {
            printf("  * %s - %s\n", cterm->commands[jj].command, cterm->commands[jj].helpdesc);
            jp++;
        }
        jj++;
    }
    return true;
}

bool cmd_info(void *args) {
    printf("CTerm %s\nMade by SergeyMC9730\n", cterm->version);

    return true;
}

bool api_getdatalocation(void *args) {
    cterm_command_reference_t ext = cterm->find("resethome");
    generic_datalocation *dl = (generic_datalocation *)args;

    if(!args) return false;

    if(!ext.callback) {
        dl->home_dir = ".";
        dl->etc_dir = ".";
        dl->bin_dir = ".";
        dl->usr_dir = ".";
    } else {
        dl->home_dir = "usr/home";
        dl->etc_dir = "usr/etc";
        dl->bin_dir = "usr/bin";
        dl->usr_dir = "usr";
    }

    return true;
}

void init(cterm_t *info) {
    cterm = info;
    efun = cterm->embedded;
    info->register_command("hello", "Hello, World!", false, cmd_hello);
    info->register_command("line", "Command Line", false, cmd_line);
    info->register_command("shutdown", "Shutdowns CTerm safely", false, cmd_shutdown);
    info->register_command("exit", "Synonim for quit command", false, cmd_exit);
    info->register_command("info", "CTerm information", false, cmd_info);
    info->register_command("help", "Help Command", false, cmd_help);
    info->register_command("CTERM_getdatalocation", "Gets main system directory paths", true, api_getdatalocation);
    return;
}

SET_INFORMATION("cterm_generic", "CTerm basic commands", "1.3")