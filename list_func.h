#ifndef _LIST_FUNC_H_
#define _LIST_FUNC_H_

#include "define_lib.h"

#include "logger.h"
#include "list.h"


#define PRINT_LIST_ELEMENT(pref, elem, next) printf("%d | %d | %d\n", pref, elem, next)

#define MAKE_LIST(name) list_type name = {}; \
                        char full_folder_name[LOG_FOLDER_NAME_SIZE] = ""; \
                        GetFullFolderName(#name, full_folder_name); \
                        UpdateFolder(full_folder_name); \
                        ListCtor(&name, #name)

list_error_t ListCtor(list_type* list, const char* name);
list_error_t ListDtor(list_type* list);

list_error_t ListChangeCapacity(list_type* list, const size_t new_capacity);

int ListGetHead(list_type* list);
int ListGetTail(list_type* list);

list_error_t ListInsertBefore(list_type* list, const int position, const list_elem_t value);
list_error_t ListInsertAfter(list_type* list, const int position, const list_elem_t value);
list_error_t ListRemove(list_type* list, const int position, list_elem_t* value);

void ListPrint(list_type* list);

#endif //_LIST_FUNC_H_
