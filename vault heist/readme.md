# Vault Heist

Vault Heist is a C++ data-structures learning project. Its model implements
binary trees, binary search trees, AVL trees, max heaps, and heap sort. The
Raylib GUI currently remains a mock prototype.

## Project decisions

- Target IDE/build: Visual Studio Community 2026, x64, MSBuild, C++20.
- CMake is not used; see [DECISIONS.md](./DECISIONS.md).
- Raylib 6.2.0 is expected under `E:\vcpkg\installed\x64-windows`.
- The model and tests use C++11-compatible code.
- The project implements its own data structures and does not use STL
  containers, enums, or global variables.

## Build and run the tests

Open a PowerShell terminal in the `vault heist` project root. With MinGW
`g++` available on `PATH`, compile and run the tests:

```powershell
g++ -std=c++11 -Wall -Wextra -I model -I test test/binarytreetset.cpp -o test/binarytreetset-run.exe
.\test\binarytreetset-run.exe

g++ -std=c++11 -Wall -Wextra -I model -I test test/test_bst.cpp -o test/test-bst-run.exe
.\test\test-bst-run.exe

g++ -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp -o test/test-gamemodel-run.exe
.\test\test-gamemodel-run.exe
```

The test files print a `Passed: ... Failed: ...` summary.

## Build and run the benchmarks

Compile and run the benchmark suite from the project root:

```powershell
g++ -std=c++11 -O2 -Wall -Wextra -I model benchmarks/bench.cpp -o benchmarks/bench.exe
.\benchmarks\bench.exe
```

To save a fresh results file using PowerShell:

```powershell
.\benchmarks\bench.exe | Out-File -Encoding utf8 docs/benchmark_results.txt
```

Recorded results are in [docs/benchmark_results.txt](./docs/benchmark_results.txt).

## Run the game (TODO: GUI owner)

The GUI prototype currently uses `MockGameModel`. The Visual Studio solution
and project files are not present in this checkout, so the GUI owner still
needs to add the game target and connect the real model.

Planned controls: **Space** play/pause, **Left/Right** step, **Z** undo,
**H** hint, **Esc** menu, mouse wheel to zoom, and drag to pan. These controls
are not implemented in the current mock GUI.

## Folder layout

```text
model/        Data structures, validation, serialization, and GameModel
gui/          Raylib application and temporary mock model
test/         Model and integration tests
benchmarks/   Section 10 benchmark program
docs/         Contract, reports, and benchmark results
data/levels/  Planned JSON level data (not present yet)
```

## Submission

Name the submission zip:

```text
25I-XXXX_25I-XXXX_Section.zip
```

Replace each `XXXX` with the corresponding student identifier.
