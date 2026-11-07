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
    //ITERATIVE TRAVERSALS

    // node, left, right
    void preorderIter(DynArray<int>& result) {
        if (root == 0) {
            return;
        }
        DynArray<TreeNode*> stack;
        stack.push(root);
        while (stack.empty() == false) {
            TreeNode* current = stack.pop();
            result.push(current->id);
            // push pehla push krraha bcz while popping left pehla aega im so smart 
            if (current->right != 0) {
                stack.push(current->right);
            }
            if (current->left != 0) {
                stack.push(current->left);
            }
        }
    }

    // left, node, right
    void inorderIter(DynArray<int>& result) {
        DynArray<TreeNode*> stack;
        TreeNode* current = root;
        while (current != 0 || stack.empty() == false) {
            // go as far left as possible, remembering the path
            while (current != 0) {
                stack.push(current);
                current = current->left;
            }
            // nothing more on the left: visit this room
            current = stack.pop();
            result.push(current->id);
            // then do the same for its right side
            current = current->right;
        }
    }

    // left, right, node   
    void postorderIter(DynArray<int>& result) {
        if (root == 0) {
            return;
        }
        // were do node, right, left, then reverse it.
        DynArray<TreeNode*> stack;
        DynArray<int> backwards;
        stack.push(root);
        while (stack.empty() == false) {
            TreeNode* current = stack.pop();
            backwards.push(current->id);
            if (current->left != 0) {
                stack.push(current->left);
            }
            if (current->right != 0) {
                stack.push(current->right);
            }
        }
        // copy into result from the end to the start
        for (int i = backwards.size() - 1; i >= 0; i--) {
            result.push(backwards[i]);
        }
    }
    
    // ---------- Height, diameter, path, LCA ----------

    
    int computeHeight(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        int leftH = computeHeight(node->left);
        int rightH = computeHeight(node->right);
        if (leftH > rightH) {
            return leftH + 1;
        }
        return rightH + 1;
    }

    // Returns the height of 'node', and also updates 'best' with the longest
    // walk that bends at any room (leftHeight + rightHeight).
    int diameterWalk(TreeNode* node, int& best) {
        if (node == 0) {
            return 0;
        }
        int leftH = diameterWalk(node->left, best);
        int rightH = diameterWalk(node->right, best);
        if (leftH + rightH > best) {
            best = leftH + rightH;
        }
        if (leftH > rightH) {
            return leftH + 1;
        }
        return rightH + 1;
    }

    // Longest walk between any two rooms, in edges.
    int diameter() {
        int best = 0;
        diameterWalk(root, best);
        return best;
    }

    // Fills 'path' with ids from 'node' down to the room with this id.
    // Returns true if found. If not found, path is left as it was.
    bool pathTo(TreeNode* node, int id, DynArray<int>& path) {
        if (node == 0) {
            return false;
        }
        path.push(node->id);              // step onto this room
        if (node->id == id) {
            return true;
        }
        if (pathTo(node->left, id, path)) {
            return true;
        }
        if (pathTo(node->right, id, path)) {
            return true;
        }
        path.pop();                       // dead end: step back off
        return false;
    }

    // Lowest common ancestor of the rooms with ids a and b.
    // Both rooms must exist in the tree.
    TreeNode* lca(TreeNode* node, int a, int b) {
        if (node == 0) {
            return 0;
        }
        if (node->id == a || node->id == b) {
            return node;
        }
        TreeNode* fromLeft = lca(node->left, a, b);
        TreeNode* fromRight = lca(node->right, a, b);
        if (fromLeft != 0 && fromRight != 0) {
            return node;                  // one on each side: this is the meeting room
        }
        if (fromLeft != 0) {
            return fromLeft;
        }
        return fromRight;
    }

    // Steps between two rooms, travelling through their LCA. Returns -1 if either is missing.
    int pathLength(int a, int b) {
        DynArray<int> pathA, pathB, pathMeet;
        if (pathTo(root, a, pathA) == false) {
            return -1;
        }
        if (pathTo(root, b, pathB) == false) {
            return -1;
        }
        TreeNode* meet = lca(root, a, b);
        pathTo(root, meet->id, pathMeet);
        int depthA = pathA.size() - 1;
        int depthB = pathB.size() - 1;
        int depthMeet = pathMeet.size() - 1;
        return depthA + depthB - 2 * depthMeet;
    }
        // ---------- Mirror and comparison ----------

    // Swap left and right children of every room under 'node'.
    void mirror(TreeNode* node) {
        if (node == 0) {
            return;
        }
        TreeNode* temp = node->left;
        node->left = node->right;
        node->right = temp;
        mirror(node->left);
        mirror(node->right);
    }

    // Same shape and same values at every position?
    // (We compare values, not ids, because two separate trees have different ids.)
    bool isSameTree(TreeNode* a, TreeNode* b) {
        if (a == 0 && b == 0) {
            return true;
        }
        if (a == 0 || b == 0) {
            return false;
        }
        if (a->value != b->value) {
            return false;
        }
        return isSameTree(a->left, b->left) && isSameTree(a->right, b->right);
    }

    // Is the left side a mirror image of the right side?
    bool isMirrorOf(TreeNode* a, TreeNode* b) {
        if (a == 0 && b == 0) {
            return true;
        }
        if (a == 0 || b == 0) {
            return false;
        }
        if (a->value != b->value) {
            return false;
        }
        return isMirrorOf(a->left, b->right) && isMirrorOf(a->right, b->left);
    }

    bool isSymmetric() {
        if (root == 0) {
            return true;
        }
        return isMirrorOf(root->left, root->right);
    }



    //build a tree using preoder + postorder


    // Builds a tree from a preorder list and an inorder list.
    TreeNode* buildpre(DynArray<int>& pre, DynArray<int>& in) {
        if (pre.size() == 0) {
            return 0;                       // no numbers = no room
        }

        int rootValue = pre[0];             // step 1: first preorder number is the root
        TreeNode* node = createNode(rootValue);

        // step 2: find the root in the inorder list
        int pos = 0;
        while (in[pos] != rootValue) {
            pos = pos + 1;
        }

        // step 3: make the small lists
        DynArray<int> leftPre, leftIn, rightPre, rightIn;
        for (int i = 0; i < pos; i++) {
            leftIn.push(in[i]);             // inorder numbers before the root
        }
        for (int i = pos + 1; i < in.size(); i++) {
            rightIn.push(in[i]);            // inorder numbers after the root
        }
        for (int i = 1; i <= pos; i++) {
            leftPre.push(pre[i]);           // the next 'pos' preorder numbers
        }
        for (int i = pos + 1; i < pre.size(); i++) {
            rightPre.push(pre[i]);          // the rest of the preorder numbers
        }

        // step 4: build each side the same way
        setLeft(node, buildpre(leftPre, leftIn));
        setRight(node, buildpre(rightPre, rightIn));
        return node;
    }

    void buildFromPreIn(DynArray<int>& pre, DynArray<int>& in) {
        clear();
        root = buildpre(pre, in);
    }

    // Builds a tree from a postorder list and an inorder list.
    // Same idea, but the root is the LAST postorder number.
    TreeNode* buildpost(DynArray<int>& post, DynArray<int>& in) {
        if (post.size() == 0) {
            return 0;
        }

        int rootValue = post[post.size() - 1];   // last number is the root
        TreeNode* node = createNode(rootValue);

        int pos = 0;
        while (in[pos] != rootValue) {
            pos = pos + 1;
        }

        DynArray<int> leftPost, leftIn, rightPost, rightIn;
        for (int i = 0; i < pos; i++) {
            leftIn.push(in[i]);
        }
        for (int i = pos + 1; i < in.size(); i++) {
            rightIn.push(in[i]);
        }
        for (int i = 0; i < pos; i++) {
            leftPost.push(post[i]);              // first 'pos' postorder numbers
        }
        for (int i = pos; i < post.size() - 1; i++) {
            rightPost.push(post[i]);             // the rest, except the last (the root)
        }

        setLeft(node, buildpost(leftPost, leftIn));
        setRight(node, buildpost(rightPost, rightIn));
        return node;
    }

    void buildFromPostIn(DynArray<int>& post, DynArray<int>& in) {
        clear();
        root = buildpost(post, in);
    }
        
    //tree copier

    // Makes a brand new room for every room under node.
    TreeNode* copyFrom(TreeNode* node) {
        if (node == 0) {
            return 0;                          // nothing to copy
        }
        TreeNode* fresh = createNode(node->value);   // new room, same value
        fresh->height = node->height;                // keep height and size too
        fresh->size = node->size;
        setLeft(fresh, copyFrom(node->left));        // copy the left side, attach it
        setRight(fresh, copyFrom(node->right));      // copy the right side, attach it
        return fresh;
    }

    // Make THIS tree a copy of any other tree 
    void copyTreeFrom(BinaryTree& other) {
        clear();
        root = copyFrom(other.root);
    }
};