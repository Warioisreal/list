#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"

#include "list_func.h"
#include "unit_tests.h"


#define SAFE_USING(error) if (error != list_error_t::OK) { return; }

void UT1(void) {
    MAKE_LIST(lst1);

    SAFE_USING(ListInsertAfter(&lst1, 0, 10));
    SAFE_USING(ListInsertAfter(&lst1, 1, 20));
    SAFE_USING(ListInsertAfter(&lst1, 2, 30));
    SAFE_USING(ListInsertAfter(&lst1, 2, 40));
    SAFE_USING(ListInsertAfter(&lst1, 4, 50));
    SAFE_USING(ListInsertAfter(&lst1, 8, 60));
    lst1.prev[3] = 600;
    lst1.next[3] = 700;

    ListPrint(&lst1, "(1) REPLACE NODE3(prev) on 600<br>(2) REPLACE NODE3(next) on 700");

    ListDtor(&lst1);
    printf("UNIT TEST 1 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT2(void) {
    MAKE_LIST(lst2);

    SAFE_USING(ListInsertAfter(&lst2, 0, 10));
    SAFE_USING(ListInsertAfter(&lst2, 1, 20));
    SAFE_USING(ListInsertAfter(&lst2, 2, 30));
    SAFE_USING(ListInsertBefore(&lst2, 1, 1));
    SAFE_USING(ListInsertBefore(&lst2, 2, 11));
    SAFE_USING(ListInsertBefore(&lst2, 4, 21));
    SAFE_USING(ListInsertAfter(&lst2, 4, 31));

    ListDtor(&lst2);
    printf("UNIT TEST 2 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT3(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst3);

    SAFE_USING(ListInsertAfter(&lst3, 0, 10));
    SAFE_USING(ListInsertAfter(&lst3, 1, 20));
    SAFE_USING(ListInsertAfter(&lst3, 2, 30));
    SAFE_USING(ListInsertAfter(&lst3, 4, 40));
    SAFE_USING(ListInsertAfter(&lst3, 3, 50));
    SAFE_USING(ListRemove(&lst3, 8, &value));

    ListDtor(&lst3);
    printf("UNIT TEST 3 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT4(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst4);

    SAFE_USING(ListInsertAfter(&lst4, 0, 10));
    SAFE_USING(ListInsertAfter(&lst4, 1, 20));
    SAFE_USING(ListInsertAfter(&lst4, 2, 30));
    SAFE_USING(ListInsertAfter(&lst4, 4, 40));
    SAFE_USING(ListInsertAfter(&lst4, 3, 50));

    SAFE_USING(ListRemove(&lst4, 1, &value));
    SAFE_USING(ListRemove(&lst4, 8, &value));
    SAFE_USING(ListRemove(&lst4, 3, &value));
    SAFE_USING(ListRemove(&lst4, 2, &value));
    SAFE_USING(ListRemove(&lst4, 4, &value));

    ListDtor(&lst4);
    printf("UNIT TEST 4 FINISH\n");
}

//----------------------------------------------------------------------------------

void UT5(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst5);

    SAFE_USING(ListInsertAfter(&lst5, 0, 10));
    SAFE_USING(ListInsertAfter(&lst5, 1, 20));
    SAFE_USING(ListInsertAfter(&lst5, 2, 30));
    SAFE_USING(ListInsertBefore(&lst5, 1, 1));
    SAFE_USING(ListInsertBefore(&lst5, 2, 11));
    SAFE_USING(ListInsertBefore(&lst5, 4, 21));
    SAFE_USING(ListInsertAfter(&lst5, 4, 31));

    int head = 0;
    int tail = 0;
    head = ListGetHead(&lst5);
    tail = ListGetTail(&lst5);

    ListPrint(&lst5, "PRINT");

    PRINT_COLOR_VAR(CYAN, "head: %d\ntail: %d\n", head, tail);

    SAFE_USING(ListRemove(&lst5, 1, &value));
    SAFE_USING(ListRemove(&lst5, 8, &value));
    SAFE_USING(ListRemove(&lst5, 3, &value));
    SAFE_USING(ListRemove(&lst5, 2, &value));
    SAFE_USING(ListRemove(&lst5, 4, &value));
    SAFE_USING(ListRemove(&lst5, 6, &value));
    SAFE_USING(ListRemove(&lst5, 7, &value));

    ListDtor(&lst5);
    printf("UNIT TEST 5 FINISH\n");
}

