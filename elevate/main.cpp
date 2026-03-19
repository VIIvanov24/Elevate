// ============================================================
//  main.cpp
//  Entry point for the Elevate Electronic School application.
//
//  Project  : Elevate -- Electronic School
//  Subject  : C++ Programming Basics
//  Grade    : IX, 2025/2026
//  Language : C++
// ============================================================

#include <iostream>
#include <cstdlib>   // srand
#include <ctime>     // time
using namespace std;

#include "globals.h"
#include "questions.h"
#include "utils.h"
#include "menu.h"
#include "test.h"
#include "statistics.h"

int main() {
    // Seed the random number generator once at startup
    srand((unsigned int)time(nullptr));

    // Load the question bank data
    initQuestionBank();

    bool running = true;
    while (running) {
        clearScreen();
        displayMainMenu();

        int choice = getValidInput(0, 4);
        switch (choice) {
            case 1: studyMode();         break;
            case 2: startTest();         break;
            case 3: displayStatistics(); break;
            case 4: displayAbout();      break;
            case 0:
                cout << "\n  Goodbye!\n\n";
                running = false;
                break;
        }
    }

    return 0;
}
