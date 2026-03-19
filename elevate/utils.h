#pragma once

// ============================================================
//  utils.h
//  Declares general-purpose utility functions used throughout
//  the application: input validation, screen control, and
//  display helpers.
// ============================================================

#include <string>
using namespace std;

// Clears the console by printing blank lines
void clearScreen();

// Prints a horizontal divider line
// c    - the character to repeat
// width - total number of characters
void printDivider(char c = '=', int width = 60);

// Prints the Elevate ASCII art header
void printHeader();

// Reads and validates an integer in [minVal, maxVal].
// Keeps prompting until a valid value is entered.
int getValidInput(int minVal, int maxVal);

// Converts a percentage score to a Bulgarian grade (2-6)
int calculateGrade(double percentage);

// Randomly shuffles arr[0..size-1] using Fisher-Yates
void shuffleIndices(int* arr, int size);

// Trims leading and trailing spaces from a string (in-place)
void trimString(string& str);
