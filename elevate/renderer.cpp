// ============================================================
//  renderer.cpp
//  All Raylib drawing and input handling for Elevate.
//
//  NEW CODE - All Raylib-specific lines are commented.
//  The C++ logic (grading, hashing, structs) is unchanged
//  and lives in separate files with no Raylib dependency.
// ============================================================

#include "renderer.h"
#include "utils.h"
#include "questions.h"
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;

// ============================================================
//  RAYLIB QUICK REFERENCE (functions used in this file):
//
//  InitWindow(w, h, title)          - open the OS window
//  SetTargetFPS(n)                  - cap frame rate
//  SetConfigFlags(flag)             - set window options
//  SetWindowMinSize(w, h)           - minimum resize limit
//  WindowShouldClose()              - true when X is clicked
//  BeginDrawing() / EndDrawing()    - start / end a frame
//  ClearBackground(color)           - fill screen with color
//  DrawText(text, x, y, size, col)  - render a string
//  DrawRectangle(x,y,w,h,col)       - filled rectangle
//  DrawRectangleRec(rect, col)      - filled rectangle (rect)
//  DrawRectangleLinesEx(rect,t,col) - rectangle outline
//  MeasureText(text, size)          - pixel width of text
//  GetScreenWidth() / Height()      - current window size
//  GetMousePosition()               - mouse coords (Vector2)
//  CheckCollisionPointRec(pt, rect) - point inside rect?
//  IsMouseButtonPressed(btn)        - mouse click this frame?
//  IsKeyPressed(KEY_*)              - keyboard press this frame
//  GetCharPressed()                 - Unicode char typed
//  GetTime()                        - seconds since start
//  GetFrameTime()                   - delta time (seconds)
//  BeginScissorMode(x,y,w,h)       - clip drawing region
//  EndScissorMode()                 - end clipping
//  CloseWindow()                    - destroy window
// ============================================================

// ── Color palette ────────────────────────────────────────────
// Raylib Color is a struct: { r, g, b, a } all unsigned char

static const Color BG_DARK      = {  13,  17,  23, 255 };
static const Color BG_CARD      = {  22,  27,  34, 255 };
static const Color BG_CARD2     = {  30,  37,  46, 255 };
static const Color ACCENT_BLUE  = {  88, 166, 255, 255 };
static const Color ACCENT_GREEN = {  63, 185,  80, 255 };
static const Color ACCENT_RED   = { 248,  81,  73, 255 };
static const Color ACCENT_PURP  = { 139,  92, 246, 255 };
static const Color ACCENT_ORG   = { 255, 163,  26, 255 };
static const Color TEXT_PRI     = { 230, 237, 243, 255 };
static const Color TEXT_MUT     = { 125, 133, 144, 255 };
static const Color TEXT_DARK    = {  13,  17,  23, 255 };
static const Color BORDER       = {  48,  54,  61, 255 };

// ── Shared UI state ───────────────────────────────────────────

AppScreen     currentScreen      = SCREEN_LOGIN;
int           selectedCategory   = 0;
int           testIndices[TEST_SIZE];
int           currentQuestion    = 0;
StudentResult activeResult;
int           lastAnswerIndex    = -1;
bool          lastAnswerCorrect  = false;
bool          showAnswerFeedback = false;
float         feedbackTimer      = 0.0f;

// ── Text input state ──────────────────────────────────────────
// Raylib has no built-in text box so we manage buffers manually

static char  sBufUser[64] = "";
static char  sBufPass[64] = "";
static char  sBufConf[64] = "";
static int   sLenUser = 0, sLenPass = 0, sLenConf = 0;
static bool  sFocUser = true, sFocPass = false, sFocConf = false;
static string sAuthErr = "";
static float  sScrollLesson = 0.0f;
static float  sScrollStats  = 0.0f;

// ============================================================
//  HELPER FUNCTIONS
// ============================================================

// Center text horizontally on screen at Y
static void drawCentered(const char* txt, int y, int sz, Color col) {
    int w = MeasureText(txt, sz);                    // Raylib: pixel width
    DrawText(txt, (GetScreenWidth() - w) / 2, y, sz, col); // Raylib: draw
}

// Filled rect + outline card
static void drawCard(int x, int y, int w, int h) {
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    DrawRectangleRec(r, BG_CARD);                    // Raylib: filled rect
    DrawRectangleLinesEx(r, 1.5f, BORDER);           // Raylib: outline
}

// Button — returns true if clicked this frame
static bool drawBtn(int x, int y, int w, int h,
                    const char* label, Color bg, Color tc, int sz = 18) {
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    Vector2   m = GetMousePosition();                // Raylib: mouse pos
    bool hov    = CheckCollisionPointRec(m, r);      // Raylib: hit test

    // Lighten on hover
    Color c = hov ? Color{
        (unsigned char)min(255, (int)bg.r + 25),
        (unsigned char)min(255, (int)bg.g + 25),
        (unsigned char)min(255, (int)bg.b + 25), 255 } : bg;

    DrawRectangleRec(r, c);                          // Raylib: bg
    if (hov) DrawRectangleLinesEx(r, 2.0f, ACCENT_BLUE); // Raylib: hover border

    int tw = MeasureText(label, sz);
    DrawText(label, x + (w - tw) / 2, y + (h - sz) / 2, sz, tc); // Raylib: label

    return hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON); // Raylib: click
}

// Text input field with keyboard handling
static void drawInput(int x, int y, int w, int h,
                      char* buf, int& len, bool focused,
                      const char* placeholder, bool masked = false) {
    Rectangle r = { (float)x, (float)y, (float)w, (float)h };
    DrawRectangleRec(r, BG_DARK);                    // Raylib: bg
    DrawRectangleLinesEx(r, focused ? 2.0f : 1.5f,
                         focused ? ACCENT_BLUE : BORDER); // Raylib: border

    if (len == 0 && !focused) {
        DrawText(placeholder, x + 12, y + (h - 18) / 2, 18, TEXT_MUT);
    } else {
        string disp = masked ? string(len, '*') : string(buf, len);
        // Blinking cursor using GetTime() modulo
        if (focused && (int)(GetTime() * 2) % 2 == 0) // Raylib: elapsed time
            disp += "|";
        DrawText(disp.c_str(), x + 12, y + (h - 18) / 2, 18, TEXT_PRI);
    }

    if (!focused) return;

    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && len > 0) // Raylib: key press
        buf[--len] = '\0';

    // Typed characters
    int ch;
    while ((ch = GetCharPressed()) > 0)          // Raylib: char typed
        if (ch >= 32 && ch < 127 && len < 63)
            { buf[len++] = (char)ch; buf[len] = '\0'; }
}

// Progress bar 0.0 – 1.0
static void drawBar(int x, int y, int w, int h, float pct, Color col) {
    Rectangle bg   = { (float)x, (float)y, (float)w,         (float)h };
    Rectangle fill = { (float)x, (float)y, (float)(w * pct), (float)h };
    DrawRectangleRec(bg,   BG_DARK);               // Raylib: track
    DrawRectangleRec(fill, col);                   // Raylib: fill
    DrawRectangleLinesEx(bg, 1.0f, BORDER);        // Raylib: border
}

// Grade -> color
static Color gradeCol(int g) {
    if (g >= 6) return ACCENT_GREEN;
    if (g >= 5) return ACCENT_BLUE;
    if (g >= 3) return ACCENT_ORG;
    return ACCENT_RED;
}

// ============================================================
//  SCREEN: LOGIN
// ============================================================
static void drawLogin() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);                        // Raylib: clear

    drawCentered("ELEVATE", sh / 2 - 260, 56, ACCENT_BLUE);
    drawCentered("Electronic School  --  C++ Programming", sh / 2 - 195, 20, TEXT_MUT);

    int cw = 480, ch = 380;
    int cx = (sw - cw) / 2;
    int cy = (sh - ch) / 2 - 10;

    drawCard(cx, cy, cw, ch);
    DrawText("Login", cx + 24, cy + 22, 28, TEXT_PRI);

    // Username
    DrawText("Username", cx + 24, cy + 68, 15, TEXT_MUT);
    drawInput(cx + 24, cy + 88, cw - 48, 44, sBufUser, sLenUser, sFocUser, "Enter username");

    // Password
    DrawText("Password", cx + 24, cy + 148, 15, TEXT_MUT);
    drawInput(cx + 24, cy + 168, cw - 48, 44, sBufPass, sLenPass, sFocPass, "Enter password", true);

    if (!sAuthErr.empty())
        DrawText(sAuthErr.c_str(), cx + 24, cy + 228, 15, ACCENT_RED);

    if (drawBtn(cx + 24, cy + 268, cw - 48, 48, "Login", ACCENT_BLUE, TEXT_DARK, 19)) {
        string u(sBufUser, sLenUser), p(sBufPass, sLenPass);
        if (loginAccount(u, p)) { currentScreen = SCREEN_MAIN_MENU; sAuthErr = ""; }
        else sAuthErr = "Invalid username or password.";
    }

    if (drawBtn(cx + 24, cy + 328, cw - 48, 38, "Don't have an account? Register", BG_CARD2, TEXT_MUT, 15)) {
        currentScreen = SCREEN_REGISTER; sAuthErr = "";
        memset(sBufUser,0,64); sLenUser=0;
        memset(sBufPass,0,64); sLenPass=0;
        sFocUser=true; sFocPass=false;
    }

    // Tab switches focus
    if (IsKeyPressed(KEY_TAB))                       // Raylib: key
        { sFocUser = !sFocUser; sFocPass = !sFocPass; }

    // Click to focus
    Vector2 m = GetMousePosition();                  // Raylib: mouse
    Rectangle ru = {(float)(cx+24),(float)(cy+88),(float)(cw-48),44};
    Rectangle rp = {(float)(cx+24),(float)(cy+168),(float)(cw-48),44};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {   // Raylib: click
        sFocUser = CheckCollisionPointRec(m, ru);    // Raylib: hit test
        sFocPass = CheckCollisionPointRec(m, rp);
    }

    // Enter submits
    if (IsKeyPressed(KEY_ENTER)) {                   // Raylib: enter key
        string u(sBufUser,sLenUser), p(sBufPass,sLenPass);
        if (loginAccount(u,p)) { currentScreen=SCREEN_MAIN_MENU; sAuthErr=""; }
        else sAuthErr = "Invalid username or password.";
    }
}

// ============================================================
//  SCREEN: REGISTER
// ============================================================
static void drawRegister() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    drawCentered("ELEVATE", sh / 2 - 300, 56, ACCENT_BLUE);
    drawCentered("Create your account", sh / 2 - 238, 20, TEXT_MUT);

    int cw = 480, ch = 450;
    int cx = (sw - cw) / 2;
    int cy = (sh - ch) / 2;

    drawCard(cx, cy, cw, ch);
    DrawText("Register", cx + 24, cy + 22, 28, TEXT_PRI);

    DrawText("Username  (min 3 chars)", cx + 24, cy + 68, 15, TEXT_MUT);
    drawInput(cx+24, cy+86, cw-48, 44, sBufUser, sLenUser, sFocUser, "Choose a username");

    DrawText("Password  (min 4 chars)", cx + 24, cy + 146, 15, TEXT_MUT);
    drawInput(cx+24, cy+164, cw-48, 44, sBufPass, sLenPass, sFocPass, "Choose a password", true);

    DrawText("Confirm Password", cx + 24, cy + 224, 15, TEXT_MUT);
    drawInput(cx+24, cy+242, cw-48, 44, sBufConf, sLenConf, sFocConf, "Repeat your password", true);

    if (!sAuthErr.empty())
        DrawText(sAuthErr.c_str(), cx + 24, cy + 302, 14, ACCENT_RED);

    if (drawBtn(cx+24, cy+330, cw-48, 48, "Create Account", ACCENT_GREEN, TEXT_DARK, 19)) {
        string u(sBufUser,sLenUser), p(sBufPass,sLenPass), c(sBufConf,sLenConf);
        if      (u.size()<3)              sAuthErr = "Username must be at least 3 characters.";
        else if (p.size()<4)              sAuthErr = "Password must be at least 4 characters.";
        else if (p != c)                  sAuthErr = "Passwords do not match.";
        else if (!registerAccount(u, p))  sAuthErr = "Username already taken.";
        else { loginAccount(u,p); currentScreen=SCREEN_MAIN_MENU; sAuthErr=""; }
    }

    if (drawBtn(cx+24, cy+392, cw-48, 38, "Back to Login", BG_CARD2, TEXT_MUT, 15)) {
        currentScreen=SCREEN_LOGIN; sAuthErr="";
        memset(sBufUser,0,64); sLenUser=0;
        memset(sBufPass,0,64); sLenPass=0;
        memset(sBufConf,0,64); sLenConf=0;
        sFocUser=true; sFocPass=false; sFocConf=false;
    }

    // Tab cycles 3 fields
    if (IsKeyPressed(KEY_TAB)) {
        if (sFocUser)      { sFocUser=false; sFocPass=true;  sFocConf=false; }
        else if (sFocPass) { sFocUser=false; sFocPass=false; sFocConf=true;  }
        else               { sFocUser=true;  sFocPass=false; sFocConf=false; }
    }
    Vector2 m = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        sFocUser = CheckCollisionPointRec(m, {(float)(cx+24),(float)(cy+86), (float)(cw-48),44});
        sFocPass = CheckCollisionPointRec(m, {(float)(cx+24),(float)(cy+164),(float)(cw-48),44});
        sFocConf = CheckCollisionPointRec(m, {(float)(cx+24),(float)(cy+242),(float)(cw-48),44});
    }
}

// ============================================================
//  SCREEN: MAIN MENU
// ============================================================
static void drawMainMenu() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    // ── Header bar ──────────────────────────────────────────
    int hdrH = 72;
    DrawRectangle(0, 0, sw, hdrH, BG_CARD);          // Raylib: header bg
    DrawRectangle(0, hdrH, sw, 2, BORDER);            // Raylib: divider

    DrawText("ELEVATE", 28, 16, 34, ACCENT_BLUE);
    DrawText("Electronic School", 28, 52, 15, TEXT_MUT);

    string uinfo = "Logged in as: " + getCurrentUsername();
    if (isCurrentUserAdmin()) uinfo += "  [Admin]";
    int uw = MeasureText(uinfo.c_str(), 16);
    DrawText(uinfo.c_str(), sw - uw - 24, 28, 16, TEXT_MUT);

    // ── Subtitle ────────────────────────────────────────────
    drawCentered("What would you like to do?", hdrH + 20, 20, TEXT_MUT);

    // ── 2x2 card grid — fully relative to screen size ───────
    int pad   = 28;
    int gap   = 18;
    int cw    = (sw - pad*2 - gap) / 2;   // card width
    int ch    = (sh - hdrH - 72 - gap*3) / 2; // card height
    int row1Y = hdrH + 62;
    int row2Y = row1Y + ch + gap;
    int col1X = pad;
    int col2X = pad + cw + gap;

    // Helper lambda to draw one menu card
    auto menuCard = [&](int x, int y, int w, int h,
                        const char* title, Color tcol,
                        const char* sub1, const char* sub2,
                        AppScreen target, bool extraLogic = false) {
        drawCard(x, y, w, h);
        // Colored top accent line
        DrawRectangle(x, y, w, 5, tcol);              // Raylib: accent strip
        DrawText(title, x + 20, y + 18, 24, tcol);
        DrawText(sub1,  x + 20, y + 54, 16, TEXT_MUT);
        DrawText(sub2,  x + 20, y + 76, 15, TEXT_MUT);
        Rectangle r = {(float)x,(float)y,(float)w,(float)h};
        if (CheckCollisionPointRec(GetMousePosition(), r) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!extraLogic) currentScreen = target;
        }
        return CheckCollisionPointRec(GetMousePosition(), r) &&
               IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    };

    // Study Mode
    menuCard(col1X, row1Y, cw, ch,
             "Study Mode", ACCENT_BLUE,
             "Browse lessons & review answers",
             "3 categories  |  30 questions",
             SCREEN_STUDY);

    // Take a Test
    if (menuCard(col2X, row1Y, cw, ch,
                 "Take a Test", ACCENT_GREEN,
                 "20 random questions",
                 "Graded on Bulgarian scale 2-6",
                 SCREEN_TEST, true)) {
        generateTest(testIndices);
        activeResult       = createNewResult(getCurrentUsername());
        currentQuestion    = 0;
        showAnswerFeedback = false;
        currentScreen      = SCREEN_TEST;
    }

    // Statistics
    {
        string si = to_string(totalResults) + " result(s) recorded";
        drawCard(col1X, row2Y, cw, ch);
        DrawRectangle(col1X, row2Y, cw, 5, ACCENT_PURP);
        DrawText("Statistics",        col1X+20, row2Y+18, 24, ACCENT_PURP);
        DrawText("View scores & rankings", col1X+20, row2Y+54, 16, TEXT_MUT);
        DrawText(si.c_str(),          col1X+20, row2Y+76, 15, TEXT_MUT);
        Rectangle r={(float)col1X,(float)row2Y,(float)cw,(float)ch};
        if (CheckCollisionPointRec(GetMousePosition(),r) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sScrollStats=0; currentScreen=SCREEN_STATISTICS;
        }
    }

    // About
    menuCard(col2X, row2Y, cw, ch,
             "About", ACCENT_ORG,
             "Grading scale & project info",
             "C++ Programming Basics",
             SCREEN_ABOUT);

    // ── Logout button ────────────────────────────────────────
    int btnW=180, btnH=40;
    if (drawBtn(sw/2-btnW/2, sh-btnH-14, btnW, btnH, "Logout", BG_CARD2, TEXT_MUT, 17)) {
        logoutAccount();
        currentScreen = SCREEN_LOGIN;
        memset(sBufUser,0,64); sLenUser=0;
        memset(sBufPass,0,64); sLenPass=0;
        sFocUser=true; sFocPass=false;
    }
}

// ============================================================
//  SCREEN: STUDY — category picker
// ============================================================
static void drawStudy() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("Study Mode", 28, 18, 26, TEXT_PRI);
    DrawText("Choose a category to review", 28, 48, 14, TEXT_MUT);
    if (drawBtn(sw-120, 17, 96, 38, "< Back", BG_CARD2, TEXT_MUT, 15))
        currentScreen = SCREEN_MAIN_MENU;

    int cw = sw - 80;
    int cx = 40;
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        int cy = 90 + i * 140;
        int ch = 120;
        drawCard(cx, cy, cw, ch);
        DrawRectangle(cx, cy, cw, 5, ACCENT_BLUE);
        DrawText(categoryNames[i].c_str(), cx+20, cy+18, 22, ACCENT_BLUE);
        string info = to_string(categoryCount[i]) + " questions  |  "
                    + to_string(categoryPoints[i]) + " pt each";
        DrawText(info.c_str(), cx+20, cy+54, 15, TEXT_MUT);
        if (drawBtn(cw-100, cy+38, 120, 40, "Study ->", ACCENT_BLUE, TEXT_DARK, 16)) {
            selectedCategory = i;
            sScrollLesson    = 0;
            currentScreen    = SCREEN_STUDY_LESSON;
        }
    }
    (void)sh;
}

// ============================================================
//  SCREEN: STUDY LESSON — questions with answers
// ============================================================
static void drawStudyLesson() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText(categoryNames[selectedCategory].c_str(), 28, 18, 22, ACCENT_BLUE);
    DrawText("Correct answer marked with  *", 28, 48, 13, TEXT_MUT);
    if (drawBtn(sw-120, 17, 96, 38, "< Back", BG_CARD2, TEXT_MUT, 15)) {
        currentScreen = SCREEN_STUDY;
        sScrollLesson = 0;
    }

    // Scroll with mouse wheel
    sScrollLesson -= GetMouseWheelMove() * 32.0f;  // Raylib: wheel
    if (sScrollLesson < 0) sScrollLesson = 0;

    BeginScissorMode(0, 70, sw, sh - 70);          // Raylib: clip

    int start = categoryStart[selectedCategory];
    int count = categoryCount[selectedCategory];
    int y     = 80 - (int)sScrollLesson;

    for (int i = 0; i < count; i++) {
        Question& q = questionBank[start + i];
        int cardH   = 52 + 4 * 30 + 10;
        int cx      = 28;
        int cw      = sw - 56;
        drawCard(cx, y, cw, cardH);

        string qLabel = "Q" + to_string(i+1) + ": " + q.text;
        DrawText(qLabel.c_str(), cx+16, y+10, 16, TEXT_PRI);

        for (int opt = 0; opt < 4; opt++) {
            bool correct = (opt == q.correctIndex);
            string line  = string(1,(char)('A'+opt)) + ".  " + q.options[opt];
            if (correct) line += "   *";
            DrawText(line.c_str(), cx+32, y+42+opt*30, 14,
                     correct ? ACCENT_GREEN : TEXT_MUT);
        }
        y += cardH + 10;
    }

    EndScissorMode();                               // Raylib: end clip
}

// ============================================================
//  SCREEN: TEST
// ============================================================
static void drawTest() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    // All questions answered — finalize and go to results
    if (currentQuestion >= TEST_SIZE) {
        finalizeResult(activeResult);
        storeResult(activeResult);
        currentScreen = SCREEN_TEST_RESULT;
        return;
    }

    Question& q = questionBank[testIndices[currentQuestion]];

    // ── Header ───────────────────────────────────────────────
    DrawRectangle(0, 0, sw, 76, BG_CARD);
    string prog = "Question  " + to_string(currentQuestion+1) + " / " + to_string(TEST_SIZE);
    DrawText(prog.c_str(), 28, 14, 20, TEXT_PRI);
    DrawText(q.category.c_str(), 28, 44, 14, TEXT_MUT);
    string pts = to_string(q.points) + " pt";
    DrawText(pts.c_str(), sw-80, 26, 18, ACCENT_ORG);
    string sc = "Score: " + to_string(activeResult.score);
    int scw = MeasureText(sc.c_str(), 16);
    DrawText(sc.c_str(), sw-scw-24, 50, 16, ACCENT_GREEN);

    // Progress bar
    drawBar(0, 68, sw, 8, (float)currentQuestion / TEST_SIZE, ACCENT_BLUE);

    // ── Question card ─────────────────────────────────────────
    int qx = 40, qw = sw - 80;
    drawCard(qx, 96, qw, 80);
    DrawText(q.text.c_str(), qx+20, 118, 18, TEXT_PRI);

    // ── Answer buttons ────────────────────────────────────────
    int startY = 198;
    int btnH   = 62;
    int gap    = 12;

    // Handle feedback timer using delta time
    if (showAnswerFeedback) {
        feedbackTimer -= GetFrameTime();             // Raylib: delta time
        if (feedbackTimer <= 0) {
            showAnswerFeedback = false;
            currentQuestion++;
        }
    }

    for (int i = 0; i < 4; i++) {
        int by = startY + i * (btnH + gap);
        Color bg = BG_CARD, tc = TEXT_PRI;

        if (showAnswerFeedback) {
            if (i == q.correctIndex)       { bg = ACCENT_GREEN; tc = TEXT_DARK; }
            else if (i == lastAnswerIndex) { bg = ACCENT_RED;   tc = TEXT_DARK; }
        }

        Rectangle r = {(float)qx,(float)by,(float)qw,(float)btnH};
        DrawRectangleRec(r, bg);
        DrawRectangleLinesEx(r, 1.5f, BORDER);

        string label = string(1,(char)('A'+i)) + ".   " + q.options[i];
        DrawText(label.c_str(), qx+20, by+(btnH-18)/2, 18, tc);

        // Only register clicks when not showing feedback
        if (!showAnswerFeedback &&
            CheckCollisionPointRec(GetMousePosition(), r) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lastAnswerIndex   = i;
            lastAnswerCorrect = evaluateAnswer(activeResult, testIndices[currentQuestion], i);
            showAnswerFeedback = true;
            feedbackTimer      = 1.0f;
        }
    }

    // Feedback message at bottom
    if (showAnswerFeedback) {
        const char* msg = lastAnswerCorrect ? "Correct!" : "Wrong!";
        drawCentered(msg, sh - 52, 26, lastAnswerCorrect ? ACCENT_GREEN : ACCENT_RED);
    }
    (void)sh;
}

// ============================================================
//  SCREEN: TEST RESULT
// ============================================================
static void drawTestResult() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    drawCentered("Test Complete!", sh/2 - 260, 34, TEXT_PRI);

    Color gc = gradeCol(activeResult.grade);
    string gradeStr = "Grade: " + to_string(activeResult.grade);
    drawCentered(gradeStr.c_str(), sh/2 - 200, 56, gc);

    string scoreStr = to_string(activeResult.score) + " / "
                    + to_string(activeResult.maxScore) + " points";
    drawCentered(scoreStr.c_str(), sh/2 - 128, 22, TEXT_PRI);

    string pctStr = to_string((int)activeResult.percentage) + "%";
    drawCentered(pctStr.c_str(), sh/2 - 96, 18, TEXT_MUT);

    // Category breakdown card
    int cw = 580, cx = (sw-cw)/2;
    int cy = sh/2 - 60;
    drawCard(cx, cy, cw, 160);
    DrawText("Breakdown by category:", cx+20, cy+14, 16, TEXT_MUT);

    for (int c = 0; c < NUM_CATEGORIES; c++) {
        int y   = cy + 44 + c * 36;
        double pct = (activeResult.categoryTotal[c] > 0)
            ? (double)activeResult.categoryCorrect[c] / activeResult.categoryTotal[c] * 100.0
            : 0.0;
        DrawText(categoryNames[c].c_str(), cx+20, y, 15, TEXT_PRI);
        string cs = to_string(activeResult.categoryCorrect[c])
                  + "/" + to_string(activeResult.categoryTotal[c]);
        DrawText(cs.c_str(), cx+360, y, 15, TEXT_MUT);
        drawBar(cx+420, y+2, 140, 16, (float)(pct/100.0), ACCENT_BLUE);
    }

    int btnY = sh/2 + 120;
    if (drawBtn(sw/2-220, btnY, 200, 48, "Take Another Test", ACCENT_BLUE, TEXT_DARK, 16)) {
        generateTest(testIndices);
        activeResult       = createNewResult(getCurrentUsername());
        currentQuestion    = 0;
        showAnswerFeedback = false;
        currentScreen      = SCREEN_TEST;
    }
    if (drawBtn(sw/2+20, btnY, 200, 48, "Main Menu", BG_CARD2, TEXT_PRI, 16))
        currentScreen = SCREEN_MAIN_MENU;
    (void)sh;
}

// ============================================================
//  SCREEN: STATISTICS
// ============================================================
static void drawStatistics() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("Statistics", 28, 18, 26, TEXT_PRI);
    DrawText("All student results", 28, 48, 14, TEXT_MUT);
    if (drawBtn(sw-120, 17, 96, 38, "< Back", BG_CARD2, TEXT_MUT, 15))
        currentScreen = SCREEN_MAIN_MENU;

    if (totalResults == 0) {
        drawCentered("No results yet. Take a test first!", sh/2, 20, TEXT_MUT);
        return;
    }

    double avg = getAverageScore();
    int    hi  = getHighestScoreIndex();
    int    lo  = getLowestScoreIndex();

    // Summary row
    int scw = (sw - 72) / 3;
    struct { const char* label; string val; Color col; } sums[3] = {
        { "Average Score",  to_string((int)avg) + "%",                           ACCENT_BLUE  },
        { "Highest Score",  results[hi].studentName + "  " + to_string((int)results[hi].percentage) + "%", ACCENT_GREEN },
        { "Lowest Score",   results[lo].studentName + "  " + to_string((int)results[lo].percentage) + "%", ACCENT_RED   },
    };
    for (int i = 0; i < 3; i++) {
        int x = 24 + i * (scw + 12);
        drawCard(x, 86, scw, 84);
        DrawText(sums[i].label,       x+16, 98,  14, TEXT_MUT);
        DrawText(sums[i].val.c_str(), x+16, 118, 20, sums[i].col);
    }

    // Category performance
    drawCard(24, 182, sw-48, 90);
    DrawText("Category Performance", 44, 194, 15, TEXT_MUT);
    for (int c = 0; c < NUM_CATEGORIES; c++) {
        int cx = 44 + c * ((sw-96)/NUM_CATEGORIES);
        double pct = getCategoryPassRate(c);
        DrawText(categoryNames[c].c_str(), cx, 218, 13, TEXT_PRI);
        drawBar(cx, 244, 200, 16, (float)(pct/100.0), ACCENT_PURP);
        string ps = to_string((int)pct) + "%";
        DrawText(ps.c_str(), cx+208, 244, 13, TEXT_MUT);
    }

    // Scrollable results table
    sScrollStats -= GetMouseWheelMove() * 32.0f;    // Raylib: wheel scroll
    if (sScrollStats < 0) sScrollStats = 0;

    int tableTop = 284;
    int rowH     = 40;
    BeginScissorMode(0, tableTop, sw, sh-tableTop); // Raylib: clip

    // Header row
    int hy = tableTop - (int)sScrollStats;
    DrawRectangle(24, hy, sw-48, rowH, BG_CARD2);
    DrawText("Name",    50,         hy+12, 14, TEXT_MUT);
    DrawText("Score",   sw/2-100,   hy+12, 14, TEXT_MUT);
    DrawText("Percent", sw/2+20,    hy+12, 14, TEXT_MUT);
    DrawText("Grade",   sw/2+140,   hy+12, 14, TEXT_MUT);

    for (int i = 0; i < totalResults; i++) {
        int ry = tableTop - (int)sScrollStats + (i+1)*rowH;
        DrawRectangle(24, ry, sw-48, rowH, i%2==0 ? BG_CARD : BG_DARK);
        DrawText(results[i].studentName.c_str(), 50, ry+12, 15, TEXT_PRI);
        string sc2 = to_string(results[i].score)+"/"+to_string(results[i].maxScore);
        DrawText(sc2.c_str(), sw/2-100, ry+12, 15, TEXT_MUT);
        string pc2 = to_string((int)results[i].percentage)+"%";
        DrawText(pc2.c_str(), sw/2+20,  ry+12, 15, TEXT_PRI);
        string gr2 = to_string(results[i].grade);
        DrawText(gr2.c_str(), sw/2+140, ry+12, 16, gradeCol(results[i].grade));
    }

    EndScissorMode();                               // Raylib: end clip
}

// ============================================================
//  SCREEN: ABOUT
// ============================================================
static void drawAbout() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ClearBackground(BG_DARK);

    DrawRectangle(0, 0, sw, 70, BG_CARD);
    DrawText("About Elevate", 28, 18, 26, TEXT_PRI);
    DrawText("Project info and grading scale", 28, 48, 14, TEXT_MUT);
    if (drawBtn(sw-120, 17, 96, 38, "< Back", BG_CARD2, TEXT_MUT, 15))
        currentScreen = SCREEN_MAIN_MENU;

    int cw = sw - 80, cx = 40;

    // Project info card
    drawCard(cx, 86, cw, 130);
    DrawRectangle(cx, 86, cw, 5, ACCENT_BLUE);
    DrawText("Project Info",                            cx+20, 100,  18, ACCENT_BLUE);
    DrawText("Subject: C++ Programming Basics",        cx+20, 130,  16, TEXT_PRI);
    DrawText("Grade: IX  |  Year: 2025/2026",          cx+20, 154,  16, TEXT_PRI);
    string bk = "Question bank: " + to_string(TOTAL_QUESTIONS)
              + "  |  Test size: " + to_string(TEST_SIZE) + " questions";
    DrawText(bk.c_str(),                               cx+20, 178,  15, TEXT_MUT);

    // Grading scale card
    drawCard(cx, 232, cw, 220);
    DrawRectangle(cx, 232, cw, 5, ACCENT_ORG);
    DrawText("Grading Scale", cx+20, 246, 18, ACCENT_ORG);

    const char* grades[]  = { "Grade 6", "Grade 5", "Grade 4", "Grade 3", "Grade 2" };
    const char* thresh[]  = { ">= 90%",  ">= 75%",  ">= 62%",  ">= 50%",  "< 50%"  };
    Color        gcols[]  = { ACCENT_GREEN, ACCENT_BLUE, ACCENT_ORG, ACCENT_ORG, ACCENT_RED };
    for (int i = 0; i < 5; i++) {
        int gy = 276 + i * 32;
        DrawRectangle(cx+20, gy, 10, 22, gcols[i]);
        DrawText(grades[i], cx+42,  gy, 16, TEXT_PRI);
        DrawText(thresh[i], cx+160, gy, 16, TEXT_MUT);
    }

    // Password hashing card
    drawCard(cx, 468, cw, 80);
    DrawRectangle(cx, 468, cw, 5, ACCENT_PURP);
    DrawText("Password Hashing",                                    cx+20, 482, 16, ACCENT_PURP);
    DrawText("Custom XOR + hex string algorithm — plain text is never stored.", cx+20, 508, 14, TEXT_MUT);

    (void)sh;
}

// ============================================================
//  PUBLIC API
// ============================================================

void initRenderer() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);           // Raylib: allow resize
    InitWindow(1440, 900, "ELEVATE - Electronic School"); // Raylib: open window
    SetWindowMinSize(1100, 680);                     // Raylib: minimum size
    SetTargetFPS(60);                                // Raylib: 60 fps cap
}

void closeRenderer() {
    CloseWindow();                                   // Raylib: destroy window
}

void drawFrame() {
    BeginDrawing();                                  // Raylib: start frame

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

    EndDrawing();                                    // Raylib: end frame, swap buffers
}

void handleInput() {
    // Input is handled inside each draw function since Raylib
    // input queries are valid between BeginDrawing/EndDrawing
}
