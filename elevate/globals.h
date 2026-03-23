#pragma once

// ============================================================
//  globals.h
//  Shared constants, data structures, and extern declarations
//  used across all modules of the Elevate application.
// ============================================================

#include <string>
using namespace std;

// -- Constants ------------------------------------------------

const int TOTAL_QUESTIONS = 30;
const int TEST_SIZE       = 20;
const int MAX_STUDENTS    = 100;
const int NUM_CATEGORIES  = 3;

// Grading thresholds (percentage)
const double GRADE_3_THRESHOLD = 50.0;
const double GRADE_4_THRESHOLD = 62.0;
const double GRADE_5_THRESHOLD = 75.0;
const double GRADE_6_THRESHOLD = 90.0;

// -- Data Structures ------------------------------------------

struct Question {
    string text;
    string options[4];
    int    correctIndex;
    string category;
    int    points;
};

struct StudentResult {
    string studentName;
    int    score;
    int    maxScore;
    double percentage;
    int    grade;
    int    categoryCorrect[NUM_CATEGORIES];
    int    categoryTotal[NUM_CATEGORIES];
};

// -- Shared global data (defined in questions.cpp) ------------

extern string   categoryNames[NUM_CATEGORIES];
extern int      categoryPoints[NUM_CATEGORIES];
extern int      categoryStart[NUM_CATEGORIES];
extern int      categoryCount[NUM_CATEGORIES];
extern int      testFromCategory[NUM_CATEGORIES];
extern Question questionBank[TOTAL_QUESTIONS];

// -- Shared results storage (defined in statistics.cpp) -------

extern StudentResult results[MAX_STUDENTS];
extern int           totalResults;
