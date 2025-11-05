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
    int position_ = pos; \
    list_error_t error_ = ListVerify(list, &position_, pos1, pos2, check_pos, check_size); \
    if (error_ != list_error_t::OK) { \
        ListDump(list, error_, position_, message); \
        return error_; \
    } \
} END

static void FillListPoison(list_type* list, const size_t start, const size_t end);
static void FindFree(list_type* list);
static void AddFree(list_type* list, const int position);
static list_error_t ListSafeAllocationMemory(list_type* list, const size_t new_capacity);
static list_error_t ListVerify(list_type* list, int* position, const int pos1, const int pos2, const int check_position, const int check_size);
static void ListDump(list_type* list, list_error_t error, const int pos, const char* message);


list_error_t ListCtor(list_type* list, const char* name) {

    snprintf(list->name, LIST_NAME_SIZE, "%s", name);

    FILE* file_ = nullptr;
    StartLog(&file_, list->name);
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

    list->capacity = 0;
    list->size     = 0;

    list->prev = nullptr;
    list->data = nullptr;
    list->next = nullptr;

    list->free = -1;

    list_error_t error = ListSafeAllocationMemory(list, DEFAULT_CTOR_CAPACITY);
    if (error != list_error_t::OK) { return error; }

    list->prev[0] = 0;
    list->data[0] = DATA_POISON;
    list->next[0] = 0;

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

void ListPrint(list_type* list, const char* message) {
    ListDump(list, list_error_t::OK, INDX_POISON, message);
}

//----------------------------------------------------------------------------------

list_error_t ListInsertBefore(list_type* list, const int position, const list_elem_t value) {
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE EXECUTION INS(B)[%d]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 1, (int)list->capacity, 1, 0, message_ver_b);

    if (list->size == list->capacity) {
        list_error_t error = ListSafeAllocationMemory(list, list->capacity * 2);
        if (error != list_error_t::OK) { return error; }
    }

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

    char message_log[LOG_MESSAGE_SIZE] = {};
    snprintf(message_log, LOG_MESSAGE_SIZE, "INSERT %d BEFORE POSITION: %d", value, position);
    ListLog(list, message_log, list->file, &updated_els);

    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER EXECUTION INS(B)[%d]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 1, (int)list->capacity, 1, 0, message_ver_a);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListInsertAfter(list_type* list, const int position, const list_elem_t value) {
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE EXECUTION INS(A)[%d]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 0, (int)list->capacity, 1, 0, message_ver_b);

    if (list->size == list->capacity) {
        list_error_t error = ListSafeAllocationMemory(list, list->capacity * 2);
        if (error != list_error_t::OK) { return error; }
    }

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

    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "INSERT %d AFTER POSITION: %d", value, position);
    ListLog(list, message, list->file, &updated_els);

    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER EXECUTION INS(A)[%d]: %d ", position, value);
    LIST_VERIFY_AND_RETURN(list, position, 0, (int)list->capacity, 1, 0, message_ver_a);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListRemove(list_type* list, const int position, list_elem_t* value) {
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE EXECUTION REM[%d]", position);
    LIST_VERIFY_AND_RETURN(list, position, 1, (int)list->capacity, 1, 1, message_ver_b);

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

    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "REMOVE ELEMENT ON POSITION: %d", position);
    ListLog(list, message, list->file, &updated_els);

    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER EXECUTION REM[%d]", position);
    LIST_VERIFY_AND_RETURN(list, position, 1, (int)list->capacity, 0, (int)list->size, message_ver_a);

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
    if (list->free < 1) {
        list->next[position] = -1;
    } else {
        list->next[position] = list->free;
    }
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

//----------------------------------------------------------------------------------

static list_error_t ListVerify(list_type* list, int* position, const int pos1, const int pos2, const int check_position, const int check_size) {
    if (*position < pos1 || *position > pos2) {
        return list_error_t::INVALID_POSITION;
    }
    if (list->size > list->capacity) {
        return list_error_t::LIST_CORRUPTED;
    }
    if (check_position == 1 && *position > 0 && list->data[*position] == DATA_POISON) {
        return list_error_t::POSITION_FREE;
    }
    if (check_size > 0 && list->size == 0) {
        return list_error_t::LIST_EMPTY;
    }
    int pos      = 0;
    size_t count = 0;

    while (true) {
        pos = list->next[pos];

        if (pos == 0) { break; }

        if (list->data[pos] == DATA_POISON) {
            return list_error_t::INVALID_ELEMENT;
        }
        if (list->prev[pos] > (int)list->capacity) {
            *position = pos;
            return list_error_t::LIST_PREV_ADDR_ERROR;
        }
        if (list->next[pos] > (int)list->capacity) {
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

static void ListDump(list_type* list, list_error_t error, const int pos, const char* message) {
    PRINT_COLOR(CYAN, "\n============LIST DUMP=============\n");
    if (error != list_error_t::OK) {
        PRINT_COLOR_VAR(RED, "ERROR: %s\n", ListErrorsArray[static_cast <int>(error)]);
    }
    printf("List: %s\n\n", list->name);
    pos_color error_el = {INDX_POISON, INDX_POISON, INDX_POISON, INDX_POISON};

    switch(error) {
        case list_error_t::OK:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
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
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "CALLED POSITION: %d\n", pos);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::INVALID_ELEMENT:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
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
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu", list->capacity);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "ELEMENT %d = POISON\n", pos);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_SIZE_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
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
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
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
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
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
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %d\n", pos);
            printf("INVALID PREVIOUS ADDRESS: ");
            PRINT_COLOR_VAR(RED, "%d\n", list->prev[pos]);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        case list_error_t::LIST_NEXT_ADDR_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___FREE___ | ___SIZE___ | _CAPACITY_ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->next[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->prev[0]);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10d ", list->free);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->capacity);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %d\n", pos);
            printf("INVALID NEXT ADDRESS: ");
            PRINT_COLOR_VAR(RED, "%d\n", list->next[pos]);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = pos;
            ListLog(list, message, list->file, &error_el);
            break;
        default:
            PRINT_COLOR(RED, "UNKNOWN ERROR\n");
    }
    PRINT_COLOR(CYAN, "==================================\n\n");
}
