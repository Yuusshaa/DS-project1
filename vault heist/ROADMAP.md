# Vault Heist work plan

Deadline: November 20, 2026

This plan uses one integration boundary: `model/GameModel.h`. Hussain builds
the GUI against `MockGameModel`; Yusha implements the real model behind the
same interface. Shared headers change only when both people agree.

## Rules for working in parallel

1. Keep model algorithms in `model/` and Raylib code in `gui/`.
2. The GUI reads `ModelView` and `EventLog`; it never changes tree links or
   performs tree/heap algorithms.
3. Do not edit `GameModel.h`, `event.h`, `eventlog.h`, or `dynamicarray.h`
   independently. Discuss changes first and update the contract in the same
   commit.
4. Build and integrate one feature group each day. Avoid waiting for a final
   merge day.
5. Use the mock only for GUI development. Before submission, the app must use
   Yusha's real model.

## Nov 13: agree, configure, and make the first screen

**Together (1-2 hours):** confirm the Visual Studio project opens, Raylib is
available through vcpkg, event semantics, and exact `GameModel` method
signatures. Keep the clean MSBuild steps in the README.

**Yusha (2-4 hours):** review `GameModel.h`; make sure the model can supply a
flat tree/heap view and append the agreed events. Keep Raylib out of model
headers.

**Hussain (3-5 hours):** build the Raylib screen using `MockGameModel`; show a
small tree, traversal buttons, search, status text, and event log.

**Integration gate:** the GUI builds against the mock and needs no model node
pointers.

## Nov 14: finish BinaryTree display and BST operations

**Yusha (5-7 hours):** finish BinaryTree API and events, then BST search,
insert, delete cases, range query, successor/predecessor, and duplicate
handling. Add focused model tests as each operation lands.

**Hussain (4-6 hours):** implement tree layout, edges/nodes, selection/hover,
traversal and search animation, and the level HUD. Keep the mock responses
representative of Yusha's event batches.

**Integration gate:** traversal and BST search run through the real facade;
the displayed visit/comparison count agrees with the event log.

## Nov 15: AVL and rotation display

**Yusha (5-7 hours):** implement AVL insertion/deletion, height/size updates,
LL/RR/LR/RL rotations, `kth`, and `rank`. Verify each rotation and metadata.

**Hussain (4-6 hours):** add height, balance, and subtree-size labels; animate
the agreed rotation event from old layout to new layout; add the rotation
demonstration level flow.

**Integration gate:** all four rotation cases appear correctly in the GUI.

## Nov 16: heap model and synchronized views

**Yusha (5-7 hours):** implement heap build, sift-up/down, heapify, extract-max,
heap sort, heap validation, and heap events.

**Hussain (4-6 hours):** add the heap array view, active/sorted regions, swap
highlighting, heap-size display, and extraction/heap-sort animation.

**Integration gate:** tree and array views show the same heap after every
swap and extraction.

## Nov 17: level flow, scoring, and validation

**Yusha (3-5 hours):** finish validators, fix model edge cases, and expose
copy/restore or serialization needed by reset and undo.

**Hussain (5-7 hours):** finish reusable level loading, win/lose/retry/next
flow, alarm, penalties, score, stars, and result screen. Add the campaign and
sandbox flows that fit the approved submission scope.

**Integration gate:** complete at least one representative level from each
act with the real model.

## Nov 18: level coverage, save/undo, and benchmarks

**Yusha (4-6 hours):** complete missing model tests, save/load support, and
benchmark measurements for BST/AVL/heap operations.

**Hussain (4-6 hours):** populate the required level data, wire reset/undo,
and capture the GUI and rotation/heap screenshots with Yusha.

**Integration gate:** use the real model for the submitted demo; remove any
mock-only behavior from the final run path.

## Nov 19: documentation and clean handoff

**Together (4-6 hours):** prepare the report, README/run instructions,
controls, level-authoring guide, benchmark and complexity sections, and final
screenshots. Build from a clean copy and create the required ZIP.

**Integration gate:** both partners can explain the model/GUI boundary and
demonstrate the operations they submit.

## Nov 20: final submission

Allow time for one final clean build, ZIP inspection, and Google Classroom
submission before 11:59 PM. Do not schedule new features for this day.

## Scope warning

The assignment describes 20 campaign levels plus endless mode, sandbox,
leaderboard, undo/replay, save/load, tests, benchmarks, and an 8-12 page
report. Seven days is very tight. Keep the model and integration gates above
first; if time is short, agree with the instructor/TA which optional polish
can be simplified. Do not silently omit a written deliverable.
