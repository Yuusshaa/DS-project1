# Remaining Test Cases

This checklist supplements the existing model tests. It targets the specific
cases in the assignment's Section 9.1 and GUI checklist that are not clearly
covered by `test/binarytreetset.cpp`, `test/test_bst.cpp`, or
`test/test_gamemodel.cpp`.

Comprehensive runnable suites are in `test/test_avl.cpp` and
`test/test_heap.cpp`. Step-by-step GUI procedures and a results log are in
`docs/GUI_MANUAL_TEST_STEPS.md`. The model-test rows below are covered by
those source files where the corresponding operations are available;
GUI-only checks must be performed manually.

## Model tests to add

| ID | Area | Setup and action | Expected result |
|---|---|---|---|
| AVL-01 | LL insertion rotation | Insert `30, 20, 10` into an empty `AVLTree`. | Root is 20; children are 10 and 30; `lastRotations == 1`; exactly one `EV_ROTATE` with `ROT_LL`; tree is balanced and metadata is correct. |
| AVL-02 | RR insertion rotation | Insert `10, 20, 30`. | Root is 20; children are 10 and 30; one `ROT_RR`; tree is balanced. |
| AVL-03 | LR insertion rotation | Insert `30, 10, 20`. | Root is 20; children are 10 and 30; one `ROT_LR`; tree is balanced. |
| AVL-04 | RL insertion rotation | Insert `10, 30, 20`. | Root is 20; children are 10 and 30; one `ROT_RL`; tree is balanced. |
| AVL-05 | AVL delete with heavy child BF zero | Insert `4, 2, 6, 1, 3, 5, 7`, then delete `1`. | The deletion completes, resulting tree remains a valid AVL tree, and any emitted rotation code and count match the resulting tree. Include the symmetric case by deleting `7` from the same setup. |
| AVL-06 | Deletion rebalances multiple ancestors | Build an AVL tree from a deterministic sequence that becomes unbalanced at two ancestors after one deletion; record the sequence as a fixture. | `deleteKey` succeeds, AVL validation passes, and all height/size values match independently recomputed values. |
| AVL-07 | Order statistics after each mutation | Insert `8, 3, 10, 1, 6, 14, 4, 7, 13`; after each insertion, then each deletion, compare every valid `kth(k)` and `rank(x)` against a sorted reference array. | Results match the reference after every operation; `isBalanced()` remains true; each node's stored `size` matches its subtree count. |
| AVL-08 | Order statistic boundaries | On empty and nonempty AVL trees, request `k = 0`, `k = 1`, `k = nodeCount`, and `k = nodeCount + 1`; check rank below minimum, at existing keys, between keys, and above maximum. | Invalid `k` returns null; valid results are correct; rank is the number of keys `<= x`; visit count is one per node stepped on and no comparison events are emitted by `kth`. |
| DUP-01 | Duplicate behavior across structures | Attempt duplicate insertion in BST, AVL, and heap where supported by the product contract. | BST/AVL reject duplicates without changing node count, tree shape, or metadata. For heap, confirm duplicates are retained and sorted correctly. |
| HEAP-01 | Build edge arrays | Run `buildMaxHeap` on empty, singleton, duplicate-heavy, already-heap, ascending, descending, negative-only, and mixed-sign arrays. | Active size is unchanged, every parent is at least each existing child, and multiset of values is unchanged. |
| HEAP-02 | Heapify boundaries and locality | Heapify a leaf, root, and internal violating node; include one-child and two-child parents. | Heap property is restored within the requested active range; elements after active size remain unchanged; empty/out-of-range model action is rejected with status. |
| HEAP-03 | Extract maximum repeatedly | Build from `[4, 10, 3, 5, 1, 10, -2]`; extract until empty, checking after each extraction. Then attempt one extra extraction. | Returned maxima are nonincreasing; remaining active prefix remains a valid max heap; extracted values occupy the sorted suffix; extra extraction returns false without changing the array. |
| HEAP-04 | Insert restriction after extraction | Build a heap, extract once, then attempt insertion. Also test insertion before extraction. | Insertion succeeds before sorted suffix exists and preserves heap property; insertion after extraction is rejected and leaves data/size unchanged. |
| HEAP-05 | Heap sort reference cases | Sort empty, singleton, duplicates, negative values, already sorted, reverse sorted, and mixed arrays. | Output equals an independently sorted reference; heap size is zero; each original value appears exactly once; completion event occurs once and is followed by `EV_DONE`. |
| HEAP-06 | Heap event fields and order | Perform one build, one extraction, and one swap. Inspect event records. | Build event's `otherId` is starting heap size; extraction records the maximum and last active index; size-change and sorted events use documented indices/values; batch ends in `EV_DONE`. |
| MODEL-01 | Wrong-mode and invalid parameters | Try each heap action on tree mode and tree-only action on heap mode; try invalid node IDs, invalid heap indices, and reversed range bounds. | Operation is rejected or follows the documented empty-range behavior; model remains valid; status text is useful; final event is `EV_DONE`. |
| MODEL-02 | Snapshot round trips by mode | Save, mutate, restore, and compare binary tree, BST, AVL, and heap views. For AVL, include metadata; for heap, include a partially extracted sorted suffix. | Restored view matches saved structure/data and mode; AVL metadata is recomputed; heap active size and sorted region are preserved; validators pass. |

### AVL deletion fixture note

For AVL-05, the canonical heavy-child-zero case is often tested with a
balanced tree whose root becomes unbalanced after deleting from the shorter
side, while its heavy child has balance factor zero. Verify the fixture's
balance factors before relying on it; add a small helper that reports each
node's factor so the test proves it reached the intended case.

## Manual GUI tests

Run these in the actual game and write **Pass/Fail**, build/version, and a
short note for each. Capture screenshots where specified.

| ID | Test | Steps | Expected result / evidence |
|---|---|---|---|
| GUI-01 | Campaign smoke test | Open every implemented level, play to success, then use reset and fail/retry. | Correct level objective, success/failure result, counters, reset state, and unlock behavior. Capture one traversal/search event sequence. |
| GUI-02 | Reconstruction | Complete both reconstruction missions. | Dragged tokens create the expected hidden tree and success is recognized only for the correct structure. |
| GUI-03 | AVL rotations | Complete one LL, RR, LR, and RL rotation mission. | Correct pivot and rotation repair the tree; wrong choice triggers the specified penalty. Capture one screenshot per rotation type. |
| GUI-04 | Heap build and heapify | Complete Act 4.1 and 4.2, observing both views. | Tree and array show the same values; legal swaps/build/heapify restore max-heap order. Capture build and heapify. |
| GUI-05 | Heap extraction and sort | Complete Act 4.3, 4.4, and 4.5. | Each extraction moves the maximum to the sorted region, shrinks active size, and restores heap order; final array is ascending. Capture extraction and final sorted result. |
| GUI-06 | Undo/reset/replay | Save or perform an action, undo/reset, then step or replay the event sequence. | Model and all visual highlights/positions return to the matching state; no stale node positions or array cells remain. |
| GUI-07 | View synchronization | During heap operations, compare each tree node with its array index before and after swaps/extractions. | Both representations remain synchronized; active and sorted regions are visibly distinct. |
| GUI-08 | Large view performance | Use the largest available tree/heap (target 200 tree nodes) and test zoom, pan, and fit. | No missing/off-screen controls; view remains responsive near 60 fps. Record the largest tested size and observed frame rate. |
| GUI-09 | Input and navigation | Test Escape, keyboard input, range fields, reset, and return-to-menu from campaign and sandbox. | Inputs go to the focused field, invalid/missing inputs show clear feedback, and navigation returns to the expected screen. |

## Blocked until the feature exists

- **Level 3.4:** Add automated model tests and the GUI test for automatic AVL
  insertion/rebalancing only after the level is implemented.
- **Level 3.5:** Add order-statistic campaign interaction tests after the level
  is implemented. AVL `kth` and `rank` logic should still be covered now by
  AVL-07 and AVL-08.

Do not mark a case passed based only on source inspection. Record actual run
results in `docs/TEST_REPORT.md` after executing the relevant test.
