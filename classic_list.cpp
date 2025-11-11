#include <stdlib.h>

#include "logger.h"
#include "classic_list.h"

#ifndef TEST
#define CLASSIC_LIST_VERIFY_AND_RETURN(list, elem, check_ptr, check_val, check_size, message) BEGIN { \
    ClassicListElement* elem_ = elem; \
    error_t error_ = ClassicListVerify(list, &elem_, check_ptr, check_val, check_size); \
    if (error_ != error_t::OK) { \
        ClassicListDump(list, error_, elem_, message); \
        return error_; \
    } \
} END


static error_t ClassicListVerify(List* list, ClassicListElement** element, const int check_ptr, const int check_value, const int check_size);
#endif
static error_t ClassicListDtorElement(List* list, ClassicListElement** element);
static void ClassicListDump(List* list, error_t error, ClassicListElement* element, const char* message);


error_t ClassicListCtor(List* list, const char* name) {
    snprintf(list->name, CLASSIC_LIST_NAME_SIZE, "%s", name);

    FILE* file_ = nullptr;
    StartLog(&file_, list->name);
    if (file_ == nullptr) {
        return error_t::FILE_OPEN_ERROR;
    }
    list->file = file_;
    list->dump_count = 1;

    list->size = 0;

    ClassicListElement* buf = (ClassicListElement*)calloc(1, sizeof(ClassicListElement));

    if (buf == nullptr) { return error_t::ALLOCATION_ERROR; }

    list->fict_elem = buf;

    list->fict_elem->value = POISON;
    list->fict_elem->prev  = list->fict_elem;
    list->fict_elem->next  = list->fict_elem;

    #ifndef TEST
    CLASSIC_LIST_VERIFY_AND_RETURN(list, list->fict_elem, 0, 0, 0, "ERROR IN CTOR");
    #endif

    return error_t::OK;
}

//----------------------------------------------------------------------------------

error_t ClassicListDtor(List* list) {
    #ifndef TEST
    CLASSIC_LIST_VERIFY_AND_RETURN(list, list->fict_elem, 1, 0, 0, "ERROR IN DTOR");
    #endif

    FinishLog(&(list->file));

    ClassicListElement* elem  = list->fict_elem;
    ClassicListElement* elem_ = nullptr;

    do {
        elem_ = elem->next;
        ClassicListDtorElement(list, &elem);
        elem = elem_;
    } while (elem != list->fict_elem);

    return error_t::OK;
}

//----------------------------------------------------------------------------------

static error_t ClassicListDtorElement(List* list, ClassicListElement** element) {
    (*element)->value = POISON;
    (*element)->prev  = nullptr;
    (*element)->next  = nullptr;

    free(*element);
    *element = nullptr;

    list->size--;

    return error_t::OK;
}

//----------------------------------------------------------------------------------

ClassicListElement* ClassicListGetHead(List* list) {
    return list->fict_elem->next;
}

//----------------------------------------------------------------------------------

ClassicListElement* ClassicListGetTail(List* list) {
    return list->fict_elem->prev;
}

//----------------------------------------------------------------------------------

void ClassicListPrint(List* list, const char* message) {
    ClassicListDump(list, error_t::OK, nullptr, message);
}

//----------------------------------------------------------------------------------

error_t ClassicListInsertBefore(List* list, ClassicListElement* element, const classic_list_elem_t value) {
    ClassicListElement* new_element = element->prev;

    return ClassicListInsertAfter(list, new_element, value);
}

//----------------------------------------------------------------------------------

error_t ClassicListInsertAfter(List* list, ClassicListElement* element, const classic_list_elem_t value) {
    #ifndef TEST
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE INSERT %d AFTER ELEMENT: %zX", value, (size_t)element);
    CLASSIC_LIST_VERIFY_AND_RETURN(list, element, 1, 1, 0, message_ver_b);
    #endif

    ClassicListElement* prev = element;
    ClassicListElement* next = element->next;

    ClassicListElement* buf = (ClassicListElement*)calloc(1, sizeof(ClassicListElement));

    if (buf == nullptr) { return error_t::ALLOCATION_ERROR; }

    next->prev = buf; // update next element
    prev->next = buf; // update previous element

    buf->value = value;
    buf->prev  = prev;
    buf->next  = next;

    #ifdef DEBUG
    elems_color updated_els = {nullptr, buf, prev, next};
    #endif

    list->size++;

    #ifndef TEST
    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER INSERT %d AFTER ELEMENT: %zX", value, (size_t)element);
    CLASSIC_LIST_VERIFY_AND_RETURN(list, element, 1, 1, 1, message_ver_a);
    #endif

    #ifdef DEBUG
    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "INSERT %d AFTER ELEMENT: %zX", value, (size_t)element);
    ClassicListLog(list, message, list->file, &updated_els);
    #endif

    return error_t::OK;
}

//----------------------------------------------------------------------------------

error_t ClassicListRemove(List* list, ClassicListElement* element, classic_list_elem_t* value) {
    #ifndef TEST
    char message_ver_b[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_b, LOG_MESSAGE_SIZE, "INVALID LIST BEFORE REMOVE[%zX]", (size_t)element);
    CLASSIC_LIST_VERIFY_AND_RETURN(list, element, 1, 1, 1, message_ver_b);
    #endif

    *value = element->value;

    ClassicListElement* prev = element->prev;
    ClassicListElement* next = element->next;

    prev->next = next; // update previous element
    next->prev = prev; // update next element

    element->value = POISON;
    element->prev  = nullptr;
    element->next  = nullptr;

    free(element);
    element = nullptr;

    #ifdef DEBUG
    elems_color updated_els = {element, nullptr, prev, next};
    #endif

    list->size--;

    #ifndef TEST
    char message_ver_a[LOG_MESSAGE_SIZE] = {};
    snprintf(message_ver_a, LOG_MESSAGE_SIZE, "INVALID LIST AFTER REMOVE[%zX]", (size_t)element);
    CLASSIC_LIST_VERIFY_AND_RETURN(list, element, 0, 0, 0, message_ver_a);
    #endif

    #ifdef DEBUG
    char message[LOG_MESSAGE_SIZE] = {};
    snprintf(message, LOG_MESSAGE_SIZE, "REMOVE ELEMENT: %zX", (size_t)element);
    ClassicListLog(list, message, list->file, &updated_els);
    #endif

    return error_t::OK;
}

//----------------------------------------------------------------------------------
#ifndef TEST
static error_t ClassicListVerify(List* list, ClassicListElement** element, const int check_ptr, const int check_value, const int check_size) {
    if (check_ptr == 1 && ((size_t)(*element) < MIN_NORMAL_ADDR || (size_t)(*element) > MAX_NORMAL_ADDR)) {
        return error_t::POINTER_ERROR;
    }
    if (check_value == 1 && *element != list->fict_elem && (*element)->value == POISON) {
        return error_t::POSITION_FREE;
    }
    if (check_size > 0 && list->size == 0) {
        return error_t::LIST_EMPTY;
    }

    #ifdef DEBUG
    ClassicListElement* elem = list->fict_elem;
    size_t count = 0;

    while (true) {
        elem = elem->next;

        if (elem == list->fict_elem) { break; }

        if (elem->value == POISON) {
            return error_t::INVALID_ELEMENT;
        }
        if (elem->prev->next != elem) {
            *element = elem;
            return error_t::PREV_ADDR_ERROR;
        }
        if (elem->next->prev != elem) {
            *element = elem;
            return error_t::NEXT_ADDR_ERROR;
        }
        count++;
    }
    if (count != list->size) {
        return error_t::LIST_SIZE_ERROR;
    }
    #endif

    return error_t::OK;
}
#endif
//----------------------------------------------------------------------------------

static void ClassicListDump(List* list, error_t error, ClassicListElement* element, const char* message) {
    PRINT_COLOR(CYAN, "\n============LIST DUMP=============\n");
    if (error != error_t::OK) {
        PRINT_COLOR_VAR(RED, "ERROR: %s\n", ListErrorsArray[static_cast <int>(error)]);
    }
    printf("COMMENT: %s\n", message);
    printf("List: %s\n\n", list->name);
    elems_color error_el = {nullptr, nullptr, nullptr, nullptr};

    switch(error) {
        case error_t::OK:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::FILE_OPEN_ERROR:
            PRINT_COLOR(RED, "FILE FOR LOG OPEN ERROR\n");
            break;
        case error_t::ALLOCATION_ERROR:
            PRINT_COLOR_VAR(RED, "LOG ADDR: %p\n", list->file);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::POINTER_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "INVALID ELEMENT POINTER = %zX \n", (size_t)element);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::INVALID_ELEMENT:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::POSITION_FREE:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            PRINT_COLOR_VAR(RED, "ELEMENT %zX = POISON\n", (size_t)element);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::LIST_SIZE_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            PRINT_COLOR(RED, "LIST SIZE != COUNT LIST'S ELEMENTS\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::LIST_EMPTY:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::PREV_ADDR_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %zX\n", (size_t)element);
            printf("INVALID PREVIOUS ELEMENT: ");
            PRINT_COLOR_VAR(RED, "%zX\n", (size_t)element->prev);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        case error_t::NEXT_ADDR_ERROR:
            printf("| ___HEAD___ | ___TAIL___ | ___SIZE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->next);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)list->fict_elem->prev);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", list->size);
            printf("|");
            printf("\n\n");
            printf("FOR ELEMENT: %zX\n", (size_t)element);
            printf("INVALID NEXT ELEMENT: ");
            PRINT_COLOR_VAR(RED, "%zX\n", (size_t)element->next);
            printf("DUMP NUMBER: %zu\n", list->dump_count);
            error_el.posR = element;
            ClassicListLog(list, message, list->file, &error_el);
            break;
        default:
            PRINT_COLOR(RED, "UNKNOWN ERROR\n");
    }
    PRINT_COLOR(CYAN, "==================================\n\n");
}
