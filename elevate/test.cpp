// ============================================================
//  test.cpp
//  Implements test generation, question delivery, answer
//  collection, and result storage.
// ============================================================

#include "test.h"
#include "utils.h"
#include "statistics.h"

#include <iostream>
#include <iomanip>
using namespace std;

// -- Test generation ------------------------------------------

void generateTest(int* testIndices) {
    int pos = 0;

    for (int cat = 0; cat < NUM_CATEGORIES; cat++) {
        // Build a pool of all indices belonging to this category
        int pool[10];
        for (int i = 0; i < categoryCount[cat]; i++)
            pool[i] = categoryStart[cat] + i;

        // Shuffle the pool so selection is random
        shuffleIndices(pool, categoryCount[cat]);

        // Take the required number of questions from this category
        for (int i = 0; i < testFromCategory[cat]; i++)
            testIndices[pos++] = pool[i];
    }

    // Shuffle the final test so categories are interleaved
    shuffleIndices(testIndices, TEST_SIZE);
}

// -- Test taking ----------------------------------------------

StudentResult takeTest(const string& studentName, int* testIndices) {
    StudentResult result;
    result.studentName = studentName;
    result.score       = 0;
    result.maxScore    = 0;

    // Initialise per-category counters
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        result.categoryCorrect[i] = 0;
        result.categoryTotal[i]   = 0;
    }

    clearScreen();
    printDivider();
    cout << "  TEST -- " << studentName << "\n";
    cout << "  Answer each question by entering 1, 2, 3, or 4.\n";
    printDivider();

    for (int q = 0; q < TEST_SIZE; q++) {
        Question& question = questionBank[testIndices[q]];

        // Determine which category index this question belongs to
        int catIndex = -1;
        for (int c = 0; c < NUM_CATEGORIES; c++) {
            if (question.category == categoryNames[c]) {
                catIndex = c;
                break;
            }
        }

        result.maxScore += question.points;
        if (catIndex >= 0)
            result.categoryTotal[catIndex]++;

        // Display the question
        cout << "\n  Question " << (q + 1) << " / " << TEST_SIZE;
        cout << "   [" << question.category << "  |  " << question.points << " pt]\n";
        printDivider('-', 60);
        cout << "  " << question.text << "\n\n";

        for (int opt = 0; opt < 4; opt++)
            cout << "    " << (opt + 1) << ". " << question.options[opt] << "\n";

        cout << "\n  Your answer (1-4): ";
        int answer = getValidInput(1, 4) - 1;  // convert to 0-based index

        // Evaluate the answer
        if (answer == question.correctIndex) {
            result.score += question.points;
            if (catIndex >= 0)
                result.categoryCorrect[catIndex]++;
            cout << "  Correct!\n";
        } else {
            cout << "  Wrong. Correct answer: "
                 << (char)('A' + question.correctIndex) << ". "
                 << question.options[question.correctIndex] << "\n";
        }
    }

    // Calculate final percentage and grade
    result.percentage = (result.maxScore > 0)
        ? (double)result.score / result.maxScore * 100.0
        : 0.0;
    result.grade = calculateGrade(result.percentage);

    return result;
}

// -- Start test entry point ------------------------------------

void startTest() {
    clearScreen();
    printDivider();
    cout << "  START TEST\n";
    printDivider();

    if (totalResults >= MAX_STUDENTS) {
        cout << "  Maximum number of student results has been reached.\n";
        cout << "  Press ENTER to continue...";
        cin.get();
        return;
    }

    cout << "  Enter your name: ";
    string name;
    getline(cin, name);
    trimString(name);

    if (name.empty()) {
        cout << "  Name cannot be empty.\n";
        cout << "  Press ENTER to continue...";
        cin.get();
        return;
    }

    // Generate, run, display, and store the test result
    int testIndices[TEST_SIZE];
    generateTest(testIndices);

    StudentResult result = takeTest(name, testIndices);
    displayTestResults(result);

    results[totalResults++] = result;
}
