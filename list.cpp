#include "list.h"


void UpdatePrev(list_type* list, const ssize_t pos, const ssize_t new_addr) {
    #ifdef SoA
    list->prev[pos] = new_addr;
    #endif
    #ifdef AoS
    list->data[pos].prev = new_addr;
    #endif
}

//----------------------------------------------------------------------------------

ssize_t GetPrev(list_type* list, const ssize_t pos) {
    #ifdef SoA
    return list->prev[pos];
    #endif
    #ifdef AoS
    return list->data[pos].prev;
    #endif
}

//----------------------------------------------------------------------------------

void UpdateValue(list_type* list, const ssize_t pos, const list_elem_t new_value) {
    #ifdef SoA
    list->data[pos] = new_value;
    #endif
    #ifdef AoS
    list->data[pos].value = new_value;
    #endif
}

//----------------------------------------------------------------------------------

list_elem_t GetValue(list_type* list, const ssize_t pos) {
    #ifdef SoA
    return list->data[pos];
    #endif
    #ifdef AoS
    return list->data[pos].value;
    #endif
}

//----------------------------------------------------------------------------------

void UpdateNext(list_type* list, const ssize_t pos, const ssize_t new_addr) {
    #ifdef SoA
    list->next[pos] = new_addr;
    #endif
    #ifdef AoS
    list->data[pos].next = new_addr;
    #endif
}

//----------------------------------------------------------------------------------

ssize_t GetNext(list_type* list, const ssize_t pos) {
    #ifdef SoA
    return list->next[pos];
    #endif
    #ifdef AoS
    return list->data[pos].next;
    #endif
}
