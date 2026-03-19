// ============================================================
//  utils.cpp
//  Implements general-purpose utility functions.
// ============================================================

#include "utils.h"
#include "globals.h"

#include <iostream>
#include <cstdlib>   // rand
using namespace std;

// -- Screen helpers ------------------------------------------

void clearScreen() {
    cout << string(50, '\n');
}

void printDivider(char c, int width) {
    cout << string(width, c) << "\n";
}

void printHeader() {
    printDivider();
    cout << "  #######  #       #######  #     #     #    #######  #######\n";
    cout << "  #        #       #        #     #    # #      #     #      \n";
    cout << "  #####    #       #####    #     #   #####     #     ######  \n";
    cout << "  #        #       #         #   #   #     #    #     #      \n";
    cout << "  #######  ######  #######    ###   #       #   #     #######\n";
    cout << "              Electronic School -- C++ Programming\n";
    printDivider();
}

// -- Input validation ----------------------------------------

int getValidInput(int minVal, int maxVal) {
    int input;
    while (true) {
        cin >> input;
        if (cin.fail() || input < minVal || input > maxVal) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input. Enter a number between "
                 << minVal << " and " << maxVal << ": ";
        } else {
            cin.ignore(1000, '\n');
            return input;
        }
    }
}

// -- Grading --------------------------------------------------

int calculateGrade(double percentage) {
    if (percentage >= GRADE_6_THRESHOLD) return 6;
    if (percentage >= GRADE_5_THRESHOLD) return 5;
    if (percentage >= GRADE_4_THRESHOLD) return 4;
    if (percentage >= GRADE_3_THRESHOLD) return 3;
    return 2;
}

// -- Shuffle (Fisher-Yates) ------------------------------------

void shuffleIndices(int* arr, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i]   = arr[j];
        arr[j]   = temp;
    }
}

// -- String utility --------------------------------------------

void trimString(string& str) {
    while (!str.empty() && str.front() == ' ') str.erase(str.begin());
    while (!str.empty() && str.back()  == ' ') str.pop_back();
}
