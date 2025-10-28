#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#define MAKE_LIST(name) list_type name = {}; ListCtor(&name)


int main (void) {
    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst);

    ListPush(&lst, 1, 10);
    ListPush(&lst, 2, 20);
    ListPush(&lst, 3, 30);
    ListPush(&lst, 4, 40);
    ListPush(&lst, 5, 50);
    ListPush(&lst, 6, 60);
    ListPush(&lst, 6, 59);

    return 0;
}
