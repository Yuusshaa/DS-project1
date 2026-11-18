# Run the Full Test Suite

Run these commands in PowerShell from the `vault heist` folder. The executables
are written to your Windows temporary folder so the checked-in `.exe` files
are not overwritten.

```powershell
$testOut = $env:TEMP

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/binarytreetset.cpp -o "$testOut\binarytree-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Binary tree tests did not compile.' }
& "$testOut\binarytree-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Binary tree tests failed.' }

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_bst.cpp -o "$testOut\bst-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'BST tests did not compile.' }
& "$testOut\bst-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'BST tests failed.' }

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_avl.cpp -o "$testOut\avl-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'AVL tests did not compile.' }
& "$testOut\avl-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'AVL tests failed.' }

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_heap.cpp -o "$testOut\heap-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Heap tests did not compile.' }
& "$testOut\heap-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Heap tests failed.' }

& 'C:\MinGW\bin\g++.exe' -std=c++11 -Wall -Wextra -I model -I test test/test_gamemodel.cpp gui/LevelManager.cpp -o "$testOut\gamemodel-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Game model tests did not compile.' }
& "$testOut\gamemodel-tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Game model tests failed.' }
```

The original project report lists its existing compile commands in
`docs/TEST_REPORT.md`. If MinGW is installed somewhere other than
`C:\MinGW\bin\g++.exe`, replace that compiler path with the installed path.

## What each test file covers

- `test/binarytreetset.cpp`: general binary tree creation, traversal,
  reconstruction, path/LCA, mirror, diameter, and copying.
- `test/test_bst.cpp`: BST insertion, search, deletion, ordering, range,
  successor/predecessor, events, and skewed input.
- `test/test_avl.cpp`: AVL balancing, all four insertion rotations, deletion
  rebalancing, inherited BST operations, height/size metadata, rank, kth, and
  range counts.
- `test/test_heap.cpp`: heap indexing, build, sift, insertion, extraction,
  active/sorted regions, events/counters, and Heap Sort against reference
  outputs.
- `test/test_gamemodel.cpp`: model scenarios, actions, views, snapshots, and
  campaign level loading.

After each program finishes, a nonzero **Failed** count means that suite did
not pass. Save each output and enter the totals in `docs/TEST_REPORT.md`.
GUI checks must be performed separately using
`docs/GUI_MANUAL_TEST_STEPS.md`.
