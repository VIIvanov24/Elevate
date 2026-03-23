// ============================================================
//  test.cpp
//  Test generation and answer evaluation logic.
//  Pure C++ logic - no UI code here.
// ============================================================

#include "test.h"
#include "utils.h"
using namespace std;

// Generates a random test by picking questions from each category
void generateTest(int* testIndices) {
    int pos = 0;

    for (int cat = 0; cat < NUM_CATEGORIES; cat++) {
        // Build a pool of all question indices in this category
        int pool[10];
        for (int i = 0; i < categoryCount[cat]; i++)
            pool[i] = categoryStart[cat] + i;

        // Shuffle the pool using Fisher-Yates (defined in utils.cpp)
        shuffleIndices(pool, categoryCount[cat]);

        // Take the required number from this category
        for (int i = 0; i < testFromCategory[cat]; i++)
            testIndices[pos++] = pool[i];
    }

    // Shuffle the full test so categories are interleaved
    shuffleIndices(testIndices, TEST_SIZE);
}

// Creates a blank result ready for a new test session
StudentResult createNewResult(const string& studentName) {
    StudentResult result;
    result.studentName = studentName;
    result.score       = 0;
    result.maxScore    = 0;
    result.percentage  = 0.0;
    result.grade       = 2;

    for (int i = 0; i < NUM_CATEGORIES; i++) {
        result.categoryCorrect[i] = 0;
        result.categoryTotal[i]   = 0;
    }
    return result;
}

// Evaluates one answer and updates the result struct
bool evaluateAnswer(StudentResult& result, int questionIndex, int answerIndex) {
    Question& q = questionBank[questionIndex];

    // Add to max score
    result.maxScore += q.points;

    // Find which category this question belongs to
    for (int c = 0; c < NUM_CATEGORIES; c++) {
        if (q.category == categoryNames[c]) {
            result.categoryTotal[c]++;
            if (answerIndex == q.correctIndex) {
                result.score += q.points;
                result.categoryCorrect[c]++;
                return true;
            }
            return false;
        }
    }
    return false;
}

// Calculates final percentage and grade once all answers are in
void finalizeResult(StudentResult& result) {
    result.percentage = (result.maxScore > 0)
        ? (double)result.score / result.maxScore * 100.0
        : 0.0;
    result.grade = calculateGrade(result.percentage);
}
