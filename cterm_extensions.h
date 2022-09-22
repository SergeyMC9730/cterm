#pragma once

#pragma pack(push, 1)
typedef struct {
    unsigned int number;
    //bitset is allocated
    unsigned char *bitSet;
    //boolset is allocated
    bool *boolset;
} binary_set_t;
#pragma pack(pop)