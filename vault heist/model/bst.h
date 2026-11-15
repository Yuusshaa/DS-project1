// model/BST.h  -- must NOT include raylib
// Alarm rule: every EV_COMPARE costs 1. Searches never emit EV_VISIT.
// EV_COMPARE code = the NODE's key relative to the TARGET:
//   CMP_GREATER = node key > target (go LEFT)
//   CMP_LESS    = node key < target (go RIGHT)
//   CMP_EQUAL   = equal
#pragma once
#include "Binarytree.h"
#include "eventlog.h"

class BST : public BinaryTree {
public:
    // ---------- helper: write one event into the log ----------
    void record(EventLog& log, int type, int nodeId, int otherId,
                int value, int code, const char* text) {
        Event e;
        e.type = type;
        e.nodeId = nodeId;
        e.otherId = otherId;
        e.value = value;
        e.code = code;
        e.setNote(text);
        log.add(e);
    }

    // ---------- helper: one comparison step (costs 1 alarm) ----------
    // Compares the target key with 'node' and logs ONE EV_COMPARE.
    // Returns the code that was logged.
    int compareStep(TreeNode* node, int key, EventLog& log, const char* equalText) {
        int code = CMP_EQUAL;
        const char* text = equalText;
        if (key < node->value) {
            code = CMP_GREATER;              // node key is bigger than the target: go left
            text = "Target is smaller, go left";
        } else if (key > node->value) {
            code = CMP_LESS;                 // node key is smaller than the target: go right
            text = "Target is larger, go right";
        }
        record(log, EV_COMPARE, node->id, -1, key, code, text);
        return code;
    }

    // ---------- insert ----------
    // Walks down and attaches the new room. Does NOT write EV_DONE.
    // Returns the new room, or 0 if the key already exists.
    TreeNode* insertNode(int key, EventLog& log) {
        if (root == 0) {
            root = createNode(key);
            record(log, EV_INSERT, root->id, -1, key, SIDE_NONE, "Insert as root");
            return root;
        }
        TreeNode* current = root;
        while (true) {
            int c = compareStep(current, key, log, "Equal: duplicate");
            if (c == CMP_EQUAL) {
                return 0;
            }
            if (c == CMP_GREATER) {
                if (current->left == 0) {
                    TreeNode* fresh = createNode(key);
                    setLeft(current, fresh);
                    record(log, EV_INSERT, fresh->id, current->id, key, SIDE_LEFT, "Insert on the left");
                    return fresh;
                }
                current = current->left;
            } else {
                if (current->right == 0) {
                    TreeNode* fresh = createNode(key);
                    setRight(current, fresh);
                    record(log, EV_INSERT, fresh->id, current->id, key, SIDE_RIGHT, "Insert on the right");
                    return fresh;
                }
                current = current->right;
            }
        }
    }

    // Returns true if added, false if the key already exists.
    bool insert(int key, EventLog& log) {
        TreeNode* fresh = insertNode(key, log);
        if (fresh == 0) {
            record(log, EV_DONE, -1, -1, 0, 0, "Duplicate rejected");
            return false;
        }
        record(log, EV_DONE, -1, -1, 0, 0, "Inserted");
        return true;
    }

    // ---------- search ----------
    // Returns the room, or 0 if the key is not there.
    TreeNode* search(int key, EventLog& log) {
        TreeNode* current = root;
        while (current != 0) {
            int c = compareStep(current, key, log, "Equal: found it");
            if (c == CMP_EQUAL) {
                record(log, EV_DONE, -1, -1, 0, 0, "Found");
                return current;
            }
            if (c == CMP_GREATER) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        record(log, EV_DONE, -1, -1, 0, 0, "Not found");
        return 0;
    }

    // ---------- min, max, successor, predecessor ----------

    TreeNode* findMin(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        while (node->left != 0) {
            node = node->left;
        }
        return node;
    }

    TreeNode* findMax(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        while (node->right != 0) {
            node = node->right;
        }
        return node;
    }

    TreeNode* successor(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        if (node->right != 0) {
            return findMin(node->right);
        }
        TreeNode* child = node;
        TreeNode* up = node->parent;
        while (up != 0 && child == up->right) {
            child = up;
            up = up->parent;
        }
        return up;
    }

    TreeNode* predecessor(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        if (node->left != 0) {
            return findMax(node->left);
        }
        TreeNode* child = node;
        TreeNode* up = node->parent;
        while (up != 0 && child == up->left) {
            child = up;
            up = up->parent;
        }
        return up;
    }

    // ---------- delete ----------

    // Removes a room that has AT MOST ONE child. Its child (or nothing) takes its place.
    void removeNode(TreeNode* node, EventLog& log) {
        TreeNode* child = node->left;
        if (child == 0) {
            child = node->right;
        }
        TreeNode* parent = node->parent;
        int parentId = -1;
        int side = SIDE_NONE;
        const char* text = "Unlink leaf";
        if (child != 0) {
            text = "Bypass room with one child";
        }

        if (parent == 0) {
            root = child;
            if (child != 0) {
                child->parent = 0;
            }
        } else {
            parentId = parent->id;
            if (parent->left == node) {
                side = SIDE_LEFT;
                setLeft(parent, child);
            } else {
                side = SIDE_RIGHT;
                setRight(parent, child);
            }
        }

        record(log, EV_UNLINK, node->id, parentId, node->value, side, text);
        delete node;
        nodeCount = nodeCount - 1;
    }

    // Walks down, removes the room. Does NOT write EV_DONE.
    // Returns false if the key is missing. Otherwise true, and 'repairFrom' is the
    // parent of the room that was physically removed (AVL repairs upward from there).
    bool deleteNode(int key, EventLog& log, TreeNode*& repairFrom) {
        repairFrom = 0;
        TreeNode* current = root;
        while (current != 0) {
            int c = compareStep(current, key, log, "Found the room to delete");
            if (c == CMP_EQUAL) {
                break;
            }
            if (c == CMP_GREATER) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        if (current == 0) {
            return false;
        }

        TreeNode* target = current;
        if (current->left != 0 && current->right != 0) {
            TreeNode* succ = findMin(current->right);
            record(log, EV_HIGHLIGHT_SUBTREE, succ->id, -1, succ->value, HL_RESULT, "Inorder successor");

            // EV_SWAP_VALUE: N, O = the two rooms; Old = old value at N;
            //                New = new value at N; V = old value at O.
            Event swap;
            swap.type = EV_SWAP_VALUE;
            swap.nodeId = current->id;
            swap.otherId = succ->id;
            swap.oldValue = current->value;
            swap.newValue = succ->value;
            swap.value = succ->value;
            swap.setNote("Swap value with successor");
            log.add(swap);

            current->value = succ->value;
            succ->value = key;
            target = succ;
        }

        repairFrom = target->parent;
        removeNode(target, log);
        return true;
    }

    bool deleteKey(int key, EventLog& log) {
        TreeNode* ignored = 0;
        if (deleteNode(key, log, ignored) == false) {
            record(log, EV_DONE, -1, -1, 0, 0, "Key not found");
            return false;
        }
        record(log, EV_DONE, -1, -1, 0, 0, "Deleted");
        return true;
    }

    // ---------- range query ----------
    // One EV_COMPARE per room stepped on (costs 1): V = lo, O = hi,
    // code = CMP_LESS (below range), CMP_GREATER (above range), CMP_EQUAL (inside).
    // Skipped subtrees are logged with EV_HIGHLIGHT_SUBTREE (HL_SKIPPED), cost 0.
    void rangeFrom(TreeNode* node, int lo, int hi, DynArray<int>& result, EventLog& log) {
        if (node == 0) {
            return;
        }
        int code = CMP_EQUAL;
        const char* text = "In range";
        if (node->value < lo) {
            code = CMP_LESS;
            text = "Below the range";
        } else if (node->value > hi) {
            code = CMP_GREATER;
            text = "Above the range";
        }
        record(log, EV_COMPARE, node->id, hi, lo, code, text);

        if (node->value > lo) {
            rangeFrom(node->left, lo, hi, result, log);
        } else if (node->left != 0) {
            record(log, EV_HIGHLIGHT_SUBTREE, node->left->id, -1, 0, HL_SKIPPED, "Skip left subtree");
        }

        if (node->value >= lo && node->value <= hi) {
            result.push(node->value);
            record(log, EV_HIGHLIGHT_SUBTREE, node->id, -1, node->value, HL_RESULT, "In range");
        }

        if (node->value < hi) {
            rangeFrom(node->right, lo, hi, result, log);
        } else if (node->right != 0) {
            record(log, EV_HIGHLIGHT_SUBTREE, node->right->id, -1, 0, HL_SKIPPED, "Skip right subtree");
        }
    }

    void rangeQuery(int lo, int hi, DynArray<int>& result, EventLog& log) {
        rangeFrom(root, lo, hi, result, log);
        record(log, EV_DONE, -1, -1, 0, 0, "Range done");
    }

    // ---------- k-th smallest (plain BST version, O(n), no events) ----------
    // k = 1 is the smallest. Returns 0 if k is out of range.
    TreeNode* kthSmallest(int k) {
        DynArray<TreeNode*> stack;
        TreeNode* current = root;
        int count = 0;
        while (current != 0 || stack.empty() == false) {
            while (current != 0) {
                stack.push(current);
                current = current->left;
            }
            current = stack.pop();
            count = count + 1;
            if (count == k) {
                return current;
            }
            current = current->right;
        }
        return 0;
    }

    // ---------- LCA using the ordering (takes two KEYS, both must exist) ----------
    TreeNode* lcaBST(int a, int b) {
        TreeNode* current = root;
        while (current != 0) {
            if (a < current->value && b < current->value) {
                current = current->left;
            } else if (a > current->value && b > current->value) {
                current = current->right;
            } else {
                return current;
            }
        }
        return 0;
    }
};