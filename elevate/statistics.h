#pragma once
#include "globals.h"

extern StudentResult results[MAX_STUDENTS];
extern int           totalResults;

// Stores a result in the results array
void storeResult(const StudentResult& result);

// Returns average percentage across all results
double getAverageScore();

// Returns index of highest scoring student, or -1 if none
int getHighestScoreIndex();

// Returns index of lowest scoring student, or -1 if none
int getLowestScoreIndex();

// Returns pass rate percentage for a given category index
double getCategoryPassRate(int categoryIndex);
