# Test report

Tests were compiled with MinGW GCC 6.3.0 using C++11. Run the commands from
the `vault heist` project root in PowerShell; executables are written under
`$env:TEMP` to avoid replacing project files.

| Test file | Coverage | Checks | Result |
|---|---|---:|---|
| `test/binarytreetset.cpp` | Ownership, recursive/iterative traversals, height, diameter, paths, LCA, mirror/symmetry, reconstruction, and copy. | 99 | Passed: 99  Failed: 0 |
| `test/test_bst.cpp` | BST insert/search/delete, order, parent links, successor/predecessor, range queries, order statistics, LCA, alarm events, and degenerate input. | 107 | Passed: 107  Failed: 0 |
| `test/test_gamemodel.cpp` | Scenario loading, view counts, search alarm count, duplicate insertion, AVL validation, heap sort, wrong-mode handling, snapshot round trips, and malformed snapshot errors. | 56 | Passed: 56  Failed: 0 |

## Compile and run commands

```powershell
Push-Location 'vault heist'

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/binarytreetset.cpp -o "$env:TEMP\binarytreetset-doc.exe"
& "$env:TEMP\binarytreetset-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_bst.cpp -o "$env:TEMP\test-bst-doc.exe"
& "$env:TEMP\test-bst-doc.exe"

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp -o "$env:TEMP\test-gamemodel-doc.exe"
& "$env:TEMP\test-gamemodel-doc.exe"

Pop-Location
```

The game-model test also passed with the requested output path:

```powershell
g++ -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp -o test/test_gamemodel.exe
.\test\test_gamemodel.exe
```

Paste a future run's final line here: **Passed: ___  Failed: ___**

## Section 9.1 coverage mapping

The assignment specification containing Section 9.1 was not present in the
workspace or supplied contract. The topic-level mapping below is provisional;
exact row-by-row mapping needs the original Section 9.1 text.

| Section 9.1 topic represented by current tests | Test file |
|---|---|
| Binary-tree operations and reconstruction | `test/binarytreetset.cpp` |
| BST operations, ordering, and alarm events | `test/test_bst.cpp` |
| Model scenarios, views, actions, snapshots, AVL validation, and heap sort | `test/test_gamemodel.cpp` |

Warnings about backslashes in tree-diagram comments are from existing test and
AVL source comments; they do not prevent the tests from passing.
