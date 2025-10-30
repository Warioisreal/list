#include <stdlib.h>

#include "define_lib.h"

#include "list_func.h"

#define ARRAY_REALLOC_AND_RETURN(TYPE, array, size) BEGIN {\
    TYPE* buffer_arr = nullptr; \
    buffer_arr = (TYPE*)realloc(array, (size + 1) * sizeof(TYPE)); \
    if (buffer_arr == nullptr) { return list_error_t::ALLOCATION_ERROR; } \
    array = buffer_arr; \
} END

static void FillListPoison(list_type* list, const size_t start, const size_t end);
static void FindFree(list_type* list);
static void AddFree(list_type* list, const int position);
static list_error_t ListSafeAllocationMemory(list_type* list, const size_t new_capacity);


list_error_t ListCtor(list_type* list, const char* name) {
    FILE* file_ = nullptr;
    StartLog(&file_);
    if (file_ == nullptr) {
        free(list->data);
        free(list->prev);
        free(list->next);

        list->data = nullptr;
        list->prev = nullptr;
        list->next = nullptr;

        return list_error_t::FILE_OPEN_ERROR;
    }
    list->file = file_;
    list->dump_count = 1;

    snprintf(list->name, LIST_NAME_SIZE, "%s", name);

    list->capacity = 0;
    list->size     = 0;

    list->prev = nullptr;
    list->data = nullptr;
    list->next = nullptr;

    list->free = 0;

    list_error_t error = ListSafeAllocationMemory(list, DEFAULT_CTOR_CAPACITY);
    if (error != list_error_t::OK) { return error; }

    list->prev[0] = 0;
    list->data[0] = DATA_POISON;
    list->next[0] = 0;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListDtor(list_type* list) {
    FinishLog(&(list->file));

    list->free     = 0;
    list->capacity = 0;
    list->size     = 0;

    free(list->data);
    free(list->prev);
    free(list->next);

    list->data = nullptr;
    list->prev = nullptr;
    list->next = nullptr;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListChangeCapacity(list_type* list, const size_t new_capacity) {
    return ListSafeAllocationMemory(list, new_capacity);
}

//----------------------------------------------------------------------------------

int ListGetHead(list_type* list) {
    return list->next[0];
}

//----------------------------------------------------------------------------------

int ListGetTail(list_type* list) {
    return list->prev[0];
}

//----------------------------------------------------------------------------------

void ListPrint(list_type* list) {
    pos_color updated_els = {INDX_POISON, INDX_POISON, INDX_POISON, INDX_POISON};

    ListLog(list, "PRINT", list->file, &updated_els);
}

//----------------------------------------------------------------------------------

list_error_t ListInsertBefore(list_type* list, const int position, const list_elem_t value) {
    if (position < 1 || position > (int)list->capacity) {
        return list_error_t::INVALID_POSITION;
    }

    if (list->size > list->capacity) {
        return list_error_t::LIST_CORRUPTED;
    }

    if (list->size == list->capacity) {
        list_error_t error = ListSafeAllocationMemory(list, list->capacity * 2);
        if (error != list_error_t::OK) { return error; }
    }

    ListPrint(list);

    int prev_addr = list->prev[position];
    int next_addr = position;
    int free_addr = list->free;

    FindFree(list);

    pos_color updated_els = {INDX_POISON, free_addr, prev_addr, next_addr};

    // Write new element
    list->prev[free_addr] = prev_addr;
    list->data[free_addr] = value;
    list->next[free_addr] = next_addr;

    // Update previous element's next
    list->next[prev_addr] = free_addr;

    // Update next element's previous
    list->prev[next_addr] = free_addr;

    list->size++;

    ListLog(list, "INSERT BEFORE", list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListInsertAfter(list_type* list, const int position, const list_elem_t value) {
    if (position < 0 || position > (int)list->capacity) {
        return list_error_t::INVALID_POSITION;
    }

    if (list->size > list->capacity) {
        return list_error_t::LIST_CORRUPTED;
    }

    if (list->size == list->capacity) {
        list_error_t error = ListSafeAllocationMemory(list, list->capacity * 2);
        if (error != list_error_t::OK) { return error; }
    }

    ListPrint(list);

    int prev_addr = position;
    int next_addr = list->next[position];
    int free_addr = list->free;

    FindFree(list);

    pos_color updated_els = {INDX_POISON, free_addr, prev_addr, next_addr};

    // Write new element
    list->prev[free_addr] = prev_addr;
    list->data[free_addr] = value;
    list->next[free_addr] = next_addr;

    // Update previous element's next
    list->next[prev_addr] = free_addr;

    // Update next element's previous
    list->prev[next_addr] = free_addr;

    list->size++;

    ListLog(list, "INSERT AFTER", list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListRemove(list_type* list, const int position, list_elem_t* value) {
    if (position < 1 || position > (int)list->capacity) {
        return list_error_t::INVALID_POSITION;
    }

    if (list->size <= 0) {
        return list_error_t::LIST_EMPTY;
    }

    if (list->data[position] == DATA_POISON) {
        return list_error_t::POSITION_FREE;
    }

    int prev_addr = list->prev[position];
    int next_addr = list->next[position];

    *value = list->data[position];

    pos_color updated_els = {position, INDX_POISON, prev_addr, next_addr};

    // Remove element at position
    list->prev[position] = INDX_POISON;
    list->data[position] = DATA_POISON;
    list->next[position] = INDX_POISON;

    // Update previous element's next
    list->next[prev_addr] = next_addr;

    // Update next element's previous
    list->prev[next_addr] = prev_addr;

    list->size--;

    AddFree(list, position);

    ListLog(list, "REMOVE", list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

static void FillListPoison(list_type* list, const size_t start, const size_t end) {
    for (size_t pos = start; pos < end; pos++) {
        list->prev[pos] = INDX_POISON;
        list->data[pos] = DATA_POISON;
        list->next[pos] = INDX_POISON;

        AddFree(list, (int)pos);
    }
}

//----------------------------------------------------------------------------------

static void FindFree(list_type* list) {
    list->free = list->next[list->free];
}

//----------------------------------------------------------------------------------

static void AddFree(list_type* list, const int position) {
    list->next[position] = list->next[list->free];
    list->free = position;
}

//----------------------------------------------------------------------------------

static list_error_t ListSafeAllocationMemory(list_type* list, const size_t new_capacity) {

    ARRAY_REALLOC_AND_RETURN(list_elem_t, list->data, new_capacity);
    ARRAY_REALLOC_AND_RETURN(int, list->prev, new_capacity);
    ARRAY_REALLOC_AND_RETURN(int, list->next, new_capacity);

    FillListPoison(list, 1 + list->capacity, new_capacity + 1);

    list->capacity = new_capacity;

    return list_error_t::OK;
}
