#include "./api.h"
#include "cterm_networking.h"
#include "cterm_generic.h"
#include "curl/include/curl/curl.h"
#include <stdlib.h>
#include <string.h>

cterm_t *cterm;
CURL *curl_instance;

networking_get current_ng_args;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if(!current_ng_args.move_to_file) {
        char *data = (char *)ptr;
        int i = 0;
        while(i < nmemb) {
            putc(data[i], stdout);
            i++;
        }
    }
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
bool api_get(void *args_1) {
    networking_get *args = (networking_get *)args_1;

    if(!args) {
        printf("Arguments are not provided!\n");
        return false;
    }

    cterm_command_reference_t paths = cterm->find("CTERM_getdatalocation");
    generic_datalocation pathdata;
    paths.callback(&pathdata);

    bool is_user_available = false;
    if(!strcmp(pathdata.etc_dir, "usr/etc")) is_user_available = true;

    current_ng_args.move_to_file = args->move_to_file;

    if(curl_instance) { // fclose(pagefile);
        char *path = (char *)malloc(2048);
        snprintf(path, 2048, "%s/%s", (args->move_to_file) ? pathdata.home_dir : pathdata.etc_dir, (args->move_to_file) ? args->filename : "tmp0000000.tmp");

        curl_easy_setopt(curl_instance, CURLOPT_URL, args->url);
        curl_easy_setopt(curl_instance, CURLOPT_CUSTOMREQUEST, "GET");
        FILE *fp = fopen(path, "wb");
        curl_easy_setopt(curl_instance, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_instance, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl_instance, CURLOPT_VERBOSE, args->debug);
        
        args->curl_result = (int)curl_easy_perform(curl_instance);

        if(args->curl_result != CURLE_OK) {
            if(!args->is_api) printf("Cannot access %s: %s\n", args->url, curl_easy_strerror(args->curl_result));
            if (fp) fclose(fp);
            return false;
        }

        int httpStatus;
        curl_easy_getinfo(curl_instance, CURLINFO_RESPONSE_CODE, &httpStatus);
        if(!args->is_api) printf("Got HTTP status: %d\n", httpStatus);

        if (fp) fclose(fp);
        if(!args->move_to_file) remove(path);
        free(path);

        return true;
    }

    return false;
}

void print_usage(int type) {
    switch(type) {
        case 0: {
            printf("Usage for get command:\n > get <url> <filename (optional>\nIf filename is not set, file data will be printed to terminal.\n");
            break;
        }
    }

    return;
}

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
bool cmd_get(void *args_1) {
    char *args = (char *)args;

    if(!args) goto failure;

    networking_get ng = {};

    ng.url = filter_argument(strtok(NULL, " "));
    if(!ng.url) goto failure;

    ng.filename = filter_argument(strtok(NULL, " "));
    ng.move_to_file = true;
    if(!ng.filename) ng.move_to_file = false;

    ng.is_api = false;

    return api_get(&ng);

    failure:
    print_usage(0);
    return false;
}

void init(cterm_t *info) {
    cterm = info;
    info->register_command("CTERM_get", "Download videos, files and more", true, api_get);
    info->register_command("get", "Download videos, files and more", false, cmd_get);

    curl_instance = curl_easy_init();
    return;
}
void cterm_on_shutdown() {
    curl_easy_cleanup(curl_instance);
    curl_global_cleanup();
}

SET_INFORMATION("cterm_networking", "Basic networking commands", "1.31")