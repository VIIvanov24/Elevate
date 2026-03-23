// ============================================================
//  main.cpp
//  Entry point for the Elevate Electronic School application.
//
//  Project  : Elevate -- Electronic School
//  Subject  : C++ Programming Basics
//  Grade    : IX, 2025/2026
//  Language : C++ with Raylib for graphics
// ============================================================

#include <cstdlib>
#include <ctime>
#include "globals.h"
#include "questions.h"
#include "utils.h"
#include "renderer.h"   // NEW: Raylib-based renderer

// WindowShouldClose() is a Raylib function that returns true
// when the user closes the window or presses Escape
int main() {
    srand((unsigned int)time(nullptr));  // seed random for test generation

    initQuestionBank();   // load all 30 questions into questionBank[]
    initRenderer();       // NEW: open the Raylib window

    // Main game loop - Raylib runs at 60fps
    // WindowShouldClose() returns true when user closes the window
    while (!WindowShouldClose()) {                               // Raylib: loop condition
        drawFrame();      // draw the current screen
    }

    closeRenderer();      // NEW: clean up Raylib
    return 0;
}
