// ============================================================
//  statistics.cpp
//  Defines student result storage and implements all
//  statistics and result display functions.
// ============================================================

#include "statistics.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
using namespace std;

// -- Global result storage ------------------------------------

StudentResult results[MAX_STUDENTS];
int           totalResults = 0;

// -- Display single test result --------------------------------

void displayTestResults(const StudentResult& result) {
    clearScreen();
    printDivider();
    cout << "  TEST RESULTS -- " << result.studentName << "\n";
    printDivider();
    cout << "  Score      : " << result.score << " / " << result.maxScore << "\n";
    cout << "  Percentage : " << fixed << setprecision(1) << result.percentage << "%\n";
    cout << "  Grade      : " << result.grade << "\n\n";

    printDivider('-');
    cout << "  Breakdown by category:\n";
    printDivider('-');

    for (int c = 0; c < NUM_CATEGORIES; c++) {
        double pct = (result.categoryTotal[c] > 0)
            ? (double)result.categoryCorrect[c] / result.categoryTotal[c] * 100.0
            : 0.0;

        cout << "  " << left  << setw(30) << categoryNames[c]
             << result.categoryCorrect[c] << " / " << result.categoryTotal[c]
             << "   (" << fixed << setprecision(1) << pct << "%)\n";
    }

    printDivider();
    cout << "  Press ENTER to continue...";
    cin.get();
}

// -- Overall statistics ----------------------------------------

void displayStatistics() {
    clearScreen();
    printDivider();
    cout << "  STATISTICS\n";
    printDivider();

    if (totalResults == 0) {
        cout << "  No results recorded yet.\n";
        printDivider();
        cout << "  Press ENTER to continue...";
        cin.get();
        return;
    }

    // Calculate average and find highest / lowest scoring students
    int    highIdx = 0, lowIdx = 0;
    double total   = 0.0;

    for (int i = 0; i < totalResults; i++) {
        total += results[i].percentage;
        if (results[i].percentage > results[highIdx].percentage) highIdx = i;
        if (results[i].percentage < results[lowIdx].percentage)  lowIdx  = i;
    }

    double average = total / totalResults;

    cout << "  Students tested : " << totalResults << "\n";
    cout << "  Average score   : " << fixed << setprecision(1) << average << "%\n\n";

    cout << "  Highest : " << results[highIdx].studentName
         << "  --  " << fixed << setprecision(1) << results[highIdx].percentage << "%"
         << "  (Grade " << results[highIdx].grade << ")\n";

    cout << "  Lowest  : " << results[lowIdx].studentName
         << "  --  " << fixed << setprecision(1) << results[lowIdx].percentage << "%"
         << "  (Grade " << results[lowIdx].grade << ")\n";

    // Per-category statistics across all students
    printDivider('-');
    cout << "  Category Performance:\n";
    printDivider('-');

    double catPct[NUM_CATEGORIES] = {};

    for (int c = 0; c < NUM_CATEGORIES; c++) {
        int totalCorrect = 0, totalAnswered = 0;

        for (int i = 0; i < totalResults; i++) {
            totalCorrect  += results[i].categoryCorrect[c];
            totalAnswered += results[i].categoryTotal[c];
        }

        catPct[c] = (totalAnswered > 0)
            ? (double)totalCorrect / totalAnswered * 100.0
            : 0.0;

        cout << "  " << left << setw(30) << categoryNames[c]
             << fixed << setprecision(1) << catPct[c] << "%\n";
    }

    // Identify best and worst category
    int bestCat = 0, worstCat = 0;
    for (int c = 1; c < NUM_CATEGORIES; c++) {
        if (catPct[c] > catPct[bestCat])  bestCat  = c;
        if (catPct[c] < catPct[worstCat]) worstCat = c;
    }

    cout << "\n  Best category  : " << categoryNames[bestCat]  << "\n";
    cout << "  Worst category : " << categoryNames[worstCat] << "\n";

    // Full results table
    printDivider('-');
    cout << "  All Results:\n";
    printDivider('-');
    cout << "  " << left
         << setw(22) << "Name"
         << setw(12) << "Score"
         << setw(10) << "Percent"
         << "Grade\n";
    printDivider('-');

    for (int i = 0; i < totalResults; i++) {
        string scoreStr = to_string(results[i].score) + "/" + to_string(results[i].maxScore);
        string pctStr   = to_string((int)results[i].percentage) + "%";

        cout << "  " << left
             << setw(22) << results[i].studentName
             << setw(12) << scoreStr
             << setw(10) << pctStr
             << results[i].grade << "\n";
    }

    printDivider();
    cout << "  Press ENTER to continue...";
    cin.get();
}
