#include <curses.h>
#include "./cJSON/cJSON.h"
#include <stdlib.h>
#include <stdbool.h>
#include "./libcterm.h"
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

    if(!cterm_info.commands) return found0;

    while(i < total_commands) {
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
    printf("** Registered %s\n", command);
    char *tmp = (char *)malloc(1024);
    sprintf(tmp, "** Registered %s\n", command);
    strcat(logData, tmp);
    free(tmp);
    cterm_command_reference_t logcmd = find_command("extension_logfile");
    if(logcmd.callback) logcmd.callback(logData);    
    return;
}

cterm_module_t *cterm_modules;
int cterm_modules_i = 0;

void system_shutdown() {
    int i = 0;
    char *tmp = (char *)malloc(1024);
    cterm_command_reference_t logcmd = find_command("extension_logfile");

    while(i < cterm_modules_i) {
        // printf("%X\n", cterm_modules[cterm_modules_i].shutdown_handler);
        if(cterm_modules[i].shutdown_handler) {
            sprintf(tmp, "Starting shutdown event for %s\n", cterm_modules[i].name);
            strcat(logData, tmp);
            if(logcmd.callback) logcmd.callback(logData); 
            cterm_modules[i].shutdown_handler();
        }
        i++;
    }
    
    sprintf(tmp, "Bye!\n");
    strcat(logData, tmp);
    if(logcmd.callback) logcmd.callback(logData); 

    i = 0;
    while(i < cterm_modules_i) {
        if(cterm_modules[i].handler) dlclose(cterm_modules[i].handler);
        i++;
    }
    
    cJSON_Delete(__main_config_parsed);
    free(logData);
    free(cterm_info.commands);
    free(tmp);
    free(cterm_modules);

    exit(0);
}

void critical_sigsegv(int sig, siginfo_t *si, void *unused) {
    if(!inEInit) {
        printf("\nExternal CTerm module failed execution with: SIGSEGV.\n");
    } else {
        printf("\nCTerm init failed execution with: SIGSEGV.\n");
    }

    system_shutdown();
}

cterm_module_t load_module(const char *file, const char *init_function) {
    void *cmd_handler = NULL;
    char *cmd_error = NULL;
    cterm_module_t mod;
    mod.shutdown_handler = NULL;
    mod.error = cmd_error;
    char *tmp = (char *)malloc(1024);
    sprintf(tmp, "./applications/%s", file);
    cmd_handler = dlopen(tmp, RTLD_NOW);
    mod.handler = cmd_handler;
    free(tmp);
    cmd_error = dlerror();
    if(cmd_handler != NULL) {
        void (*app_init)(cterm_t *ctrm);
        app_init = (void (*)(cterm_t *ctrm))dlsym(cmd_handler, init_function);
        cmd_error = dlerror();
        if(cmd_error) {
            printf("%s error: %s", file, cmd_error);
            tmp = (char *)malloc(1024);
            sprintf(tmp, "%s error: %s\n", file, cmd_error);
            strcat(logData, tmp);
            free(tmp);
            cterm_command_reference_t logcmd = find_command("extension_logfile");
            if(logcmd.callback) logcmd.callback(logData); 
            dlclose(cmd_handler);
        } else {
            (*app_init)(&cterm_info);
            printf("* Loaded %s\n", file);
            tmp = (char *)malloc(1024);
            sprintf(tmp, "* Loaded %s\n", file);
            strcat(logData, tmp);
            free(tmp);
            cterm_command_reference_t logcmd = find_command("extension_logfile");
            if(logcmd.callback) logcmd.callback(logData); 
            mod.shutdown_handler = (void (*)())dlsym(cmd_handler, "cterm_on_shutdown");
            cmd_error = NULL;
            cmd_error = dlerror();
            if(cmd_error) {
                printf("%s error: %s", file, cmd_error);
                tmp = (char *)malloc(1024);
                sprintf(tmp, "%s error: %s\n", file, cmd_error);
                strcat(logData, tmp);
                free(tmp);
                cterm_command_reference_t logcmd = find_command("extension_logfile");
                if(logcmd.callback) logcmd.callback(logData); 
            } else {
                // mod.shutdown_handler();
            }
            cterm_module_t *cmt = &cterm_modules[cterm_modules_i];

            cterm_modules[cterm_modules_i].error = mod.error;
            cterm_modules[cterm_modules_i].handler = mod.handler;
            cterm_modules[cterm_modules_i].shutdown_handler = mod.shutdown_handler;
            cterm_modules[cterm_modules_i].name = file;
            cterm_modules_i++;
        }
    }

    return mod;
}

bool __main_early() {
    logData = (char *)malloc(8 * 1024);
    __main_config_parsed = cJSON_ParseWithLength(config_json_data, config_json_size);
    cJSON *entry_cterm = cJSON_GetObjectItemCaseSensitive(__main_config_parsed, "cterm");
    cterm_info.version = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(entry_cterm, "version"));
    if(!cterm_info.version) {
        printf("Unable to parse CTerm config!\n");
        return false;
    }
    printf("Reading application data...\n");
    strcat(logData, "Reading application data...\n");
    DIR *application_directory;
    struct dirent *app_directory;
    application_directory = opendir("./applications");
    if(!application_directory) {
        printf("Unable to get applications!\n");
        return false;
    }
    cterm_info.register_command = register_command;
    cterm_info.command_size = &total_commands;
    cterm_info.commands = (cterm_command_t *)malloc(1 * sizeof(cterm_command_t));
    cterm_info.find = find_command;
    cterm_info.system_shutdown = system_shutdown;
    cterm_info.config_instance = __main_config_parsed;
    cterm_info.embedded.e_cJSON_Delete = cJSON_Delete;
    cterm_info.embedded.dlsym = dlsym;

    cterm_modules = (cterm_module_t *)malloc((sizeof(cterm_module_t)) * 1024);

    while((app_directory = readdir(application_directory)) != NULL) {
        load_module(app_directory->d_name, "init");
    }
    return true;
}

uint8_t run_by_main = 128;

int _cterm_init() {
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
    printf("Welcome to %s\n", cterm_info.version);
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
        printf("Unable to find line command!\n");
        strcat(logData, "Unable to find line command!\n");
        cterm_command_reference_t logcmd = find_command("extension_logfile");
        if(logcmd.callback) logcmd.callback(logData); 
        system_shutdown();
        return 1;
    }
    if(!linecmd.callback(&run_by_main)) {
        cJSON_Delete(__main_config_parsed);
        printf("line command is crashed!\n");
        strcat(logData, "line command is crashed!\n");
        cterm_command_reference_t logcmd = find_command("extension_logfile");
        if(logcmd.callback) logcmd.callback(logData); 
        system_shutdown();
        return 1;
    }
    strcat(logData, "Closing\n");
    if(logcmd.callback) logcmd.callback(logData); 

    system_shutdown();
    return 0;
}