#pragma once

// ============================================================
//  renderer.h
//  Declares all Raylib-based screen drawing functions and
//  the AppScreen enum used for navigation.
//
//  NEW: Everything in this file uses Raylib. Comments explain
//  each Raylib concept used.
// ============================================================

// Raylib is a simple C library for window/input/drawing.
// We include it here so all screens can use its types.
#include "raylib.h"
#include "globals.h"
#include "accounts.h"
#include "test.h"
#include "statistics.h"

// -- Screen enum ----------------------------------------------
// Tracks which screen the app is currently showing.
// The main loop switches drawing/input based on this value.
enum AppScreen {
    SCREEN_LOGIN,
    SCREEN_REGISTER,
    SCREEN_MAIN_MENU,
    SCREEN_STUDY,
    SCREEN_STUDY_LESSON,
    SCREEN_TEST,
    SCREEN_TEST_RESULT,
    SCREEN_STATISTICS,
    SCREEN_ABOUT
};

// -- UI Helper structs ----------------------------------------

// Represents a clickable button on screen
struct Button {
    Rectangle rect;   // Raylib Rectangle: x, y, width, height
    string    label;
    bool      hovered;
};

// -- Shared UI state ------------------------------------------

extern AppScreen currentScreen;
extern int       selectedCategory;   // for study mode
extern int       testIndices[TEST_SIZE];
extern int       currentQuestion;
extern StudentResult activeResult;
extern int       lastAnswerIndex;
extern bool      lastAnswerCorrect;
extern bool      showAnswerFeedback;
extern float     feedbackTimer;

// -- Function declarations ------------------------------------

// Initialises Raylib window and loads resources
void initRenderer();

// Cleans up Raylib resources on exit
void closeRenderer();

// Master draw function - calls the correct screen draw function
void drawFrame();

// Master input function - handles input for current screen
void handleInput();
