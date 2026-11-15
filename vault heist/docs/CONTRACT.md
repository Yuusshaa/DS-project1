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
| `EV_DONE` | Ends the batch; `note` may summarize the result. |

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

## Model interface

The GUI calls only `GameModel` from `model/GameModel.h`: `loadScenario`,
`performAction`, `copyView`, and `statusText`. The model returns a `ModelView`
snapshot containing tree IDs and links or heap values and active size.
`RealGameModel` additionally offers snapshot save/restore for undo and reset.

