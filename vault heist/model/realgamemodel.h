#pragma once

#include "GameModel.h"
#include "binarytree.h"
#include "bst.h"
#include "avltree.h"
#include "maxheap.h"
#include "savemanager.h"
#include "validator.h"

class RealGameModel : public GameModel {
private:
    static const int MODE_NONE = 0;
    static const int MODE_BINARY = 1;
    static const int MODE_BST = 2;
    static const int MODE_AVL = 3;
    static const int MODE_HEAP = 4;

    BinaryTree binaryTree;
    BST bst;
    AVLTree avlTree;
    MaxHeap heap;
    int mode;
    bool heapBuilt;
    std::string message;

    void clearModel() {
        binaryTree.clear();
        bst.clear();
        avlTree.clear();
        avlTree.totalRotations = 0;
        avlTree.lastRotations = 0;
        heap.data.clear();
        heap.heapSize = 0;
        heap.resetCounters();
        heapBuilt = false;
        mode = MODE_NONE;
    }

    BinaryTree* treeForMode() {
        if (mode == MODE_BINARY) {
            return &binaryTree;
        }
        if (mode == MODE_BST) {
            return &bst;
        }
        if (mode == MODE_AVL) {
            return &avlTree;
        }
        return 0;
    }

    BST* searchTreeForMode() {
        if (mode == MODE_BST) {
            return &bst;
        }
        if (mode == MODE_AVL) {
            return &avlTree;
        }
        return 0;
    }

    void record(EventLog& events, int type, int nodeId, int otherId,
                int value, int code, const char* note) {
        Event event;
        event.type = type;
        event.nodeId = nodeId;
        event.otherId = otherId;
        event.value = value;
        event.code = code;
        event.setNote(note);
        events.add(event);
    }

    void finish(EventLog& events, const char* note) {
        Event event;
        event.type = EV_DONE;
        event.setNote(note);
        events.add(event);
        message = event.note;
    }

    void finishResult(EventLog& events, const char* note, int nodeId,
                      int value) {
        Event event;
        event.type = EV_DONE;
        event.nodeId = nodeId;
        event.value = value;
        event.setNote(note);
        events.add(event);
        message = event.note;
    }

    // Keep statusText synchronized with the last completed event batch.
    void updateStatus(EventLog& events, const char* fallback) {
        for (int i = events.size() - 1; i >= 0; i--) {
            if (events.get(i).type == EV_DONE) {
                message = events.get(i).note;
                return;
            }
        }
        message = fallback;
    }

    void appendVisits(BinaryTree& tree, DynArray<int>& ids, EventLog& events) {
        for (int i = 0; i < ids.size(); i++) {
            TreeNode* node = tree.findById(tree.root, ids[i]);
            Event event;
            event.type = EV_VISIT;
            event.nodeId = ids[i];
            event.value = node->value;
            event.setNote("Visit room");
            events.add(event);
        }
    }

    int subtreeSize(TreeNode* node) {
        if (node == 0) {
            return 0;
        }
        return 1 + subtreeSize(node->left) + subtreeSize(node->right);
    }

    void appendViewNodes(TreeNode* node, ModelView& output, BinaryTree& tree) {
        if (node == 0) {
            return;
        }

        ViewNode viewNode;
        viewNode.id = node->id;
        viewNode.value = node->value;
        if (node->left != 0) {
            viewNode.leftId = node->left->id;
        }
        if (node->right != 0) {
            viewNode.rightId = node->right->id;
        }
        if (node->parent != 0) {
            viewNode.parentId = node->parent->id;
        }

        if (mode == MODE_AVL) {
            viewNode.height = node->height;
            viewNode.size = node->size;
        } else {
            viewNode.height = tree.computeHeight(node);
            viewNode.size = subtreeSize(node);
        }
        output.treeNodes.push(viewNode);

        appendViewNodes(node->left, output, tree);
        appendViewNodes(node->right, output, tree);
    }

    bool actionUnavailable(EventLog& events) {
        finish(events, "Action is not available in this scenario");
        return false;
    }

public:
    RealGameModel() {
        mode = MODE_NONE;
        heapBuilt = false;
        message = "No scenario loaded";
    }

    bool loadScenario(int scenarioId, EventLog& events) {
        if ((scenarioId < 1 || scenarioId > 6) && scenarioId != 12 &&
            scenarioId != 13 && scenarioId != 25 && scenarioId != 31) {
            finish(events, "Unknown scenario");
            return false;
        }

        clearModel();
        EventLog buildEvents;
        buildEvents.enabled = false;

        if (scenarioId == 1) {
            mode = MODE_BINARY;
            binaryTree.root = binaryTree.createNode(10);
            TreeNode* left = binaryTree.createNode(20);
            TreeNode* right = binaryTree.createNode(30);
            binaryTree.setLeft(binaryTree.root, left);
            binaryTree.setRight(binaryTree.root, right);
            binaryTree.setLeft(left, binaryTree.createNode(40));
        } else if (scenarioId == 2) {
            mode = MODE_BST;
            int keys[] = {50, 30, 70, 20, 40, 60, 80};
            for (int i = 0; i < 7; i++) {
                bst.insert(keys[i], buildEvents);
            }
        } else if (scenarioId == 3) {
            mode = MODE_AVL;
            int keys[] = {50, 30, 70, 20, 40, 60, 80};
            for (int i = 0; i < 7; i++) {
                avlTree.insert(keys[i], buildEvents);
            }
            avlTree.totalRotations = 0;
            avlTree.lastRotations = 0;
        } else if (scenarioId == 4) {
            mode = MODE_HEAP;
            DynArray<int> values;
            int initialValues[] = {3, 9, 2, 1, 4, 5, 8, 7};
            for (int i = 0; i < 8; i++) {
                values.push(initialValues[i]);
            }
            heap.loadArray(values);
            heapBuilt = false;
        } else if (scenarioId == 5) {
            mode = MODE_BST;
            for (int key = 1; key <= 10; key++) {
                bst.insert(key, buildEvents);
            }
        } else if (scenarioId == 6) {
            mode = MODE_AVL;
            for (int key = 1; key <= 10; key++) {
                avlTree.insert(key, buildEvents);
            }
            avlTree.totalRotations = 0;
            avlTree.lastRotations = 0;
        } else if (scenarioId == 12) {
            mode = MODE_BINARY;
            binaryTree.root = binaryTree.createNode(40);
            TreeNode* left = binaryTree.createNode(20);
            TreeNode* right = binaryTree.createNode(70);
            binaryTree.setLeft(binaryTree.root, left);
            binaryTree.setRight(binaryTree.root, right);
            binaryTree.setLeft(left, binaryTree.createNode(10));
            binaryTree.setRight(left, binaryTree.createNode(30));
            binaryTree.setLeft(right, binaryTree.createNode(60));
            binaryTree.setRight(right, binaryTree.createNode(90));
        } else if (scenarioId == 13) {
            mode = MODE_BINARY;
            binaryTree.root = binaryTree.createNode(50);
            TreeNode* left = binaryTree.createNode(30);
            TreeNode* leftLeft = binaryTree.createNode(20);
            TreeNode* right = binaryTree.createNode(70);
            binaryTree.setLeft(binaryTree.root, left);
            binaryTree.setLeft(left, leftLeft);
            binaryTree.setLeft(leftLeft, binaryTree.createNode(10));
            binaryTree.setRight(left, binaryTree.createNode(40));
            binaryTree.setRight(binaryTree.root, right);
            binaryTree.setLeft(right, binaryTree.createNode(60));
            TreeNode* farRight = binaryTree.createNode(90);
            binaryTree.setRight(right, farRight);
            binaryTree.setRight(farRight, binaryTree.createNode(100));
        } else if (scenarioId == 25) {
            mode = MODE_BST;
        } else {
            mode = MODE_BINARY;
            binaryTree.root = binaryTree.createNode(50);
            TreeNode* left = binaryTree.createNode(30);
            TreeNode* right = binaryTree.createNode(70);
            TreeNode* leftLeft = binaryTree.createNode(20);
            TreeNode* deepest = binaryTree.createNode(10);
            binaryTree.setLeft(binaryTree.root, left);
            binaryTree.setRight(binaryTree.root, right);
            binaryTree.setLeft(left, leftLeft);
            binaryTree.setLeft(leftLeft, deepest);
        }

        finish(events, "Scenario loaded");
        return true;
    }

    bool performAction(int action, int value, int secondValue,
                       EventLog& events) {
        if (action == ACTION_HEIGHT) {
            BinaryTree* tree = treeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            TreeNode* node = tree->findById(tree->root, value);
            if (node == 0) {
                finishResult(events, "Node ID not found", -1, 0);
                return false;
            }
            finishResult(events, "Height calculated", -1,
                         tree->computeHeight(node));
            return true;
        }

        if (action == ACTION_LCA) {
            BinaryTree* tree = treeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            TreeNode* first = tree->findById(tree->root, value);
            TreeNode* second = tree->findById(tree->root, secondValue);
            if (first == 0 || second == 0) {
                finishResult(events, "Node ID not found", -1, 0);
                return false;
            }
            TreeNode* ancestor = tree->lca(tree->root, value, secondValue);
            int pathLength = tree->pathLength(value, secondValue);
            if (ancestor == 0 || pathLength < 0) {
                finishResult(events, "Could not compute LCA", -1, 0);
                return false;
            }
            finishResult(events, "LCA calculated", ancestor->id, pathLength);
            return true;
        }

        if (action == ACTION_MIRROR) {
            if (mode != MODE_BINARY) {
                return actionUnavailable(events);
            }
            binaryTree.mirror(binaryTree.root);
            finish(events, "Tree mirrored");
            return true;
        }

        if (action == ACTION_TRAVERSE_PREORDER ||
            action == ACTION_TRAVERSE_INORDER ||
            action == ACTION_TRAVERSE_POSTORDER ||
            action == ACTION_TRAVERSE_LEVEL_ORDER) {
            BinaryTree* tree = treeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }

            DynArray<int> ids;
            if (action == ACTION_TRAVERSE_PREORDER) {
                tree->preorder(tree->root, ids);
            } else if (action == ACTION_TRAVERSE_INORDER) {
                tree->inorder(tree->root, ids);
            } else if (action == ACTION_TRAVERSE_POSTORDER) {
                tree->postorder(tree->root, ids);
            } else {
                tree->levelOrder(ids);
            }
            appendVisits(*tree, ids, events);
            finish(events, "Traversal complete");
            return true;
        }

        if (action == ACTION_SEARCH) {
            BST* tree = searchTreeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            bool found = tree->search(value, events) != 0;
            updateStatus(events, found ? "Found" : "Not found");
            return found;
        }

        if (action == ACTION_INSERT) {
            BST* tree = searchTreeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            bool inserted = tree->insert(value, events);
            updateStatus(events, inserted ? "Inserted" : "Duplicate rejected");
            return inserted;
        }

        if (action == ACTION_DELETE) {
            BST* tree = searchTreeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            bool deleted = tree->deleteKey(value, events);
            updateStatus(events, deleted ? "Deleted" : "Key not found");
            return deleted;
        }

        if (action == ACTION_RANGE_QUERY) {
            BST* tree = searchTreeForMode();
            if (tree == 0) {
                return actionUnavailable(events);
            }
            DynArray<int> result;
            tree->rangeQuery(value, secondValue, result, events);
            updateStatus(events, "Range complete");
            return true;
        }

        if (action == ACTION_HEAP_BUILD) {
            if (mode != MODE_HEAP) {
                return actionUnavailable(events);
            }
            heap.buildMaxHeap(events);
            heapBuilt = true;
            updateStatus(events, "Heap built");
            return true;
        }

        if (action == ACTION_HEAPIFY) {
            if (mode != MODE_HEAP) {
                return actionUnavailable(events);
            }
            if (value < 0 || value >= heap.heapSize) {
                finish(events, "Heapify index is out of range");
                return false;
            }
            heap.siftDown(value, heap.heapSize, events);
            heapBuilt = heap.isValidState();
            finish(events, "Heapified");
            return true;
        }

        if (action == ACTION_EXTRACT_MAX) {
            if (mode != MODE_HEAP) {
                return actionUnavailable(events);
            }
            int maximum = 0;
            bool extracted = heap.extractMax(maximum, events);
            heapBuilt = heap.isValidState();
            updateStatus(events, extracted ? "Extracted" : "Heap is empty");
            return extracted;
        }

        if (action == ACTION_HEAP_SORT) {
            if (mode != MODE_HEAP) {
                return actionUnavailable(events);
            }
            heap.heapSort(events);
            heapBuilt = true;
            updateStatus(events, "Sorted");
            return true;
        }

        finish(events, "Unknown action");
        return false;
    }

    void copyView(ModelView& output) {
        output.clear();
        if (mode == MODE_HEAP) {
            output.viewType = MODEL_VIEW_HEAP;
            output.heapSize = heap.heapSize;
            for (int i = 0; i < heap.data.size(); i++) {
                output.heapValues.push(heap.data[i]);
            }
            return;
        }

        output.viewType = MODEL_VIEW_TREE;
        BinaryTree* tree = treeForMode();
        if (tree != 0) {
            appendViewNodes(tree->root, output, *tree);
        }
    }

    std::string statusText() {
        return message;
    }

    void saveSnapshot(DynArray<char>& out) {
        out.clear();
        SaveManager manager;
        DynArray<char> data;

        if (mode == MODE_BINARY || mode == MODE_BST || mode == MODE_AVL) {
            manager.saveTree(*treeForMode(), data);
        } else if (mode == MODE_HEAP) {
            manager.saveHeap(heap, data);
        }

        manager.appendInt(out, mode);
        if (mode == MODE_HEAP) {
            out.push(' ');
            manager.appendInt(out, heapBuilt ? 1 : 0);
        }
        if (mode != MODE_NONE) {
            out.push(' ');
            for (int i = 0; i < data.size() - 1; i++) {
                out.push(data[i]);
            }
        }
        manager.finish(out);
    }

    bool restoreSnapshot(DynArray<char>& in) {
        SaveManager manager;
        int position = 0;
        int restoredMode = MODE_NONE;
        bool restoredHeapBuilt = false;
        if (manager.readInt(in, position, restoredMode) == false ||
            restoredMode < MODE_NONE || restoredMode > MODE_HEAP) {
            message = "Invalid snapshot";
            return false;
        }

        manager.skipSpaces(in, position);
        if (restoredMode == MODE_HEAP) {
            int heapBuiltValue = 0;
            if (manager.readInt(in, position, heapBuiltValue) == false ||
                (heapBuiltValue != 0 && heapBuiltValue != 1)) {
                message = "Invalid snapshot";
                return false;
            }
            restoredHeapBuilt = heapBuiltValue == 1;
            manager.skipSpaces(in, position);
        }
        if (restoredMode == MODE_NONE) {
            if (manager.atEnd(in, position) == false) {
                message = "Invalid snapshot";
                return false;
            }
            clearModel();
            message = "Snapshot restored";
            return true;
        }

        if (manager.atEnd(in, position)) {
            message = "Invalid snapshot";
            return false;
        }

        DynArray<char> data;
        bool foundEnd = false;
        for (int i = position; i < in.size(); i++) {
            data.push(in[i]);
            if (in[i] == '\0') {
                foundEnd = true;
                break;
            }
        }
        if (foundEnd == false) {
            data.push('\0');
        }

        clearModel();
        bool loaded = false;
        const char* restoreError = "Snapshot contents are invalid";
        Validator validator;

        if (restoredMode == MODE_BINARY) {
            loaded = manager.loadTree(binaryTree, data);
            if (loaded && validator.validateStructure(binaryTree) == false) {
                loaded = false;
                restoreError = validator.lastError;
            }
        } else if (restoredMode == MODE_BST) {
            loaded = manager.loadTree(bst, data);
            if (loaded && validator.validateBST(bst) == false) {
                loaded = false;
                restoreError = validator.lastError;
            }
        } else if (restoredMode == MODE_AVL) {
            loaded = manager.loadTree(avlTree, data);
            if (loaded) {
                avlTree.recomputeAll(avlTree.root);
                if (validator.validateAVL(avlTree) == false) {
                    loaded = false;
                    restoreError = validator.lastError;
                }
            }
        } else {
            loaded = manager.loadHeap(heap, data);
            if (loaded) {
                bool validHeap = validator.validateHeap(heap);
                if (restoredHeapBuilt && validHeap == false) {
                    loaded = false;
                    restoreError = validator.lastError;
                }
            }
        }

        if (loaded == false) {
            clearModel();
            message = restoreError;
            return false;
        }

        mode = restoredMode;
        heapBuilt = restoredHeapBuilt;
        message = "Snapshot restored";
        return true;
    }

    std::string serialize() override {
        DynArray<char> snapshot;
        saveSnapshot(snapshot);
        std::string output;
        for (int i = 0; i + 1 < snapshot.size(); i++) {
            output += snapshot[i];
        }
        return output;
    }

    bool deserialize(const std::string& data) override {
        if (data.find('\0') != std::string::npos) {
            message = "Invalid snapshot";
            return false;
        }
        DynArray<char> snapshot;
        for (std::size_t i = 0; i < data.size(); i++) {
            snapshot.push(data[i]);
        }
        snapshot.push('\0');
        return restoreSnapshot(snapshot);
    }
};
