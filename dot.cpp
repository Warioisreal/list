#include "dot.h"

void StartDot(FILE* file_dot) {
    fprintf(file_dot,
        "digraph G{\n"
        "node[shape=Mrecord, style=filled];\n"
        "rankdir=LR;\n"
        "ordering=out;\n");
}

//----------------------------------------------------------------------------------

void MakeDotElement(FILE* file_dot, const size_t label_number, const char* params) {
    fprintf(file_dot, "node%zu [%s];\n", label_number, params);
}

//----------------------------------------------------------------------------------

void MakeDotElementConnection(FILE* file_dot, const size_t position1, const int position2, const char* params) {
    fprintf(file_dot, "node%zu -> node%d [%s];\n", position1, position2, params);
}

//----------------------------------------------------------------------------------

void FinishDot(FILE* file_dot) {
    fprintf(file_dot, "}");
    fclose(file_dot);
}
