# Model and GUI contract

The model stays independent of Raylib. The GUI reads display snapshots and
event batches; it does not edit model links or implement data-structure
algorithms.

## Shared node meaning

- `id` uniquely identifies a node and remains stable through rotations.
- `value` is the displayed value and the BST/AVL key.
- `left`, `right`, and `parent` are model links. The root's parent is null.
- A leaf has height 1; an empty child has height 0. AVL leaf size is 1.
- Trees loaded from a snapshot receive fresh IDs; saved IDs are not serialized.

## Event batches

- The caller clears `EventLog` before each action.
- The model appends events in operation order and ends each action batch with
  `EV_DONE`.
- The GUI processes events in order. For tree events, `nodeId` is a tree node
  ID; for heap events, `nodeId` and `otherId` are array indices.
- `note` is display-only text. The GUI must use event fields, not parse notes,
  to determine behavior.
- Each `EV_VISIT` and `EV_COMPARE` costs one alarm step. One operation step
  must not emit both event types.

## Event fields

`N` = `nodeId`; `O` = `otherId`; `V` = `value`; `Old`/`New` =
`oldValue`/`newValue`; `Code` = `code`. Unused fields keep their `Event`
defaults.

| Event | Field meanings |
|---|---|
| `EV_VISIT` | N=node ID; V=node value. |
| `EV_COMPARE` | N=compared node ID; V=target key; Code describes the node key relative to the target. |
| `EV_INSERT` | N=new node ID; O=parent ID or -1 for root; V=inserted key; Code=side. |
| `EV_UNLINK` | N=removed node ID; O=former parent ID or -1 for root; V=removed key; Code=former side. |
| `EV_SWAP_VALUE` | N and O=the two node IDs; Old=old value at N; New=new value at N; V=old value at O. |
| `EV_ROTATE` | N=old root of the rebalanced subtree; O=new root; Code=rotation case. |
| `EV_UPDATE_HEIGHT` / `EV_UPDATE_SIZE` | N=updated node ID; Old=previous value; New=updated value. |
| `EV_BALANCE_ALERT` | N=node ID; V=balance factor. |
| `EV_HIGHLIGHT_SUBTREE` | N=subtree root ID; Code=highlight reason. |
| `EV_HEAP_BUILD` | O=active heap size; signals construction, not an array snapshot. |
| `EV_HEAP_SWAP` | N and O=swapped array indices; Old=old value at N; New=value moved to N. |
| `EV_HEAPIFY_START` / `EV_HEAPIFY_DONE` | N=start index; O=active heap size. |
| `EV_EXTRACT_MAX` | O=last active index before shrinking; V=extracted maximum. |
| `EV_MARK_SORTED` | N=sorted array index; V=value at that index. |
| `EV_HEAP_SIZE_CHANGE` | Old=active heap size before change; New=active heap size after change. |
| `EV_HEAP_SORT_COMPLETE` | Signals completion; read the final array from the model. |
| `EV_DONE` | Ends the batch; result actions may also set N and V as specified below; `note` summarizes the result. |

## Notes for GUI

- `EV_COMPARE` codes describe the **node key relative to the target**:
  `CMP_GREATER` means go left, `CMP_LESS` means go right, and `CMP_EQUAL`
  means equal.
- For range-query `EV_COMPARE`, N is the node ID, V is the lower bound, O is
  the upper bound, and Code is `CMP_LESS` (below range), `CMP_GREATER` (above
  range), or `CMP_EQUAL` (inside range).
- For `EV_ROTATE`, N identifies the subtree's old root, O its new root, and
  Code is `ROT_LL`, `ROT_RR`, `ROT_LR`, or `ROT_RL`. Animate from the old
  layout to the updated layout.
- A loaded tree gets newly assigned IDs. Do not expect IDs from a previous
  saved version to persist.
- Heap event node IDs are array indices, not tree IDs.

## Action codes and results

Action identifiers are integer macros in `model/GameModel.h`:

| Code | Action | Parameters and result |
|---|---|---|
| 13 | `ACTION_DIAMETER` | Whole tree; `EV_DONE.value` is the diameter in edges. |
| 14 | `ACTION_HEIGHT` | `value` is a node ID; `EV_DONE.value` is its height (leaf = 1). |
| 15 | `ACTION_LCA` | `value` and `secondValue` are node IDs; `EV_DONE.nodeId` is the LCA ID and `EV_DONE.value` is their path length in edges. |
| 16 | `ACTION_MIRROR` | Mirror the binary tree and refresh its displayed heights. |
| 17 | `ACTION_ROTATE_LEFT` | `value` is the pivot ID; perform one left primitive rotation, no auto-rebalance; `EV_ROTATE.code = ROT_RR`. |
| 18 | `ACTION_ROTATE_RIGHT` | `value` is the pivot ID; perform one right primitive rotation, no auto-rebalance; `EV_ROTATE.code = ROT_LL`. |
| 19 | `ACTION_ROTATION_CASE` | Query; `EV_DONE.nodeId` is the lowest unbalanced node and `EV_DONE.value` is `ROT_LL`, `ROT_RR`, `ROT_LR`, `ROT_RL`, or `ROT_NONE`. |
| 20 | `ACTION_KTH` | `value` is 1-based k; `EV_DONE.nodeId` is the result node and `EV_DONE.value` is its key. |
| 21 | `ACTION_RANK` | `value` is a key; `EV_DONE.value` is the number of keys less than or equal to it and `nodeId = -1`. |
| 22 | `ACTION_HEAP_SWAP` | `value` and `secondValue` are indexes; only swap an active parent/child pair when the child value is strictly larger. |
| 23 | `ACTION_IS_MAX_HEAP` | Query; `EV_DONE.value` is 1 for a max heap and 0 otherwise. |

`EV_DONE` scalar results use `value`; node results use `nodeId`, with `-1`
when there is no result node. Missing or invalid nodes are rejected with
`performAction == false` and a short status message. `ACTION_HEAPIFY` takes
the index to sift down in `value`. `ACTION_EXTRACT_MAX` performs exactly one
extraction: swap the root with the last active element, shrink the active
heap, mark the extracted element sorted, then sift down.

`GameModel` also exposes virtual `serialize()` and `deserialize(const
std::string&)` methods. Tree snapshots use preorder with `#` null markers;
heap snapshots include the active size and the array. Loaded tree metadata is
recomputed.

## Scenario fixtures

IDs are stable within each loaded scenario and are allocated in the order
listed below. The GUI reads the model view and does not hardcode these trees.

- **12 — reconstruction:** binary tree `40(20(10,30),70(60,90))`, seven
  distinct keys. IDs: 1=40, 2=20, 3=70, 4=10, 5=30, 6=60, 7=90.
- **13 — height/LCA:** binary tree rooted at 50. Its left branch is
  `30(20(10,null),40)` and its right branch is `70(60,90(null,100))`.
  This has nine distinct keys and height 4. IDs are assigned in construction
  order: 1=50, 2=30, 3=20, 4=70, 5=10, 6=40, 7=60, 8=90, 9=100.
- **25 — degeneration:** empty BST; the level setup inserts its sorted keys.
- **31 — balance badges:** manually built binary tree
  `50(30(20(10,null),null),70)`, intentionally unbalanced for the badge
  challenge. Heights and subtree sizes are derived from its links; insertion
  and deletion do not auto-rebalance this fixture.

## Model interface

The GUI calls only `GameModel` from `model/GameModel.h`: `loadScenario`,
`performAction`, `copyView`, and `statusText`. The model returns a `ModelView`
snapshot containing tree IDs and links or heap values and active size.
`RealGameModel` implements the shared interface and backs the GUI. Scenario
fixtures and operations are supplied by the model rather than duplicated in
the GUI.

