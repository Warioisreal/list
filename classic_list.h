#ifndef _CLASSIC_LIST_H_
#define _CLASSIC_LIST_H_

#include <stdio.h>

//#define DEBUG

//#define TEST


typedef int classic_list_elem_t;

const classic_list_elem_t POISON = -1;

const size_t CLASSIC_LIST_NAME_SIZE = 100;

const size_t MIN_NORMAL_ADDR = 0x100000000;

const size_t MAX_NORMAL_ADDR = 0xD00000000;

#define PRINT_LIST_ELEMENT(prev, elem, next) printf("%zd | %d | %zd\n", prev, elem, next)

#define MAKE_CLASSIC_LIST(name) \
    List name = {}; \
    char full_folder_name_classic[LOG_FOLDER_NAME_SIZE] = ""; \
    GetFullFolderName(#name, full_folder_name_classic); \
    UpdateFolder(full_folder_name_classic); \
    ClassicListCtor(&name, #name)

typedef enum class ClassicListErrors : char {
    OK = 0,
    ALLOCATION_ERROR = 1,
    POINTER_ERROR    = 2,
    LIST_EMPTY       = 3,
    POSITION_FREE    = 4,
    FILE_OPEN_ERROR  = 5,
    PREV_ADDR_ERROR  = 6,
    NEXT_ADDR_ERROR  = 7,
    LIST_SIZE_ERROR  = 8,
    INVALID_ELEMENT  = 9
} error_t;

static const char* ClassicListErrorsArray[10] = {
    "OK",
    "ALLOCATION_ERROR",
    "POINTER_ERROR",
    "LIST_EMPTY",
    "POSITION_FREE",
    "FILE_OPEN_ERROR",
    "LIST_PREV_ADDR_ERROR",
    "LIST_NEXT_ADDR_ERROR",
    "LIST_SIZE_ERROR",
    "INVALID_ELEMENT"
};

struct ClassicListElement {
    classic_list_elem_t value = POISON;
    ClassicListElement* prev = nullptr;
    ClassicListElement* next = nullptr;
};

typedef struct ColorElements {
    ClassicListElement* posR  = nullptr;
    ClassicListElement* posG  = nullptr;
    ClassicListElement* posY1 = nullptr;
    ClassicListElement* posY2 = nullptr;
} elems_color;

struct List {
    char name[CLASSIC_LIST_NAME_SIZE] = "";

    size_t size = 0;

    ClassicListElement* fict_elem = nullptr;

    FILE* file = nullptr;
    size_t dump_count = 0;
};

error_t ClassicListCtor(List* list, const char* name);
error_t ClassicListDtor(List* list);

error_t ClassicListInsertBefore(List* list, ClassicListElement* element, const classic_list_elem_t value);
error_t ClassicListInsertAfter(List* list,  ClassicListElement* element, const classic_list_elem_t value);
error_t ClassicListRemove(List* list, ClassicListElement* element, classic_list_elem_t* value);

ClassicListElement* ClassicListGetHead(List* list);
ClassicListElement* ClassicListGetTail(List* list);

void ClassicListPrint(List* list, const char* message);

#endif //_CLASSIC_LIST_H_
