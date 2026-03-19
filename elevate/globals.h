#pragma once

// ============================================================
//  globals.h
//  Shared constants, data structures, and extern declarations
//  used across all modules of the Elevate application.
// ============================================================

#include <string>
using namespace std;

// -- Constants ------------------------------------------------

const int TOTAL_QUESTIONS = 30;  // total questions in the bank
const int TEST_SIZE       = 20;  // questions per generated test
const int MAX_STUDENTS    = 50;  // max stored student results
const int NUM_CATEGORIES  = 3;   // number of question categories

// Grading thresholds (percentage)
const double GRADE_3_THRESHOLD = 50.0;
const double GRADE_4_THRESHOLD = 62.0;
const double GRADE_5_THRESHOLD = 75.0;
const double GRADE_6_THRESHOLD = 90.0;

// -- Data Structures ------------------------------------------

struct Question {
    string text;            // question text
    string options[4];      // answer choices A-D
    int    correctIndex;    // index of the correct answer (0-3)
    string category;        // category name
    int    points;          // point value / weight
};

struct StudentResult {
    string studentName;
    int    score;                            // raw points earned
    int    maxScore;                         // maximum possible points
    double percentage;                       // score / maxScore * 100
    int    grade;                            // Bulgarian grade 2-6
    int    categoryCorrect[NUM_CATEGORIES];  // correct answers per category
    int    categoryTotal[NUM_CATEGORIES];    // total questions per category
};

// -- Shared global data (defined in questions.cpp) ------------

extern string       categoryNames[NUM_CATEGORIES];
extern int          categoryPoints[NUM_CATEGORIES];
extern int          categoryStart[NUM_CATEGORIES];
extern int          categoryCount[NUM_CATEGORIES];
extern int          testFromCategory[NUM_CATEGORIES];
extern Question     questionBank[TOTAL_QUESTIONS];

// -- Shared results storage (defined in statistics.cpp) ------

extern StudentResult results[MAX_STUDENTS];
extern int           totalResults;
