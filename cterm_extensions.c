#include "./api.h"
#include <stdlib.h>
#include <stdio.h>
#include "./cterm_extensions.h"

cterm_t *cterm;

bool ext_get(void *args) {
    if(!args) return false;
    binary_set_t *binset = (binary_set_t *)args;
    binset->bitSet = (unsigned char *)malloc(33);
    int temp = binset->number;
    if(!binset->bitSet) return false;
    int i = 31;
    while(i >= 0) {
        binset->bitSet[i] = (binset->number & 1) + '0';
        binset->number >>= 1;
        i--;
    }
    binset->bitSet[32] = 0;
    i = 0;
    binset->boolset = (bool *)malloc(32);
    if(!binset->boolset) return false;
    while(binset->bitSet[i] != 0) {
        if(binset->bitSet[i] == '0') binset->boolset[i] = false;
        else {
            binset->boolset[i] = true;
        }
        i++;
    }
    binset->number = temp;
    return true;
}
bool ext_log(void *args) {
    remove("logs.txt");
    FILE *logfile = fopen("logs.txt", "w");
    fprintf(logfile, "%s", (const char *)args);
    fclose(logfile);
    return true;
}

void init(cterm_t *info) {
    cterm = info;
    info->register_command("extension_int2binstr", "EXTENSION: Convert some number to binary set", true, ext_get);
    info->register_command("extension_logfile", "EXTENSION: Logger", true, ext_log);
    return;
}