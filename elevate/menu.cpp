// ============================================================
//  menu.cpp
//  Implements the main menu, study mode, and about screen.
// ============================================================

#include "menu.h"
#include "globals.h"
#include "utils.h"

#include <iostream>
using namespace std;

// -- Study mode helpers ----------------------------------------

static void displayLessonMenu() {
    printDivider('-');
    cout << "  STUDY MODE -- Choose a topic:\n";
    printDivider('-');
    for (int i = 0; i < NUM_CATEGORIES; i++)
        cout << "  " << (i + 1) << ". " << categoryNames[i] << "\n";
    cout << "  0. Back to main menu\n";
    printDivider('-');
    cout << "  Choice: ";
}

static void displayLesson(int categoryIndex) {
    clearScreen();
    printDivider();
    cout << "  LESSON: " << categoryNames[categoryIndex] << "\n";
    printDivider();
    cout << "  All questions in this category (correct answer marked with *):\n\n";

    int start = categoryStart[categoryIndex];
    int end   = start + categoryCount[categoryIndex];

    for (int i = start; i < end; i++) {
        cout << "  Q" << (i - start + 1) << ": " << questionBank[i].text << "\n";

        for (int opt = 0; opt < 4; opt++) {
            bool isCorrect = (opt == questionBank[i].correctIndex);
            cout << "    " << (char)('A' + opt) << ". "
                 << questionBank[i].options[opt]
                 << (isCorrect ? "  *" : "") << "\n";
        }
        cout << "\n";
    }

    printDivider();
    cout << "  Press ENTER to go back...";
    cin.ignore(1000, '\n');
    cin.get();
}

// -- Study mode ------------------------------------------------

void studyMode() {
    bool active = true;
    while (active) {
        clearScreen();
        printHeader();
        displayLessonMenu();

        int choice = getValidInput(0, NUM_CATEGORIES);
        if (choice == 0) {
            active = false;
        } else {
            displayLesson(choice - 1);
        }
    }
}

// -- Main menu ------------------------------------------------

void displayMainMenu() {
    printHeader();
    cout << "  1. Study Mode\n";
    cout << "  2. Take a Test\n";
    cout << "  3. Statistics\n";
    cout << "  4. About\n";
    cout << "  0. Exit\n";
    printDivider();
    cout << "  Choice: ";
}

// -- About ----------------------------------------------------

void displayAbout() {
    clearScreen();
    printDivider();
    cout << "  ABOUT ELEVATE\n";
    printDivider();
    cout << "  Topic       : C++ Programming Basics\n";
    cout << "  Questions   : " << TOTAL_QUESTIONS << " in bank\n";
    cout << "  Test size   : " << TEST_SIZE << " questions\n\n";

    cout << "  Test structure:\n";
    for (int c = 0; c < NUM_CATEGORIES; c++) {
        cout << "    " << categoryNames[c]
             << " -- " << testFromCategory[c] << " questions"
             << " (" << categoryPoints[c] << " pt each)\n";
    }

    // Calculate and show maximum possible score
    int maxPossible = 0;
    for (int c = 0; c < NUM_CATEGORIES; c++)
        maxPossible += testFromCategory[c] * categoryPoints[c];
    cout << "\n  Max score   : " << maxPossible << " points\n";

    cout << "\n  Grading Scale:\n";
    cout << "    Grade 6  :  >= 90%\n";
    cout << "    Grade 5  :  >= 75%\n";
    cout << "    Grade 4  :  >= 62%\n";
    cout << "    Grade 3  :  >= 50%\n";
    cout << "    Grade 2  :   < 50%\n";

    printDivider();
    cout << "  Press ENTER to continue...";
    cin.get();
}
