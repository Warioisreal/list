#include "dot.h"

void StartDot(FILE* file_dot) {
    fprintf(file_dot,
        "digraph G{\n"
        "node[shape=Mrecord, style=filled];\n"
        "rankdir=LR;\n"
        "ordering=out;\n");
}

//----------------------------------------------------------------------------------

void MakeDotElement(FILE* file_dot, const ssize_t node_number, const char* params) {
    fprintf(file_dot, "node%zd [%s];\n", node_number, params);
}

//----------------------------------------------------------------------------------

void MakeDotElementConnection(FILE* file_dot, const ssize_t position1, const ssize_t position2, const char* params) {
    fprintf(file_dot, "node%zd -> node%zd [%s];\n", position1, position2, params);
}

//----------------------------------------------------------------------------------

void FinishDot(FILE* file_dot) {
    fprintf(file_dot, "}");
    fclose(file_dot);
}
