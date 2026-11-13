#include "MockGameModel.h"

MockGameModel::MockGameModel() {
    message = "Mock model ready";
    EventLog initialEvents;
    loadScenario(1, initialEvents);
}

int MockGameModel::findNodeIndex(int id) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].id == id) {
            return i;
        }
    }
    return -1;
}

void MockGameModel::addVisit(int id, EventLog& events) {
    int index = findNodeIndex(id);
    if (index < 0) {
        return;
    }
    Event event;
    event.type = EV_VISIT;
    event.nodeId = id;
    event.value = view.treeNodes[index].value;
    events.add(event);
}

bool MockGameModel::loadScenario(int scenarioId, EventLog& events) {
    (void)scenarioId;
    view.clear();

    ViewNode node;
    node.id = 1; node.value = 50; node.leftId = 2; node.rightId = 3;
    node.parentId = -1; node.height = 3; node.size = 5;
    view.treeNodes.push(node);

    node = ViewNode();
    node.id = 2; node.value = 30; node.parentId = 1;
    node.height = 1; node.size = 1;
    view.treeNodes.push(node);

    node = ViewNode();
    node.id = 3; node.value = 70; node.leftId = 4; node.rightId = 5;
    node.parentId = 1; node.height = 2; node.size = 3;
    view.treeNodes.push(node);

    node = ViewNode();
    node.id = 4; node.value = 60; node.parentId = 3;
    node.height = 1; node.size = 1;
    view.treeNodes.push(node);

    node = ViewNode();
    node.id = 5; node.value = 90; node.parentId = 3;
    node.height = 1; node.size = 1;
    view.treeNodes.push(node);

    view.viewType = MODEL_VIEW_TREE;
    message = "Demo BST: search for 60 or play a traversal";
    events.clear();
    return true;
}

bool MockGameModel::performAction(int action, int value, int secondValue,
                                  EventLog& events) {
    (void)secondValue;
    events.clear();

    if (action == ACTION_TRAVERSE_PREORDER) {
        addVisit(1, events); addVisit(2, events); addVisit(3, events);
        addVisit(4, events); addVisit(5, events);
        message = "Preorder: 50, 30, 70, 60, 90";
    } else if (action == ACTION_TRAVERSE_INORDER) {
        addVisit(2, events); addVisit(1, events); addVisit(4, events);
        addVisit(3, events); addVisit(5, events);
        message = "Inorder: 30, 50, 60, 70, 90";
    } else if (action == ACTION_TRAVERSE_POSTORDER) {
        addVisit(2, events); addVisit(4, events); addVisit(5, events);
        addVisit(3, events); addVisit(1, events);
        message = "Postorder: 30, 60, 90, 70, 50";
    } else if (action == ACTION_TRAVERSE_LEVEL_ORDER) {
        addVisit(1, events); addVisit(2, events); addVisit(3, events);
        addVisit(4, events); addVisit(5, events);
        message = "Level order: 50, 30, 70, 60, 90";
    } else if (action == ACTION_SEARCH) {
        int ids[3] = {1, 3, 4};
        for (int i = 0; i < 3; i++) {
            int index = findNodeIndex(ids[i]);
            if (index < 0) {
                break;
            }
            Event event;
            event.type = EV_COMPARE;
            event.nodeId = ids[i];
            event.value = value;
            if (view.treeNodes[index].value < value) {
                event.code = CMP_LESS;
            } else if (view.treeNodes[index].value > value) {
                event.code = CMP_GREATER;
            } else {
                event.code = CMP_EQUAL;
            }
            events.add(event);
            if (event.code == CMP_EQUAL) {
                break;
            }
        }
        message = "Mock search path for " + std::to_string(value);
    } else {
        message = "This mock action is not wired yet";
        return false;
    }

    Event done;
    done.type = EV_DONE;
    events.add(done);
    return true;
}

void MockGameModel::copyView(ModelView& output) {
    output.clear();
    output.viewType = view.viewType;
    output.heapSize = view.heapSize;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        output.treeNodes.push(view.treeNodes[i]);
    }
    for (int i = 0; i < view.heapValues.size(); i++) {
        output.heapValues.push(view.heapValues[i]);
    }
}

std::string MockGameModel::statusText() {
    return message;
}
