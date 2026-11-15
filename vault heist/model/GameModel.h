#pragma once

#include <string>
#include "dynamicarray.h"
#include "eventlog.h"

// Snapshot data for rendering. The GUI receives values and IDs, not model links.
struct ViewNode {
    int id;
    int value;
    int leftId;
    int rightId;
    int parentId;
    int height;
    int size;

    ViewNode() {
        id = -1;
        value = 0;
        leftId = -1;
        rightId = -1;
        parentId = -1;
        height = 0;
        size = 0;
    }
};

#define MODEL_VIEW_TREE 0
#define MODEL_VIEW_HEAP 1

struct ModelView {
    DynArray<ViewNode> treeNodes;
    DynArray<int> heapValues;
    int heapSize;
    int viewType;

    ModelView() {
        heapSize = 0;
        viewType = MODEL_VIEW_TREE;
    }

    void clear() {
        treeNodes.clear();
        heapValues.clear();
        heapSize = 0;
        viewType = MODEL_VIEW_TREE;
    }
};

// Action identifiers follow the project's no-enums course restriction.
#define ACTION_TRAVERSE_PREORDER 1
#define ACTION_TRAVERSE_INORDER 2
#define ACTION_TRAVERSE_POSTORDER 3
#define ACTION_TRAVERSE_LEVEL_ORDER 4
#define ACTION_SEARCH 5
#define ACTION_INSERT 6
#define ACTION_DELETE 7
#define ACTION_RANGE_QUERY 8
#define ACTION_HEAP_BUILD 9
#define ACTION_HEAPIFY 10
#define ACTION_EXTRACT_MAX 11
#define ACTION_HEAP_SORT 12

class GameModel {
public:
    virtual ~GameModel() {}

    // Load a scenario and return its events in the supplied batch.
    virtual bool loadScenario(int scenarioId, EventLog& events) = 0;

    // Perform one action using its primary and secondary values.
    virtual bool performAction(int action, int value, int secondValue,
                               EventLog& events) = 0;

    // Copy the current model state into display-only data for the GUI.
    virtual void copyView(ModelView& output) = 0;

    // Return the status message to display after the latest operation.
    virtual std::string statusText() = 0;
};
