#include "./api.h"
#include <curses.h>
#include <stdlib.h>
#include <string.h>

cterm_t *cterm;
cterm_embed_t efun;

char buffer[256];
int line_x = 0;

bool cmd_hello(void *args) {
    efun.e_printw("Hello, World!\n");
    efun.e_move(++*cterm->terminal_y, 0);
    efun.e_refresh();
    return true;
}
bool cmd_line(void *args) {
    //if(!args) return false;
    cterm->embedded.e_clear();
    unsigned char i = 0;
    while(i < 254) {
        buffer[i] = 0;
        i++;
    }
    line_x = 1;
    *cterm->terminal_y++;
    efun.e_move(*cterm->terminal_y, line_x);
    goto cmdline_start;
    cmdline_start:
        memset(buffer, 0, 255);
        if(false) {
            efun.e_clear();
            *cterm->terminal_y = 0;
            line_x = 1;
            efun.e_move(*cterm->terminal_y, line_x);
        }
        i = 0;
        line_x = 0;
        *cterm->terminal_y++;
        efun.e_move(*cterm->terminal_y, line_x);
        efun.e_printw("> ");
        efun.e_refresh();
        while(true) {
            char c = (char)efun.e_getchar();
            if(c == 0x0D) {
                *cterm->terminal_y++;
                line_x = 3;
                efun.e_move(*cterm->terminal_y, line_x);
                efun.e_refresh();
                i = 0;
                if(buffer[i] == 0) {
                    line_x = 1;
                    efun.e_move(*cterm->terminal_y, line_x);
                    goto cmdline_start;
                } else {
                    int jj = 0;
                    while(jj < 32) {
                        efun.e_move(jj, 0);
                        efun.e_printw(" ");
                        jj++;
                    }
                    efun.e_refresh();
                    efun.e_move(*cterm->terminal_y, line_x);
                    cterm_command_reference_t cmd = cterm->find(buffer);
                    line_x--;
                    efun.e_clear();
                    efun.e_move(*cterm->terminal_y, line_x);
                    if(!cmd.command) {
                        *cterm->terminal_y++;
                        efun.e_move(*cterm->terminal_y, line_x);
                        efun.e_printw("Command not found!\n");
                        efun.e_refresh();
                        i = 0;
                        while(i < 254) {
                            buffer[i] = 0;
                            i++;
                        }
                        goto cmdline_start;
                    } else {
                        cmd.callback(NULL);
                        i = 0;
                        while(i < 254) {
                            buffer[i] = 0;
                            i++;
                        }
                        goto cmdline_start;
                    }
                }
            } else if (c == '\b') {
                line_x--;
                if(line_x < 1) line_x = 1;
                efun.e_move(*cterm->terminal_y, line_x);
            } else {
                line_x++;
                efun.e_move(line_x, *cterm->terminal_y);
                efun.e_printw("%c", c);
                buffer[i++] = c;
                efun.e_refresh();
            }
        }
    return true;
}
bool cmd_quit(void *args) {
    cterm->system_shutdown();
    return true;
}
bool cmd_exit(void *args) {
    return cmd_quit(args);
}

bool cmd_help(void *args) {
    int jj = 0;
    int jp = 0;
    while(jj < *cterm->command_size) {
        if(!cterm->commands[jj].helpHide) {
            efun.e_printw("  * %s - %s\n", cterm->commands[jj].command, cterm->commands[jj].helpdesc);
            jp++;
        }
        jj++;
    }
    *cterm->terminal_y += *cterm->command_size;
    efun.e_move(*cterm->terminal_y, 0);
    efun.e_refresh();
    return true;
}

void init(cterm_t *info) {
    cterm = info;
    efun = cterm->embedded;
    info->register_command("hello", "Hello, World!", false, cmd_hello);
    info->register_command("line", "Command Line", false, cmd_line);
    info->register_command("quit", "Quit Command", false, cmd_quit);
    info->register_command("exit", "Synonim for quit command", false, cmd_quit);
    info->register_command("help", "Help Command", false, cmd_help);
    return;
}