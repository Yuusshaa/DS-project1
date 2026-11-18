#include <cstdio>
#include "testkit.h"
#include "../gui/LevelManager.h"
#include "../model/realgamemodel.h"

int findValue(ModelView& view, int value) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].value == value) {
            return view.treeNodes[i].id;
        }
    }
    return -1;
}

int findNodeIndex(ModelView& view, int id) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].id == id) {
            return i;
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

    for (int scenario = 7; scenario <= 8; scenario++) {
        events.clear();
        CHECK(model.loadScenario(scenario, events) == true);
        model.copyView(view);
        CHECK(view.viewType == MODEL_VIEW_HEAP);
        CHECK(view.heapValues.size() == (scenario == 7 ? 8 : 10));
        CHECK(view.heapSize == view.heapValues.size());
    }
}

void testActOneScenariosAndActions(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;

    CHECK(model.loadScenario(12, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 7);
    CHECK(view.treeNodes[0].value == 40);
    CHECK(view.treeNodes[0].leftId == 2);
    CHECK(view.treeNodes[0].rightId == 3);

    events.clear();
    CHECK(model.performAction(ACTION_HEIGHT, 1, 0, events) == true);
    Event& heightResult = events.get(events.size() - 1);
    CHECK(heightResult.type == EV_DONE);
    CHECK(heightResult.nodeId == -1);
    CHECK(heightResult.value == 3);

    events.clear();
    CHECK(model.performAction(ACTION_HEIGHT, 4, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == 1);

    events.clear();
    CHECK(model.performAction(ACTION_LCA, 4, 5, events) == true);
    Event& lcaResult = events.get(events.size() - 1);
    CHECK(lcaResult.type == EV_DONE);
    CHECK(lcaResult.nodeId == 2);
    CHECK(lcaResult.value == 2);

    events.clear();
    CHECK(model.performAction(ACTION_LCA, 4, 99, events) == false);
    CHECK(model.statusText() == "Node ID not found");
    CHECK(events.get(events.size() - 1).type == EV_DONE);

    events.clear();
    CHECK(model.performAction(ACTION_MIRROR, 0, 0, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes[0].leftId == 3);
    CHECK(view.treeNodes[0].rightId == 2);
    int leftIndex = findNodeIndex(view, 2);
    CHECK(leftIndex >= 0);
    if (leftIndex >= 0) {
        CHECK(view.treeNodes[leftIndex].leftId == 5);
        CHECK(view.treeNodes[leftIndex].rightId == 4);
    }
    CHECK(events.get(events.size() - 1).type == EV_DONE);

    CHECK(model.loadScenario(13, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 9);
    events.clear();
    CHECK(model.performAction(ACTION_HEIGHT, 1, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == 4);
    events.clear();
    CHECK(model.performAction(ACTION_DIAMETER, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == 6);

    CHECK(model.loadScenario(25, events) == true);
    model.copyView(view);
    CHECK(view.viewType == MODEL_VIEW_TREE);
    CHECK(view.treeNodes.size() == 0);
    events.clear();
    CHECK(model.performAction(ACTION_INSERT, 10, 0, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 1);
    CHECK(view.treeNodes[0].value == 10);

    CHECK(model.loadScenario(31, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 5);
    CHECK(view.treeNodes[0].height == 4);
    CHECK(view.treeNodes[0].size == 5);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATION_CASE, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).nodeId == 2);
    CHECK(events.get(events.size() - 1).value == ROT_LL);

    CHECK(model.loadScenario(14, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 6);
    CHECK(view.treeNodes[0].value == 50);
    CHECK(view.treeNodes[0].leftId == 2);
    CHECK(view.treeNodes[0].rightId == 3);

    CHECK(model.loadScenario(15, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes.size() == 7);
    CHECK(view.treeNodes[0].value == 45);

    CHECK(model.loadScenario(33, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATION_CASE, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).nodeId == 1);
    CHECK(events.get(events.size() - 1).value == ROT_LR);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATE_LEFT, 2, 0, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATE_RIGHT, 1, 0, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes[0].id == 5);
    CHECK(view.treeNodes[0].value == 50);

    CHECK(model.loadScenario(32, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATION_CASE, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == ROT_RR);

    CHECK(model.loadScenario(34, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_ROTATION_CASE, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == ROT_RL);

    CHECK(model.loadScenario(31, events) == true);
    std::string snapshot = model.serialize();
    CHECK(model.loadScenario(2, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_MIRROR, 0, 0, events) == false);
    CHECK(model.deserialize(snapshot) == true);
    model.copyView(view);
    CHECK(view.treeNodes[0].value == 50);
    CHECK(view.treeNodes[0].leftId == 2);
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

void testNewRealModelFeatures(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;
    const int scenarios[5] = {39, 35, 36, 37, 38};
    const int insertions[5] = {60, 20, 60, 40, 60};
    const int expectedCases[5] = {
        ROT_NONE, ROT_LL, ROT_RR, ROT_LR, ROT_RL
    };
    const int expectedRoots[5] = {50, 30, 50, 40, 60};
    const int expectedHeights[5] = {3, 2, 2, 2, 2};

    for (int i = 0; i < 5; i++) {
        events.clear();
        CHECK(model.loadScenario(scenarios[i], events) == true);
        events.clear();
        CHECK(model.performAction(ACTION_INSERT, insertions[i], 0,
                                  events) == true);
        int observedCase = ROT_NONE;
        int rotationEvents = 0;
        for (int j = 0; j < events.size(); j++) {
            if (events.get(j).type == EV_BALANCE_ALERT) {
                observedCase = events.get(j).code;
            }
            if (events.get(j).type == EV_ROTATE) rotationEvents++;
        }
        CHECK(observedCase == expectedCases[i]);
        CHECK(rotationEvents == (i == 0 ? 0 : 1));
        model.copyView(view);
        CHECK(view.treeNodes.size() == (i == 0 ? 4 : 3));
        CHECK(view.treeNodes[0].value == expectedRoots[i]);
        CHECK(view.treeNodes[0].height == expectedHeights[i]);
        CHECK(view.treeNodes[0].size == view.treeNodes.size());
    }

    events.clear();
    CHECK(model.loadScenario(3, events) == true);
    model.copyView(view);
    events.clear();
    CHECK(model.performAction(ACTION_KTH, 5, 0, events) == true);
    Event& kthResult = events.get(events.size() - 1);
    CHECK(kthResult.type == EV_DONE);
    CHECK(kthResult.nodeId == findValue(view, 60));
    CHECK(kthResult.value == 60);

    events.clear();
    CHECK(model.performAction(ACTION_RANK, 55, 0, events) == true);
    Event& rankResult = events.get(events.size() - 1);
    CHECK(rankResult.type == EV_DONE);
    CHECK(rankResult.nodeId == -1);
    CHECK(rankResult.value == 4);

    events.clear();
    CHECK(model.performAction(ACTION_KTH, 0, 0, events) == false);
    CHECK(events.get(events.size() - 1).type == EV_DONE);
    CHECK(model.statusText() == "k out of range");

    events.clear();
    CHECK(model.loadScenario(3, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_DELETE, 20, 0, events) == true);
    model.copyView(view);
    CHECK(view.treeNodes[0].size == 6);
    int thirtyIndex = findNodeIndex(view, findValue(view, 30));
    CHECK(thirtyIndex >= 0);
    if (thirtyIndex >= 0) {
        CHECK(view.treeNodes[thirtyIndex].size == 2);
        CHECK(view.treeNodes[thirtyIndex].height == 2);
    }

    events.clear();
    CHECK(model.loadScenario(2, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_KTH, 1, 0, events) == false);
    CHECK(model.statusText() == "Action is not available in this scenario");
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

void testInteractiveHeapActions(Tally& T) {
    RealGameModel model;
    EventLog events;
    ModelView view;

    CHECK(model.loadScenario(4, events) == true);
    CHECK(model.performAction(ACTION_HEAP_SWAP, 3, 7, events) == true);
    CHECK(model.performAction(ACTION_HEAP_SWAP, 2, 6, events) == true);
    CHECK(model.performAction(ACTION_HEAP_SWAP, 0, 1, events) == true);
    CHECK(model.performAction(ACTION_HEAP_SWAP, 1, 3, events) == true);
    events.clear();
    CHECK(model.performAction(ACTION_IS_MAX_HEAP, 0, 0, events) == true);
    CHECK(events.get(events.size() - 1).value == 1);
    model.copyView(view);
    CHECK(view.heapValues[0] == 9);
    CHECK(view.heapSize == 8);

    CHECK(model.loadScenario(8, events) == true);
    CHECK(model.performAction(ACTION_HEAP_SORT, 0, 0, events) == true);
    model.copyView(view);
    CHECK(view.heapSize == 0);
    CHECK(view.heapValues[0] == -4);
    CHECK(view.heapValues[1] == -1);
    CHECK(view.heapValues[8] == 12);
    CHECK(view.heapValues[9] == 12);
}

void testCampaignLevels(Tally& T) {
    LevelManager levels;
    CHECK(levels.loadFromJson("data/levels.json") == true);
    CHECK(levels.count() == 20);
    for (int act = 1; act <= 4; act++) {
        for (int number = 1; number <= 5; number++) {
            int index = levels.findLevel(act, number);
            CHECK(index >= 0);
            if (index >= 0) CHECK(levels.getLevel(index).implemented == true);
        }
    }
    int bstSearch = levels.findLevel(2, 1);
    CHECK(bstSearch >= 0);
    if (bstSearch >= 0) CHECK(levels.getLevel(bstSearch).scenarioId == 2);
    for (int number = 1; number <= 4; number++) {
        int index = levels.findLevel(2, number);
        if (index >= 0) CHECK(levels.getLevel(index).scenarioId == 2);
    }
    int avlPrediction = levels.findLevel(3, 4);
    int orderStatistics = levels.findLevel(3, 5);
    CHECK(avlPrediction >= 0);
    CHECK(orderStatistics >= 0);
    if (avlPrediction >= 0) CHECK(levels.getLevel(avlPrediction).scenarioId == 39);
    if (orderStatistics >= 0) CHECK(levels.getLevel(orderStatistics).scenarioId == 3);
    int finalHeapSort = levels.findLevel(4, 5);
    CHECK(finalHeapSort >= 0);
    if (finalHeapSort >= 0) {
        CHECK(levels.getLevel(finalHeapSort).scenarioId == 8);
        CHECK(levels.getLevel(finalHeapSort).implemented == true);
        CHECK(levels.isUnlocked(finalHeapSort) == false);
    }
    int lastOldLevelBeforeActFour = levels.findLevel(3, 3);
    int finalActThreeLevel = levels.findLevel(3, 5);
    CHECK(lastOldLevelBeforeActFour >= 0);
    CHECK(finalActThreeLevel >= 0);
    if (lastOldLevelBeforeActFour >= 0 && finalActThreeLevel >= 0) {
        levels.recordResult(lastOldLevelBeforeActFour, 1, 100);
        CHECK(levels.isUnlocked(levels.findLevel(3, 4)) == true);
        CHECK(levels.isUnlocked(levels.findLevel(4, 1)) == false);
        levels.recordResult(levels.findLevel(3, 4), 1, 100);
        CHECK(levels.isUnlocked(finalActThreeLevel) == true);
        CHECK(levels.isUnlocked(levels.findLevel(4, 1)) == false);
        levels.recordResult(finalActThreeLevel, 1, 100);
        CHECK(levels.isUnlocked(levels.findLevel(4, 1)) == true);
    }
}

int main() {
    Tally T;
    testScenarios(T);
    testActOneScenariosAndActions(T);
    testTreeActions(T);
    testNewRealModelFeatures(T);
    testHeapAndSnapshots(T);
    testInteractiveHeapActions(T);
    testCampaignLevels(T);

    std::printf("Passed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed == 0 ? 0 : 1;
}
