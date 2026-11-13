# Project decisions

These decisions apply to the whole project. If the course instructor gives a
different rule, update this file and both halves of the code together.

## Course restrictions

- Implement required data structures ourselves; do not use STL containers.
- Do not use global variables or enums.
- `std::string` and standard C++ headers are allowed. Raylib is used only for
  window, rendering, input, and related GUI work.
- A leaf has height 1. An empty child has height 0.

## Architecture

- The model does not include Raylib or depend on GUI code.
- The GUI asks the model to perform operations and displays model state and
  events. The GUI does not implement tree or heap algorithms.
- Each player action produces one event batch. The GUI clears the event log
  before requesting the next action.
- Node IDs are unique and stable for the life of a node. A node's `value` is
  its displayed treasure value and its key in the BST and AVL.

## Alarm

- A traversal visit costs one alarm step.
- A comparison in a search or range operation costs one alarm step.
- An interaction must not emit both a visit and a comparison for the same
  step. Incorrect answers and invalid heap actions add their separate
  penalties as described by the assignment.

## GUI

- GUI framework: Raylib.
- IDE/build: Visual Studio Community 2026, x64, MSBuild, C++20; no CMake.
- Raylib is supplied by the existing vcpkg installation under
  `E:\vcpkg\installed\x64-windows`.
