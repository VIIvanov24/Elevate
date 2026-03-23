// ============================================================
//  utils.cpp
//  General-purpose utility functions: grading, shuffle,
//  string helpers, and password hashing.
// ============================================================

#include "utils.h"
#include "globals.h"
#include <cstdlib>
using namespace std;

// Converts percentage to Bulgarian grade 2-6
int calculateGrade(double percentage) {
    if (percentage >= GRADE_6_THRESHOLD) return 6;
    if (percentage >= GRADE_5_THRESHOLD) return 5;
    if (percentage >= GRADE_4_THRESHOLD) return 4;
    if (percentage >= GRADE_3_THRESHOLD) return 3;
    return 2;
}

// Fisher-Yates shuffle - randomly reorders array elements
void shuffleIndices(int* arr, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i]   = arr[j];
        arr[j]   = temp;
    }
}

// Trims spaces from both ends of a string using string methods
void trimString(string& str) {
    while (!str.empty() && str.front() == ' ') str.erase(str.begin());
    while (!str.empty() && str.back()  == ' ') str.pop_back();
}

// Password hashing using string class manipulation
// XORs each character with its position, converts to hex string
string hashPassword(const string& password) {
    string hash     = "";
    string hexChars = "0123456789ABCDEF";  // hex lookup using string
    int    checksum = 0;

    for (int i = 0; i < (int)password.size(); i++) {
        int charVal = (int)password[i];
        int mixed   = ((charVal ^ (i + 7)) + (i * 3)) % 256;

        // Build 2-char hex representation using string indexing
        hash += hexChars[mixed / 16];
        hash += hexChars[mixed % 16];

        checksum += charVal;
    }

    // Append 4-char checksum
    checksum = checksum % 65536;
    string cs = "";
    cs += hexChars[(checksum / 4096) % 16];
    cs += hexChars[(checksum / 256)  % 16];
    cs += hexChars[(checksum / 16)   % 16];
    cs += hexChars[checksum          % 16];

    // Prepend length tag and append checksum - uses string concatenation
    return "L" + to_string(password.size()) + "-" + hash + "-" + cs;
}
