# Vault Heist

Vault Heist is a C++ data structures learning game built with Raylib. It
visualizes binary trees, BSTs, AVL trees, Max Heaps, and Heap Sort.

## Current status

The GUI prototype draws a sample BST and plays traversal/search events from
`MockGameModel`. The mock is temporary. Yusha's real model must implement
`model/GameModel.h` before it is used for the final demonstration.

## Build setup

- Visual Studio Community 2026
- x64 platform
- C++20
- Raylib 6.2.0 from the existing vcpkg installation
- No CMake

The project currently expects Raylib here:

```text
E:\vcpkg\installed\x64-windows
```

If Raylib is installed elsewhere, edit `RaylibRoot` in `VaultHeist.vcxproj`.

## Build and run

1. Open `VaultHeist.slnx` in Visual Studio.
2. Select **Debug** and **x64** in the toolbar.
3. Choose **Build > Build Solution** or press **Ctrl+Shift+B**.
4. Press **F5** to run with the debugger, or **Ctrl+F5** to run without it.

The project copies the Raylib runtime DLLs beside the executable after a
successful build.

If a command-line build reports `LNK1101` for `MSPDB140.DLL`, open an x64
Visual Studio Developer Command Prompt in this folder and run:

```text
msbuild VaultHeist.vcxproj /t:Build /p:Configuration=Debug /p:Platform=x64 /p:PreferredToolArchitecture=x64
```

## Controls

- Click **Preorder**, **Inorder**, **Postorder**, or **Level order** to play a
  traversal event batch.
- Click **Search 60** to show a sample BST search path.
- Click **Reset demo** to restore the sample tree.
- Close the window to exit.

## Course restrictions

The project implements its required data structures itself. It does not use
STL containers, enums, or global variables. `std::string` and Raylib are used
for text and GUI work. AVL leaf height is 1; an empty child has height 0.

## Adding levels

Level format and authoring instructions will be added when the level data
loader is implemented.
