#include <stdlib.h>

#include "define_lib.h"

#include "list_func.h"

#define ARRAY_REALLOC_AND_RETURN(TYPE, array, size) BEGIN { \
    TYPE* buffer_arr = nullptr; \
    buffer_arr = (TYPE*)realloc(array, (size + 1) * sizeof(TYPE)); \
    if (buffer_arr == nullptr) { return list_error_t::ALLOCATION_ERROR; } \
    array = buffer_arr; \
} END

#define LIST_VERIFY_AND_RETURN(list, pos, pos1, pos2, check_pos, check_size, message) BEGIN { \
    ssize_t position_ = pos; \
    list_error_t error_ = ListVerify(list, &position_, pos1, pos2, check_pos, check_size); \
    if (error_ != list_error_t::OK) { \
        ListDump(list, error_, position_, message); \
        return error_; \
    } \
} END

static void FillListPoison(list_type* list, const ssize_t start, const ssize_t end);
static void FindFree(list_type* list);
static void AddFree(list_type* list, const ssize_t position);
static list_error_t ListSafeAllocationMemory(list_type* list, const size_t new_capacity);
static list_error_t ListVerify(list_type* list, ssize_t* position, const ssize_t pos1, const ssize_t pos2, const int check_position, const int check_size);
static void ListDump(list_type* list, list_error_t error, const ssize_t pos, const char* message);


list_error_t ListCtor(list_type* list, const char* name) {

    snprintf(list->name, LIST_NAME_SIZE, "%s", name);

    FILE* file_ = nullptr;
    StartLog(&file_, list->name);
    if (file_ == nullptr) {
        return list_error_t::FILE_OPEN_ERROR;
    }
    list->file = file_;
    list->dump_count = 1;

    list->capacity = 0;
    list->size     = 0;
/*
    list->prev = nullptr;
    list->data = nullptr;
    list->next = nullptr;
*/
    list->data = nullptr;

    list->free = -1;

    list_error_t error = ListSafeAllocationMemory(list, DEFAULT_CTOR_CAPACITY);
    if (error != list_error_t::OK) { return error; }

    UpdatePrev(list,  0, 0);
    UpdateValue(list, 0, DATA_POISON);
    UpdateNext(list,  0, 0);

    LIST_VERIFY_AND_RETURN(list, 1, 0, 2, 0, 0, "ERROR IN CTOR");

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListDtor(list_type* list) {
    LIST_VERIFY_AND_RETURN(list, 1, 0, 2, 0, 0, "ERROR IN DTOR");

    FinishLog(&(list->file));

    list->free     = 0;
    list->capacity = 0;
    list->size     = 0;
/*
    free(list->data);
    free(list->prev);
    free(list->next);

    list->data = nullptr;
    list->prev = nullptr;
    list->next = nullptr;
*/
    free(list->data);

    list->data = nullptr;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListLinearize(list_type* list) {
    LIST_VERIFY_AND_RETURN(list, 1, 0, 2, 0, 1, "ERROR BEFORE LINEARIZE");

    /*
    ssize_t* prev     = nullptr;
    list_elem_t* data = nullptr;
    ssize_t* next     = nullptr;

    ARRAY_REALLOC_AND_RETURN(ssize_t,     prev, list->capacity);
    ARRAY_REALLOC_AND_RETURN(list_elem_t, data, list->capacity);
    ARRAY_REALLOC_AND_RETURN(ssize_t,     next, list->capacity);
    */
    list_data_type* buffer = nullptr;
    ARRAY_REALLOC_AND_RETURN(list_data_type, buffer, list->capacity);

    /*
    prev[0] = (ssize_t)list->size);
    data[0] = DATA_POISON);
    next[0] = 1;
    */
    buffer[0] = {(ssize_t)list->size, DATA_POISON, 1};

    ssize_t logic_pos = 1;

    for (ssize_t pos = ListGetHead(list); pos != 0; pos = GetNext(list, pos)) {
        /*
        prev[logic_pos] = logic_pos - 1;
        data[logic_pos] = GetValue(list, pos);
        next[logic_pos] = logic_pos + 1;
        */
        buffer[logic_pos] = {logic_pos - 1, GetValue(list, pos), logic_pos + 1};

        logic_pos++;
    }
    buffer[list->size].next = 0;

    for (ssize_t pos = list->free; pos != INDX_POISON; pos = GetNext(list, pos)) {
        /*
        prev[logic_pos] = INDX_POISON);
        data[logic_pos] = DATA_POISON);
        next[logic_pos] = logic_pos + 1;
        */
        buffer[logic_pos] = {INDX_POISON, DATA_POISON, logic_pos + 1};

        logic_pos++;
    }
    buffer[list->capacity].next = -1;

    /*
    free(list->prev);
    free(list->data);
    free(list->next);

    list->prev = prev;
    list->data = data;
    list->next = next;
    */
    free(list->data);
    list->data = buffer;

    list->free = (ssize_t)list->size + 1;

    LIST_VERIFY_AND_RETURN(list, 1, 0, 2, 0, 1, "ERROR AFTER LINEARIZE");

    pos_color updated_els = {INDX_POISON, INDX_POISON, INDX_POISON, INDX_POISON};
    ListLog(list, "LINEARIZE", list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

ssize_t ListGetHead(list_type* list) {
    return GetNext(list, 0);
}

//----------------------------------------------------------------------------------

ssize_t ListGetTail(list_type* list) {
    return GetPrev(list, 0);
}

//----------------------------------------------------------------------------------

void ListPrint(list_type* list, const char* message) {
    ListDump(list, list_error_t::OK, INDX_POISON, message);
}

//----------------------------------------------------------------------------------

list_error_t ListInsertBefore(list_type* list, const ssize_t position, const list_elem_t value) {
    const ssize_t new_position = GetPrev(list, position);

    return ListInsertAfter(list, new_position, value);
}

//----------------------------------------------------------------------------------

list_error_t ListInsertAfter(list_type* list, const ssize_t position, const list_elem_t value) {
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE EXECUTION INS(A)[%zd]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 0, (ssize_t)list->capacity, 1, 0, message_ver_b);

    if (list->size == list->capacity) {
        list_error_t error = ListSafeAllocationMemory(list, list->capacity * 2);
        if (error != list_error_t::OK) { return error; }
    }

    ssize_t prev_addr = position;
    ssize_t next_addr = GetNext(list, position);
    ssize_t free_addr = list->free;

    FindFree(list);

    pos_color updated_els = {INDX_POISON, free_addr, prev_addr, next_addr};

    // Write new element
    UpdatePrev(list,  free_addr, prev_addr);
    UpdateValue(list, free_addr, value);
    UpdateNext(list,  free_addr, next_addr);

    // Update previous element's next
    UpdateNext(list, prev_addr, free_addr);

    // Update next element's previous
    UpdatePrev(list, next_addr, free_addr);

    list->size++;

    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER EXECUTION INS(A)[%zd]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 0, (ssize_t)list->capacity, 1, 0, message_ver_a);

    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "INSERT %d AFTER POSITION: %zd", value, position);
    ListLog(list, message, list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListRemove(list_type* list, const ssize_t position, list_elem_t* value) {
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE EXECUTION REM[%zd]", position);
    LIST_VERIFY_AND_RETURN(list, position, 1, (ssize_t)list->capacity, 1, 1, message_ver_b);

    ssize_t prev_addr = GetPrev(list, position);
    ssize_t next_addr = GetNext(list, position);

    *value = GetValue(list, position);;

    pos_color updated_els = {position, INDX_POISON, prev_addr, next_addr};

    // Remove element at position
    UpdatePrev(list,  position, INDX_POISON);
    UpdateValue(list, position, DATA_POISON);
    UpdateNext(list,  position, INDX_POISON);

    // Update previous element's next
    UpdateNext(list, prev_addr, next_addr);

    // Update next element's previous
    UpdatePrev(list, next_addr, prev_addr);

    list->size--;

    AddFree(list, position);

    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER EXECUTION REM[%zd]", position);
    LIST_VERIFY_AND_RETURN(list, position, 1, (ssize_t)list->capacity, 0, (int)list->size, message_ver_a);

    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "REMOVE ELEMENT ON POSITION: %zd", position);
    ListLog(list, message, list->file, &updated_els);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

static void FillListPoison(list_type* list, const ssize_t start, const ssize_t end) {
    for (ssize_t pos = start; pos < end; pos++) {
        UpdatePrev(list,  pos, INDX_POISON);
        UpdateValue(list, pos, DATA_POISON);
        UpdateNext(list,  pos, INDX_POISON);

        AddFree(list, pos);
    }
}

//----------------------------------------------------------------------------------

static void FindFree(list_type* list) {
    list->free = GetNext(list, list->free);
}

//----------------------------------------------------------------------------------

static void AddFree(list_type* list, const ssize_t position) {
    if (list->free < 1) {
        UpdateNext(list, position, -1);
    } else {
        UpdateNext(list, position, list->free);
    }
    list->free = position;
}

//----------------------------------------------------------------------------------

static list_error_t ListSafeAllocationMemory(list_type* list, const size_t new_capacity) {
/*
    ARRAY_REALLOC_AND_RETURN(list_elem_t, list->data, new_capacity);
    ARRAY_REALLOC_AND_RETURN(ssize_t, list->prev, new_capacity);
    ARRAY_REALLOC_AND_RETURN(ssize_t, list->next, new_capacity);
*/
    ARRAY_REALLOC_AND_RETURN(list_data_type, list->data, new_capacity);

    FillListPoison(list, (ssize_t)(1 + list->capacity), (ssize_t)(new_capacity + 1));

    list->capacity = new_capacity;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

static list_error_t ListVerify(list_type* list, ssize_t* position, const ssize_t pos1, const ssize_t pos2, const int check_position, const int check_size) {
    if (*position < pos1 || *position > pos2) {
        return list_error_t::INVALID_POSITION;
    }
    if (list->size > list->capacity) {
        return list_error_t::LIST_CORRUPTED;
    }
    if (check_position == 1 && *position > 0 && GetValue(list, *position) == DATA_POISON) {
        return list_error_t::POSITION_FREE;
    }
    if (check_size > 0 && list->size == 0) {
        return list_error_t::LIST_EMPTY;
    }
    ssize_t pos  = 0;
    size_t count = 0;

    while (true) {
        pos = GetNext(list, pos);

        if (pos == 0) { break; }

        if (GetValue(list, pos) == DATA_POISON) {
            return list_error_t::INVALID_ELEMENT;
        }
        if (GetPrev(list, pos) > (ssize_t)list->capacity) {
            *position = pos;
            return list_error_t::LIST_PREV_ADDR_ERROR;
        }
        if (GetNext(list, pos) > (ssize_t)list->capacity) {
            *position = pos;
            return list_error_t::LIST_NEXT_ADDR_ERROR;
        }
        count++;
    }
    if (count != list->size) {
        return list_error_t::LIST_SIZE_ERROR;
    }
    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

static void ListDump(list_type* list, list_error_t error, const ssize_t pos, const char* message) {
    PRINT_COLOR(CYAN, "\n============LIST DUMP=============\n");
    if (error != list_error_t::OK) {
        PRINT_COLOR_VAR(RED, "ERROR: %s\n", ListErrorsArray[static_cast <int>(error)]);
    }
    printf("COMMENT: %s\n", message);
    printf("List: %s\n\n", list->name);
    pos_color error_el = {INDX_POISON, INDX_POISON, INDX_POISON, INDX_POISON};

    switch(error) {
        case list_error_t::OK:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::FILE_OPEN_ERROR:
            PRINT_COLOR(RED, "FILE FOR LOG OPEN ERROR\n");
            break;
        case list_error_t::ALLOCATION_ERROR:
            PRINT_COLOR_VAR(RED, "LOG ADDR: %p\n", list->file);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::INVALID_POSITION:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "CALLED POSITION: %zd\n", pos);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::INVALID_ELEMENT:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::POSITION_FREE:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "ELEMENT %zd = POISON\n", pos);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_SIZE_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            PRINT_COLOR(RED, "LIST SIZE != COUNT LIST'S ELEMENTS\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_CORRUPTED:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_EMPTY:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_PREV_ADDR_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %zd\n", pos);
            printf("INVALID PREVIOUS ELEMENT: ");
            PRINT_COLOR_VAR(RED, "%zd\n", GetPrev(list, pos));
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_NEXT_ADDR_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetHead(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", ListGetTail(list));
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zd ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %zd\n", pos);
            printf("INVALID NEXT ELEMENT: ");
            PRINT_COLOR_VAR(RED, "%zd\n", GetNext(list, pos));
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        default:
            PRINT_COLOR(RED, "UNKNOWN ERROR\n");
    }
    PRINT_COLOR(CYAN, "==================================\n\n");
}
