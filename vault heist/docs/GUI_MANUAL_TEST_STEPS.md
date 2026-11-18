# GUI Manual Test Steps

Use this guide to manually check the running game. These checks are not
automated by the C++ model tests.

## Before you start

1. Open `VaultHeist.vcxproj` in Visual Studio and select **Debug** and **x64**.
2. Build and start the game. If using VS Code, follow the build steps in
   `readme.md` and launch the game from the project instructions.
3. Keep a copy of this document or the results table below. For each case,
   record **Pass**, **Fail**, or **Not implemented**, plus a short note.
4. Capture screenshots for each AVL rotation type, heap build, heapify,
   extraction, and the final sorted array. Name them with the test ID.

## Test cases

### GUI-01: Campaign navigation, success, failure, and retry

1. From the main menu, choose **Campaign**.
2. Open each level marked implemented in `data/levels.json`.
3. Read the objective, then complete it using the on-screen controls.
4. Check that the result screen reports completion, steps, penalties, and
   stars or score. Return to the campaign and confirm the next level's unlock
   state is consistent with the result.
5. Restart a level. Deliberately make an incorrect move or answer, if that
   level allows it, and confirm the alarm/penalty changes and failure state.
6. Use **Retry** and confirm the starting tree/array and counters are restored.

**Pass when:** implemented levels can be entered and completed; incorrect
actions and retry reset the expected state. Record any level that cannot be
opened or completed by its title and number.

### GUI-02: Traversal and reconstruction

1. Open Act 1 level 1.1 and click rooms in the requested traversal order.
2. Try one wrong room first and note the alarm or penalty response; retry and
   complete the route correctly.
3. Open Act 1 level 1.2. Drag the room tokens into the tree slots for both
   reconstruction challenges.
4. Submit one deliberately incorrect arrangement, then complete each tree
   correctly.

**Pass when:** incorrect choices are rejected and correct traversal and tree
structures reach the success state. Capture a screenshot of the traversal
event sequence and a completed reconstruction.

### GUI-03: All four AVL rotations

1. Open the Act 3 rotation levels that are currently unlocked.
2. For each available case, identify the unbalanced node and choose the pivot
   and rotation requested by the level.
3. Exercise LL, RR, LR, and RL. Check the resulting tree links and balance
   badges after every rotation.
4. Make one wrong rotation choice where the game permits it; check the stated
   alarm and score penalty, then retry.
5. Save a screenshot of the correctly repaired tree for each rotation type.

**Pass when:** all four cases can be demonstrated, the tree is balanced after
the correct move, and the wrong-choice penalty is applied. If the campaign
does not expose a rotation type, mark that type **Not reachable** and record
the level needed. Do not treat a model-only test as a GUI pass.

### GUI-04: Heap build and heapify

1. Open Act 4 level 4.1 and inspect the initial heap array and tree view.
2. Perform the required parent/child swaps to build a max heap.
3. After each swap, compare the affected array cells with the corresponding
   tree nodes.
4. Open level 4.2 and heapify the violating node.
5. Check the parent is at least as large as each child in the active heap.
6. Capture screenshots of the completed build and heapify states.

**Pass when:** the tree and array represent the same values, legal swaps
update both views, and the active region satisfies the max-heap property.

### GUI-05: Extraction, race, and complete heap sort

1. Open Act 4 level 4.3 and extract maxima until the level completes.
2. After each extraction, confirm the maximum moves to the sorted region and
   the active heap shrinks by one.
3. Confirm the remaining active region still satisfies the heap property.
4. Complete level 4.4 and check the timer and alarm behavior while performing
   heap build and extraction.
5. Complete level 4.5, which includes duplicate and negative values.
6. Confirm the final array is ascending and every input value is present.
7. Capture a screenshot of an extraction in progress and the final sorted
   result.

**Pass when:** the active and sorted regions remain correct throughout all
operations, both views stay synchronized, and the final array is ascending.

### GUI-06: Playback, reset, and state restoration

1. In a level with a visible event log, perform an operation that produces
   several events.
2. Use **Pause**, **Play**, **Left**, and **Right** (or the on-screen playback
   controls) to step backward and forward through the event sequence.
3. Press **R** or use the level's **Reset level** button to reset.
4. In Sandbox, make a structure-changing operation, then use **Undo** and
   **Redo** (or **Ctrl+Z** and **Ctrl+Y**) and compare the tree/heap with the
   before and after states.
5. Return to a campaign level and compare the tree or heap, event highlight,
   alarm, and score after reset.

**Pass when:** playback updates the event highlight correctly and reset
restores the level state; Sandbox undo and redo restore the corresponding
model state and update the visualization.

### GUI-07: Sandbox inputs and navigation

1. From the main menu, open **Sandbox**.
2. Enter a value and run Search, Insert, and Delete; verify visible feedback
   and the resulting tree.
3. Enter lower and upper bounds and run Range Query. Check included keys and
   greyed/skipped branches when shown.
4. Try a duplicate insertion, missing search/delete, empty input, and a range
   with low greater than high. Note the response and confirm the app remains
   usable.
5. Use **Undo**, **Redo**, and **Reset selected structure**; verify each
   restores the expected model state.
6. Press Escape and confirm return to the main menu.

**Pass when:** inputs affect the intended operation, invalid or missing input
does not crash the app, reset restores the demo, and navigation returns to the
expected screen.

### GUI-08: View size and responsiveness

1. Try to load the largest tree or heap the current UI supports. Record the
   number of nodes/elements.
2. Check whether the full visualization and controls are visible and whether
   interactions remain responsive.
3. If the app provides zoom, pan, or fit controls, exercise each one and check
   that nodes/cells remain selectable. If those controls are absent, mark them
   **Not implemented** rather than guessing the controls.
4. For the assignment's 200-node target, record whether a 200-node view can be
   created. If it cannot, record **Not testable: no 200-node fixture/input**.

**Pass when:** the largest available case remains usable without clipped
controls or a frozen window; record observed performance rather than assuming
60 fps.

## Results log

| Test ID | Result (Pass/Fail/Not implemented) | Build / date | Notes or screenshot filename |
|---|---|---|---|
| GUI-01 | | | |
| GUI-02 | | | |
| GUI-03 LL | | | |
| GUI-03 RR | | | |
| GUI-03 LR | | | |
| GUI-03 RL | | | |
| GUI-04 | | | |
| GUI-05 | | | |
| GUI-06 | | | |
| GUI-07 | | | |
| GUI-08 | | | |

## Act 3.4 and 3.5

Both levels are currently marked implemented in `data/levels.json`, so include
them in GUI-01. For 3.4, verify the predicted AVL insertion case and resulting
automatic rebalance. For 3.5, verify the requested order statistic and the
reported node/key. Record a separate pass or fail for each level.
