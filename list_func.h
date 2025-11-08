#ifndef _LIST_FUNC_H_
#define _LIST_FUNC_H_

#include "define_lib.h"

#include "logger.h"
#include "list.h"


#define PRINT_LIST_ELEMENT(prev, elem, next) printf("%lld | %d | %lld\n", prev, elem, next)

#define MAKE_LIST(name) list_type name = {}; \
                        char full_folder_name[LOG_FOLDER_NAME_SIZE] = ""; \
                        GetFullFolderName(#name, full_folder_name); \
                        UpdateFolder(full_folder_name); \
                        ListCtor(&name, #name)

list_error_t ListCtor(list_type* list, const char* name);
list_error_t ListDtor(list_type* list);

list_error_t ListLinearize(list_type* list);

ssize_t ListGetHead(list_type* list);
ssize_t ListGetTail(list_type* list);

list_error_t ListInsertBefore(list_type* list, const ssize_t position, const list_elem_t value);
list_error_t ListInsertAfter(list_type* list,  const ssize_t position, const list_elem_t value);

list_error_t ListRemove(list_type* list, const ssize_t position, list_elem_t* value);

void ListPrint(list_type* list, const char* message);

#endif //_LIST_FUNC_H_
