#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"

#include "list_func.h"
#include "unit_tests.h"


void UT1(void) {
    MAKE_LIST(lst);

    ListInsertAfter(&lst, 0, 10);
    ListInsertAfter(&lst, 1, 20);
    ListInsertAfter(&lst, 2, 30);

    ListDtor(&lst);
    printf("UNIT TEST 1 FINISH\n");
}

//----------------------------------------------------------------------------------
/*
void UT2(void) {
    MAKE_LIST(lst);

    ListInsertAfter(&lst, 0, 10);
    ListInsertAfter(&lst, 1, 20);
    ListInsertAfter(&lst, 2, 30);
    ListInsertBefore(&lst, 1, 1);
    ListInsertBefore(&lst, 2, 11);
    ListInsertBefore(&lst, 3, 21);
    ListInsertAfter(&lst, 3, 31);

    ListDtor(&lst);
    printf("UNIT TEST 2 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT3(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst);

    ListInsertAfter(&lst, 0, 10);
    ListInsertAfter(&lst, 1, 20);
    ListInsertAfter(&lst, 2, 30);
    ListInsertAfter(&lst, 3, 40);
    ListInsertAfter(&lst, 4, 50);
    ListRemove(&lst, 5, &value);

    ListDtor(&lst);
    printf("UNIT TEST 3 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT4(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst);

    ListInsertAfter(&lst, 0, 10);
    ListInsertAfter(&lst, 1, 20);
    ListInsertAfter(&lst, 2, 30);
    ListInsertAfter(&lst, 3, 40);
    ListInsertAfter(&lst, 4, 50);
    ListRemove(&lst, 1, &value);
    ListRemove(&lst, 5, &value);
    ListRemove(&lst, 3, &value);
    ListRemove(&lst, 2, &value);
    ListRemove(&lst, 4, &value);

    ListDtor(&lst);
    printf("UNIT TEST 4 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT5(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst);

    ListInsertAfter(&lst, 0, 10);
    ListInsertAfter(&lst, 1, 20);
    ListInsertAfter(&lst, 2, 30);
    ListInsertBefore(&lst, 1, 1);
    ListInsertBefore(&lst, 2, 11);
    ListInsertBefore(&lst, 3, 21);
    ListInsertAfter(&lst, 3, 31);

    int head = 0;
    int tail = 0;
    head = ListGetHead(&lst);
    tail = ListGetTail(&lst);

    ListPrint(&lst);

    PRINT_COLOR_VAR(CYAN, "head: %d\ntail: %d\n", head, tail);

    ListRemove(&lst, 1, &value);
    ListRemove(&lst, 7, &value);
    ListRemove(&lst, 4, &value);
    ListRemove(&lst, 2, &value);
    ListRemove(&lst, 6, &value);
    ListRemove(&lst, 3, &value);
    ListRemove(&lst, 5, &value);

    ListDtor(&lst);
    printf("UNIT TEST 5 FINISH\n");
}
*/
