#include "libcterm.h"

#include <unistd.h>
#include <stdio.h>

int main() {
    _cterm_init();

    while(1) {
        usleep(100000);
        if (_cterm_closed) {
            return _cterm_result;
        }
    }

    return 0;
}