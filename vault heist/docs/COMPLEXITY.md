# Model operation complexity

`n` is the number of nodes or heap elements, `h` is tree height, and `k` is
the number of returned range-query keys. Recursive operations use `O(h)`
call-stack space unless noted.

| Operation | Complexity | Reason |
|---|---|---|
| Recursive and iterative tree traversals | `O(n)` time | Each node is processed once; iterative traversals use an explicit stack/queue. |
| Level-order traversal | `O(n)` time, `O(n)` extra space | Every node enters the array-backed queue once. |
| Build from preorder/inorder or postorder/inorder | `O(n^2)` worst case | Each recursive step scans and copies traversal subarrays. |
| Copy or mirror a tree | `O(n)` time | Each node is visited once. |
| Compute height | `O(n)` time | Height is recomputed from both subtrees. |
| Compute diameter | `O(n)` time | One postorder walk returns heights and updates the best diameter. |
| Find a path or general LCA | `O(n)` time | The tree may need to be searched in full. |
| Path length | `O(n)` time | Finds both paths and their common ancestor. |
| BST search, insert, delete, min/max, successor/predecessor, LCA | `O(h)` time | Each follows a single tree path, except successor/predecessor which climb or descend one path. |
| BST range query | `O(h + k)` typical, `O(n)` worst case | Ordering prunes irrelevant subtrees, but a wide range can visit all nodes. |
| BST `kthSmallest` | `O(n)` time, `O(h)` space | Inorder traversal may pass all preceding nodes. |
| AVL search | `O(log n)` time | AVL height remains logarithmic. |
| AVL insert/delete | `O(log n)` time | Searches and repairs one root path; each local rotation is constant time. |
| Single AVL rotation | `O(1)` time | Rewires a fixed number of links and updates two nodes. |
| AVL `kth`, rank, count-in-range | `O(log n)` time | Stored subtree sizes select or skip whole subtrees. |
| AVL validation/recompute | `O(n)` time | Checks or rebuilds metadata for every node. |
| Heap array load | `O(n)` time | Copies each input element once. |
| Bottom-up max-heap build | `O(n)` time | Most nodes are near leaves and move only a short distance. |
| Heap sift-up/sift-down | `O(log n)` time | Each step moves at most one level. |
| Heap extract-max | `O(log n)` time | Replaces the root and sifts down. |
| Heap sort | `O(n log n)` time | Builds in linear time and extracts `n` maxima. |
| Heap validation or sortedness check | `O(n)` time | Checks each relevant array position once. |
| Save/load a tree or heap | `O(n)` time | Serializes or parses every node/element once. |
| Real model `copyView` for AVL | `O(n)` time | Copies every node and reads stored metadata. |
| Real model `copyView` for binary tree/BST | `O(n^2)` worst case | It recomputes height and subtree size separately at every node. |
| Scenario build | BST `O(n^2)` worst case; AVL `O(n log n)` | Repeated BST insertion can form a chain; AVL insertion remains logarithmic. |

