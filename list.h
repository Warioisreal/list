#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>

typedef int list_elem_t;

const int DATA_POISON = -1;
const int INDX_POISON = -1;

const size_t DEFAULT_CTOR_CAPACITY = 1;

const size_t LIST_NAME_SIZE = 100;

typedef enum class ListErrors : char {
    OK = 0,
    ALLOCATION_ERROR     = 1,
    INVALID_POSITION     = 2,
    LIST_EMPTY           = 3,
    POSITION_FREE        = 4,
    FILE_OPEN_ERROR      = 5,
    LIST_CORRUPTED       = 6,
    LIST_PREV_ADDR_ERROR = 7,
    LIST_NEXT_ADDR_ERROR = 8,
    LIST_SIZE_ERROR      = 9,
    INVALID_ELEMENT      = 10
} list_error_t;

static const char* ListErrorsArray[11] = {
    "OK",
    "ALLOCATION_ERROR",
    "INVALID_POSITION",
    "LIST_EMPTY",
    "POSITION_FREE",
    "FILE_OPEN_ERROR",
    "LIST_CORRUPTED",
    "LIST_PREV_ADDR_ERROR",
    "LIST_NEXT_ADDR_ERROR",
    "LIST_SIZE_ERROR",
    "INVALID_ELEMENT"
};

typedef struct List {
    char name[LIST_NAME_SIZE] = "";

    int free = 0;

    size_t size     = 0;
    size_t capacity = 0;

    int* prev         = nullptr;
    list_elem_t* data = nullptr;
    int* next         = nullptr;

    FILE* file = nullptr;
    size_t dump_count = 0;
} list_type;

#endif //_LIST_H_
