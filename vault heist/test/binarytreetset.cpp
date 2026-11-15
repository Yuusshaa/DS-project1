// Tests every BinaryTree function at once.
#include <cstdio>
#include "testkit.h"
#include "../model/binarytree.h"

int listIs(DynArray<int>& list, int* expected, int n) {
    if (list.size() != n) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        if (list[i] != expected[i]) {
            return 0;
        }
    }
    return 1;
}

int sameLists(DynArray<int>& x, DynArray<int>& y) {
    if (x.size() != y.size()) {
        return 0;
    }
    for (int i = 0; i < x.size(); i++) {
        if (x[i] != y[i]) {
            return 0;
        }
    }
    return 1;
}

void toValues(BinaryTree& t, DynArray<int>& ids, DynArray<int>& values) {
    for (int i = 0; i < ids.size(); i++) {
        values.push(t.findById(t.root, ids[i])->value);
    }
}

//       10 (id 1)
//      /        \
//  20 (id 2)   30 (id 3)
//    /
//  40 (id 4)
void buildSample(BinaryTree& t) {
    t.root = t.createNode(10);
    t.setLeft(t.root, t.createNode(20));
    t.setRight(t.root, t.createNode(30));
    t.setLeft(t.root->left, t.createNode(40));
}

// Chain going right: values 1,2,3,4
void buildRightChain(BinaryTree& t) {
    t.root = t.createNode(1);
    TreeNode* current = t.root;
    for (int i = 2; i <= 4; i++) {
        TreeNode* n = t.createNode(i);
        t.setRight(current, n);
        current = n;
    }
}

void testBuilding(Tally& T) {
    std::printf("Building and owning\n");
    BinaryTree t;
    CHECK(t.isEmpty() == true);
    CHECK(t.nodeCount == 0);

    buildSample(t);
    CHECK(t.isEmpty() == false);
    CHECK(t.nodeCount == 4);
    CHECK(t.root->value == 10);
    CHECK(t.root->left->value == 20);
    CHECK(t.root->right->value == 30);
    CHECK(t.root->left->left->value == 40);
    CHECK(t.root->id == 1);
    CHECK(t.root->left->parent == t.root);
    CHECK(t.root->left->left->parent == t.root->left);
    CHECK(t.root->parent == 0);

    CHECK(t.findById(t.root, 3)->value == 30);
    CHECK(t.findById(t.root, 4)->value == 40);
    CHECK(t.findById(t.root, 99) == 0);

    t.clear();
    CHECK(t.isEmpty() == true);
    CHECK(t.nodeCount == 0);
}

void testTraversals(Tally& T) {
    std::printf("Traversals (recursive)\n");
    BinaryTree t;
    buildSample(t);
    DynArray<int> a, b, c, d;
    t.preorder(t.root, a);
    t.inorder(t.root, b);
    t.postorder(t.root, c);
    t.levelOrder(d);
    int ePre[] = {1, 2, 4, 3};
    int eIn[] = {4, 2, 1, 3};
    int ePost[] = {4, 2, 3, 1};
    int eLevel[] = {1, 2, 3, 4};
    CHECK(listIs(a, ePre, 4));
    CHECK(listIs(b, eIn, 4));
    CHECK(listIs(c, ePost, 4));
    CHECK(listIs(d, eLevel, 4));

    std::printf("Traversals (iterative match recursive)\n");
    DynArray<int> a2, b2, c2;
    t.preorderIter(a2);
    t.inorderIter(b2);
    t.postorderIter(c2);
    CHECK(sameLists(a, a2));
    CHECK(sameLists(b, b2));
    CHECK(sameLists(c, c2));

    std::printf("Traversals on a skewed chain\n");
    BinaryTree chain;
    buildRightChain(chain);
    DynArray<int> p, q, o, p2, q2, o2;
    chain.preorder(chain.root, p);
    chain.inorder(chain.root, q);
    chain.postorder(chain.root, o);
    chain.preorderIter(p2);
    chain.inorderIter(q2);
    chain.postorderIter(o2);
    int chainPre[] = {1, 2, 3, 4};
    int chainPost[] = {4, 3, 2, 1};
    CHECK(listIs(p, chainPre, 4));
    CHECK(listIs(q, chainPre, 4));
    CHECK(listIs(o, chainPost, 4));
    CHECK(sameLists(p, p2));
    CHECK(sameLists(q, q2));
    CHECK(sameLists(o, o2));

    std::printf("Traversals on empty and single-room trees\n");
    BinaryTree e;
    DynArray<int> x;
    e.preorder(e.root, x);
    e.inorder(e.root, x);
    e.postorder(e.root, x);
    e.levelOrder(x);
    e.preorderIter(x);
    e.inorderIter(x);
    e.postorderIter(x);
    CHECK(x.size() == 0);

    BinaryTree one;
    one.root = one.createNode(7);
    DynArray<int> y, y2;
    one.preorder(one.root, y);
    one.inorderIter(y2);
    int eOne[] = {1};
    CHECK(listIs(y, eOne, 1));
    CHECK(listIs(y2, eOne, 1));
}

void testHeightDiameter(Tally& T) {
    std::printf("Height and diameter\n");
    BinaryTree t;
    buildSample(t);
    CHECK(t.computeHeight(t.root) == 3);
    CHECK(t.computeHeight(t.root->left) == 2);
    CHECK(t.computeHeight(t.root->right) == 1);
    CHECK(t.diameter() == 3);

    BinaryTree e;
    CHECK(e.computeHeight(e.root) == 0);
    CHECK(e.diameter() == 0);

    BinaryTree one;
    one.root = one.createNode(5);
    CHECK(one.computeHeight(one.root) == 1);
    CHECK(one.diameter() == 0);

    BinaryTree chain;
    buildRightChain(chain);
    CHECK(chain.computeHeight(chain.root) == 4);
    CHECK(chain.diameter() == 3);

    // Longest walk does NOT pass through the root.
    BinaryTree w;
    w.root = w.createNode(1);
    TreeNode* A = w.createNode(2);
    TreeNode* B = w.createNode(3);
    TreeNode* C = w.createNode(4);
    TreeNode* D = w.createNode(5);
    TreeNode* E = w.createNode(6);
    w.setLeft(w.root, A);
    w.setLeft(A, B);
    w.setRight(A, C);
    w.setLeft(B, D);
    w.setRight(C, E);
    CHECK(w.diameter() == 4);
}

void testPathsAndLca(Tally& T) {
    std::printf("pathTo\n");
    BinaryTree t;
    buildSample(t);
    DynArray<int> p;
    CHECK(t.pathTo(t.root, 4, p) == true);
    int e4[] = {1, 2, 4};
    CHECK(listIs(p, e4, 3));

    DynArray<int> q;
    CHECK(t.pathTo(t.root, 3, q) == true);
    int e3[] = {1, 3};
    CHECK(listIs(q, e3, 2));

    DynArray<int> missing;
    CHECK(t.pathTo(t.root, 99, missing) == false);
    CHECK(missing.size() == 0);

    std::printf("lca\n");
    CHECK(t.lca(t.root, 4, 3)->id == 1);
    CHECK(t.lca(t.root, 4, 2)->id == 2);
    CHECK(t.lca(t.root, 2, 3)->id == 1);
    CHECK(t.lca(t.root, 4, 4)->id == 4);

    std::printf("pathLength\n");
    CHECK(t.pathLength(4, 3) == 3);
    CHECK(t.pathLength(4, 2) == 1);
    CHECK(t.pathLength(2, 3) == 2);
    CHECK(t.pathLength(2, 2) == 0);
    CHECK(t.pathLength(4, 99) == -1);
    CHECK(t.pathLength(99, 4) == -1);
}

void testMirrorAndCompare(Tally& T) {
    std::printf("Mirror\n");
    BinaryTree t, u;
    buildSample(t);
    buildSample(u);
    CHECK(t.isSameTree(t.root, u.root) == true);

    t.mirror(t.root);
    CHECK(t.root->left->value == 30);
    CHECK(t.root->right->value == 20);
    CHECK(t.root->right->right->value == 40);
    CHECK(t.root->right->left == 0);
    CHECK(t.isSameTree(t.root, u.root) == false);

    t.mirror(t.root);
    CHECK(t.isSameTree(t.root, u.root) == true);

    std::printf("isSameTree edge cases\n");
    BinaryTree e1, e2;
    CHECK(e1.isSameTree(e1.root, e2.root) == true);
    CHECK(t.isSameTree(t.root, e1.root) == false);
    BinaryTree diffValue;
    buildSample(diffValue);
    diffValue.root->right->value = 31;
    CHECK(t.isSameTree(t.root, diffValue.root) == false);

    std::printf("isSymmetric\n");
    BinaryTree s;
    s.root = s.createNode(1);
    s.setLeft(s.root, s.createNode(2));
    s.setRight(s.root, s.createNode(2));
    CHECK(s.isSymmetric() == true);
    CHECK(t.isSymmetric() == false);
    BinaryTree empty;
    CHECK(empty.isSymmetric() == true);
}

void testReconstruction(Tally& T) {
    std::printf("Reconstruction: round trip\n");
    BinaryTree t;
    buildSample(t);
    DynArray<int> preIds, inIds, postIds, pre, in, post;
    t.preorder(t.root, preIds);
    t.inorder(t.root, inIds);
    t.postorder(t.root, postIds);
    toValues(t, preIds, pre);
    toValues(t, inIds, in);
    toValues(t, postIds, post);

    BinaryTree a, b;
    a.buildFromPreIn(pre, in);
    b.buildFromPostIn(post, in);
    CHECK(t.isSameTree(t.root, a.root) == true);
    CHECK(t.isSameTree(t.root, b.root) == true);
    CHECK(a.nodeCount == 4);
    CHECK(a.root->left->parent == a.root);

    std::printf("Reconstruction: by hand\n");
    DynArray<int> hp, hi;
    hp.push(10); hp.push(20); hp.push(40); hp.push(30);
    hi.push(40); hi.push(20); hi.push(10); hi.push(30);
    BinaryTree h;
    h.buildFromPreIn(hp, hi);
    CHECK(t.isSameTree(t.root, h.root) == true);

    std::printf("Reconstruction: chains\n");
    DynArray<int> cp, ci, cpost;
    cp.push(1); cp.push(2); cp.push(3); cp.push(4);
    ci.push(1); ci.push(2); ci.push(3); ci.push(4);
    cpost.push(4); cpost.push(3); cpost.push(2); cpost.push(1);
    BinaryTree rc, rc2, expected;
    buildRightChain(expected);
    rc.buildFromPreIn(cp, ci);
    rc2.buildFromPostIn(cpost, ci);
    CHECK(expected.isSameTree(expected.root, rc.root) == true);
    CHECK(expected.isSameTree(expected.root, rc2.root) == true);

    DynArray<int> lp;
    lp.push(4); lp.push(3); lp.push(2); lp.push(1);
    BinaryTree lc;
    lc.buildFromPreIn(lp, ci);
    CHECK(lc.root->value == 4);
    CHECK(lc.root->right == 0);
    CHECK(lc.root->left->left->left->value == 1);

    std::printf("Reconstruction: empty and single\n");
    BinaryTree e;
    DynArray<int> none;
    e.buildFromPreIn(none, none);
    CHECK(e.isEmpty() == true);
    e.buildFromPostIn(none, none);
    CHECK(e.isEmpty() == true);

    DynArray<int> sp, si;
    sp.push(9);
    si.push(9);
    BinaryTree single;
    single.buildFromPreIn(sp, si);
    CHECK(single.root->value == 9);
    CHECK(single.nodeCount == 1);

    std::printf("Reconstruction: rebuilding twice on the same tree object\n");
    a.buildFromPreIn(pre, in);
    CHECK(a.nodeCount == 4);
    CHECK(t.isSameTree(t.root, a.root) == true);
}

void testCopy(Tally& T) {
    std::printf("Copy\n");
    BinaryTree a;
    buildSample(a);
    BinaryTree b;
    b.copyTreeFrom(a);
    CHECK(a.isSameTree(a.root, b.root) == true);
    CHECK(a.root != b.root);
    CHECK(b.nodeCount == 4);
    CHECK(b.root->left->parent == b.root);

    b.mirror(b.root);
    CHECK(a.isSameTree(a.root, b.root) == false);
    CHECK(a.root->left->value == 20);
    CHECK(a.root->right->value == 30);

    b.copyTreeFrom(a);
    CHECK(a.isSameTree(a.root, b.root) == true);
    CHECK(b.nodeCount == 4);

    BinaryTree e, f;
    f.copyTreeFrom(e);
    CHECK(f.isEmpty() == true);
}

int main() {
    Tally T;
    testBuilding(T);
    testTraversals(T);
    testHeightDiameter(T);
    testPathsAndLca(T);
    testMirrorAndCompare(T);
    testReconstruction(T);
    testCopy(T);

    std::printf("\nPassed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed;
}