#include "symmetric_task.h"
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

SymmetricTask ProcessEven(int number) {
    cout << "[Even Worker] Processing number: " << number << endl;
    co_return;
}

SymmetricTask ProcessOdd(int number) {
    int result = number * number;
    cout << "[Odd Worker]  Processing number (" << number
        << "). Square result: " << result << endl;

    co_return;
}

SymmetricTask NumberGenerator() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 256);

    cout << "--- Generator Started ---" << endl;

    for (int i = 1; i <= 10; ++i) {
        int val = distrib(gen);

        this_thread::sleep_for(chrono::milliseconds(50));

        cout << "Step " << i << ": Generated " << val << " -> ";

        if (val % 2 == 0) {
            cout << "Switching to Even Worker" << endl;
            co_await ProcessEven(val);
        }
        else {
            cout << "Switching to Odd Worker" << endl;
            co_await ProcessOdd(val);
        }
    }

    cout << "--- Generator Finished ---" << endl;
    co_return;
}

void run_processing_demo() {
    cout << "=========================================" << endl;
    cout << "    Lab Work: Variant 6 Execution Log    " << endl;
    cout << "=========================================" << endl;

    auto task = NumberGenerator();

    task.resume();

    while (!task.is_done()) {
        break;
    }
    cout << endl;
}