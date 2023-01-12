#include "./api.h"
#include "./cterm_extensions.h"
#include <stdlib.h>

cterm_t *cterm;

bool cmd_test00(void *args) {
    static binary_set_t binset;
    binset.bitSet = NULL;
    binset.boolset = NULL;
    binset.number = 8;
    cterm_command_reference_t ext = cterm->find("extension_int2binstr");
    if(!ext.callback) {
        printf("Download Extension module before executing\n");
        return false;
    }
    if(!ext.callback(&binset)) {
        printf("Allocation is failed\n");
        return false;
    }
    printf("Int: %d | Binary: %s\n", binset.number, binset.bitSet);
    free(binset.bitSet);
    free(binset.boolset);
    return true;
}
bool cmd_test01(void *args) {
    cterm_command_reference_t ref;
    
    ref = cterm->find("CTERM_test00");
    if(ref.helpHide) *ref.helpHide = !*ref.helpHide;
    ref = cterm->find("CTERM_test01");
    if(ref.helpHide) *ref.helpHide = !*ref.helpHide;
    ref = cterm->find("extension_int2binstr");
    if(ref.helpHide) *ref.helpHide = !*ref.helpHide;
    ref = cterm->find("extension_logfile");
    if(ref.helpHide) *ref.helpHide = !*ref.helpHide;

    return true;
}

void init(cterm_t *info) {
    cterm = info;
    info->register_command("CTERM_test00", "CTerm Test 00 (Ext Test)", true, cmd_test00);
    info->register_command("CTERM_test01", "CTerm Test 01 (Hide/Show)", true, cmd_test01);
    return;
}

SET_INFORMATION("cterm_tests", "Test commands", "1.3")