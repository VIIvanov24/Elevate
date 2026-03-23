# ELEVATE — Electronic School

> Interactive C++ Electronic School for C++ Programming Basics  
> IX Grade Project | PGKPI Burgas | 2025/2026

---

## About

Elevate is a C++ desktop application built with **Raylib** that functions as an interactive
Electronic School. It covers C++ Programming Basics through study mode, randomized tests,
and detailed statistics.

---

## Features

- **Account System** — Register & login with password hashing (XOR + hex, `string` class)
- **Study Mode** — Browse all 30 questions with correct answers per category
- **Test Mode** — 20 randomly generated questions, graded on Bulgarian scale 2–6
- **Statistics** — Average score, best/worst student, category performance
- **Raylib UI** — Full graphical interface with 9 screens

---

## C++ Concepts Used

| Concept | Where |
|---------|-------|
| Functions | Separate input / output / processing functions throughout |
| Recursion | Fisher-Yates shuffle in `utils.cpp` |
| Strings (`std::string`) | Account system, display, hashing, category matching |
| Structs | `Question`, `StudentResult` in `globals.h` |
| Arrays | `questionBank[30]`, `results[100]`, `categoryCorrect[3]` |
| Bitwise (XOR) | Password hashing in `utils.cpp` — `charVal ^ (i + 7)` |

---

## Project Structure

```
Elevate_Final_Commit/
├── elevate.sln              ← Open this in Visual Studio
├── .gitignore
├── README.md
├── docs/
│   ├── Elevate_Documentation.docx
│   └── Elevate_Presentation.pptx
├── raylib/
│   ├── include/             ← raylib.h, raymath.h, rlgl.h
│   └── lib/                 ← raylibdll.lib, raylib.dll
└── elevate/
    ├── elevate.vcxproj
    ├── main.cpp
    ├── globals.h
    ├── questions.h / .cpp
    ├── accounts.h / .cpp
    ├── utils.h / .cpp
    ├── test.h / .cpp
    ├── statistics.h / .cpp
    └── renderer.h / .cpp     ← all Raylib code (commented)
```

---

## How to Build

1. Open `elevate.sln` in **Visual Studio 2022**
2. Select **Debug | x64**
3. **Build → Rebuild Solution**
4. Run with **Ctrl + F5**

> `raylib.dll` is automatically copied next to the `.exe` by a post-build step.

---

## Grading Scale

| Grade | Threshold |
|-------|-----------|
| 6 (Excellent)   | >= 90% |
| 5 (Very Good)   | >= 75% |
| 4 (Good)        | >= 62% |
| 3 (Average)     | >= 50% |
| 2 (Fail)        | < 50%  |
