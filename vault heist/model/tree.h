//this is supposed to be a tree node and its corresponding functions
#pragma once

struct TreeNode {
    int id;            // permanent identity, never changes
    int value;         // the key / treasure value
    TreeNode* left;    // left child, or 0 if none
    TreeNode* right;   // right child, or 0 if none
    TreeNode* parent;  // parent, or 0 if this is the root
    int height;        // leaf = 1, empty spot = 0 (our DECISIONS.md rule)
    int size;          // number of rooms in this subtree, leaf = 1

    TreeNode(int newId, int newValue) {
        id = newId;
        value = newValue;
        left = 0;
        right = 0;
        parent = 0;
        height = 1;
        size = 1;
    }
};

// Height of a spot that might be empty. An empty spot has height 0.
inline int heightOf(TreeNode* node) {
    if (node == 0) {
        return 0;
    }
    return node->height;
}

// Size of a spot that might be empty. An empty spot has size 0.
inline int sizeOf(TreeNode* node) {
    if (node == 0) {
        return 0;
    }
    return node->size;
}

// Balance factor = height(left) - height(right).
inline int balanceFactorOf(TreeNode* node) {
    if (node == 0) {
        return 0;
    }
    return heightOf(node->left) - heightOf(node->right);
}