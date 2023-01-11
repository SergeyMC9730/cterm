#include <curses.h>
#include "./cJSON/cJSON.h"
#include <stdlib.h>
#include <stdbool.h>
#include "./main.h"
#include <dlfcn.h>
#include "./applications/api.h"
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "config_json.h"

cJSON *__main_config_parsed;
FILE *__main_config_stream;
char __main_config_data[8192];
cterm_t cterm_info;
int total_commands = 0;
int __main_console_y = 0;
char *logData = NULL;

bool inEInit = false;

cterm_command_reference_t find_command(char *command) {
    int i = 0;
    cterm_command_reference_t found0 = {0, 0, 0, 0};
    while(i < total_commands) {
        refresh();
        if(!strcmp(command, cterm_info.commands[i].command)) {
            // found0 = cterm_info.commands[i];
            found0.callback = cterm_info.commands[i].callback;
            found0.command = cterm_info.commands[i].command;
            found0.helpdesc = cterm_info.commands[i].helpdesc;
            found0.helpHide = &cterm_info.commands[i].helpHide;
            return found0;
        }
        i++;
    }
    return found0;
}
void register_command(char *command, char *helpdesc, bool helpHide, bool (*callback)(void *args)) {
    total_commands++;
    cterm_info.commands = (cterm_command_t *)realloc(cterm_info.commands, total_commands * sizeof(cterm_command_t));
    cterm_info.commands[total_commands - 1].command = command;
    cterm_info.commands[total_commands - 1].helpdesc = helpdesc;
    cterm_info.commands[total_commands - 1].callback = callback;
    cterm_info.commands[total_commands - 1].helpHide = helpHide;
    printw("** Registered %s", command);
    char *tmp = (char *)malloc(1024);
    sprintf(tmp, "** Registered %s\n", command);
    strcat(logData, tmp);
    free(tmp);
    cterm_command_reference_t logcmd = find_command("extension_logfile");
    if(logcmd.callback) logcmd.callback(logData);    
    move(++__main_console_y, 0);
    refresh();
    return;
}

int __embed_func0() {
    wrefresh(stdscr);
}
int __embed_func1(int x, int y) {
    wmove(stdscr, y, x);
}
int __embed_func2() {
    wclear(stdscr);
}

void system_shutdown() {
    endwin();
    cJSON_Delete(__main_config_parsed);
    exit(0);
}

void critical_sigsegv(int sig, siginfo_t *si, void *unused) {
    endwin();
    cJSON_Delete(__main_config_parsed);
    
    printf(logData);

    if(!inEInit) {
        printf("External CTerm module failed execution with: SIGSEGV.\n");
    } else {
        printf("CTerm init failed execution with: SIGSEGV.\n");
    }

    exit(0);
}

cterm_module_t load_module(const char *file, const char *init_function) {
    void *cmd_handler = NULL;
    char *cmd_error = NULL;
    cterm_module_t mod;
    mod.handler = cmd_handler;
    mod.error = cmd_error;
    char *tmp = (char *)malloc(1024);
    sprintf(tmp, "./applications/%s", file);
    cmd_handler = dlopen(tmp, RTLD_NOW);
    free(tmp);
    cmd_error = dlerror();
    if(cmd_handler != NULL) {
        void (*app_init)(cterm_t *ctrm);
        app_init = (void (*)(cterm_t *ctrm))dlsym(cmd_handler, init_function);
        cmd_error = dlerror();
        if(cmd_error) {
            printw("%s error: %s", file, cmd_error);
            tmp = (char *)malloc(1024);
            sprintf(tmp, "%s error: %s\n", file, cmd_error);
            strcat(logData, tmp);
            free(tmp);
            cterm_command_reference_t logcmd = find_command("extension_logfile");
            if(logcmd.callback) logcmd.callback(logData); 
            move(++__main_console_y, 0);
            refresh();
            dlclose(cmd_handler);
        } else {
            (*app_init)(&cterm_info);
            printw("* Loaded %s", file);
            tmp = (char *)malloc(1024);
            sprintf(tmp, "* Loaded %s\n", file);
            strcat(logData, tmp);
            free(tmp);
            cterm_command_reference_t logcmd = find_command("extension_logfile");
            if(logcmd.callback) logcmd.callback(logData); 
            refresh();
            move(++__main_console_y, 0);
        }
    }
    return mod;
}

bool __main_early() {
    logData = (char *)malloc(8 * 1024);
    initscr();
    __main_config_parsed = cJSON_ParseWithLength(config_json_data, config_json_size);
    cJSON *entry_cterm = cJSON_GetObjectItemCaseSensitive(__main_config_parsed, "cterm");
    cterm_info.version = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(entry_cterm, "version"));
    if(!cterm_info.version) {
        endwin();
        printf("Unable to parse CTerm config!\n");
        return false;
    }
    printw("Reading application data...");
    strcat(logData, "Reading application data...\n");
    move(++__main_console_y, 0);
    refresh();
    DIR *application_directory;
    struct dirent *app_directory;
    application_directory = opendir("./applications");
    if(!application_directory) {
        endwin();
        printf("Unable to get applications!\n");
        return false;
    }
    cterm_info.terminal_y = &__main_console_y;
    cterm_info.register_command = register_command;
    cterm_info.command_size = &total_commands;
    cterm_info.commands = (cterm_command_t *)malloc(1 * sizeof(cterm_command_t));
    cterm_info.find = find_command;
    cterm_info.system_shutdown = system_shutdown;
    cterm_info.config_instance = __main_config_parsed;
    cterm_info.embedded.e_cJSON_Delete = cJSON_Delete;
    cterm_info.embedded.e_getchar = getchar;
    cterm_info.embedded.e_endwin = endwin;
    cterm_info.embedded.e_move = __embed_func1;
    cterm_info.embedded.e_printw = printw;
    cterm_info.embedded.e_refresh = __embed_func0;
    cterm_info.embedded.e_clear = __embed_func2;
    cterm_info.embedded.dlsym = dlsym;
    cterm_info.embedded.e_LINES = &LINES;
    cterm_info.embedded.e_COLS = &COLS;
    while((app_directory = readdir(application_directory)) != NULL) {
        load_module(app_directory->d_name, "init");
    }
    return true;
}

uint8_t run_by_main = 128;

int main() {
    inEInit = true;

    struct sigaction csigsegv;
    csigsegv.sa_handler = critical_sigsegv;
    sigemptyset(&csigsegv.sa_mask);
    csigsegv.sa_flags = 0;
    sigaction(SIGSEGV, &csigsegv, NULL);

    if(!__main_early()) {
        free(cterm_info.commands);
        return 1;
    }
    printw("Welcome to %s\n", cterm_info.version);
    char *tmp = (char *)malloc(1024);
    sprintf(tmp, "Welcome to %s\n", cterm_info.version);
    strcat(logData, tmp);
    free(tmp);

    cterm_command_reference_t logcmd = find_command("extension_logfile");
    if(logcmd.callback) logcmd.callback(logData); 

    cterm_command_reference_t linecmd = find_command("line");
    inEInit = false;
    if(!linecmd.callback) {
        cJSON_Delete(__main_config_parsed);
        endwin();
        printf(logData);
        printf("Unable to find line command!\n");
        strcat(logData, "Unable to find line command!\n");
        cterm_command_reference_t logcmd = find_command("extension_logfile");
        if(logcmd.callback) logcmd.callback(logData); 
        free(logData);
        free(cterm_info.commands);
        return 1;
    }
    if(!linecmd.callback(&run_by_main)) {
        cJSON_Delete(__main_config_parsed);
        endwin();
        printf("line command is crashed!\n");
        strcat(logData, "line command is crashed!\n");
        cterm_command_reference_t logcmd = find_command("extension_logfile");
        if(logcmd.callback) logcmd.callback(logData); 
        free(logData);
        free(cterm_info.commands);
        return 1;
    }
    strcat(logData, "Closing\n");
    if(logcmd.callback) logcmd.callback(logData); 
    free(logData);
    free(cterm_info.commands);
    return 0;
}