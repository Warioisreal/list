#ifndef _LIST_H_
#define _LIST_H_

typedef int list_elem_t;

const int DATA_POISON = -1;
const int INDX_POISON = -1;

#define PRINT_LIST_ELEMENT(pref, elem, next) printf("%d | %d | %d\n", pref, elem, next)

typedef enum class ListErrors : char {
    OK    = 0,
    ERROR = 1
} list_error_t;

typedef struct List {

    int free = 0;

    size_t size     = 0;
    size_t capacity = 0;

    int* pref         = nullptr;
    list_elem_t* data = nullptr;
    int* next         = nullptr;

    FILE* file = nullptr;
    size_t dump_count = 0;
} list_type;

list_error_t ListCtor(list_type* list);
list_error_t ListDtor(list_type* list);

list_error_t ListPush(list_type* list, const int position, list_elem_t value);
list_error_t ListPop(list_type* list, const int position, list_elem_t* value);

void ListPrint(list_type* list);

#endif // _LIST_H_
