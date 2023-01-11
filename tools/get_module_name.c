#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        printf("Unknown 1");
        return 0;
    }

    void *cmd_handler = dlopen(argv[1], RTLD_NOW);
    if(cmd_handler == NULL) {
        printf("Unknown 2");
        return 0;
    }

    const char *(*get_info)();
    const char *cmd_error;
    const char *info;

    get_info = (const char *(*)())dlsym(cmd_handler, "get_module_name");
    cmd_error = dlerror();

    if(cmd_error != NULL) {
        printf("Unknown 3");
        return 0;
    }

    info = (*get_info)();
    printf("%s", info);

    return 0;
}
