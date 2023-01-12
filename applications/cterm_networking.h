#pragma once

#include <stdbool.h>

typedef struct {
    const char *url;
    bool move_to_file;
    const char *filename;
    bool is_api;
    bool debug;

    void *resulting_buffer; // creates on result
    int curl_result; // read only
} networking_get;