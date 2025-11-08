#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"

#include "list_func.h"
#include "unit_tests.h"


#define CHECK_ERRORS(error) if (error != list_error_t::OK) { return; }

void UT1(void) {
    MAKE_LIST(lst1);

    CHECK_ERRORS(ListInsertAfter(&lst1, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst1, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst1, 2, 30));
    CHECK_ERRORS(ListInsertAfter(&lst1, 4, 40));
    CHECK_ERRORS(ListInsertAfter(&lst1, 3, 50));
    CHECK_ERRORS(ListInsertAfter(&lst1, 8, 60));

    CHECK_ERRORS(ListDtor(&lst1));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 1 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}

//----------------------------------------------------------------------------------

void UT2(void) {
    MAKE_LIST(lst2);

    CHECK_ERRORS(ListInsertAfter(&lst2, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst2, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst2, 2, 30));
    CHECK_ERRORS(ListInsertBefore(&lst2, 1, 1));
    CHECK_ERRORS(ListInsertBefore(&lst2, 2, 11));
    CHECK_ERRORS(ListInsertBefore(&lst2, 4, 21));
    CHECK_ERRORS(ListInsertAfter(&lst2, 4, 31));

    CHECK_ERRORS(ListDtor(&lst2));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 2 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}

//----------------------------------------------------------------------------------

void UT3(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst3);

    CHECK_ERRORS(ListInsertAfter(&lst3, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst3, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst3, 2, 30));
    CHECK_ERRORS(ListInsertAfter(&lst3, 4, 40));
    CHECK_ERRORS(ListInsertAfter(&lst3, 3, 50));

    CHECK_ERRORS(ListRemove(&lst3, 8, &value));

    CHECK_ERRORS(ListDtor(&lst3));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 3 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}

//----------------------------------------------------------------------------------

void UT4(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst4);

    CHECK_ERRORS(ListInsertAfter(&lst4, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst4, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst4, 2, 30));
    CHECK_ERRORS(ListInsertAfter(&lst4, 4, 40));
    CHECK_ERRORS(ListInsertAfter(&lst4, 3, 50));

    CHECK_ERRORS(ListRemove(&lst4, 1, &value));
    CHECK_ERRORS(ListRemove(&lst4, 8, &value));
    CHECK_ERRORS(ListRemove(&lst4, 3, &value));
    CHECK_ERRORS(ListRemove(&lst4, 2, &value));
    CHECK_ERRORS(ListRemove(&lst4, 4, &value));

    CHECK_ERRORS(ListDtor(&lst4));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 4 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}

//----------------------------------------------------------------------------------

void UT5(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst5);

    CHECK_ERRORS(ListInsertAfter(&lst5, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst5, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst5, 2, 30));
    CHECK_ERRORS(ListInsertBefore(&lst5, 1, 1));
    CHECK_ERRORS(ListInsertBefore(&lst5, 2, 11));
    CHECK_ERRORS(ListInsertBefore(&lst5, 4, 21));
    CHECK_ERRORS(ListInsertAfter(&lst5, 4, 31));

    ssize_t head = 0;
    ssize_t tail = 0;
    head = ListGetHead(&lst5);
    tail = ListGetTail(&lst5);

    ListPrint(&lst5, "PRINT");

    PRINT_COLOR_VAR(CYAN, "head: %zd\ntail: %zd\n", head, tail);

    CHECK_ERRORS(ListRemove(&lst5, 1, &value));
    CHECK_ERRORS(ListRemove(&lst5, 8, &value));
    CHECK_ERRORS(ListRemove(&lst5, 3, &value));
    CHECK_ERRORS(ListRemove(&lst5, 2, &value));
    CHECK_ERRORS(ListRemove(&lst5, 4, &value));
    CHECK_ERRORS(ListRemove(&lst5, 6, &value));
    CHECK_ERRORS(ListRemove(&lst5, 7, &value));

    CHECK_ERRORS(ListDtor(&lst5));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 5 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}

//----------------------------------------------------------------------------------

void UT6(void) {

    list_elem_t value = DATA_POISON;

    MAKE_LIST(lst6);

    CHECK_ERRORS(ListInsertAfter(&lst6, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst6, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst6, 2, 30));
    CHECK_ERRORS(ListInsertBefore(&lst6, 1, 1));
    CHECK_ERRORS(ListInsertBefore(&lst6, 2, 11));
    CHECK_ERRORS(ListInsertBefore(&lst6, 4, 21));
    CHECK_ERRORS(ListInsertAfter(&lst6, 4, 31));

    CHECK_ERRORS(ListRemove(&lst6, 1, &value));
    CHECK_ERRORS(ListRemove(&lst6, 8, &value));
    CHECK_ERRORS(ListRemove(&lst6, 3, &value));

    ListPrint(&lst6, "PRINT");

    ListLinearize(&lst6);

    CHECK_ERRORS(ListDtor(&lst6));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 6 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}


void UT7(void) {
    MAKE_LIST(lst7);

    CHECK_ERRORS(ListInsertAfter(&lst7, 0, 10));
    CHECK_ERRORS(ListInsertAfter(&lst7, 1, 20));
    CHECK_ERRORS(ListInsertAfter(&lst7, 2, 30));
    CHECK_ERRORS(ListInsertAfter(&lst7, 4, 40));
    CHECK_ERRORS(ListInsertAfter(&lst7, 3, 50));
    CHECK_ERRORS(ListInsertAfter(&lst7, 8, 60));

    ListPrint(&lst7, "NORMAL LIST");

    lst7.data[3].prev = 600;
    lst7.data[3].next = 700;

    ListPrint(&lst7, "(1) REPLACE NODE3(prev) on 600<br>(2) REPLACE NODE3(next) on 700");

    CHECK_ERRORS(ListDtor(&lst7));

    printf("\n" " ==================== " "\n"
                "|                    |" "\n"
                "| UNIT TEST 7 FINISH |" "\n"
                "|                    |" "\n"
                " ==================== " "\n"
           "\n");
}
