// Comprehensive AVL tree tests, including inherited BST behavior.
#include <cstdio>
#include "testkit.h"
#include "../model/avltree.h"

int eventCount(EventLog& log, int type, int code = -999) {
    int count = 0;
    for (int i = 0; i < log.size(); i++) {
        if (log.get(i).type == type && (code == -999 || log.get(i).code == code)) count++;
    }
    return count;
}

int validateNode(TreeNode* node, TreeNode* parent, int& nodes) {
    if (node == 0) { nodes = 0; return 0; }
    if (node->parent != parent) return -1;
    int leftNodes = 0, rightNodes = 0;
    int leftHeight = validateNode(node->left, node, leftNodes);
    int rightHeight = validateNode(node->right, node, rightNodes);
    if (leftHeight < 0 || rightHeight < 0) return -1;
    nodes = leftNodes + rightNodes + 1;
    int height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    if (node->height != height || node->size != nodes) return -1;
    if (leftHeight - rightHeight < -1 || leftHeight - rightHeight > 1) return -1;
    if (node->left != 0 && node->left->value >= node->value) return -1;
    if (node->right != 0 && node->right->value <= node->value) return -1;
    return height;
}

int validTree(AVLTree& tree) {
    int nodes = 0;
    int height = validateNode(tree.root, 0, nodes);
    return height >= 0 && nodes == tree.nodeCount && tree.isBalanced();
}

void testEmptyAndSingle(Tally& T) {
    std::printf("AVL empty and single-node cases\n");
    AVLTree tree;
    CHECK(tree.isEmpty());
    CHECK(tree.nodeCount == 0);
    CHECK(tree.isBalanced());
    CHECK(tree.root == 0);
    CHECK(tree.rank(10) == 0);
    CHECK(tree.countInRange(-10, 10) == 0);
    EventLog emptyKth;
    CHECK(tree.kth(1, emptyKth) == 0);
    CHECK(emptyKth.countVisits() == 0);
    CHECK(tree.search(4, emptyKth) == 0);
    CHECK(tree.deleteKey(4, emptyKth) == false);

    EventLog log;
    CHECK(tree.insert(42, log));
    CHECK(tree.root->value == 42);
    CHECK(tree.root->height == 1 && tree.root->size == 1);
    CHECK(tree.totalRotations == 0 && tree.lastRotations == 0);
    CHECK(tree.search(42, log) == tree.root);
    CHECK(tree.search(41, log) == 0);
    CHECK(tree.kth(1, log) == tree.root);
    CHECK(tree.rank(41) == 0 && tree.rank(42) == 1 && tree.rank(43) == 1);
    CHECK(tree.countInRange(42, 42) == 1);
    CHECK(tree.insert(42, log) == false);
    CHECK(tree.nodeCount == 1 && validTree(tree));
    tree.root->height = 9;
    tree.root->size = 9;
    CHECK(tree.isBalanced() == false);
    tree.recomputeAll(tree.root);
    CHECK(validTree(tree));
    CHECK(tree.deleteKey(42, log));
    CHECK(tree.isEmpty() && tree.nodeCount == 0);
}

void testFourInsertRotationTypes(Tally& T) {
    std::printf("All four AVL insertion rotations\n");
    int keys[4][3] = {{30,20,10}, {10,20,30}, {30,10,20}, {10,30,20}};
    int codes[] = {ROT_LL, ROT_RR, ROT_LR, ROT_RL};
    for (int c = 0; c < 4; c++) {
        AVLTree tree;
        EventLog log;
        for (int i = 0; i < 3; i++) CHECK(tree.insert(keys[c][i], log));
        CHECK(tree.root != 0 && tree.root->value == 20);
        CHECK(tree.root != 0 && tree.root->left != 0 && tree.root->left->value == 10);
        CHECK(tree.root != 0 && tree.root->right != 0 && tree.root->right->value == 30);
        CHECK(tree.lastRotations == 1);
        CHECK(eventCount(log, EV_ROTATE, codes[c]) == 1);
        CHECK(tree.totalRotations == 1);
        CHECK(validTree(tree));
        CHECK(log.get(log.size() - 1).type == EV_DONE);
    }
}

void testSearchAndInheritedBstOperations(Tally& T) {
    std::printf("AVL search and inherited BST operations\n");
    AVLTree tree;
    EventLog log;
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) CHECK(tree.insert(keys[i], log));
    CHECK(validTree(tree));
    CHECK(tree.findMin(tree.root)->value == 20);
    CHECK(tree.findMax(tree.root)->value == 80);
    CHECK(tree.search(60, log) != 0 && tree.search(60, log)->value == 60);
    CHECK(tree.search(99, log) == 0);
    CHECK(tree.successor(tree.search(40, log))->value == 50);
    CHECK(tree.successor(tree.search(80, log)) == 0);
    CHECK(tree.predecessor(tree.search(60, log))->value == 50);
    CHECK(tree.predecessor(tree.search(20, log)) == 0);
    CHECK(tree.lcaBST(20, 40)->value == 30);
    CHECK(tree.lcaBST(20, 80)->value == 50);
    CHECK(tree.kthSmallest(1)->value == 20);
    CHECK(tree.kthSmallest(4)->value == 50);
    CHECK(tree.kthSmallest(8) == 0 && tree.kthSmallest(0) == 0);
    CHECK(tree.nodeCount == 7);
    EventLog searchLog;
    tree.search(60, searchLog);
    CHECK(searchLog.countAlarm() == 3);
    CHECK(searchLog.countVisits() == 0);

    DynArray<int> range;
    EventLog rangeLog;
    tree.rangeQuery(35, 65, range, rangeLog);
    CHECK(range.size() == 3);
    CHECK(range[0] == 40 && range[1] == 50 && range[2] == 60);
    CHECK(rangeLog.countAlarm() < tree.nodeCount);
    CHECK(eventCount(rangeLog, EV_HIGHLIGHT_SUBTREE, HL_SKIPPED) > 0);
    DynArray<int> noRange;
    EventLog noRangeLog;
    tree.rangeQuery(100, 120, noRange, noRangeLog);
    CHECK(noRange.size() == 0);
    CHECK(validTree(tree));
}

void testDeleteCases(Tally& T) {
    std::printf("AVL deletion: leaf, one/two children, root, missing key\n");
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    AVLTree tree;
    EventLog log;
    for (int i = 0; i < 7; i++) tree.insert(keys[i], log);
    CHECK(tree.deleteKey(20, log)); // leaf
    CHECK(validTree(tree));
    CHECK(tree.deleteKey(30, log)); // one remaining child
    CHECK(validTree(tree));
    CHECK(tree.deleteKey(70, log)); // two children
    CHECK(validTree(tree));
    CHECK(tree.deleteKey(50, log)); // root
    CHECK(validTree(tree));
    int before = tree.nodeCount;
    CHECK(tree.deleteKey(999, log) == false);
    CHECK(tree.nodeCount == before && validTree(tree));

    AVLTree childRoot;
    EventLog rootLog;
    childRoot.insert(2, rootLog);
    childRoot.insert(1, rootLog);
    CHECK(childRoot.deleteKey(2, rootLog));
    CHECK(childRoot.root != 0 && childRoot.root->value == 1 && childRoot.root->parent == 0);
    CHECK(validTree(childRoot));

    AVLTree one;
    EventLog oneLog;
    one.insert(7, oneLog);
    CHECK(one.deleteKey(7, oneLog));
    CHECK(one.root == 0 && one.nodeCount == 0 && one.isBalanced());
}

void testDeleteRebalanceCases(Tally& T) {
    std::printf("AVL deletion rebalance with heavy-child balance zero\n");
    AVLTree ll;
    EventLog a;
    int aKeys[] = {4,2,6,1,3};
    for (int i = 0; i < 5; i++) ll.insert(aKeys[i], a);
    CHECK(ll.deleteKey(6, a));
    CHECK(ll.root != 0 && ll.root->value == 2);
    CHECK(eventCount(a, EV_ROTATE, ROT_LL) == 1);
    CHECK(ll.lastRotations == 1 && validTree(ll));

    AVLTree rr;
    EventLog b;
    int bKeys[] = {4,2,6,5,7};
    for (int i = 0; i < 5; i++) rr.insert(bKeys[i], b);
    CHECK(rr.deleteKey(2, b));
    CHECK(rr.root != 0 && rr.root->value == 6);
    CHECK(eventCount(b, EV_ROTATE, ROT_RR) == 1);
    CHECK(rr.lastRotations == 1 && validTree(rr));

    // Delete a node with two children and verify the successor replacement.
    AVLTree twoChild;
    EventLog c;
    for (int i = 0; i < 7; i++) twoChild.insert((i * 13) % 7 + 1, c);
    if (twoChild.search(4, c) != 0) CHECK(twoChild.deleteKey(4, c));
    CHECK(validTree(twoChild));
}

void checkStatisticsAgainstSorted(AVLTree& tree, int* sorted, int count, Tally& T) {
    CHECK(validTree(tree));
    CHECK(tree.nodeCount == count);
    for (int k = 1; k <= count; k++) {
        EventLog log;
        TreeNode* node = tree.kth(k, log);
        CHECK(node != 0 && node->value == sorted[k - 1]);
        CHECK(log.countVisits() == log.countAlarm());
        CHECK(log.get(log.size() - 1).type == EV_DONE);
    }
    EventLog zeroK;
    CHECK(tree.kth(0, zeroK) == 0);
    EventLog outOfRange;
    CHECK(tree.kth(count + 1, outOfRange) == 0);
    CHECK(tree.rank(-1000) == 0 && tree.rank(1000) == count);
    for (int i = 0; i < count; i++) {
        CHECK(tree.rank(sorted[i]) == i + 1);
        if (i + 1 < count && sorted[i + 1] - sorted[i] > 1) {
            CHECK(tree.rank(sorted[i] + 1) == i + 1);
        }
    }
    if (count > 0) {
        CHECK(tree.countInRange(sorted[0], sorted[count - 1]) == count);
        CHECK(tree.countInRange(sorted[count - 1], sorted[count - 1]) == 1);
        CHECK(tree.countInRange(sorted[count - 1] + 1, sorted[count - 1]) == 0);
    }
}

int addSorted(int* values, int count, int value) {
    int pos = 0;
    while (pos < count && values[pos] < value) pos++;
    for (int i = count; i > pos; i--) values[i] = values[i - 1];
    values[pos] = value;
    return count + 1;
}

int removeSorted(int* values, int count, int value) {
    int pos = 0;
    while (pos < count && values[pos] != value) pos++;
    if (pos == count) return count;
    for (int i = pos; i + 1 < count; i++) values[i] = values[i + 1];
    return count - 1;
}

void testRankKthAfterEveryMutation(Tally& T) {
    std::printf("AVL kth, rank, and range count after each change\n");
    AVLTree tree;
    EventLog log;
    int sorted[32];
    int count = 0;
    int inserts[] = {8,3,10,1,6,14,4,7,13};
    for (int i = 0; i < 9; i++) {
        CHECK(tree.insert(inserts[i], log));
        count = addSorted(sorted, count, inserts[i]);
        checkStatisticsAgainstSorted(tree, sorted, count, T);
    }
    CHECK(tree.insert(6, log) == false);
    CHECK(tree.nodeCount == count && validTree(tree));
    int deletes[] = {8,1,14,6,3,10,4,7,13};
    for (int i = 0; i < 9; i++) {
        CHECK(tree.deleteKey(deletes[i], log));
        count = removeSorted(sorted, count, deletes[i]);
        checkStatisticsAgainstSorted(tree, sorted, count, T);
    }
}

void testSortedInputStaysBalanced(Tally& T) {
    std::printf("AVL sorted input stress and event toggle\n");
    AVLTree tree;
    EventLog quiet;
    quiet.enabled = false;
    for (int key = 1; key <= 1000; key++) CHECK(tree.insert(key, quiet));
    CHECK(quiet.size() == 0);
    CHECK(validTree(tree));
    CHECK(tree.root->height < 20);
    CHECK(tree.totalRotations > 0);
}

int main() {
    Tally T;
    testEmptyAndSingle(T);
    testFourInsertRotationTypes(T);
    testSearchAndInheritedBstOperations(T);
    testDeleteCases(T);
    testDeleteRebalanceCases(T);
    testRankKthAfterEveryMutation(T);
    testSortedInputStaysBalanced(T);
    std::printf("\nPassed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed == 0 ? 0 : 1;
}
