# Adding levels

Level files are planned under `data/levels/` as `act1.json` through
`act4.json`. Keep IDs stable and use fixed seeds so test and benchmark
scenarios are reproducible.

## Fields

| Field | Meaning |
|---|---|
| `id` | Level ID, such as `"1.1"`. |
| `act` | Integer act number from 1 to 4. |
| `title` | Short display title. |
| `mission` | One of the mission strings below. |
| `treeType` | `binary`, `bst`, `avl`, or `heap`. |
| `seed` | Fixed integer seed for any generated input. |
| `keys` | Integer node keys or heap array values. |
| `target` | Integer target, or `{ "lo": ..., "hi": ... }` for a range mission. |
| `story` | Short player-facing narrative. |

## Allowed mission values

The current working mission set is:

- `traversal`
- `search`
- `insert`
- `delete`
- `range_query`
- `avl_rotation`
- `heap_build`
- `heapify`
- `extract_max`
- `heap_sort`

If the assignment spec defines a different exact vocabulary, use the spec and
update this list before adding levels.

## Add a level

1. Choose the act and an unused level ID.
2. Select a mission compatible with the `treeType`.
3. Use fixed `keys` and `seed` values; add every field from the table.
4. Add the level object to the appropriate act file.
5. Test the level with the real model and confirm the expected operation.

Optimal alarm steps and alarm capacity are computed by running the model on a
copy of the level. Capacity is `(3 * optimal + 1) / 2 + 2` using integer
arithmetic. Never store either value in JSON.

