#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <ctype.h>

#include "color_lib.h"

#include "list.h"
#include "classic_list.h"

static const char* LOG_FOLDER = "log_folder";
static const size_t LOG_FOLDER_NAME_SIZE = 200;

static const char* DUMP_FILENAME = "dump";
static const size_t LOG_FILE_NAME_SIZE = 200;

static const size_t COMMAND_SIZE = 500;

static const size_t LOG_MESSAGE_SIZE = 100;


void GetFullFolderName(const char* list_name, char* buffer);
void UpdateFolder(const char* name_folder);
void StartLog(FILE** file, const char* list_name);
void FinishLog(FILE** file);
void ListLog(list_type* list, const char* message, FILE* file, pos_color* positions);
void ClassicListLog(List* list, const char* message, FILE* file, elems_color* elems);

#endif //_LOGGER_H_
