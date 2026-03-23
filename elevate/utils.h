#pragma once
#include <string>
using namespace std;

// Converts a percentage score to a Bulgarian grade (2-6)
int calculateGrade(double percentage);

// Randomly shuffles arr[0..size-1] using Fisher-Yates algorithm
void shuffleIndices(int* arr, int size);

// Trims leading and trailing spaces from a string (in-place)
void trimString(string& str);

// Hashes a password using string manipulation (educational, not cryptographic)
string hashPassword(const string& password);
