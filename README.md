# Sudoku Studio

A graphical C++17 Sudoku game for Windows, built with the native Windows API. No game framework or downloaded runtime is required for the packaged executable.

## Play

Download `SudokuStudio.exe` from the repository's Releases page and double-click it. Windows may show a warning because this personal build is unsigned.

- Click a square, then type **1–9** (the numeric keypad works too).
- Use **arrow keys** to move, **Delete / Backspace / 0** to clear.
- Toggle pencil marks with **N** or the Notes button.
- **Ctrl+Z** or Undo reverses a move, note edit, or hint.
- Hint fills the selected incorrect/empty square, or the next unsolved square.
- Check answers reports how many filled cells are incorrect.
- Choose a difficulty, then press New puzzle. Theme switches between light and dark.

Every generated puzzle has exactly one solution. Difficulty controls target clue counts (42, 34, 28); it is not a human-solving difficulty rating. A puzzle may retain extra clues to preserve uniqueness. Conflicting row, column and box entries are rejected. A locally valid entry can still be wrong; Check answers helps with that.

The timer stops when solved. Progress is held only for the current session and is not saved on exit.

## Build on Windows

Install a C++17 compiler such as MSYS2 UCRT64 GCC and put its `bin` directory on PATH. From PowerShell in this folder:

```powershell
./build.ps1
./build/SudokuStudio.exe
```

Or use CMake with Visual Studio's C++ build tools:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The graphical application targets Windows. The puzzle engine and its tests are portable C++17. Tests verify 36 seeded puzzles across all difficulty levels, uniqueness, complete solutions, clue preservation and row/column/box rules.

## Files

- `main.cpp`: native window, rendering, input, game state and controls.
- `engine.h`: randomized generation and solution counting.
- `tests.cpp`: deterministic engine verification.
- `build.ps1`: standalone Windows build and tests.
- `CMakeLists.txt`: alternative compiler-independent build configuration.
