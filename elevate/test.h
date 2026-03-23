#pragma once
#include "globals.h"

// Fills testIndices[] with TEST_SIZE randomly selected indices
// distributed across categories per testFromCategory[]
void generateTest(int* testIndices);

// Evaluates a single answer - returns true if correct
// Updates the result struct in place
bool evaluateAnswer(StudentResult& result, int questionIndex, int answerIndex);

// Builds a fresh empty StudentResult for a new test session
StudentResult createNewResult(const string& studentName);

// Calculates final percentage and grade after test is complete
void finalizeResult(StudentResult& result);
