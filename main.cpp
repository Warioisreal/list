#include <stdio.h>
#include <chrono>

#include "unit_tests.h"



#ifdef SoA
    #define TEST_NAME "SoA"
#elif defined(AoS)
    #define TEST_NAME "AoS"
#else
    #define TEST_NAME "Classic"
#endif

class HighPrecisionTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;

public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double elapsed_milliseconds() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }

    double elapsed_nanoseconds() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::nano>(end_time - start_time).count();
    }
};

void RunPerformanceTest(int elements_count);




int main (int argc, char* argv[]) {

    //UTclassic();
    //UT1();
    //UT2();
    //UT3();
    //UT4();
    //UT5();
    //UT6();
    //UT7();

    int elements_count = 0;

    if (argc > 1) { elements_count = atoi(argv[1]); }

    RunPerformanceTest(elements_count);

    return 0;
}



void RunPerformanceTest(int elements_count) {
    printf("Running performance test with %d elements...\n", elements_count);

    HighPrecisionTimer timer;
    double total_time = 0.0;

    // Warm-up run
    #ifdef SoA
        TestSoA(1000);
        TestSoA(1000);
    #elif defined(AoS)
        TestAoS(1000);
        TestAoS(1000);
    #else
        TestClassic(1000);
        TestClassic(1000);
    #endif

    // Основные замеры
    const int RUNS = 5;
    for (int i = 0; i < RUNS; i++) {
        timer.start();

        #ifdef SoA
            TestSoA(elements_count);
        #elif defined(AoS)
            TestAoS(elements_count);
        #else
            TestClassic(elements_count);
        #endif

        double run_time = timer.elapsed_milliseconds();
        total_time += run_time;

        printf("Run %d: %.2f ms\n", i + 1, run_time);
    }

    double avg_time = total_time / RUNS;
    printf("\n=== %s Performance ===\n", TEST_NAME);
    printf("Elements: %d\n", elements_count);
    printf("Average time: %.2f ms\n", avg_time);
    printf("Time per operation: %.3f ns\n", (avg_time * 1000000) / (elements_count * 2));
    printf("========================\n\n");
}
