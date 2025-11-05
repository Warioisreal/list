#ifndef _DOT_H_
#define _DOT_H_

#include <stdio.h>
#include <stdlib.h>

static const size_t DOT_FILE_NAME_SIZE = 100;
static const size_t DOT_PARAMS_SIZE = 300;


void StartDot(FILE* file_dot);
void MakeDotElement(FILE* file_dot, const size_t label_number, const char* params);
void MakeDotElementConnection(FILE* file_dot, const size_t position1, const int position2, const char* params);
void FinishDot(FILE* file_dot);

#endif //_DOT_H_
