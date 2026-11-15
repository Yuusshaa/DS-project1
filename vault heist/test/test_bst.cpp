#include <cstdio>
#include "testkit.h"
#include "../model/bst.h"

int parentsOk(TreeNode* node, TreeNode* expectedParent) {
    if (node == 0) {
        return 1;
    }
    if (node->parent != expectedParent) {
        return 0;
    }
    return parentsOk(node->left, node) && parentsOk(node->right, node);
}

int countRooms(TreeNode* node) {
    if (node == 0) {
        return 0;
    }
    return 1 + countRooms(node->left) + countRooms(node->right);
}

// BST order (inorder strictly increasing) + parent links + room count.
int isValidBST(BST& t) {
    DynArray<int> ids;
    t.inorder(t.root, ids);
    for (int i = 1; i < ids.size(); i++) {
        int before = t.findById(t.root, ids[i - 1])->value;
        int now = t.findById(t.root, ids[i])->value;
        if (before >= now) {
            return 0;
        }
    }
    if (parentsOk(t.root, 0) == 0) {
        return 0;
    }
    if (countRooms(t.root) != t.nodeCount) {
        return 0;
    }
    return 1;
}

int valuesAre(BST& t, int* expected, int n) {
    DynArray<int> ids;
    t.inorder(t.root, ids);
    if (ids.size() != n) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        if (t.findById(t.root, ids[i])->value != expected[i]) {
            return 0;
        }
    }
    return 1;
}

int countType(EventLog& log, int type) {
    int n = 0;
    for (int i = 0; i < log.size(); i++) {
        if (log.get(i).type == type) {
            n = n + 1;
        }
    }
    return n;
}

//         50
//       /    \
//     30      70
//    /  \    /  \
//  20   40  60   80
void buildFull(BST& t) {
    EventLog log;
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        t.insert(keys[i], log);
    }
}

void testInsertSearch(Tally& T) {
    std::printf("Insert and search\n");
    BST t;
    EventLog log;
    CHECK(t.insert(50, log) == true);
    CHECK(t.insert(70, log) == true);
    CHECK(t.insert(60, log) == true);
    CHECK(t.insert(30, log) == true);
    CHECK(t.insert(70, log) == false);
    CHECK(t.insert(50, log) == false);
    CHECK(t.nodeCount == 4);
    int e1[] = {30, 50, 60, 70};
    CHECK(valuesAre(t, e1, 4));
    CHECK(isValidBST(t));

    EventLog s;
    TreeNode* found = t.search(60, s);
    CHECK(found != 0);
    CHECK(found->value == 60);
    CHECK(s.countAlarm() == 3);          // compares at 50, 70, 60
    CHECK(s.countVisits() == 0);         // searches never emit EV_VISIT
    CHECK(s.get(0).type == EV_COMPARE);
    CHECK(s.get(0).code == CMP_LESS);    // node 50 is LESS than target 60 (go right)
    CHECK(s.get(1).code == CMP_GREATER); // node 70 is GREATER than target 60 (go left)
    CHECK(s.get(2).code == CMP_EQUAL);

    EventLog miss;
    CHECK(t.search(99, miss) == 0);
    CHECK(miss.countAlarm() == 2);       // 50, 70

    BST empty;
    EventLog e;
    CHECK(empty.search(5, e) == 0);
    CHECK(e.countAlarm() == 0);

    CHECK(s.get(s.size() - 1).type == EV_DONE);

    EventLog off;
    off.enabled = false;
    t.search(60, off);
    CHECK(off.size() == 0);

    // insert events: compares, then one EV_INSERT, then EV_DONE
    BST u;
    EventLog first;
    u.insert(50, first);
    CHECK(countType(first, EV_INSERT) == 1);
    CHECK(first.get(0).code == SIDE_NONE);
    EventLog second;
    u.insert(70, second);
    CHECK(countType(second, EV_COMPARE) == 1);
    CHECK(countType(second, EV_INSERT) == 1);
    CHECK(second.countAlarm() == 1);
}

void testMinMaxSuccPred(Tally& T) {
    std::printf("Min, max, successor, predecessor\n");
    BST t;
    buildFull(t);
    CHECK(t.findMin(t.root)->value == 20);
    CHECK(t.findMax(t.root)->value == 80);
    CHECK(t.successor(t.root)->value == 60);
    CHECK(t.successor(t.root->left->right)->value == 50);
    CHECK(t.successor(t.root->right->right) == 0);
    CHECK(t.predecessor(t.root)->value == 40);
    CHECK(t.predecessor(t.root->left->left) == 0);
    CHECK(t.predecessor(t.root->right->left)->value == 50);

    BST e;
    CHECK(e.findMin(e.root) == 0);
    CHECK(e.findMax(e.root) == 0);
    CHECK(e.successor(0) == 0);

    TreeNode* n = t.findMin(t.root);
    int expected[] = {20, 30, 40, 50, 60, 70, 80};
    int i = 0;
    int ok = 1;
    while (n != 0) {
        if (i >= 7 || n->value != expected[i]) {
            ok = 0;
        }
        i = i + 1;
        n = t.successor(n);
    }
    CHECK(ok == 1);
    CHECK(i == 7);
}

void testDelete(Tally& T) {
    std::printf("Delete\n");
    EventLog log;

    // leaf
    BST a;
    buildFull(a);
    CHECK(a.deleteKey(20, log) == true);
    int ea[] = {30, 40, 50, 60, 70, 80};
    CHECK(valuesAre(a, ea, 6));
    CHECK(a.nodeCount == 6);
    CHECK(isValidBST(a));

    // one child
    CHECK(a.deleteKey(30, log) == true);
    int eb[] = {40, 50, 60, 70, 80};
    CHECK(valuesAre(a, eb, 5));
    CHECK(a.root->left->value == 40);
    CHECK(isValidBST(a));

    // one LEFT child
    BST b;
    b.insert(50, log);
    b.insert(30, log);
    b.insert(20, log);
    CHECK(b.deleteKey(30, log) == true);
    CHECK(b.root->left->value == 20);
    CHECK(isValidBST(b));

    // two children: 70 replaced by successor 80
    BST c;
    buildFull(c);
    int idOf70 = c.root->right->id;
    CHECK(c.deleteKey(70, log) == true);
    CHECK(c.root->right->value == 80);
    CHECK(c.root->right->id == idOf70);
    CHECK(c.root->right->right == 0);
    CHECK(c.root->right->left->value == 60);
    int ec[] = {20, 30, 40, 50, 60, 80};
    CHECK(valuesAre(c, ec, 6));
    CHECK(isValidBST(c));

    // root with two children
    BST d;
    buildFull(d);
    CHECK(d.deleteKey(50, log) == true);
    CHECK(d.root->value == 60);
    CHECK(d.root->id == 1);
    CHECK(isValidBST(d));

    // root with one child
    BST f;
    f.insert(50, log);
    f.insert(40, log);
    CHECK(f.deleteKey(50, log) == true);
    CHECK(f.root->value == 40);
    CHECK(f.root->parent == 0);
    CHECK(isValidBST(f));

    // single room, then empty
    BST g;
    g.insert(5, log);
    CHECK(g.deleteKey(5, log) == true);
    CHECK(g.root == 0);
    CHECK(g.nodeCount == 0);
    CHECK(g.deleteKey(5, log) == false);

    // missing key
    BST h;
    buildFull(h);
    CHECK(h.deleteKey(99, log) == false);
    CHECK(h.nodeCount == 7);
    CHECK(isValidBST(h));

    // delete everything one by one
    BST k;
    buildFull(k);
    int order[] = {50, 20, 80, 30, 70, 40, 60};
    int allOk = 1;
    for (int i = 0; i < 7; i++) {
        if (k.deleteKey(order[i], log) == false) {
            allOk = 0;
        }
        if (isValidBST(k) == 0) {
            allOk = 0;
        }
    }
    CHECK(allOk == 1);
    CHECK(k.isEmpty());

    // two-child delete events
    BST m;
    buildFull(m);
    EventLog dl;
    m.deleteKey(70, dl);
    CHECK(countType(dl, EV_SWAP_VALUE) == 1);
    CHECK(countType(dl, EV_UNLINK) == 1);
    CHECK(countType(dl, EV_VISIT) == 0);
    CHECK(dl.countAlarm() == 2);          // compares at 50 and 70
    int swapIndex = -1;
    for (int i = 0; i < dl.size(); i++) {
        if (dl.get(i).type == EV_SWAP_VALUE) {
            swapIndex = i;
        }
    }
    CHECK(swapIndex >= 0);
    CHECK(dl.get(swapIndex).oldValue == 70);   // old value at N
    CHECK(dl.get(swapIndex).newValue == 80);   // new value at N
    CHECK(dl.get(swapIndex).value == 80);      // old value at O
}

void testRange(Tally& T) {
    std::printf("Range query\n");
    BST t;
    buildFull(t);
    EventLog log;
    DynArray<int> result;
    t.rangeQuery(35, 65, result, log);
    CHECK(result.size() == 3);
    CHECK(result[0] == 40);
    CHECK(result[1] == 50);
    CHECK(result[2] == 60);
    CHECK(log.countAlarm() == 5);         // 50, 30, 40, 70, 60: fewer than all 7
    CHECK(log.countVisits() == 0);

    int skipped = 0;
    for (int i = 0; i < log.size(); i++) {
        if (log.get(i).type == EV_HIGHLIGHT_SUBTREE && log.get(i).code == HL_SKIPPED) {
            skipped = skipped + 1;
        }
    }
    CHECK(skipped >= 1);

    DynArray<int> all;
    EventLog log2;
    t.rangeQuery(0, 1000, all, log2);
    CHECK(all.size() == 7);

    DynArray<int> none;
    EventLog log3;
    t.rangeQuery(81, 100, none, log3);
    CHECK(none.size() == 0);

    BST e;
    DynArray<int> r;
    EventLog log4;
    e.rangeQuery(1, 5, r, log4);
    CHECK(r.size() == 0);
}

void testKthAndLca(Tally& T) {
    std::printf("kthSmallest and lcaBST\n");
    BST t;
    buildFull(t);
    CHECK(t.kthSmallest(1)->value == 20);
    CHECK(t.kthSmallest(4)->value == 50);
    CHECK(t.kthSmallest(7)->value == 80);
    CHECK(t.kthSmallest(8) == 0);
    CHECK(t.kthSmallest(0) == 0);

    CHECK(t.lcaBST(20, 40)->value == 30);
    CHECK(t.lcaBST(20, 80)->value == 50);
    CHECK(t.lcaBST(60, 80)->value == 70);
    CHECK(t.lcaBST(30, 30)->value == 30);
    CHECK(t.lcaBST(30, 20)->value == 30);
}

void testDegenerate(Tally& T) {
    std::printf("Sorted input makes a chain (Level 2.5 boss)\n");
    BST t;
    EventLog log;
    for (int i = 1; i <= 50; i++) {
        t.insert(i, log);
    }
    CHECK(t.computeHeight(t.root) == 50);
    EventLog s;
    t.search(50, s);
    CHECK(s.countAlarm() == 50);
    CHECK(isValidBST(t));
}

int main() {
    Tally T;
    testInsertSearch(T);
    testMinMaxSuccPred(T);
    testDelete(T);
    testRange(T);
    testKthAndLca(T);
    testDegenerate(T);

    std::printf("\nPassed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed;
}