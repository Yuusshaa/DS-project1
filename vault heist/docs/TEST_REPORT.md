# Test report

Tests were compiled with MinGW GCC 6.3.0 using C++11. Run the commands from
the `vault heist` project root in PowerShell; executables are written under
`$env:TEMP` to avoid replacing project files.

| Test file | Coverage | Checks | Result |
|---|---|---:|---|
| `test/binarytreetset.cpp` | Ownership, recursive/iterative traversals, height, diameter, paths, LCA, mirror/symmetry, reconstruction, and copy. | 99 | Passed: 99  Failed: 0 |
| `test/test_bst.cpp` | BST insert/search/delete, order, parent links, successor/predecessor, range queries, order statistics, LCA, alarm events, and degenerate input. | 107 | Passed: 107  Failed: 0 |
| `test/test_avl.cpp` | AVL rotations, deletion rebalance, BST operations, metadata, order statistics, range counts, and sorted-input balancing. | Pending | Run with `docs/RUN_ALL_TESTS.md`; enter actual totals after the run. |
| `test/test_heap.cpp` | Heap construction, sift, insert/extract, sorted region, counters/events, and Heap Sort edge cases. | Pending | Run with `docs/RUN_ALL_TESTS.md`; enter actual totals after the run. |
| `test/test_gamemodel.cpp` | Scenario loading, view counts, search alarm count, duplicate insertion, AVL validation, heap sort, wrong-mode handling, snapshot round trips, and malformed snapshot errors. | 56 | Passed: 56  Failed: 0 |

## Compile and run commands

```powershell
Push-Location 'vault heist'

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/binarytreetset.cpp -o "$env:TEMP\binarytreetset-doc.exe"
& "$env:TEMP\binarytreetset-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_bst.cpp -o "$env:TEMP\test-bst-doc.exe"
& "$env:TEMP\test-bst-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_avl.cpp -o "$env:TEMP\test-avl-doc.exe"
& "$env:TEMP\test-avl-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_heap.cpp -o "$env:TEMP\test-heap-doc.exe"
& "$env:TEMP\test-heap-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp gui/LevelManager.cpp -o "$env:TEMP\test-gamemodel-doc.exe"
& "$env:TEMP\test-gamemodel-doc.exe"

Pop-Location
```

The game-model test also passed with the requested output path:

```powershell
g++ -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp -o test/test_gamemodel.exe
.\test\test_gamemodel.exe
```

Paste a future run's final line here: **Passed: ___  Failed: ___**

## Assignment Section 9.1 coverage mapping

| Section 9.1 topic represented by current tests | Test file |
|---|---|
| Binary-tree operations and reconstruction | `test/binarytreetset.cpp` |
| BST operations, ordering, and alarm events | `test/test_bst.cpp` |
| AVL rotations, order-statistic operations, metadata, and deletion | `test/test_avl.cpp` |
| Heap property, construction, extraction, and sorting | `test/test_heap.cpp` |
| Model scenarios, views, actions, snapshots, and campaign loading | `test/test_gamemodel.cpp` |

The GUI checklist in Section 9.2 is manual; follow
`docs/GUI_MANUAL_TEST_STEPS.md` and record results separately. Act 3 levels
3.4 and 3.5 are currently unimplemented and cannot pass their campaign GUI
checks until those features are added.

Warnings about backslashes in tree-diagram comments are from existing test and
AVL source comments; they do not prevent the tests from passing.
