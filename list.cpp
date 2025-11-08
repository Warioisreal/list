#include "list.h"


void UpdatePrev(list_type* list, const ssize_t pos, const ssize_t new_addr) {
    //list->prev[pos] = new_addr;
    list->data[pos].prev = new_addr;
}

//----------------------------------------------------------------------------------

ssize_t GetPrev(list_type* list, const ssize_t pos) {
    //return list->prev[pos];
    return list->data[pos].prev;
}

//----------------------------------------------------------------------------------

void UpdateValue(list_type* list, const ssize_t pos, const list_elem_t new_value) {
    //list->data[pos] = new_value;
    list->data[pos].value = new_value;
}

//----------------------------------------------------------------------------------

list_elem_t GetValue(list_type* list, const ssize_t pos) {
    //return list->data[pos];
    return list->data[pos].value;
}

//----------------------------------------------------------------------------------

void UpdateNext(list_type* list, const ssize_t pos, const ssize_t new_addr) {
    //list->next[pos] = new_addr;
    list->data[pos].next = new_addr;
}

//----------------------------------------------------------------------------------

ssize_t GetNext(list_type* list, const ssize_t pos) {
    //return list->next[pos];
    return list->data[pos].next;
}
