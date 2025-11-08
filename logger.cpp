#include <stdio.h>
#include <stdlib.h>

#include "dot.h"
#include "logger.h"

void GetFullFolderName(const char* list_name, char* buffer) {
    const char* folder_name = LOG_FOLDER;
    snprintf(buffer, LOG_FOLDER_NAME_SIZE, "%s_%s", folder_name, list_name);
}

//----------------------------------------------------------------------------------

void UpdateFolder(const char* name_folder) {
    char command[COMMAND_SIZE] = "";
    snprintf(command, LOG_FOLDER_NAME_SIZE, "%s%s", "rm -r ", name_folder);
    system(command);
    snprintf(command, LOG_FOLDER_NAME_SIZE, "%s%s", "mkdir ", name_folder);
    system(command);
}

//----------------------------------------------------------------------------------

void StartLog(FILE** file, const char* list_name) {
    char filename[LOG_FILE_NAME_SIZE] = "";
    snprintf(filename, LOG_FOLDER_NAME_SIZE, "%s_%s.htm", DUMP_FILENAME, list_name);
    *file = fopen(filename, "wb");
}

//----------------------------------------------------------------------------------

void FinishLog(FILE** file) {
    fclose(*file);
    *file = nullptr;
}

//----------------------------------------------------------------------------------

void ListLog(list_type* list, const char* message, FILE* file, pos_color* positions) {
    char filename_dot[DOT_FILE_NAME_SIZE] = "";
    snprintf(filename_dot, DOT_FILE_NAME_SIZE, "%s_%s/push_dot_%s_%zu.txt", LOG_FOLDER, list->name, list->name, list->dump_count);
    FILE* file_dot = fopen(filename_dot, "wb");
    StartDot(file_dot);

    unsigned int color    = 0;
    unsigned int bg_color = 0;

    for (ssize_t pos = 0; pos < (ssize_t)list->capacity + 1; pos++) {
        if (pos == positions->posG) {
            color    = 0x00c000;
            bg_color = 0xa0f0a0;
        } else
        if (pos == positions->posR) {
            color    = 0xc00000;
            bg_color = 0xf0a0a0;
        } else
        if (pos == positions->posY1 || pos == positions->posY2) {
            color    = 0xc0c000;
            bg_color = 0xf0f0a0;
        } else
        if (pos == 0) {
            color    = 0x0000c0;
            bg_color = 0xa0a0f0;
        } else
        if (GetValue(list, pos) != DATA_POISON) {
            color    = 0xa0c0a0;
            bg_color = 0xe0f0e0;
        } else {
            color    = 0x808080;
            bg_color = 0xc0c0c0;
        }

        char extra_label[DOT_PARAMS_SIZE] = "";
        if (pos == GetNext(list, 0)) {
            snprintf(extra_label, 100, "%s%s", extra_label, "HEAD | ");
        }
        if (pos == GetPrev(list, 0)) {
            snprintf(extra_label, 100, "%s%s", extra_label, "TAIL | ");
        }
        if (pos == list->free) {
            snprintf(extra_label, 100, "%s%s", extra_label, "FREE | ");
        }

        char params[DOT_PARAMS_SIZE] = "";
        snprintf(
            params,
            300,
            "color=\"#%06X\", fillcolor=\"#%06X\", penwidth=\"2\", label=\"%s index = %zd | value = %d | {prev = %zd | next = %zd}\"",
            color,
            bg_color,
            extra_label,
            pos,
            GetValue(list, pos),
            GetPrev(list, pos),
            GetNext(list, pos));
        MakeDotElement(file_dot, pos, params);
    }

    for (ssize_t pos = 0; pos < (ssize_t)list->capacity; pos++) {
        MakeDotElementConnection(file_dot, pos, pos + 1, "style=invis, weight=100");
    }

    for (ssize_t pos = 1; pos < (ssize_t)list->capacity + 1; pos++) {
        // draw prev and next

        if (GetValue(list, pos) != DATA_POISON && \
            GetNext(list, pos) != INDX_POISON && \
            GetValue(list, GetNext(list, pos)) != DATA_POISON && \
            pos == GetPrev(list, GetNext(list, pos)))
        {
            MakeDotElementConnection(file_dot, pos, GetNext(list, pos), "constraint=false, weight=0, color=\"#000000\", dir=both");
        }
        // draw prev xor next
        if (((GetNext(list, GetPrev(list, pos)) != pos) != \
            (pos != GetPrev(list, GetNext(list, pos)))) || \
            GetPrev(list, pos) == 0 || \
            GetPrev(list, pos) > (int)list->capacity || \
            GetNext(list, pos) == 0 || \
            GetNext(list, pos) > (int)list->capacity)
        {
            // draw prev
            if (GetPrev(list, pos) != INDX_POISON && (GetPrev(list, pos) == 0 || (pos != GetNext(list, GetPrev(list, pos)))) && GetValue(list, pos) != DATA_POISON) {
                MakeDotElementConnection(file_dot, pos, GetPrev(list, pos), "constraint=false, weight=0, color=\"#0000ff\"");
                if (GetPrev(list, pos) > (int)list->capacity) {
                    char params[300] = "";
                    snprintf(params, 300, "shape=\"octagon\", color=\"#ff0000\", fillcolor=\"#ffc0c0\", penwidth=\"2\", label=\" pos: %zd \"", GetPrev(list, pos));
                    MakeDotElement(file_dot, GetPrev(list, pos), params);
                }
            }
            // draw next
            if (GetNext(list, pos) != INDX_POISON && (GetNext(list, pos) == 0 || (pos != GetPrev(list, GetNext(list, pos)))) && GetValue(list, pos) != DATA_POISON) {
                MakeDotElementConnection(file_dot, pos, GetNext(list, pos), "constraint=false, weight=0, color=\"#ff0000\"");
                if (GetNext(list, pos) > (int)list->capacity) {
                    char params[300] = "";
                    snprintf(params, 300, "shape=\"octagon\", color=\"#ff0000\", fillcolor=\"#ffc0c0\", penwidth=\"2\", label=\" pos: %zd \"", GetNext(list, pos));
                    MakeDotElement(file_dot, GetNext(list, pos), params);
                }
            }
        }
        // draw free
        if (GetNext(list, pos) != INDX_POISON && GetValue(list, pos) == DATA_POISON) {
            MakeDotElementConnection(file_dot, pos, GetNext(list, pos), "constraint=false, weight=0, color=\"#00ff00\"");
            if (GetNext(list, pos) > (ssize_t)list->capacity) {
                char params[300] = "";
                snprintf(params, 300, "shape=\"octagon\", color=\"#ff0000\", fillcolor=\"#ffc0c0\", penwidth=\"2\", label=\" pos: %zd \"", GetNext(list, pos));
                MakeDotElement(file_dot, GetNext(list, pos), params);
            }
        }
    }

    FinishDot(file_dot);

    char command[COMMAND_SIZE] = "";
    snprintf(command, COMMAND_SIZE, "dot -Tsvg %s_%s/push_dot_%s_%zu.txt -o %s_%s/push_dot_%s_%zu.svg", LOG_FOLDER, list->name, list->name, list->dump_count, LOG_FOLDER, list->name, list->name, list->dump_count);
    system(command);

    fprintf(file,
        "<div style=\"margin: 20px; padding: 15px; border: 2px solid #4CAF50; border-radius: 10px; background: #f8f9fa; box-shadow: 0 4px 8px rgba(0,0,0,0.1);\">\n"
            "<h2 style=\"color: #2E86AB; margin-top: 0;\">List \"%s\" Debug %zu</h2>\n"
            "<h2 style=\"margin-top: 0;\">%s</h2>\n"
            "<div style=\"display: table;\">\n"
                "<div style=\"width: 20%%;\">\n"
                    "<h3 style=\"color: #A23B72;\">Parameters</h3>\n"
                    "<table style=\"width: 100%%; border-collapse: collapse;\">\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">HEAD</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zd</td>"
                        "</tr>\n"
                        "<tr>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">TAIL</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zd</td>"
                        "</tr>\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">FREE</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zd</td>"
                        "</tr>\n"
                        "<tr>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">SIZE</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                        "</tr>\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">CAPACITY</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                        "</tr>\n"
                    "</table>\n"
                "</div>\n",
        list->name,
        list->dump_count,
        message,
        GetNext(list, 0),
        GetPrev(list, 0),
        list->free,
        list->size,
        list->capacity);

    fprintf(file,
        "<div style=\"margin-top: 10px;\">\n"
        "<h3 style=\"color: #A23B72;\">List Array Representation</h3>\n"
        "<pre style=\"background: #f0f0f0; width: fit-content; padding: 10px; border: 1px solid #ccc; border-radius: 5px;\">\n");

    // index
    fprintf(file, "index: |");
    for (size_t i = 0; i <= list->capacity; i++) {
        fprintf(file, " %10zu |", i);
    } fprintf(file, "\n");

    fprintf(file, "-------|");
    for (size_t i = 0; i <= list->capacity; i++) {
        fprintf(file, "------------|");
    } fprintf(file, "\n");

    // data
    fprintf(file, "value: |");
    for (ssize_t i = 0; i <= (ssize_t)list->capacity; i++) {
        if (GetValue(list, i) == DATA_POISON) {
            fprintf(file, " %10s |", "poison");
        } else {
            fprintf(file, " %10d |", GetValue(list, i));
        }
    } fprintf(file, "\n");

    fprintf(file, "-------|");
    for (size_t i = 0; i <= list->capacity; i++) {
        fprintf(file, "------------|");
    } fprintf(file, "\n");

    // prev
    fprintf(file, " prev: |");
    for (ssize_t i = 0; i <= (ssize_t)list->capacity; i++) {
        if (GetPrev(list, i) == INDX_POISON) {
            fprintf(file, " %10s |", "poison");
        } else {
            fprintf(file, " %10zd |", GetPrev(list, i));
        }
    } fprintf(file, "\n");

    fprintf(file, "-------|");
    for (size_t i = 0; i <= list->capacity; i++) {
        fprintf(file, "------------|");
    } fprintf(file, "\n");

    // next
    fprintf(file, " next: |");
    for (ssize_t i = 0; i <= (ssize_t)list->capacity; i++) {
        if (GetNext(list, i) == INDX_POISON) {
            fprintf(file, " %10s |", "poison");
        } else {
            fprintf(file, " %10zd |", GetNext(list, i));
        }
    } fprintf(file, "\n");

    fprintf(file, "</pre>\n</div>\n");

    fprintf(file,
                "<div style=\"min-width: 40%%;\">\n"
                    "<h3 style=\"color: #A23B72;\">Visualization</h3>\n"
                    "<img src=\"%s_%s/push_dot_%s_%zu.svg\" style=\"max-width: 100%%; height: auto; border: 1px solid #ddd; border-radius: 5px;\" alt=\"List visualization\">\n"
                "</div>\n"
            "</div>\n"
            "<hr style=\"margin: 15px 0; border: 0; border-top: 1px solid #ccc;\">\n"
            "<small style=\"color: #666;\">Generated from: %s</small>\n"
        "</div>\n\n",
        LOG_FOLDER,
        list->name,
        list->name,
        list->dump_count++,
        filename_dot
    );
}
