// ============================================================
//  renderer.cpp
//  All Raylib drawing and input handling for Elevate.
//
//  NEW CODE - Raylib concepts are explained in comments.
//  The C++ logic (grading, hashing, structs etc.) is unchanged.
// ============================================================

#include "renderer.h"
#include "utils.h"
#include "questions.h"
#include <string>
#include <cstring>
#include <algorithm>   // for std::min
using namespace std;

// ============================================================
//  Raylib basics used in this file:
//
//  BeginDrawing() / EndDrawing() - start/end a frame
//  ClearBackground(color)        - fill screen with a color
//  DrawText(text, x, y, size, color) - draw a string
//  DrawRectangleRec(rect, color) - draw a filled rectangle
//  DrawRectangleLinesEx(rect, thick, color) - draw border
//  GetMousePosition()            - returns Vector2 {x, y}
//  CheckCollisionPointRec(pt, rect) - point inside rectangle?
//  IsMouseButtonPressed(MOUSE_LEFT_BUTTON) - click detected?
//  IsKeyPressed(KEY_*)           - keyboard input
//  GetCharPressed()              - typed character
//  GetScreenWidth/Height()       - window dimensions
//  MeasureText(text, size)       - pixel width of text
//  DrawFPS(x, y)                 - debug FPS counter
// ============================================================

// -- Color palette (custom theme) ----------------------------
// Raylib colors are just {r, g, b, a} structs

static const Color BG_DARK       = { 13,  17,  23,  255 };  // deep dark bg
static const Color BG_CARD       = { 22,  27,  34,  255 };  // card background
static const Color BG_CARD2      = { 30,  37,  46,  255 };  // slightly lighter card
static const Color ACCENT_BLUE   = { 88, 166, 255,  255 };  // primary accent
static const Color ACCENT_GREEN  = { 63, 185, 80,   255 };  // correct / success
static const Color ACCENT_RED    = { 248, 81,  73,  255 };  // wrong / danger
static const Color ACCENT_PURPLE = { 139, 92, 246,  255 };  // secondary accent
static const Color ACCENT_ORANGE = { 255, 163, 26,  255 };  // warning / grade
static const Color TEXT_PRIMARY  = { 230, 237, 243, 255 };  // main text
static const Color TEXT_MUTED    = { 125, 133, 144, 255 };  // secondary text
static const Color TEXT_DARK     = {  13,  17,  23, 255 };  // text on light bg
static const Color BORDER_COLOR  = {  48,  54,  61, 255 };  // card borders

// -- Shared UI state -----------------------------------------

AppScreen    currentScreen      = SCREEN_LOGIN;
int          selectedCategory   = 0;
int          testIndices[TEST_SIZE];
int          currentQuestion    = 0;
StudentResult activeResult;
int          lastAnswerIndex    = -1;
bool         lastAnswerCorrect  = false;
bool         showAnswerFeedback = false;
float        feedbackTimer      = 0.0f;

// -- Text input buffers (for login/register fields) ----------
// Raylib has no built-in text box - we manage input manually

static char  usernameBuffer[64] = "";
static char  passwordBuffer[64] = "";
static char  confirmBuffer[64]  = "";
static int   usernameLen        = 0;
static int   passwordLen        = 0;
static int   confirmLen         = 0;
static bool  focusUsername      = true;
static bool  focusPassword      = false;
static bool  focusConfirm       = false;
static string authError         = "";

// -- Scroll state for statistics screen ----------------------
static float statsScrollY = 0.0f;

// ============================================================
//  HELPER FUNCTIONS
// ============================================================

// Draws text centered horizontally at a given Y position
// MeasureText returns pixel width so we can center it
static void drawCenteredText(const char* text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);                  // Raylib: measure text width
    int x = (GetScreenWidth() - textWidth) / 2;                   // Raylib: get window width
    DrawText(text, x, y, fontSize, color);                        // Raylib: draw text
}

// Draws a rounded card (filled rectangle with border)
static void drawCard(int x, int y, int w, int h, Color fill, Color border) {
    Rectangle rect = { (float)x, (float)y, (float)w, (float)h };
    DrawRectangleRec(rect, fill);                                  // Raylib: filled rect
    DrawRectangleLinesEx(rect, 1.5f, border);                     // Raylib: border
}

// Draws a button and returns true if it was clicked this frame
static bool drawButton(int x, int y, int w, int h,
                       const char* label, Color bg, Color textCol, int fontSize = 18) {
    Rectangle rect = { (float)x, (float)y, (float)w, (float)h };

    // Check if mouse is hovering - Raylib collision check
    Vector2 mouse = GetMousePosition();                            // Raylib: mouse position
    bool hovered  = CheckCollisionPointRec(mouse, rect);          // Raylib: point in rect?

    // Slightly lighten button on hover
    Color drawBg = hovered
        ? Color{ (unsigned char)min(255, (int)bg.r + 30),
                 (unsigned char)min(255, (int)bg.g + 30),
                 (unsigned char)min(255, (int)bg.b + 30), 255 }
        : bg;

    DrawRectangleRec(rect, drawBg);                               // Raylib: draw button bg
    if (hovered)
        DrawRectangleLinesEx(rect, 2.0f, ACCENT_BLUE);           // Raylib: highlight border

    // Center label text inside button
    int tw = MeasureText(label, fontSize);
    int tx = x + (w - tw) / 2;
    int ty = y + (h - fontSize) / 2;
    DrawText(label, tx, ty, fontSize, textCol);                   // Raylib: draw label

    // Return true on left click while hovering
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);    // Raylib: click check
}

// Draws a text input field and handles keyboard input for it
static void drawTextInput(int x, int y, int w, int h,
                          char* buffer, int& length, bool focused,
                          const char* placeholder, bool masked = false) {
    Rectangle rect = { (float)x, (float)y, (float)w, (float)h };
    Color border = focused ? ACCENT_BLUE : BORDER_COLOR;

    DrawRectangleRec(rect, BG_DARK);                              // Raylib: input bg
    DrawRectangleLinesEx(rect, focused ? 2.0f : 1.5f, border);   // Raylib: border

    // Show placeholder or typed text
    if (length == 0 && !focused) {
        DrawText(placeholder, x + 12, y + (h - 16) / 2, 19, TEXT_MUTED);
    } else {
        // Build display string - mask password with asterisks
        string display = "";
        if (masked) {
            display = string(length, '*');
        } else {
            display = string(buffer, length);
        }
        // Add blinking cursor when focused
        if (focused && ((int)(GetTime() * 2) % 2 == 0))          // Raylib: GetTime() for blink
            display += "|";
        DrawText(display.c_str(), x + 12, y + (h - 16) / 2, 19, TEXT_PRIMARY);
    }

    // Handle keyboard input when this field is focused
    if (focused) {
        // Backspace removes last character
        if (IsKeyPressed(KEY_BACKSPACE) && length > 0) {          // Raylib: key check
            buffer[--length] = '\0';
        }
        // GetCharPressed returns Unicode codepoint of typed key
        int ch;
        while ((ch = GetCharPressed()) > 0) {                     // Raylib: typed char
            if (ch >= 32 && ch < 127 && length < 63) {
                buffer[length++] = (char)ch;
                buffer[length]   = '\0';
            }
        }
    }
}

// Returns a color for a given grade
static Color gradeColor(int grade) {
    if (grade >= 6) return ACCENT_GREEN;
    if (grade >= 5) return ACCENT_BLUE;
    if (grade >= 4) return ACCENT_ORANGE;
    if (grade >= 3) return ACCENT_ORANGE;
    return ACCENT_RED;
}

// Draws a progress bar (0.0 - 1.0)
static void drawProgressBar(int x, int y, int w, int h, float progress, Color fill) {
    Rectangle bg   = { (float)x, (float)y, (float)w, (float)h };
    Rectangle fill_ = { (float)x, (float)y, (float)(w * progress), (float)h };
    DrawRectangleRec(bg,    BG_DARK);                             // Raylib: background track
    DrawRectangleRec(fill_, fill);                                // Raylib: filled portion
    DrawRectangleLinesEx(bg, 1.0f, BORDER_COLOR);                // Raylib: border
}

// ============================================================
//  SCREEN: LOGIN
// ============================================================

static void drawLogin() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);                                     // Raylib: clear screen

    // Title
    drawCenteredText("ELEVATE", 60, 58, ACCENT_BLUE);
    drawCenteredText("Electronic School  --  C++ Programming", 120, 21, TEXT_MUTED);

    // Card
    int cardW = 520, cardH = 420;
    int cardX = (sw - cardW) / 2;
    int cardY = (sh - cardH) / 2;
    drawCard(cardX, cardY, cardW, cardH, BG_CARD, BORDER_COLOR);

    DrawText("Login", cardX + 24, cardY + 24, 27, TEXT_PRIMARY);
    DrawText("Username", cardX + 24, cardY + 72, 17, TEXT_MUTED);
    drawTextInput(cardX + 24, cardY + 90, cardW - 48, 42,
                  usernameBuffer, usernameLen, focusUsername, "Enter username");

    DrawText("Password", cardX + 24, cardY + 148, 17, TEXT_MUTED);
    drawTextInput(cardX + 24, cardY + 166, cardW - 48, 42,
                  passwordBuffer, passwordLen, focusPassword, "Enter password", true);

    // Error message
    if (!authError.empty())
        DrawText(authError.c_str(), cardX + 24, cardY + 222, 19, ACCENT_RED);

    // Login button
    if (drawButton(cardX + 24, cardY + 256, cardW - 48, 44,
                   "Login", ACCENT_BLUE, TEXT_DARK, 20)) {
        string user(usernameBuffer, usernameLen);
        string pass(passwordBuffer, passwordLen);
        if (loginAccount(user, pass)) {
            currentScreen = SCREEN_MAIN_MENU;
            authError     = "";
        } else {
            authError = "Invalid username or password.";
        }
    }

    // Switch to register
    if (drawButton(cardX + 24, cardY + 310, cardW - 48, 36,
                   "Don't have an account? Register", BG_CARD2, TEXT_MUTED, 17)) {
        currentScreen = SCREEN_REGISTER;
        authError     = "";
        memset(usernameBuffer, 0, sizeof(usernameBuffer)); usernameLen = 0;
        memset(passwordBuffer, 0, sizeof(passwordBuffer)); passwordLen = 0;
        focusUsername = true; focusPassword = false;
    }

    // Tab / click to switch focus between fields
    if (IsKeyPressed(KEY_TAB)) {                                  // Raylib: Tab key
        focusUsername = !focusUsername;
        focusPassword = !focusPassword;
    }
    // Click to focus
    Vector2 mouse = GetMousePosition();
    Rectangle userRect = { (float)(cardX+24), (float)(cardY+90),  (float)(cardW-48), 42 };
    Rectangle passRect = { (float)(cardX+24), (float)(cardY+166), (float)(cardW-48), 42 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        focusUsername = CheckCollisionPointRec(mouse, userRect);
        focusPassword = CheckCollisionPointRec(mouse, passRect);
    }

    // Enter to submit
    if (IsKeyPressed(KEY_ENTER)) {                                // Raylib: Enter key
        string user(usernameBuffer, usernameLen);
        string pass(passwordBuffer, passwordLen);
        if (loginAccount(user, pass)) {
            currentScreen = SCREEN_MAIN_MENU;
            authError     = "";
        } else {
            authError = "Invalid username or password.";
        }
    }
}

// ============================================================
//  SCREEN: REGISTER
// ============================================================

static void drawRegister() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);
    drawCenteredText("ELEVATE", 60, 58, ACCENT_BLUE);
    drawCenteredText("Create your account", 120, 21, TEXT_MUTED);

    int cardW = 520, cardH = 480;
    int cardX = (sw - cardW) / 2;
    int cardY = (sh - cardH) / 2;
    drawCard(cardX, cardY, cardW, cardH, BG_CARD, BORDER_COLOR);

    DrawText("Register", cardX + 24, cardY + 24, 27, TEXT_PRIMARY);

    DrawText("Username  (min 3 chars)", cardX + 24, cardY + 70, 17, TEXT_MUTED);
    drawTextInput(cardX + 24, cardY + 88, cardW - 48, 42,
                  usernameBuffer, usernameLen, focusUsername, "Choose a username");

    DrawText("Password  (min 4 chars)", cardX + 24, cardY + 146, 17, TEXT_MUTED);
    drawTextInput(cardX + 24, cardY + 164, cardW - 48, 42,
                  passwordBuffer, passwordLen, focusPassword, "Choose a password", true);

    DrawText("Confirm Password", cardX + 24, cardY + 222, 17, TEXT_MUTED);
    drawTextInput(cardX + 24, cardY + 240, cardW - 48, 42,
                  confirmBuffer, confirmLen, focusConfirm, "Repeat your password", true);

    if (!authError.empty())
        DrawText(authError.c_str(), cardX + 24, cardY + 296, 17, ACCENT_RED);

    if (drawButton(cardX + 24, cardY + 322, cardW - 48, 44,
                   "Create Account", ACCENT_GREEN, TEXT_DARK, 20)) {
        string user(usernameBuffer,  usernameLen);
        string pass(passwordBuffer,  passwordLen);
        string conf(confirmBuffer,   confirmLen);

        if (user.size() < 3)        authError = "Username must be at least 3 characters.";
        else if (pass.size() < 4)   authError = "Password must be at least 4 characters.";
        else if (pass != conf)      authError = "Passwords do not match.";
        else if (!registerAccount(user, pass)) authError = "Username already taken.";
        else {
            // Auto-login after successful registration
            loginAccount(user, pass);
            currentScreen = SCREEN_MAIN_MENU;
            authError     = "";
        }
    }

    if (drawButton(cardX + 24, cardY + 378, cardW - 48, 32,
                   "Back to Login", BG_CARD2, TEXT_MUTED, 17)) {
        currentScreen = SCREEN_LOGIN;
        authError     = "";
        memset(usernameBuffer, 0, sizeof(usernameBuffer)); usernameLen = 0;
        memset(passwordBuffer, 0, sizeof(passwordBuffer)); passwordLen = 0;
        memset(confirmBuffer,  0, sizeof(confirmBuffer));  confirmLen  = 0;
        focusUsername = true; focusPassword = false; focusConfirm = false;
    }

    // Tab cycles through 3 fields
    if (IsKeyPressed(KEY_TAB)) {
        if (focusUsername)      { focusUsername = false; focusPassword = true;  focusConfirm = false; }
        else if (focusPassword) { focusUsername = false; focusPassword = false; focusConfirm = true;  }
        else                    { focusUsername = true;  focusPassword = false; focusConfirm = false; }
    }
    // Click to focus
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        focusUsername = CheckCollisionPointRec(mouse, { (float)(cardX+24), (float)(cardY+88),  (float)(cardW-48), 42 });
        focusPassword = CheckCollisionPointRec(mouse, { (float)(cardX+24), (float)(cardY+164), (float)(cardW-48), 42 });
        focusConfirm  = CheckCollisionPointRec(mouse, { (float)(cardX+24), (float)(cardY+240), (float)(cardW-48), 42 });
    }
}

// ============================================================
//  SCREEN: MAIN MENU
// ============================================================

static void drawMainMenu() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);

    // Header bar
    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawRectangle(0, 70, sw, 2, BORDER_COLOR);
    DrawText("ELEVATE", 28, 20, 40, ACCENT_BLUE);
    DrawText("Electronic School", 28, 48, 17, TEXT_MUTED);

    // User info top right
    string userInfo = "Logged in as: " + getCurrentUsername();
    if (isCurrentUserAdmin()) userInfo += "  [Admin]";
    int uiW = MeasureText(userInfo.c_str(), 15);
    DrawText(userInfo.c_str(), sw - uiW - 24, 28, 19, TEXT_MUTED);

    drawCenteredText("What would you like to do?", 110, 25, TEXT_MUTED);

    // Menu cards - 2x2 grid + one centered below
    int cardW = (sw - 80) / 2 - 8;
    int cardH = 140;
    int col1X = 24;
    int col2X = col1X + cardW + 16;

    // Study Mode card
    drawCard(col1X, 155, cardW, cardH, BG_CARD, BORDER_COLOR);
    DrawText("Study Mode", col1X + 20, 170, 25, ACCENT_BLUE);
    DrawText("Browse lessons and review answers", col1X + 20, 208, 19, TEXT_MUTED);
    DrawText("3 categories  |  30 questions", col1X + 20, 232, 17, TEXT_MUTED);
    {
        Rectangle r = { (float)col1X, 155, (float)cardW, (float)cardH };
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            currentScreen = SCREEN_STUDY;
    }

    // Take a Test card
    drawCard(col2X, 155, cardW, cardH, BG_CARD, BORDER_COLOR);
    DrawText("Take a Test", col2X + 20, 170, 25, ACCENT_GREEN);
    DrawText("20 random questions", col2X + 20, 208, 19, TEXT_MUTED);
    DrawText("Graded on Bulgarian scale 2-6", col2X + 20, 232, 17, TEXT_MUTED);
    {
        Rectangle r = { (float)col2X, 155, (float)cardW, (float)cardH };
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            generateTest(testIndices);
            activeResult       = createNewResult(getCurrentUsername());
            currentQuestion    = 0;
            showAnswerFeedback = false;
            currentScreen      = SCREEN_TEST;
        }
    }

    // Statistics card
    drawCard(col1X, 315, cardW, cardH, BG_CARD, BORDER_COLOR);
    DrawText("Statistics", col1X + 20, 330, 25, ACCENT_PURPLE);
    DrawText("View scores and rankings", col1X + 20, 338, 19, TEXT_MUTED);
    {
        string statsInfo = to_string(totalResults) + " result(s) recorded";
        DrawText(statsInfo.c_str(), col1X + 20, 362, 17, TEXT_MUTED);
        Rectangle r = { (float)col1X, 315, (float)cardW, (float)cardH };
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            statsScrollY  = 0;
            currentScreen = SCREEN_STATISTICS;
        }
    }

    // About card
    drawCard(col2X, 315, cardW, cardH, BG_CARD, BORDER_COLOR);
    DrawText("About", col2X + 20, 330, 25, ACCENT_ORANGE);
    DrawText("Grading scale, test structure", col2X + 20, 338, 15, TEXT_MUTED);
    DrawText("and project info", col2X + 20, 362, 17, TEXT_MUTED);
    {
        Rectangle r = { (float)col2X, 315, (float)cardW, (float)cardH };
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            currentScreen = SCREEN_ABOUT;
    }

    // Logout button bottom
    if (drawButton(sw/2 - 100, sh - 60, 200, 38, "Logout", BG_CARD2, TEXT_MUTED, 18)) {
        logoutAccount();
        currentScreen = SCREEN_LOGIN;
        memset(usernameBuffer, 0, sizeof(usernameBuffer)); usernameLen = 0;
        memset(passwordBuffer, 0, sizeof(passwordBuffer)); passwordLen = 0;
        focusUsername = true; focusPassword = false;
    }
}

// ============================================================
//  SCREEN: STUDY MODE (category picker)
// ============================================================

static void drawStudy() {
    int sw = GetScreenWidth();

    ClearBackground(BG_DARK);
    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("Study Mode", 32, 20, 32, TEXT_PRIMARY);
    DrawText("Choose a category to review", 28, 50, 17, TEXT_MUTED);

    if (drawButton(sw - 120, 18, 96, 36, "< Back", BG_CARD2, TEXT_MUTED, 17))
        currentScreen = SCREEN_MAIN_MENU;

    // One card per category
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        int cardY = 110 + i * 130;
        int cardX = (sw - 600) / 2;
        drawCard(cardX, cardY, 600, 110, BG_CARD, BORDER_COLOR);

        DrawText(categoryNames[i].c_str(), cardX + 24, cardY + 18, 25, ACCENT_BLUE);

        string info = to_string(categoryCount[i]) + " questions  |  "
                    + to_string(categoryPoints[i]) + " pt each";
        DrawText(info.c_str(), cardX + 24, cardY + 52, 19, TEXT_MUTED);

        if (drawButton(cardX + 420, cardY + 34, 140, 38, "Study ->", ACCENT_BLUE, TEXT_DARK, 18)) {
            selectedCategory = i;
            currentScreen    = SCREEN_STUDY_LESSON;
        }
    }
}

// ============================================================
//  SCREEN: STUDY LESSON (questions + answers for a category)
// ============================================================

static int lessonScrollY = 0;

static void drawStudyLesson() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);
    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText(categoryNames[selectedCategory].c_str(), 28, 20, 27, ACCENT_BLUE);
    DrawText("Correct answers marked in green", 28, 50, 17, TEXT_MUTED);

    if (drawButton(sw - 120, 18, 96, 36, "< Back", BG_CARD2, TEXT_MUTED, 17)) {
        currentScreen = SCREEN_STUDY;
        lessonScrollY = 0;
    }

    // Scroll with mouse wheel
    lessonScrollY -= (int)(GetMouseWheelMove() * 30.0f);
    if (lessonScrollY < 0) lessonScrollY = 0;

    int start = categoryStart[selectedCategory];
    int count = categoryCount[selectedCategory];
    int y     = 90 - lessonScrollY;

    // Enable scissor mode to clip content to screen area
    BeginScissorMode(0, 70, sw, sh - 70);                        // Raylib: clip drawing

    for (int i = 0; i < count; i++) {
        Question& q = questionBank[start + i];
        int cardX = (sw - 700) / 2;
        int cardH = 48 + 4 * 32 + 12;
        drawCard(cardX, y, 700, cardH, BG_CARD, BORDER_COLOR);

        string qLabel = "Q" + to_string(i + 1) + ": " + q.text;
        DrawText(qLabel.c_str(), cardX + 16, y + 12, 19, TEXT_PRIMARY);

        for (int opt = 0; opt < 4; opt++) {
            bool correct = (opt == q.correctIndex);
            Color col    = correct ? ACCENT_GREEN : TEXT_MUTED;
            string line  = string(1, (char)('A' + opt)) + ". " + q.options[opt];
            if (correct) line += "  [correct]";
            DrawText(line.c_str(), cardX + 32, y + 44 + opt * 32, 19, col);
        }

        y += cardH + 12;
    }

    EndScissorMode();                                             // Raylib: end clip
}

// ============================================================
//  SCREEN: TEST
// ============================================================

static void drawTest() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);

    if (currentQuestion >= TEST_SIZE) {
        // All questions answered
        finalizeResult(activeResult);
        storeResult(activeResult);
        currentScreen = SCREEN_TEST_RESULT;
        return;
    }

    Question& q = questionBank[testIndices[currentQuestion]];

    // Header with progress
    DrawRectangle(0, 0, sw, 80, BG_CARD);
    string progText = "Question  " + to_string(currentQuestion + 1) + " / " + to_string(TEST_SIZE);
    DrawText(progText.c_str(), 28, 16, 23, TEXT_PRIMARY);
    DrawText(q.category.c_str(), 28, 44, 17, TEXT_MUTED);

    string ptText = to_string(q.points) + " pt";
    DrawText(ptText.c_str(), sw - 80, 28, 21, ACCENT_ORANGE);

    // Progress bar
    float progress = (float)currentQuestion / TEST_SIZE;
    drawProgressBar(0, 72, sw, 8, progress, ACCENT_BLUE);

    // Score so far
    string scoreText = "Score: " + to_string(activeResult.score);
    int stW = MeasureText(scoreText.c_str(), 16);
    DrawText(scoreText.c_str(), sw - stW - 24, 52, 19, ACCENT_GREEN);

    // Question text
    int qCardX = (sw - 720) / 2;
    drawCard(qCardX, 110, 720, 80, BG_CARD, BORDER_COLOR);
    DrawText(q.text.c_str(), qCardX + 20, 138, 21, TEXT_PRIMARY);

    // Answer feedback overlay
    if (showAnswerFeedback) {
        // GetFrameTime returns seconds since last frame - used for timers
        feedbackTimer -= GetFrameTime();                          // Raylib: delta time
        if (feedbackTimer <= 0) {
            showAnswerFeedback = false;
            currentQuestion++;
        }
    }

    // Answer buttons
    for (int i = 0; i < 4; i++) {
        int btnY = 220 + i * 72;
        int btnX = (sw - 680) / 2;

        Color bg = BG_CARD;
        Color tc = TEXT_PRIMARY;

        if (showAnswerFeedback) {
            if (i == q.correctIndex)       { bg = ACCENT_GREEN; tc = TEXT_DARK; }
            else if (i == lastAnswerIndex) { bg = ACCENT_RED;   tc = TEXT_DARK; }
        }

        string label = string(1, (char)('A' + i)) + ".  " + q.options[i];
        if (!showAnswerFeedback) {
            if (drawButton(btnX, btnY, 680, 58, label.c_str(), bg, tc, 17)) {
                lastAnswerIndex   = i;
                lastAnswerCorrect = evaluateAnswer(activeResult,
                                                  testIndices[currentQuestion], i);
                showAnswerFeedback = true;
                feedbackTimer      = 1.0f;  // show feedback for 1 second
            }
        } else {
            // Just draw the button without click handling during feedback
            DrawRectangle(btnX, btnY, 680, 58, bg);
            Rectangle r = { (float)btnX, (float)btnY, 680, 58 };
            DrawRectangleLinesEx(r, 1.5f, BORDER_COLOR);
            DrawText(label.c_str(), btnX + 16, btnY + (58 - 17) / 2, 21, tc);
        }
    }

    // Feedback message
    if (showAnswerFeedback) {
        const char* msg = lastAnswerCorrect ? "Correct!" : "Wrong!";
        Color msgCol    = lastAnswerCorrect ? ACCENT_GREEN : ACCENT_RED;
        drawCenteredText(msg, sh - 60, 26, msgCol);
    }
}

// ============================================================
//  SCREEN: TEST RESULT
// ============================================================

static void drawTestResult() {
    int sw = GetScreenWidth();

    ClearBackground(BG_DARK);

    drawCenteredText("Test Complete!", 50, 40, TEXT_PRIMARY);

    int grade  = activeResult.grade;
    Color gcol = gradeColor(grade);

    // Big grade display
    string gradeStr = "Grade: " + to_string(grade);
    drawCenteredText(gradeStr.c_str(), 110, 58, gcol);

    // Score and percentage
    string scoreStr = to_string(activeResult.score) + " / " + to_string(activeResult.maxScore) + " points";
    drawCenteredText(scoreStr.c_str(), 174, 27, TEXT_PRIMARY);

    string pctStr = to_string((int)activeResult.percentage) + "%";
    drawCenteredText(pctStr.c_str(), 210, 23, TEXT_MUTED);

    // Category breakdown
    int cardW = 560, cardX = (sw - cardW) / 2;
    drawCard(cardX, 260, cardW, 160, BG_CARD, BORDER_COLOR);
    DrawText("Breakdown by category:", cardX + 20, 276, 21, TEXT_MUTED);

    for (int c = 0; c < NUM_CATEGORIES; c++) {
        int y = 306 + c * 34;
        double pct = (activeResult.categoryTotal[c] > 0)
            ? (double)activeResult.categoryCorrect[c] / activeResult.categoryTotal[c] * 100.0
            : 0.0;

        DrawText(categoryNames[c].c_str(), cardX + 20, y, 19, TEXT_PRIMARY);

        string catScore = to_string(activeResult.categoryCorrect[c])
                        + "/" + to_string(activeResult.categoryTotal[c]);
        DrawText(catScore.c_str(), cardX + 340, y, 19, TEXT_MUTED);

        drawProgressBar(cardX + 390, y + 2, 140, 14, (float)(pct / 100.0), ACCENT_BLUE);
    }

    if (drawButton(sw/2 - 220, 450, 200, 46, "Take Another Test", ACCENT_BLUE, TEXT_DARK, 18)) {
        generateTest(testIndices);
        activeResult    = createNewResult(getCurrentUsername());
        currentQuestion = 0;
        showAnswerFeedback = false;
        currentScreen   = SCREEN_TEST;
    }
    if (drawButton(sw/2 + 20, 450, 200, 46, "Main Menu", BG_CARD2, TEXT_PRIMARY, 18))
        currentScreen = SCREEN_MAIN_MENU;
}

// ============================================================
//  SCREEN: STATISTICS
// ============================================================

static void drawStatistics() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    ClearBackground(BG_DARK);
    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("Statistics", 32, 20, 32, TEXT_PRIMARY);
    DrawText("All student results", 28, 50, 17, TEXT_MUTED);

    if (drawButton(sw - 120, 18, 96, 36, "< Back", BG_CARD2, TEXT_MUTED, 17))
        currentScreen = SCREEN_MAIN_MENU;

    if (totalResults == 0) {
        drawCenteredText("No results yet. Take a test first!", sh / 2, 20, TEXT_MUTED);
        return;
    }

    // Summary cards row
    double avg = getAverageScore();
    int    hi  = getHighestScoreIndex();
    int    lo  = getLowestScoreIndex();

    int sumCardW = (sw - 80) / 3;
    // Average
    drawCard(24, 90, sumCardW, 90, BG_CARD, BORDER_COLOR);
    DrawText("Average Score", 44, 102, 17, TEXT_MUTED);
    string avgStr = to_string((int)avg) + "%";
    DrawText(avgStr.c_str(), 44, 124, 32, ACCENT_BLUE);

    // Highest
    drawCard(24 + sumCardW + 16, 90, sumCardW, 90, BG_CARD, BORDER_COLOR);
    DrawText("Highest Score", 44 + sumCardW + 16, 102, 17, TEXT_MUTED);
    string hiStr = results[hi].studentName + "  " + to_string((int)results[hi].percentage) + "%";
    DrawText(hiStr.c_str(), 44 + sumCardW + 16, 124, 23, ACCENT_GREEN);

    // Lowest
    drawCard(24 + (sumCardW + 16) * 2, 90, sumCardW, 90, BG_CARD, BORDER_COLOR);
    DrawText("Lowest Score", 44 + (sumCardW + 16) * 2, 102, 17, TEXT_MUTED);
    string loStr = results[lo].studentName + "  " + to_string((int)results[lo].percentage) + "%";
    DrawText(loStr.c_str(), 44 + (sumCardW + 16) * 2, 124, 23, ACCENT_RED);

    // Category performance
    drawCard(24, 196, sw - 48, 100, BG_CARD, BORDER_COLOR);
    DrawText("Category Performance", 44, 208, 19, TEXT_MUTED);
    for (int c = 0; c < NUM_CATEGORIES; c++) {
        int cx = 44 + c * ((sw - 96) / NUM_CATEGORIES);
        double pct = getCategoryPassRate(c);
        DrawText(categoryNames[c].c_str(), cx, 232, 17, TEXT_PRIMARY);
        drawProgressBar(cx, 256, 200, 16, (float)(pct / 100.0), ACCENT_PURPLE);
        string pctStr = to_string((int)pct) + "%";
        DrawText(pctStr.c_str(), cx + 208, 256, 17, TEXT_MUTED);
    }

    // Scroll with mouse wheel - GetMouseWheelMove returns float
    statsScrollY -= GetMouseWheelMove() * 30.0f;
    if (statsScrollY < 0.0f) statsScrollY = 0.0f;

    int tableY   = 314;
    int rowH     = 42;
    int tableTop = tableY;

    BeginScissorMode(0, tableTop, sw, sh - tableTop);

    // Table header
    int hy = tableTop - (int)statsScrollY;
    DrawRectangle(24, hy, sw - 48, rowH, BG_CARD2);
    DrawText("Name",       50,           hy + 12, 17, TEXT_MUTED);
    DrawText("Score",      sw/2 - 80,    hy + 12, 17, TEXT_MUTED);
    DrawText("Percent",    sw/2 + 20,    hy + 12, 17, TEXT_MUTED);
    DrawText("Grade",      sw/2 + 140,   hy + 12, 17, TEXT_MUTED);

    for (int i = 0; i < totalResults; i++) {
        int rowY = tableTop - (int)statsScrollY + (i + 1) * rowH;
        Color rowBg = (i % 2 == 0) ? BG_CARD : BG_DARK;
        DrawRectangle(24, rowY, sw - 48, rowH, rowBg);

        DrawText(results[i].studentName.c_str(), 50, rowY + 12, 19, TEXT_PRIMARY);

        string scoreStr = to_string(results[i].score) + "/" + to_string(results[i].maxScore);
        DrawText(scoreStr.c_str(), sw/2 - 80, rowY + 12, 19, TEXT_MUTED);

        string pctStr = to_string((int)results[i].percentage) + "%";
        DrawText(pctStr.c_str(), sw/2 + 20, rowY + 12, 19, TEXT_PRIMARY);

        string gradeStr = to_string(results[i].grade);
        DrawText(gradeStr.c_str(), sw/2 + 140, rowY + 12, 19, gradeColor(results[i].grade));
    }

    EndScissorMode();
}

// ============================================================
//  SCREEN: ABOUT
// ============================================================

static void drawAbout() {
    int sw = GetScreenWidth();

    ClearBackground(BG_DARK);
    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("About Elevate", 32, 20, 32, TEXT_PRIMARY);
    DrawText("Project info and grading scale", 28, 50, 17, TEXT_MUTED);

    if (drawButton(sw - 120, 18, 96, 36, "< Back", BG_CARD2, TEXT_MUTED, 17))
        currentScreen = SCREEN_MAIN_MENU;

    int cardX = (sw - 680) / 2;

    drawCard(cardX, 90, 680, 160, BG_CARD, BORDER_COLOR);
    DrawText("Project Info", cardX + 20, 106, 21, ACCENT_BLUE);
    DrawText("Subject: C++ Programming Basics", cardX + 20, 134, 19, TEXT_PRIMARY);
    DrawText("Grade: IX  |  Year: 2025/2026", cardX + 20, 158, 19, TEXT_PRIMARY);
    string bankStr = "Question bank: " + to_string(TOTAL_QUESTIONS)
                   + " questions  |  Test size: " + to_string(TEST_SIZE) + " questions";
    DrawText(bankStr.c_str(), cardX + 20, 182, 19, TEXT_MUTED);
    DrawText("Categories: Variables & Data Types | Control Flow | Functions & Strings",
             cardX + 20, 206, 17, TEXT_MUTED);

    drawCard(cardX, 270, 680, 200, BG_CARD, BORDER_COLOR);
    DrawText("Grading Scale", cardX + 20, 286, 21, ACCENT_ORANGE);

    const char* grades[]  = { "Grade 6", "Grade 5", "Grade 4", "Grade 3", "Grade 2" };
    const char* thresholds[] = { ">= 90%", ">= 75%", ">= 62%", ">= 50%", "< 50%" };
    Color gcols[] = { ACCENT_GREEN, ACCENT_BLUE, ACCENT_ORANGE, ACCENT_ORANGE, ACCENT_RED };

    for (int i = 0; i < 5; i++) {
        int gy = 318 + i * 28;
        DrawRectangle(cardX + 20, gy, 12, 18, gcols[i]);
        DrawText(grades[i],     cardX + 42, gy, 19, TEXT_PRIMARY);
        DrawText(thresholds[i], cardX + 160, gy, 19, TEXT_MUTED);
    }

    drawCard(cardX, 488, 680, 80, BG_CARD, BORDER_COLOR);
    DrawText("Password Hashing", cardX + 20, 504, 19, ACCENT_PURPLE);
    DrawText("Passwords are hashed using a custom XOR + hex string algorithm.", cardX + 20, 528, 17, TEXT_MUTED);
    DrawText("Plain text passwords are never stored.", cardX + 20, 548, 17, TEXT_MUTED);
}

// ============================================================
//  PUBLIC API
// ============================================================

void initRenderer() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);                        // Raylib: allow window resize
    InitWindow(1440, 900, "ELEVATE - Electronic School");         // Raylib: create window
    SetWindowMinSize(1100, 700);                                  // Raylib: minimum size
    SetTargetFPS(60);                                             // Raylib: cap at 60 fps
}

void closeRenderer() {
    CloseWindow();                                                // Raylib: destroy window
}

void drawFrame() {
    BeginDrawing();                                               // Raylib: start frame

    switch (currentScreen) {
        case SCREEN_LOGIN:        drawLogin();        break;
        case SCREEN_REGISTER:     drawRegister();     break;
        case SCREEN_MAIN_MENU:    drawMainMenu();     break;
        case SCREEN_STUDY:        drawStudy();        break;
        case SCREEN_STUDY_LESSON: drawStudyLesson();  break;
        case SCREEN_TEST:         drawTest();         break;
        case SCREEN_TEST_RESULT:  drawTestResult();   break;
        case SCREEN_STATISTICS:   drawStatistics();   break;
        case SCREEN_ABOUT:        drawAbout();        break;
    }

    EndDrawing();                                                 // Raylib: end frame, swap buffers
}

void handleInput() {
    // Input is handled inside each draw function
    // since Raylib input queries (IsKeyPressed, GetCharPressed etc.)
    // are valid between BeginDrawing and EndDrawing
}
