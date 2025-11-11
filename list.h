#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>

//#define SoA
//#define AoS

//#define DEBUG

//#define TEST


typedef int list_elem_t;

const list_elem_t DATA_POISON = -1;
const ssize_t     INDX_POISON = -1;

const size_t DEFAULT_CTOR_CAPACITY = 1;

const size_t LIST_NAME_SIZE = 100;

typedef enum class ListErrors : char {
    OK = 0,
    ALLOCATION_ERROR  = 1,
    INVALID_POSITION  = 2,
    LIST_EMPTY        = 3,
    POSITION_FREE     = 4,
    FILE_OPEN_ERROR   = 5,
    LIST_CORRUPTED    = 6,
    LIST_INVALID_PREV = 7,
    LIST_INVALID_NEXT = 8,
    LIST_SIZE_ERROR   = 9,
    INVALID_ELEMENT   = 10
} list_error_t;

static const char* ListErrorsArray[11] = {
    "OK",
    "ALLOCATION_ERROR",
    "INVALID_POSITION",
    "LIST_EMPTY",
    "POSITION_FREE",
    "FILE_OPEN_ERROR",
    "LIST_CORRUPTED",
    "LIST_INVALID_PREV",
    "LIST_INVALID_NEXT",
    "LIST_SIZE_ERROR",
    "INVALID_ELEMENT"
};
#ifdef SoA
typedef struct ListSoA {
    char name[LIST_NAME_SIZE] = "";

    ssize_t free = 0;

    size_t size     = 0;
    size_t capacity = 0;

    ssize_t* prev     = nullptr;
    list_elem_t* data = nullptr;
    ssize_t* next     = nullptr;

    FILE* file = nullptr;
    size_t dump_count = 0;
} list_type;
#endif
#ifdef AoS
typedef struct ListElement {
    ssize_t prev      = INDX_POISON;
    list_elem_t value = DATA_POISON;
    ssize_t next      = INDX_POISON;
} list_data_type;

typedef struct ListAoS {
    char name[LIST_NAME_SIZE] = "";

    ssize_t free = 0;

    size_t size     = 0;
    size_t capacity = 0;

    list_data_type* data = nullptr;

    FILE* file = nullptr;
    size_t dump_count = 0;
} list_type;
#endif

void UpdatePrev(list_type* list,  const ssize_t pos, const ssize_t     new_addr);
void UpdateValue(list_type* list, const ssize_t pos, const list_elem_t new_value);
void UpdateNext(list_type* list,  const ssize_t pos, const ssize_t     new_addr);

ssize_t     GetPrev(list_type* list,  const ssize_t pos);
list_elem_t GetValue(list_type* list, const ssize_t pos);
ssize_t     GetNext(list_type* list,  const ssize_t pos);

#endif //_LIST_H_
