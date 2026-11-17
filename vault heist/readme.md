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

- Select **Campaign**, then start level 1.1.
- Click a **Quiz** button to start that traversal challenge, then click rooms
  in the requested order.
- Click **Search 60** to show a sample BST search path.
- Click **Reset level** to restore the sample tree and counters.
- From the main menu, open **Sandbox** and choose Binary, BST, AVL, or Max Heap.
- Type a seed and choose **Random setup** to create a repeatable setup. The
  displayed seed can be reused to regenerate it. Binary Tree selects a seeded
  sample; BST and AVL generate seeded keys; Max Heap uses seeded legal swaps
  to build a valid heap from its sample values.
- Use the **Read**, **Edit**, **AVL**, and **Heap** operation pages. The
  available operations depend on the selected structure; tree tooltips show
  node IDs for height and LCA queries.
- Enter operation parameters in the two value fields; for range queries enter
  the high value second, and for LCA enter the second node ID. Press Enter to
  repeat the selected operation. **Reset selected structure** restores its
  starting state.
- Use **Escape** to return to the previous menu.
- Close the window to exit.

## Course restrictions

The project implements its required data structures itself. It does not use
STL containers, enums, or global variables. `std::string` and Raylib are used
for text and GUI work. AVL leaf height is 1; an empty child has height 0.

## Adding levels

Level definitions live in `data/levels.json`. See
`data/LEVEL_AUTHORING.md` for the level format and workflow. Add one flat object to its
`levels` array with `act`, `number`, `title`, `concept`, `objective`, and
`implemented`. `scenarioId`, `optimalSteps`, and `alarmCapacity` are optional;
they default to scenario 1, the number of expected visits, and the calculated
alarm limit. Set `implemented` to `true` only after that level's model action
and GUI flow are ready. The campaign unlocks implemented levels in order after
the previous level earns at least one star. Stars and best scores are saved in
`campaign_progress.dat` in the application's working directory.
