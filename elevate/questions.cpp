// ============================================================
//  questions.cpp
//  Question bank with 30 questions across 3 categories.
// ============================================================

#include "questions.h"

string categoryNames[NUM_CATEGORIES] = {
    "Variables & Data Types",
    "Control Flow",
    "Functions & Strings"
};

int categoryPoints[NUM_CATEGORIES]   = { 1, 1, 2 };
int categoryStart[NUM_CATEGORIES]    = { 0, 10, 20 };
int categoryCount[NUM_CATEGORIES]    = { 10, 10, 10 };
int testFromCategory[NUM_CATEGORIES] = { 6, 7, 7 };

Question questionBank[TOTAL_QUESTIONS];

void initQuestionBank() {

    // -- Category 0: Variables & Data Types -------------------

    questionBank[0] = {
        "What is the correct way to declare an integer variable in C++?",
        {"int x;", "integer x;", "var x = 0;", "x := 0;"},
        0, "Variables & Data Types", 1
    };
    questionBank[1] = {
        "Which data type stores a single character in C++?",
        {"string", "char", "letter", "word"},
        1, "Variables & Data Types", 1
    };
    questionBank[2] = {
        "What is the size of an 'int' on most modern systems?",
        {"1 byte", "2 bytes", "4 bytes", "8 bytes"},
        2, "Variables & Data Types", 1
    };
    questionBank[3] = {
        "Which keyword declares a constant variable in C++?",
        {"static", "const", "final", "fixed"},
        1, "Variables & Data Types", 1
    };
    questionBank[4] = {
        "What does 'double' store compared to 'float'?",
        {"Integers only", "Less precision", "More precision", "Characters"},
        2, "Variables & Data Types", 1
    };
    questionBank[5] = {
        "What is the result of: int x = 7 / 2; in C++?",
        {"3.5", "3", "4", "Compilation error"},
        1, "Variables & Data Types", 1
    };
    questionBank[6] = {
        "Which operator gives the remainder of integer division?",
        {"/", "*", "%", "^"},
        2, "Variables & Data Types", 1
    };
    questionBank[7] = {
        "What does the 'bool' type store?",
        {"Whole numbers", "Characters", "True or false", "Decimal numbers"},
        2, "Variables & Data Types", 1
    };
    questionBank[8] = {
        "What is the output of: cout << 5 + 3 * 2;",
        {"16", "11", "10", "13"},
        1, "Variables & Data Types", 1
    };
    questionBank[9] = {
        "Which of these is NOT a valid C++ variable name?",
        {"myVar", "_count", "2fast", "score1"},
        2, "Variables & Data Types", 1
    };

    // -- Category 1: Control Flow -----------------------------

    questionBank[10] = {
        "Which loop checks its condition AFTER executing the body?",
        {"for", "while", "do-while", "foreach"},
        2, "Control Flow", 1
    };
    questionBank[11] = {
        "What does 'break' do inside a loop?",
        {"Skips current iteration", "Exits the loop entirely", "Restarts the loop", "Does nothing"},
        1, "Control Flow", 1
    };
    questionBank[12] = {
        "What is the output of: for(int i=0; i<3; i++) cout << i;",
        {"123", "012", "0123", "1234"},
        1, "Control Flow", 1
    };
    questionBank[13] = {
        "Which keyword skips the rest of the current loop iteration?",
        {"skip", "break", "continue", "next"},
        2, "Control Flow", 1
    };
    questionBank[14] = {
        "What does 'if (x > 0 && x < 10)' check?",
        {"x is 0 or 10", "x is between 0 and 10 exclusive", "x equals 5", "x is negative"},
        1, "Control Flow", 1
    };
    questionBank[15] = {
        "How many times does this run: for(int i=1; i<=5; i++)?",
        {"4", "5", "6", "Infinite"},
        1, "Control Flow", 1
    };
    questionBank[16] = {
        "What is the purpose of the 'switch' statement?",
        {"Loop over an array", "Select among multiple cases", "Define a function", "Declare a variable"},
        1, "Control Flow", 1
    };
    questionBank[17] = {
        "What happens if a switch case is missing a 'break'?",
        {"Compilation error", "Falls through to next case", "Loop restarts", "Nothing changes"},
        1, "Control Flow", 1
    };
    questionBank[18] = {
        "Which operator represents logical OR in C++?",
        {"&", "||", "!", "OR"},
        1, "Control Flow", 1
    };
    questionBank[19] = {
        "Output of: int x=10; while(x>8){ cout<<x; x--; }",
        {"10", "109", "1098", "1089"},
        1, "Control Flow", 1
    };

    // -- Category 2: Functions & Strings ----------------------

    questionBank[20] = {
        "Return type of a C++ function that returns nothing?",
        {"null", "none", "void", "empty"},
        2, "Functions & Strings", 2
    };
    questionBank[21] = {
        "How do you get the length of a string 's'?",
        {"s.size() or s.length()", "len(s)", "length(s)", "s.count()"},
        0, "Functions & Strings", 2
    };
    questionBank[22] = {
        "What does 'return' do inside a function?",
        {"Prints a value", "Terminates the program", "Sends value back to caller", "Declares a variable"},
        2, "Functions & Strings", 2
    };
    questionBank[23] = {
        "Which string method extracts a portion of a string?",
        {"s.split()", "s.substr()", "s.slice()", "s.cut()"},
        1, "Functions & Strings", 2
    };
    questionBank[24] = {
        "What is function overloading in C++?",
        {"Calling a function too many times", "Same name different parameters", "A function calling itself", "No return value"},
        1, "Functions & Strings", 2
    };
    questionBank[25] = {
        "What does s.find() return when substring is not found?",
        {"0", "-1", "string::npos", "false"},
        2, "Functions & Strings", 2
    };
    questionBank[26] = {
        "What is recursion in programming?",
        {"A loop counting down", "A function that calls itself", "A special variable type", "A runtime error"},
        1, "Functions & Strings", 2
    };
    questionBank[27] = {
        "Which operator concatenates two strings in C++?",
        {"&", ".", "+", "*"},
        2, "Functions & Strings", 2
    };
    questionBank[28] = {
        "What does s.erase(0, 3) do to string s = \"HelloWorld\"?",
        {"Removes last 3 chars", "Removes first 3 chars", "Clears the string", "Does nothing"},
        1, "Functions & Strings", 2
    };
    questionBank[29] = {
        "How do you convert a string to uppercase in C++?",
        {"s.toUpper()", "toupper() on each character", "s.upper()", "uppercase(s)"},
        1, "Functions & Strings", 2
    };
}
