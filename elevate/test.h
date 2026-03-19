#pragma once

// ============================================================
//  test.h
//  Declares functions for test generation and test taking.
// ============================================================

#include "globals.h"

// Fills testIndices[] with TEST_SIZE randomly selected question
// indices, distributed across categories according to
// testFromCategory[].
void generateTest(int* testIndices);

// Presents all TEST_SIZE questions to the student and collects
// answers. Returns a filled StudentResult.
StudentResult takeTest(const string& studentName, int* testIndices);

// Entry point called from the menu: asks for the student name,
// generates a test, runs it, shows results, and stores them.
void startTest();
