#include <stdio.h>
#include <stdlib.h>

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

void StartLog(FILE** file) {
    const char* filename = DUMP_FILENAME;
    *file = fopen(filename, "wb");
}

//----------------------------------------------------------------------------------

void FinishLog(FILE** file) {
    fclose(*file);
    *file = nullptr;
}

//----------------------------------------------------------------------------------

void ListLog(list_type* list, const char* func, FILE* file, pos_color* positions) {
    char filename_dot[100] = "";
    snprintf(filename_dot, 100, "%s_%s/push_dot_%s_%zu.txt", LOG_FOLDER, list->name, list->name, list->dump_count);
    FILE* file_dot = fopen(filename_dot, "wb");
    fprintf(file_dot,
        "digraph G{\n"
        "node[shape=Mrecord, style=filled]\n"
        "rankdir=LR;\n"
        "ordering=out;\n");

    unsigned int color    = 0;
    unsigned int bg_color = 0;

    for (size_t pos = 0; pos < list->capacity + 1; pos++) {
        if (pos == 0) {
            color    = 0x0000c0;
            bg_color = 0xa0a0f0;
        } else
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
            "element%zu [color=\"#%06X\", fillcolor=\"#%06X\", label=\" pos: %zu | <prev> prev: %d | data: %d | <next> next: %d \"];\n",
            pos, color, bg_color, pos, list->prev[pos], list->data[pos], list->next[pos]);
    }

    for (size_t pos = 0; pos < list->capacity; pos++) {
        fprintf(file_dot, "element%zu -> element%zu [style=invis, weight=100];\n", pos, pos + 1);
    }

    for (size_t pos = 1; pos < list->capacity + 1; pos++) {
        if (list->prev[pos] != INDX_POISON && list->data[pos] != DATA_POISON) {
            fprintf(file_dot, "element%zu -> element%d [constraint=false, weight=0, color=\"#0000ff\"];\n", pos, list->prev[pos]);
        }
        if (list->next[pos] != INDX_POISON && list->data[pos] != DATA_POISON) {
            fprintf(file_dot, "element%zu -> element%d [constraint=false, weight=0, color=\"#ff0000\"];\n", pos, list->next[pos]);
        }
        if (list->next[pos] != INDX_POISON && list->data[pos] == DATA_POISON) {
            fprintf(file_dot, "element%zu -> element%d [constraint=false, weight=0, color=\"#00ff00\"];\n", pos, list->next[pos]);
        }
    }

    fprintf(file_dot, "}");
    fclose(file_dot);

    char command[100] = "";
    snprintf(command, 100, "dot -Tsvg %s_%s/push_dot_%s_%zu.txt -o %s_%s/push_dot_%s_%zu.svg", LOG_FOLDER, list->name, list->name, list->dump_count, LOG_FOLDER, list->name, list->name, list->dump_count);
    system(command);

    fprintf(file,
        "<pre>\n"
        "<div style=\"margin: 20px; padding: 15px; border: 2px solid #4CAF50; border-radius: 10px; background: #f8f9fa; box-shadow: 0 4px 8px rgba(0,0,0,0.1);\">\n"
            "<h2 style=\"color: #2E86AB; margin-top: 0;\">List \"%s\" Debug %zu [%s]</h2>\n"
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
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">SIZE</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                        "</tr>\n"
                        "<tr style=\"background: #e9ecef;\">"
                            "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">CAPACITY</td>"
                            "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                        "</tr>\n"
                    "</table>\n"
                "</div>\n"
                "<div style=\"min-width: 40%%;\">\n"
                    "<h3 style=\"color: #A23B72;\">Visualization</h3>\n"
                    "<img src=\"%s_%s/push_dot_%s_%zu.svg\" style=\"max-width: 100%%; height: auto; border: 1px solid #ddd; border-radius: 5px;\" alt=\"List visualization\">\n"
                "</div>\n"
            "</div>\n"
            "<hr style=\"margin: 15px 0; border: 0; border-top: 1px solid #ccc;\">\n"
            "<small style=\"color: #666;\">Generated from: %s</small>\n"
        "</div>\n\n",
        list->name,
        list->dump_count,
        func,
        list->next[0],
        list->prev[0],
        list->free,
        list->size,
        list->capacity,
        LOG_FOLDER,
        list->name,
        list->name,
        list->dump_count++,
        filename_dot);

}
