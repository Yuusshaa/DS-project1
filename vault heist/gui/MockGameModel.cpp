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

int MockGameModel::findRootId() {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].parentId < 0) return view.treeNodes[i].id;
    }
    return -1;
}

int MockGameModel::calculateHeight(int id) {
    if (id < 0) return 0;
    int index = findNodeIndex(id);
    if (index < 0) return 0;
    int leftHeight = calculateHeight(view.treeNodes[index].leftId);
    int rightHeight = calculateHeight(view.treeNodes[index].rightId);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int MockGameModel::calculateSize(int id) {
    if (id < 0) return 0;
    int index = findNodeIndex(id);
    if (index < 0) return 0;
    return 1 + calculateSize(view.treeNodes[index].leftId) +
               calculateSize(view.treeNodes[index].rightId);
}

void MockGameModel::refreshMetadata() {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        int id = view.treeNodes[i].id;
        view.treeNodes[i].height = calculateHeight(id);
        view.treeNodes[i].size = calculateSize(id);
    }
}

void MockGameModel::removeNodeRecord(int id) {
    DynArray<ViewNode> remaining;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].id != id) remaining.push(view.treeNodes[i]);
    }
    view.treeNodes.clear();
    for (int i = 0; i < remaining.size(); i++) view.treeNodes.push(remaining[i]);
}

void MockGameModel::collectRange(int id, int low, int high, EventLog& events) {
    int index = findNodeIndex(id);
    if (index < 0) return;
    int value = view.treeNodes[index].value;
    if (value > low) collectRange(view.treeNodes[index].leftId, low, high, events);
    else if (view.treeNodes[index].leftId >= 0) {
        Event skipped;
        skipped.type = EV_HIGHLIGHT_SUBTREE;
        skipped.nodeId = view.treeNodes[index].leftId;
        skipped.code = HL_SKIPPED;
        events.add(skipped);
    }
    if (value >= low && value <= high) {
        Event matched;
        matched.type = EV_VISIT;
        matched.nodeId = id;
        matched.value = value;
        events.add(matched);
    }
    if (value < high) collectRange(view.treeNodes[index].rightId, low, high, events);
    else if (view.treeNodes[index].rightId >= 0) {
        Event skipped;
        skipped.type = EV_HIGHLIGHT_SUBTREE;
        skipped.nodeId = view.treeNodes[index].rightId;
        skipped.code = HL_SKIPPED;
        events.add(skipped);
    }
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
        int currentId = findRootId();
        while (currentId >= 0) {
            int index = findNodeIndex(currentId);
            if (index < 0) break;
            Event event;
            event.type = EV_COMPARE;
            event.nodeId = currentId;
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
            currentId = event.code == CMP_LESS ? view.treeNodes[index].rightId :
                                                 view.treeNodes[index].leftId;
        }
        message = "Mock search path for " + std::to_string(value);
    } else if (action == ACTION_INSERT) {
        int currentId = findRootId();
        int parentId = -1;
        int side = SIDE_NONE;
        while (currentId >= 0) {
            int index = findNodeIndex(currentId);
            if (index < 0) break;
            Event compare;
            compare.type = EV_COMPARE;
            compare.nodeId = currentId;
            compare.value = value;
            if (view.treeNodes[index].value < value) compare.code = CMP_LESS;
            else if (view.treeNodes[index].value > value) compare.code = CMP_GREATER;
            else {
                compare.code = CMP_EQUAL;
                events.add(compare);
                message = "Duplicate keys are rejected";
                return false;
            }
            events.add(compare);
            parentId = currentId;
            if (value < view.treeNodes[index].value) {
                side = SIDE_LEFT;
                currentId = view.treeNodes[index].leftId;
            } else {
                side = SIDE_RIGHT;
                currentId = view.treeNodes[index].rightId;
            }
        }
        int newId = 1;
        for (int i = 0; i < view.treeNodes.size(); i++) {
            if (view.treeNodes[i].id >= newId) newId = view.treeNodes[i].id + 1;
        }
        ViewNode inserted;
        inserted.id = newId;
        inserted.value = value;
        inserted.parentId = parentId;
        if (parentId >= 0) {
            int parentIndex = findNodeIndex(parentId);
            if (side == SIDE_LEFT) view.treeNodes[parentIndex].leftId = newId;
            else view.treeNodes[parentIndex].rightId = newId;
        }
        view.treeNodes.push(inserted);
        Event event;
        event.type = EV_INSERT;
        event.nodeId = newId;
        event.otherId = parentId;
        event.value = value;
        event.code = side;
        events.add(event);
        refreshMetadata();
        message = "Mock BST inserted " + std::to_string(value);
    } else if (action == ACTION_DELETE) {
        int currentId = findRootId();
        int targetId = -1;
        while (currentId >= 0) {
            int index = findNodeIndex(currentId);
            if (index < 0) break;
            Event compare;
            compare.type = EV_COMPARE;
            compare.nodeId = currentId;
            compare.value = value;
            if (view.treeNodes[index].value < value) compare.code = CMP_LESS;
            else if (view.treeNodes[index].value > value) compare.code = CMP_GREATER;
            else {
                compare.code = CMP_EQUAL;
                events.add(compare);
                targetId = currentId;
                break;
            }
            events.add(compare);
            currentId = compare.code == CMP_LESS ? view.treeNodes[index].rightId :
                                                   view.treeNodes[index].leftId;
        }
        if (targetId < 0) {
            message = "Key not found; nothing was deleted";
            return false;
        }
        int targetIndex = findNodeIndex(targetId);
        if (view.treeNodes[targetIndex].leftId >= 0 &&
            view.treeNodes[targetIndex].rightId >= 0) {
            int successorId = view.treeNodes[targetIndex].rightId;
            int successorIndex = findNodeIndex(successorId);
            while (successorIndex >= 0 && view.treeNodes[successorIndex].leftId >= 0) {
                successorId = view.treeNodes[successorIndex].leftId;
                successorIndex = findNodeIndex(successorId);
            }
            if (successorIndex < 0) return false;
            int oldTargetValue = view.treeNodes[targetIndex].value;
            int successorValue = view.treeNodes[successorIndex].value;
            view.treeNodes[targetIndex].value = successorValue;
            view.treeNodes[successorIndex].value = oldTargetValue;
            Event swap;
            swap.type = EV_SWAP_VALUE;
            swap.nodeId = targetId;
            swap.otherId = successorId;
            swap.oldValue = oldTargetValue;
            swap.newValue = successorValue;
            swap.value = successorValue;
            events.add(swap);
            targetId = successorId;
        }
        int removeIndex = findNodeIndex(targetId);
        if (removeIndex < 0) return false;
        int parentId = view.treeNodes[removeIndex].parentId;
        int childId = view.treeNodes[removeIndex].leftId >= 0
            ? view.treeNodes[removeIndex].leftId : view.treeNodes[removeIndex].rightId;
        int side = SIDE_NONE;
        if (parentId >= 0) {
            int parentIndex = findNodeIndex(parentId);
            if (parentIndex >= 0) {
                if (view.treeNodes[parentIndex].leftId == targetId) {
                    view.treeNodes[parentIndex].leftId = childId;
                    side = SIDE_LEFT;
                } else {
                    view.treeNodes[parentIndex].rightId = childId;
                    side = SIDE_RIGHT;
                }
            }
        }
        if (childId >= 0) {
            int childIndex = findNodeIndex(childId);
            if (childIndex >= 0) view.treeNodes[childIndex].parentId = parentId;
        }
        Event unlink;
        unlink.type = EV_UNLINK;
        unlink.nodeId = targetId;
        unlink.otherId = parentId;
        unlink.value = view.treeNodes[removeIndex].value;
        unlink.code = side;
        events.add(unlink);
        removeNodeRecord(targetId);
        refreshMetadata();
        message = "Mock BST deleted " + std::to_string(value);
    } else if (action == ACTION_RANGE_QUERY) {
        if (value > secondValue) {
            message = "Low value must not exceed high value";
            return false;
        }
        collectRange(findRootId(), value, secondValue, events);
        message = "Mock BST range query complete";
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
