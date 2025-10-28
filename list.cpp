#include <stdio.h>
#include <stdlib.h>

#include "define_lib.h"
#include "color_lib.h"
#include "list.h"

#define ARRAY_REALLOC_AND_RETURN(TYPE, array, size) BEGIN {\
    TYPE* buffer_arr = nullptr; \
    buffer_arr = (TYPE*)realloc(array, (size + 1) * sizeof(TYPE)); \
    if (buffer_arr == nullptr) { return list_error_t::ERROR; } \
    array = buffer_arr; \
    } END

static void FillDataPoison(list_elem_t* data, size_t size);
static void FillIndxPoison(int* indx_array, size_t size);
static void FindFree(list_type* list);
static list_error_t ListSafeAllocationMemory(list_type* list, size_t new_capacity);
void ListLog(list_type* list, const char* func, FILE* file, pos_color* positions);


list_error_t ListCtor(list_type* list) {

    const size_t default_ctor_capacity = 1;

    list->capacity = 0;
    list->size     = 1;

    ListSafeAllocationMemory(list, default_ctor_capacity);

    list->pref[0] = 0;
    list->data[0] = DATA_POISON;
    list->next[0] = 0;

    list->head = 0;
    list->tail = 0;
    list->free = 1;

    const char* filename = "dump.htm";

    FILE* file = fopen(filename, "wb");
    if (file == nullptr) {
        return list_error_t::ERROR;
    }
    list->file = file;

    system("rm -r log_folder");
    system("mkdir log_folder");
    list->dump_count = 1;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListDtor(list_type* list) {

    fclose(list->file);

    list->head = 0;
    list->tail = 0;
    list->free = 0;

    list->capacity = 0;

    free(&(list->data));
    free(&(list->pref));
    free(&(list->next));

    list->data = nullptr;
    list->pref = nullptr;
    list->next = nullptr;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

void ListPrint(list_type* list) {
    for (size_t pos = 1; pos < list->capacity + 1; pos++) {
        printf("[%zu] ", pos);
        PRINT_LIST_ELEMENT(list->pref[pos], list->data[pos], list->next[pos]);
    }
}

//----------------------------------------------------------------------------------

list_error_t ListPush(list_type* list, int position, list_elem_t value) {
    if (list->size > list->capacity + 1) {
        return list_error_t::ERROR;
    } else if (list->size == list->capacity + 1) {
        ListSafeAllocationMemory(list, list->capacity * 2);
        list->free = (int)list->size;
    }
    int pref_addr = 0;
    int next_addr = 0;

    pos_color push = {INDX_POISON, list->free, INDX_POISON, INDX_POISON};

    if (list->data[position] != DATA_POISON) {
        pref_addr = list->pref[position];
        next_addr = position;
    } else {
        pref_addr = list->tail;
        next_addr = 0;
    }

    // запись нового
    list->pref[list->free] = pref_addr;
    list->data[list->free] = value;
    list->next[list->free] = next_addr;

    // перезапись next предыдущего
    if (pref_addr > 0) {
        list->next[pref_addr] = list->free;
        push.posY1 = pref_addr;
    } else {
        list->head = list->free;
    }

    // перезапись pref прошлого
    if (next_addr > 0) {
        list->pref[next_addr] = list->free;
        push.posY2 = next_addr;
    } else {
        list->tail = position;
    }

    list->size++;
    FindFree(list);

    ListLog(list, "PUSH", list->file, &push);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

list_error_t ListPop(list_type* list, int position, list_elem_t* value) {

    if (position < 1 || position > list->tail || list->size == 0) {
        return list_error_t::ERROR;
    }
    if (list->data[position] == DATA_POISON) {
        return list_error_t::ERROR;
    }

    int pref_addr = list->pref[position];
    int next_addr = list->next[position];
    *value = list->data[position];

    pos_color pop = {position, INDX_POISON, INDX_POISON, INDX_POISON};

    // удаление элемента с позицией pos
    list->pref[position] = INDX_POISON;
    list->data[position] = DATA_POISON;
    list->next[position] = INDX_POISON;

    // перезапись next предыдущего
    if (pref_addr > 0) {
        list->next[pref_addr] = next_addr;
        pop.posY1 = pref_addr;
    } else {
        list->head = next_addr;
    }

    // перезапись pref следующего
    if (next_addr > 0) {
        list->pref[next_addr] = pref_addr;
        pop.posY2 = next_addr;
    } else {
        list->tail = pref_addr;
    }

    list->size--;
    FindFree(list);

    ListLog(list, "POP", list->file, &pop);

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

static void FillDataPoison(list_elem_t* data, size_t size) {
    for (size_t pos = 0; pos < size; pos++) {
        data[pos] = DATA_POISON;
    }
}

//----------------------------------------------------------------------------------

static void FillIndxPoison(int* indx_array, size_t size) {
    for (size_t pos = 0; pos < size; pos++) {
        indx_array[pos] = INDX_POISON;
    }
}

//----------------------------------------------------------------------------------

static void FindFree(list_type* list) {
    //list->free = list->next[list->free];
    for (size_t pos = 1; pos <= list->capacity; pos++) {
        if (list->data[pos] == DATA_POISON) {
            list->free = (int)pos;
            return;
        }
    }
    list->free = -1;
}

//----------------------------------------------------------------------------------

static list_error_t ListSafeAllocationMemory(list_type* list, size_t new_capacity) {
    ARRAY_REALLOC_AND_RETURN(list_elem_t, list->data, new_capacity);
    FillDataPoison(&(list->data[1 + list->capacity]), new_capacity - list->capacity);

    ARRAY_REALLOC_AND_RETURN(int, list->pref, new_capacity);
    FillIndxPoison(list->pref + 1 + list->capacity, new_capacity - list->capacity);

    ARRAY_REALLOC_AND_RETURN(int, list->next, new_capacity);
    FillIndxPoison(list->next + 1 + list->capacity, new_capacity - list->capacity);

    list->capacity = new_capacity;

    return list_error_t::OK;
}

//----------------------------------------------------------------------------------

void ListLog(list_type* list, const char* func, FILE* file, pos_color* positions) {
    char filename_dot[100] = "";
    snprintf(filename_dot, 100, "log_folder/push_dot_%zu.txt", list->dump_count);
    FILE* file_dot = fopen(filename_dot, "wb");
    fprintf(file_dot,
        "digraph G{\n"
        "node[shape=Mrecord, style=filled]\n"
        "rankdir=LR;\n"
        "ordering=out;\n");

    unsigned int color    = 0;
    unsigned int bg_color = 0;

    for (size_t pos = 0; pos < list->capacity + 1; pos++) {
        if (pos == (size_t)positions->posG) {
            color    = 0x00c000;
            bg_color = 0xa0f0a0;
        } else
        if (pos == (size_t)positions->posR) {
            color    = 0xc00000;
            bg_color = 0xf0a0a0;
        } else
        if (pos == (size_t)positions->posY1 || pos == (size_t)positions->posY2) {
            color    = 0xc0c000;
            bg_color = 0xf0f0a0;
        } else
        if (list->data[pos] != DATA_POISON) {
            color    = 0xc0f0c0;
            bg_color = 0xe0f0e0;
        } else {
            color    = 0x808080;
            bg_color = 0xc0c0c0;
        }
        fprintf(file_dot,
            "element%zu [color=\"#%06X\", fillcolor=\"#%06X\", label=\" pos: %zu | <pref> %d | %d | <next> %d \"];\n",
            pos, color, bg_color, pos, list->pref[pos], list->data[pos], list->next[pos]);
    }

    for (size_t pos = 0; pos < list->capacity; pos++) {
        fprintf(file_dot, "element%zu -> element%zu [style=invis, weight=100];\n", pos, pos + 1);
    }

    for (size_t pos = 1; pos < list->capacity + 1; pos++) {
        if (list->pref[pos] != INDX_POISON) {
            fprintf(file_dot, "element%zu -> element%d [constraint=false, weight=0, color=\"#0000ff\"];\n", pos, list->pref[pos]);
        }
        if (list->next[pos] != INDX_POISON) {
            fprintf(file_dot, "element%zu -> element%d [constraint=false, weight=0, color=\"#ff0000\"];\n", pos, list->next[pos]);
        }
    }

    fprintf(file_dot, "}");
    fclose(file_dot);

    char command[100] = "";
    snprintf(command, 100, "dot -Tsvg log_folder/push_dot_%zu.txt -o log_folder/push_dot_%zu.svg", list->dump_count, list->dump_count);
    system(command);

    fprintf(file,
        "<pre>\n"
        "<div style=\"margin: 20px; padding: 15px; border: 2px solid #4CAF50; border-radius: 10px; background: #f8f9fa; box-shadow: 0 4px 8px rgba(0,0,0,0.1);\">\n"
            "<h2 style=\"color: #2E86AB; margin-top: 0;\">List Debug %zu %s</h2>\n"
            "<div style=\"display: table;\">\n"
                "<div style=\"width: 20%%;\">\n"
                    "<h3 style=\"color: #A23B72;\">Parameters</h3>\n"
                    "<table style=\"width: 100%%; border-collapse: collapse;\">\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">HEAD</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%d</td>"
                        "</tr>\n"
                        "<tr>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">TAIL</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%d</td>"
                        "</tr>\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">FREE</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%d</td>"
                        "</tr>\n"
                        "<tr>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">CAPACITY</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                        "</tr>\n"
                    "</table>\n"
                "</div>\n"
                "<div style=\"min-width: 40%%;\">\n"
                    "<h3 style=\"color: #A23B72;\">Visualization</h3>\n"
                    "<img src=\"log_folder/push_dot_%zu.svg\" style=\"max-width: 100%%; height: auto; border: 1px solid #ddd; border-radius: 5px;\" alt=\"List visualization\">\n"
                "</div>\n"
            "</div>\n"
            "<hr style=\"margin: 15px 0; border: 0; border-top: 1px solid #ccc;\">\n"
            "<small style=\"color: #666;\">Generated from: %s</small>\n"
        "</div>\n\n",
        list->dump_count,
        func,
        list->head,
        list->tail,
        list->free,
        list->capacity,
        list->dump_count++,
        filename_dot);

}
