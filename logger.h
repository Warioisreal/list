#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <ctype.h>

#include "color_lib.h"

#include "list.h"

static const char* LOG_FOLDER = "log_folder";
static const size_t LOG_FOLDER_NAME_SIZE = 100;

static const char* DUMP_FILENAME = "dump.htm";

static const size_t COMMAND_SIZE = 100;


void GetFullFolderName(const char* list_name, char* buffer);
void UpdateFolder(const char* name_folder);
void StartLog(FILE** file);
void FinishLog(FILE** file);
void ListLog(list_type* list, const char* func, FILE* file, pos_color* positions);

#endif //_LOGGER_H_
