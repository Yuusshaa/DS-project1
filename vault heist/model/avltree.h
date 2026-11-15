// model/avltree.h  -- must NOT include raylib
// Height convention: leaf = 1, empty spot = 0. Balance factor = height(left) - height(right).
// Alarm: search/insert/delete cost one EV_COMPARE per room (from BST.h).
//        kth costs one EV_VISIT per room. Never both for the same step.
#pragma once
#include "bst.h"

class AVLTree : public BST {
public:
    int totalRotations;   // rebalancing cases since this tree was made (double = 1)
    int lastRotations;    // rebalancing cases in the most recent insert or delete

    AVLTree() {
        totalRotations = 0;
        lastRotations = 0;
    }

    void recordChange(EventLog& log, int type, int nodeId, int oldValue,
                      int newValue, const char* text) {
        Event e;
        e.type = type;
        e.nodeId = nodeId;
        e.oldValue = oldValue;
        e.newValue = newValue;
        e.setNote(text);
        log.add(e);
    }

    // Recompute one room's height and size from its children.
    // Only logs an event when the number really changed.
    void updateNode(TreeNode* node, EventLog& log) {
        int leftH = heightOf(node->left);
        int rightH = heightOf(node->right);
        int newHeight = rightH + 1;
        if (leftH > rightH) {
            newHeight = leftH + 1;
        }
        int newSize = sizeOf(node->left) + sizeOf(node->right) + 1;

        if (newHeight != node->height) {
            recordChange(log, EV_UPDATE_HEIGHT, node->id, node->height, newHeight, "Height changed");
            node->height = newHeight;
        }
        if (newSize != node->size) {
            recordChange(log, EV_UPDATE_SIZE, node->id, node->size, newSize, "Size changed");
            node->size = newSize;
        }
    }

    //        y              x
    //       / \            / \
    //      x   C   -->    A   y
    //     / \                / \
    //    A   B              B   C
    TreeNode* rotateRight(TreeNode* y, EventLog& log) {
        TreeNode* x = y->left;
        TreeNode* middle = x->right;
        TreeNode* above = y->parent;

        x->parent = above;
        if (above == 0) {
            root = x;
        } else if (above->left == y) {
            above->left = x;
        } else {
            above->right = x;
        }

        x->right = y;
        y->parent = x;

        y->left = middle;
        if (middle != 0) {
            middle->parent = y;
        }

        updateNode(y, log);
        updateNode(x, log);
        return x;
    }

    //      x                  y
    //     / \                / \
    //    A   y      -->     x   C
    //       / \            / \
    //      B   C          A   B
    TreeNode* rotateLeft(TreeNode* x, EventLog& log) {
        TreeNode* y = x->right;
        TreeNode* middle = y->left;
        TreeNode* above = x->parent;

        y->parent = above;
        if (above == 0) {
            root = y;
        } else if (above->left == x) {
            above->left = y;
        } else {
            above->right = y;
        }

        y->left = x;
        x->parent = y;

        x->right = middle;
        if (middle != 0) {
            middle->parent = x;
        }

        updateNode(x, log);
        updateNode(y, log);
        return y;
    }

    // EV_ROTATE: N = the room that was out of balance, O = the room that becomes
    //            the new top, Code = ROT_LL / ROT_RR / ROT_LR / ROT_RL.
    // EV_BALANCE_ALERT: N = room, V = balance factor.
    TreeNode* rebalance(TreeNode* node, EventLog& log) {
        updateNode(node, log);
        int bf = balanceFactorOf(node);

        if (bf >= -1 && bf <= 1) {
            return node;
        }

        record(log, EV_BALANCE_ALERT, node->id, -1, bf, 0, "Out of balance");

        TreeNode* top = node;
        if (bf > 1) {
            if (balanceFactorOf(node->left) < 0) {
                record(log, EV_ROTATE, node->id, node->left->right->id, 0, ROT_LR,
                       "Left-Right rotation");
                rotateLeft(node->left, log);
                top = rotateRight(node, log);
            } else {
                record(log, EV_ROTATE, node->id, node->left->id, 0, ROT_LL,
                       "Left-Left rotation");
                top = rotateRight(node, log);
            }
        } else {
            if (balanceFactorOf(node->right) > 0) {
                record(log, EV_ROTATE, node->id, node->right->left->id, 0, ROT_RL,
                       "Right-Left rotation");
                rotateRight(node->right, log);
                top = rotateLeft(node, log);
            } else {
                record(log, EV_ROTATE, node->id, node->right->id, 0, ROT_RR,
                       "Right-Right rotation");
                top = rotateLeft(node, log);
            }
        }
        totalRotations = totalRotations + 1;
        lastRotations = lastRotations + 1;
        return top;
    }

    // Walk from 'start' up to the root, rebalancing every room on the way.
    void fixUpward(TreeNode* start, EventLog& log) {
        TreeNode* walker = start;
        while (walker != 0) {
            TreeNode* top = rebalance(walker, log);
            walker = top->parent;
        }
    }

    // Returns true if added, false if the key already exists.
    bool insert(int key, EventLog& log) {
        lastRotations = 0;
        TreeNode* fresh = insertNode(key, log);
        if (fresh == 0) {
            record(log, EV_DONE, -1, -1, 0, 0, "Duplicate rejected");
            return false;
        }
        fixUpward(fresh->parent, log);
        record(log, EV_DONE, -1, -1, 0, 0, "Inserted");
        return true;
    }

    // Returns true if deleted, false if the key was not there.
    bool deleteKey(int key, EventLog& log) {
        lastRotations = 0;
        TreeNode* repairFrom = 0;
        if (deleteNode(key, log, repairFrom) == false) {
            record(log, EV_DONE, -1, -1, 0, 0, "Key not found");
            return false;
        }
        fixUpward(repairFrom, log);
        record(log, EV_DONE, -1, -1, 0, 0, "Deleted");
        return true;
    }

    // k-th smallest (k = 1 is the smallest). One EV_VISIT per room stepped on.
    TreeNode* kth(int k, EventLog& log) {
        TreeNode* current = root;
        while (current != 0) {
            record(log, EV_VISIT, current->id, -1, current->value, 0, "Visit room");
            int leftSize = sizeOf(current->left);
            if (k == leftSize + 1) {
                record(log, EV_DONE, -1, -1, 0, 0, "Found k-th");
                return current;
            }
            if (k <= leftSize) {
                current = current->left;
            } else {
                k = k - (leftSize + 1);
                current = current->right;
            }
        }
        record(log, EV_DONE, -1, -1, 0, 0, "k out of range");
        return 0;
    }

    // How many keys are smaller than or equal to 'key'.
    int rank(int key) {
        int count = 0;
        TreeNode* current = root;
        while (current != 0) {
            if (key < current->value) {
                current = current->left;
            } else {
                count = count + sizeOf(current->left) + 1;
                current = current->right;
            }
        }
        return count;
    }

    int countInRange(int lo, int hi) {
        return rank(hi) - rank(lo - 1);
    }

    bool checkAvl(TreeNode* node) {
        if (node == 0) {
            return true;
        }
        if (checkAvl(node->left) == false) {
            return false;
        }
        if (checkAvl(node->right) == false) {
            return false;
        }
        int leftH = heightOf(node->left);
        int rightH = heightOf(node->right);
        int expectedHeight = rightH + 1;
        if (leftH > rightH) {
            expectedHeight = leftH + 1;
        }
        if (node->height != expectedHeight) {
            return false;
        }
        if (node->size != sizeOf(node->left) + sizeOf(node->right) + 1) {
            return false;
        }
        int bf = leftH - rightH;
        if (bf < -1 || bf > 1) {
            return false;
        }
        return true;
    }

    bool isBalanced() {
        return checkAvl(root);
    }

    // Heights and sizes are not saved in files; this rebuilds them.
    void recomputeAll(TreeNode* node) {
        if (node == 0) {
            return;
        }
        recomputeAll(node->left);
        recomputeAll(node->right);
        int leftH = heightOf(node->left);
        int rightH = heightOf(node->right);
        node->height = rightH + 1;
        if (leftH > rightH) {
            node->height = leftH + 1;
        }
        node->size = sizeOf(node->left) + sizeOf(node->right) + 1;
    }
};