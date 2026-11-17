# Model and GUI contract (working draft)

This is the interface agreement for Person A (Yusha, model) and Person B
(Hussain, GUI). Keep the model independent of Raylib.

## Ownership

- Yusha owns `model/`, data structure algorithms, model validation, and model
  unit tests.
- Hussain owns `gui/`, drawing, input, layout, animation, level flow, scoring,
  and level data.
- Both review changes to shared headers and integrate the project regularly.

## Shared node meaning

- `id` uniquely identifies a node and remains stable through rotations.
- `value` is the node's displayed value and BST/AVL key.
- `left`, `right`, and `parent` are model links. The root's parent is null.
- Leaf height is 1; null height is 0. AVL leaf subtree size is 1.
- The GUI reads the current model state; it does not alter model links.

## Event batch

- The caller clears the `EventLog` before each player action.
- The model appends that action's events in the order they occurred and ends
  the batch with `EV_DONE`.
- The GUI processes the batch in order and uses `nodeId` to locate a tree node.
- For heap events, `nodeId` and `otherId` are array indices, not tree node IDs.
- `note` is display text only. The GUI must use event fields, not parse `note`,
  to determine behavior.
- One traversal visit emits `EV_VISIT`. One comparison emits `EV_COMPARE`.
  Each costs one alarm step; do not emit both for the same step.

## Event fields

`N` means `nodeId`; `O` means `otherId`; `V` means `value`; `Old` and `New`
mean `oldValue` and `newValue`; `Code` means `code`. Unused fields use the
default value from `Event`.

| Event | Agreed field meanings |
|---|---|
| `EV_VISIT` | N=node ID; V=node value. |
| `EV_COMPARE` | N=compared node ID; V=target key; Code=`CMP_LESS`, `CMP_EQUAL`, or `CMP_GREATER`, describing node key relative to target. |
| `EV_INSERT` | N=new node ID; O=parent ID or -1 for root; V=inserted key; Code=`SIDE_LEFT`, `SIDE_RIGHT`, or `SIDE_NONE` for root. |
| `EV_UNLINK` | N=removed node ID; O=former parent ID or -1 for root; V=removed key; Code=former side. |
| `EV_SWAP_VALUE` | N and O=the two node IDs; Old=old value at N; New=new value at N; V=old value at O. |
| `EV_ROTATE` | N=old root of the rebalanced subtree; O=new root; Code=`ROT_LL`, `ROT_RR`, `ROT_LR`, or `ROT_RL`. Hussain animates the subtree from its pre-operation layout to its updated layout. |
| `EV_UPDATE_HEIGHT` / `EV_UPDATE_SIZE` | N=updated node ID; Old=previous value; New=updated value. |
| `EV_BALANCE_ALERT` | N=node ID; V=balance factor. |
| `EV_HIGHLIGHT_SUBTREE` | N=subtree root ID; Code=`HL_VISITED`, `HL_SKIPPED`, or `HL_RESULT`. |
| `EV_HEAP_BUILD` | O=active heap size. This signals construction; it does not contain an array snapshot. |
| `EV_HEAP_SWAP` | N and O=swapped array indices; Old=old value at N; New=value moved to N. |
| `EV_HEAPIFY_START` / `EV_HEAPIFY_DONE` | N=start index; O=active heap size. |
| `EV_EXTRACT_MAX` | O=last active index before shrinking; V=extracted maximum. |
| `EV_MARK_SORTED` | N=sorted array index; V=value at that index. |
| `EV_HEAP_SIZE_CHANGE` | Old=active heap size before change; New=active heap size after change. |
| `EV_HEAP_SORT_COMPLETE` | Signals completion; GUI reads final array from the model. |
| `EV_DONE` | Ends the batch; `note` may summarize the result. For `ACTION_DIAMETER`, V=diameter measured in edges (a single node has diameter 0). |

## Act 1.3 agreed fixture and action

- `ACTION_DIAMETER` (13): the answer is in `EV_DONE.value`. Diameter is counted in edges;
  leaf height is 1 and null height is 0.
- Scenario `13`: fixed 9-room tree, ids 1-9: 1:50 (left 2, right 4); 2:30 (left 3,
  right 6); 3:20 (left 5); 4:70 (left 7, right 8); 5:10; 6:40; 7:60; 8:90 (right 9);
  9:100. Postorder ids: 5 3 6 2 7 9 8 4 1. Root height 4, diameter 6.
- `ACTION_TRAVERSE_POSTORDER` must work on any loaded scenario (the GUI asks the model for the
  route; it never computes it).

## Act 1.4 addition (matches the shared model contract)

- `ACTION_LCA` (15) takes two node IDs in `value` and `secondValue`.
- The model returns the lowest common ancestor in `EV_DONE.nodeId` and the distance between
  the selected rooms, in edges, in `EV_DONE.value`. The GUI asks the model for this result;
  it does not walk parent links to compute it.
- The GUI's level flow lets the player select two distinct rooms, identify the returned
  ancestor, then trace adjacent rooms between the selected endpoints. The alarm counts the
  three selections plus each path step; the model-provided edge distance is the target length.

## Act 1.5 addition (matches the shared model contract)

- `ACTION_MIRROR` (16) mirrors the currently loaded binary tree in the model.
- The GUI calls the operation, refreshes `ModelView`, and renders the updated left/right links.
  The GUI does not swap child links itself.

## Act 1.2 traversal reconstruction

- Mock scenario 14 is the preorder-plus-inorder round; scenario 15 is the postorder-plus-inorder
  round. Each is exposed as a tree through `ModelView` and as traversal sequences through
  `ACTION_TRAVERSE_PREORDER`/`POSTORDER` and `ACTION_TRAVERSE_INORDER`.
- The GUI displays the traversal outputs and lets players drag room tokens into seven possible
  binary-tree slots. Correct placements must match the scenario's tree; an unused slot stays empty.
- Yusha's model must load scenarios 14 and 15 and emit `EV_VISIT.value` in traversal order before
  this level is integrated with the real model.

## Act 3.2/3.3 rotation interface

- `ACTION_ROTATION_CASE` (19) returns the lowest unbalanced node in `EV_DONE.nodeId` and the
  case (`ROT_LL`, `ROT_RR`, `ROT_LR`, or `ROT_RL`) in `EV_DONE.value`.
- `ACTION_ROTATE_LEFT` (17) and `ACTION_ROTATE_RIGHT` (18) take a pivot node ID in `value` and
  perform one primitive rotation. `EV_ROTATE.nodeId` is the old subtree root, `otherId` is the
  promoted node, and `code` is `ROT_RR` for a left rotation or `ROT_LL` for a right rotation.
- The GUI asks the model for the case and performs only the selected primitive operations; it
  does not rotate or rebalance tree links itself.
- Mock fixtures 31-34 represent LL, RR, LR, and RL cases respectively. Yusha's real model must
  expose these same actions and fixtures before these levels can use the real model.

## Act 3.4 AVL insertion prediction

- Before each insertion, the GUI asks the player to predict no rotation or LL/RR/LR/RL.
- `ACTION_INSERT` on AVL scenarios 35-39 must insert the key, rebalance automatically, and emit
  `EV_BALANCE_ALERT` with the first unbalanced pivot in `nodeId` and rotation case in `code`,
  followed by the primitive `EV_ROTATE` operations and height/size updates. No alert means no
  rotation was required.
- Mock scenarios 39, 35, 36, 37, and 38 are the no-rotation, LL, RR, LR, and RL predictions.
- Yusha's real facade must expose these scenarios and AVL insert behavior before integration.

## Act 3.5 order statistics

- Scenario 3 contains the seven-key balanced AVL tree with correct subtree sizes.
- `ACTION_KTH` (20) takes a one-based `k` in `value` and returns the node ID in
  `EV_DONE.nodeId`; `EV_DONE.value` is the returned key. Its `EV_VISIT` events
  show the search path using subtree sizes.
- `ACTION_RANK` (21) takes a key in `value` and returns the number of keys
  less than or equal to it in `EV_DONE.value`; `EV_DONE.nodeId`
  is -1. The GUI queries `rank(key)` after a correct click and displays it.
- The level asks the player to click the node returned by `kth(5)`.
- The GUI displays each node's subtree size from `ModelView.size`. Yusha's model
  must dispatch actions 20/21 for scenario 3 with these result meanings; its
  `AVLTree` already provides the underlying operations.

## Act 4.1 manual max-heap construction

- Scenario 4 loads the eight-value heap puzzle `{3, 9, 2, 1, 4, 5, 8, 7}`.
- `ACTION_HEAP_SWAP` (22) takes a parent array index in `value` and a child index in
  `secondValue`. It accepts only a direct parent-child pair where the child value is larger;
  success emits `EV_HEAP_SWAP` with the two indices in `nodeId` and `otherId`.
- `ACTION_IS_MAX_HEAP` (23) emits `EV_DONE` with `value = 1` when the active heap satisfies
  the max-heap property, or `0` otherwise. A false result means the puzzle is not complete,
  not that the query operation failed.
- The GUI handles selection and scoring; the model owns validation and all heap-value changes.
- Yusha's real facade must dispatch actions 22 and 23 and load scenario 4 before integration.

## Act 4.2 heapify repair

- Scenario 5 is `{4, 9, 8, 7, 3, 5, 2, 1}` with a violation at root index 0; the child subtrees
  are already max heaps.
- `ACTION_HEAPIFY` (10) takes the start index in `value` and sifts that item down through the
  active heap. It emits `EV_HEAPIFY_START`, any `EV_HEAP_SWAP` operations, and
  `EV_HEAPIFY_DONE`. Heap swap events identify array indexes in `nodeId` and `otherId`.
- Act 4.2 completes when the GUI queries `ACTION_IS_MAX_HEAP` and receives `EV_DONE.value = 1`.
- Scenario 5 and action 10 must be supported by Yusha's model facade before integration.

## Act 4.3 extract maximum

- Scenario 6 loads the valid heap `{9, 4, 8, 7, 3, 5, 2, 1}`.
- `ACTION_EXTRACT_MAX` (11) removes the active root, places it in the sorted region at the
  active heap's final index, decreases `heapSize`, and sifts down the replacement root.
- Each extraction emits `EV_EXTRACT_MAX`, `EV_HEAP_SWAP`, `EV_MARK_SORTED`, and
  `EV_HEAP_SIZE_CHANGE`; sift-down emits its heapify and swap events. Event `nodeId` and
  `otherId` for heap operations are zero-based array indexes.
- The GUI requests one extraction at a time and waits for its event playback before allowing
  the next. Completion occurs when only the final sorted value remains active.

## Act 4.4 timed heap-sort race

- Scenario 7 loads the unsorted values `{6, 3, 8, 1, 5, 2, 7, 4}`.
- The GUI calls `ACTION_HEAP_BUILD` (9) once, then `ACTION_EXTRACT_MAX` (11) once per
  maximum. It waits for each event batch to play before enabling the next operation.
- The active heap and sorted suffix stay synchronized in `ModelView`; completion occurs when
  one sorted value remains active. The level adds a 60-second limit and a 12-operation alarm.
- Mock implementation is the stand-in. Yusha's model must support scenario 7 and actions 9/11
  with the same heap-event meanings before integration.

## Act 4.5 final vault

- Scenario 8 is a fresh ten-value array with duplicate and negative keys:
  `{12, -4, 7, 12, 0, 5, 3, 9, -1, 6}`.
- The GUI reuses the build-then-extract workflow without the timer, and allows the player to
  complete the full heap sort independently under the alarm limit.
- Yusha's real model must load scenario 8 and preserve duplicates and negative values through
  `ACTION_HEAP_BUILD` and repeated `ACTION_EXTRACT_MAX` calls.

## Public model interface to finalize

Before the GUI calls model operations, Yusha records exact class names,
function names, parameter types, return values, and which functions emit
events. At minimum the GUI needs:

- Read access to the current binary tree/BST/AVL root and current heap array.
- Search/insert/delete/range operations for tree levels.
- AVL insert/delete operations whose events identify the rotation case.
- Heap build/heapify/extract/heap-sort operations and the active heap size.
- A way to copy or restore model state for reset and undo.

Do not freeze an interface until both people can compile a small caller against
it. If an event does not contain enough information for the agreed animation,
adjust the event contract before implementing that animation.

## Integration boundary

- The GUI calls only `GameModel` from `model/GameModel.h`; it does not call
  `BinaryTree`, `BST`, `AVLTree`, or `MaxHeap` directly.
- The model writes a plain `ModelView` containing node IDs and links, or heap
  array values and active size. The GUI uses this copy for drawing and does not
  change it to perform an algorithm.
- Hussain builds against `MockGameModel`, which implements the same interface.
  During integration, the app is constructed with Yusha's real implementation
  instead. The GUI source does not change for that swap.
- `GameModel` has `loadScenario`, `performAction`, `copyView`, `statusText`,
  `serialize`, and `deserialize` methods. Serialization captures the active
  structure for snapshots; restoring a tree may assign new node IDs, so the
  GUI refreshes its view and clears ID-based selections after restore. Action
  codes are documented integer macros, not enums.
- A model action receives the caller's `EventLog`, appends events in order, and
  ends its batch with `EV_DONE`. The GUI clears the log before requesting the
  next action.
- The project uses C++20 in Visual Studio. Raylib appears only in `gui/`.
