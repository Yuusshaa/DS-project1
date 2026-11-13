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
| `EV_DONE` | Ends the batch; `note` may summarize the result. |

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
- `GameModel` has `loadScenario`, `performAction`, `copyView`, and
  `statusText` methods. Action codes are documented integer macros, not enums.
- A model action receives the caller's `EventLog`, appends events in order, and
  ends its batch with `EV_DONE`. The GUI clears the log before requesting the
  next action.
- The project uses C++20 in Visual Studio. Raylib appears only in `gui/`.
