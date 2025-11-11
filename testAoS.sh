g++ -O3 -march=native -flto -DNDEBUG -DAoS -DTEST \
    main.cpp unit_tests.cpp list_func.cpp list.cpp classic_list.cpp logger.cpp dot.cpp \
    -o test_aos
