#include <cstdio>
#include "testkit.h"
#include "../model/realgamemodel.h"

int findValue(ModelView& view, int value) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].value == value) {
            return view.treeNodes[i].id;
        }
    }
    return -1;
}

void testScenarios(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;

    for (int scenario = 1; scenario <= 6; scenario++) {
        events.clear();
        CHECK(model.loadScenario(scenario, events) == true);
        model.copyView(view);
        if (scenario == 4) {
            CHECK(view.viewType == MODEL_VIEW_HEAP);
            CHECK(view.heapValues.size() == 8);
        } else if (scenario == 1) {
            CHECK(view.treeNodes.size() == 4);
        } else if (scenario == 2 || scenario == 3) {
            CHECK(view.treeNodes.size() == 7);
        } else {
            CHECK(view.treeNodes.size() == 10);
        }
        CHECK(events.get(events.size() - 1).type == EV_DONE);
    }

    events.clear();
    CHECK(model.loadScenario(7, events) == false);
}

void testTreeActions(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;

    CHECK(model.loadScenario(2, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_SEARCH, 60, 0, events) == true);
    CHECK(events.countAlarm() == 3);

    events.clear();
    CHECK(model.performAction(ACTION_INSERT, 50, 0, events) == false);
    CHECK(events.get(events.size() - 1).type == EV_DONE);

    events.clear();
    CHECK(model.loadScenario(6, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_INSERT, 11, 0, events) == true);
    Validator validator;

    model.copyView(view);
    CHECK(view.treeNodes.size() == 11);

    AVLTree tree;
    EventLog avlEvents;
    for (int key = 1; key <= 10; key++) {
        tree.insert(key, avlEvents);
    }
    CHECK(validator.validateAVL(tree) == true);

    events.clear();
    CHECK(model.loadScenario(1, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_HEAP_SORT, 0, 0, events) == false);
    CHECK(events.get(events.size() - 1).type == EV_DONE);
}

void testHeapAndSnapshots(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;
    DynArray<char> snapshot;

    CHECK(model.loadScenario(4, events) == true);
    model.copyView(view);
    CHECK(view.heapValues.size() == 8);
    model.saveSnapshot(snapshot);

    events.clear();
    CHECK(model.performAction(ACTION_HEAP_SORT, 0, 0, events) == true);
    model.copyView(view);
    CHECK(view.viewType == MODEL_VIEW_HEAP);
    CHECK(view.heapSize == 0);
    for (int i = 1; i < view.heapValues.size(); i++) {
        CHECK(view.heapValues[i - 1] <= view.heapValues[i]);
    }

    CHECK(model.restoreSnapshot(snapshot) == true);
    model.copyView(view);
    CHECK(view.heapSize == 8);
    CHECK(view.heapValues[0] == 3);
    CHECK(view.heapValues[1] == 9);

    events.clear();
    CHECK(model.loadScenario(3, events) == true);
    model.saveSnapshot(snapshot);
    events.clear();
    CHECK(model.performAction(ACTION_INSERT, 99, 0, events) == true);
    CHECK(model.restoreSnapshot(snapshot) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 7);
    CHECK(findValue(view, 99) == -1);
    CHECK(model.statusText() == "Snapshot restored");

    snapshot.clear();
    snapshot.push('X');
    snapshot.push('\0');
    CHECK(model.restoreSnapshot(snapshot) == false);
    CHECK(model.statusText() != "OK");
}

int main() {
    Tally T;
    testScenarios(T);
    testTreeActions(T);
    testHeapAndSnapshots(T);

    std::printf("Passed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed == 0 ? 0 : 1;
}
