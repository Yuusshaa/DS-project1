// model/Validator.h  -- must NOT include raylib
// Every check returns true if OK. On failure it returns false and sets lastError.
#pragma once
#include "avltree.h"
#include "maxheap.h"

class Validator {
public:
    const char* lastError;

    Validator() {
        lastError = "OK";
    }

    bool fail(const char* message) {
        lastError = message;
        return false;
    }

    // ---------- walkers (private in spirit, public for simplicity) ----------

    // Counts rooms. Stops early if the count passes 'limit'
    // (that happens with cycles or a room reachable by two routes).
    bool countWalk(TreeNode* node, int& count, int limit) {
        if (node == 0) {
            return true;
        }
        count = count + 1;
        if (count > limit) {
            return false;
        }
        if (countWalk(node->left, count, limit) == false) {
            return false;
        }
        return countWalk(node->right, count, limit);
    }

    // Every room's parent link must point at the room above it.
    bool parentWalk(TreeNode* node, TreeNode* expectedParent) {
        if (node == 0) {
            return true;
        }
        if (node->parent != expectedParent) {
            return false;
        }
        if (parentWalk(node->left, node) == false) {
            return false;
        }
        return parentWalk(node->right, node);
    }

    // Every key must be strictly between the bounds handed down from above.
    bool orderWalk(TreeNode* node, bool hasLo, int lo, bool hasHi, int hi) {
        if (node == 0) {
            return true;
        }
        if (hasLo && node->value <= lo) {
            return false;
        }
        if (hasHi && node->value >= hi) {
            return false;
        }
        if (orderWalk(node->left, hasLo, lo, true, node->value) == false) {
            return false;
        }
        return orderWalk(node->right, true, node->value, hasHi, hi);
    }

    // Stored height, stored size and balance factor of every room.
    bool avlWalk(TreeNode* node) {
        if (node == 0) {
            return true;
        }
        if (avlWalk(node->left) == false) {
            return false;
        }
        if (avlWalk(node->right) == false) {
            return false;
        }
        int leftH = heightOf(node->left);
        int rightH = heightOf(node->right);
        int expectedHeight = rightH + 1;
        if (leftH > rightH) {
            expectedHeight = leftH + 1;
        }
        if (node->height != expectedHeight) {
            return fail("Stored height is wrong");
        }
        if (node->size != sizeOf(node->left) + sizeOf(node->right) + 1) {
            return fail("Stored size is wrong");
        }
        int bf = leftH - rightH;
        if (bf < -1 || bf > 1) {
            return fail("Balance factor is outside -1..1");
        }
        return true;
    }

    // ---------- public checks ----------

    // Structure only: room count, no cycles, no shared rooms, parent links.
    bool validateStructure(BinaryTree& t) {
        lastError = "OK";
        if (t.root == 0) {
            if (t.nodeCount != 0) {
                return fail("Tree is empty but nodeCount is not 0");
            }
            return true;
        }
        int count = 0;
        if (countWalk(t.root, count, t.nodeCount) == false) {
            return fail("Cycle, shared room, or more rooms than nodeCount");
        }
        if (count != t.nodeCount) {
            return fail("nodeCount does not match the rooms found");
        }
        if (parentWalk(t.root, 0) == false) {
            return fail("A parent link is wrong");
        }
        return true;
    }

    // Structure + BST ordering.
    bool validateBST(BinaryTree& t) {
        if (validateStructure(t) == false) {
            return false;
        }
        if (orderWalk(t.root, false, 0, false, 0) == false) {
            return fail("BST ordering is broken");
        }
        return true;
    }

    // Structure + ordering + heights + sizes + balance.
    bool validateAVL(BinaryTree& t) {
        if (validateBST(t) == false) {
            return false;
        }
        return avlWalk(t.root);
    }

    // Max heap: size in range, parent >= children in the active heap,
    // sorted region ascending and not smaller than the heap's maximum.
    bool validateHeap(MaxHeap& h) {
        lastError = "OK";
        if (h.heapSize < 0 || h.heapSize > h.data.size()) {
            return fail("heapSize is out of range");
        }
        for (int i = 1; i < h.heapSize; i++) {
            if (h.data[i] > h.data[h.parentOf(i)]) {
                return fail("A child is bigger than its parent");
            }
        }
        for (int i = h.heapSize + 1; i < h.data.size(); i++) {
            if (h.data[i - 1] > h.data[i]) {
                return fail("Sorted region is not ascending");
            }
        }
        if (h.heapSize > 0 && h.heapSize < h.data.size()) {
            if (h.data[h.heapSize] < h.data[0]) {
                return fail("Sorted region holds something smaller than the heap");
            }
        }
        return true;
    }
};