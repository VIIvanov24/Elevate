#pragma once

// ============================================================
//  statistics.h
//  Declares the student results storage and all functions
//  related to displaying test results and statistics.
// ============================================================

#include "globals.h"

// -- Results storage ------------------------------------------
// Defined here so both test.cpp and statistics.cpp can access them.

extern StudentResult results[MAX_STUDENTS];
extern int           totalResults;

// -- Function declarations ------------------------------------

// Displays the outcome of a single test to the student
void displayTestResults(const StudentResult& result);

// Displays overall statistics across all stored results
void displayStatistics();
