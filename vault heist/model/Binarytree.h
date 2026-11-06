// model/BinaryTree.h  -- must NOT include raylib
#pragma once
#include "tree.h"
#include "dynamicarray.h"

class BinaryTree {
public:
    TreeNode* root;   // the entrance, or 0 if the tree is empty
    int nextId;       // the id the next created room will get
    int nodeCount;    // how many rooms exist

    BinaryTree() {
        root = 0;
        nextId = 1;
        nodeCount = 0;
    }

    ~BinaryTree() {
        clear();
    }

    // dont accidently create shallow copies ikik im smart type shi
    BinaryTree(BinaryTree& other) = delete;
    BinaryTree& operator=(BinaryTree& other) = delete;

    // Delete every room under and including this one. Children first, then the room.
    void freeAll(TreeNode* node) {
        if (node == 0) {
            return;
        }
        freeAll(node->left);
        freeAll(node->right);
        delete node;
    }

    // Find a room by id, searching from the given room downward. Returns 0 if not found.
    TreeNode* findById(TreeNode* node, int id) {
        if (node == 0) {
            return 0;
        }
        if (node->id == id) {
            return node;
        }
        TreeNode* found = findById(node->left, id);
        if (found != 0) {
            return found;
        }
        return findById(node->right, id);
    }

    // Make a new room with the next free id. It is not linked anywhere yet.
    TreeNode* createNode(int value) {
        TreeNode* n = new TreeNode(nextId, value);
        nextId = nextId + 1;
        nodeCount = nodeCount + 1;
        return n;
    }

    // Link child under parent, and set the child's parent link too.
    void setLeft(TreeNode* parent, TreeNode* child) {
        parent->left = child;
        if (child != 0) {
            child->parent = parent;
        }
    }

    void setRight(TreeNode* parent, TreeNode* child) {
        parent->right = child;
        if (child != 0) {
            child->parent = parent;
        }
    }

    // Delete every room and reset the tree to empty.
    void clear() {
        freeAll(root);
        root = 0;
        nextId = 1;
        nodeCount = 0;
    }

    bool isEmpty() {
        return root == 0;
    }

    //all the traversles after this so dont get it mixed up
    //result are the stored place(were also using the push and pop made in dynamic array headerfile)

    // node, left, right
    void preorder(TreeNode* node, DynArray<int>& result) {
        if (node == 0) {
            return;
        }
        result.push(node->id);
        preorder(node->left, result);
        preorder(node->right, result);
    }

    // left, node, right
    void inorder(TreeNode* node, DynArray<int>& result) {
        if (node == 0) {
            return;
        }
        inorder(node->left, result);
        result.push(node->id);
        inorder(node->right, result);
    }

    // left, right, node
    void postorder(TreeNode* node, DynArray<int>& result) {
        if (node == 0) {
            return;
        }
        postorder(node->left, result);
        postorder(node->right, result);
        result.push(node->id);
    }

    // row by row, using a queue made from a dynarray
    void levelOrder(DynArray<int>& result) {
        if (root == 0) {
            return;
        }
        DynArray<TreeNode*> queue;
        int head = 0; 
        queue.push(root);
        //basically made our own queue which can only push )
        while (head < queue.size()) {
            TreeNode* current = queue[head];
            head = head + 1; 
            result.push(current->id);
            if (current->left != 0) {
                queue.push(current->left);
            }
            if (current->right != 0) {
                queue.push(current->right);
            }
        }
    }
};