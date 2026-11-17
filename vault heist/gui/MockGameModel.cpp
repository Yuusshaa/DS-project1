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

// Returns the height of the subtree at 'id' (leaf = 1) and updates 'best' with
// the longest path, measured in edges, that bends at any node.
int MockGameModel::diameterWalk(int id, int& best) {
    if (id < 0) return 0;
    int index = findNodeIndex(id);
    if (index < 0) return 0;
    int leftHeight = diameterWalk(view.treeNodes[index].leftId, best);
    int rightHeight = diameterWalk(view.treeNodes[index].rightId, best);
    if (leftHeight + rightHeight > best) best = leftHeight + rightHeight;
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

void MockGameModel::mirrorWalk(int id) {
    int index = findNodeIndex(id);
    if (index < 0) return;
    int oldLeft = view.treeNodes[index].leftId;
    view.treeNodes[index].leftId = view.treeNodes[index].rightId;
    view.treeNodes[index].rightId = oldLeft;
    int newLeft = view.treeNodes[index].leftId;
    int newRight = view.treeNodes[index].rightId;
    mirrorWalk(newLeft);
    mirrorWalk(newRight);
}

// mode 0 = preorder, 1 = inorder, 2 = postorder.
void MockGameModel::traverseWalk(int id, int mode, EventLog& events) {
    int index = findNodeIndex(id);
    if (index < 0) return;
    int leftId = view.treeNodes[index].leftId;
    int rightId = view.treeNodes[index].rightId;
    if (mode == 0) addVisit(id, events);
    traverseWalk(leftId, mode, events);
    if (mode == 1) addVisit(id, events);
    traverseWalk(rightId, mode, events);
    if (mode == 2) addVisit(id, events);
}

std::string MockGameModel::visitSummary(std::string name, EventLog& events) {
    std::string text = name + ":";
    bool first = true;
    for (int i = 0; i < events.size(); i++) {
        if (events.get(i).type != EV_VISIT) continue;
        text += first ? " " : ", ";
        text += std::to_string(events.get(i).value);
        first = false;
    }
    return text;
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
    view.clear();
    if (scenarioId == 25) {
        view.viewType = MODEL_VIEW_TREE;
        message = "Empty BST loaded for the degeneration challenge";
        events.clear();
        return true;
    }

    if (scenarioId == 31) {
        ViewNode node;
        node.id = 1; node.value = 50; node.leftId = 2; node.rightId = 3;
        node.parentId = -1;
        view.treeNodes.push(node);
        node = ViewNode();
        node.id = 2; node.value = 30; node.leftId = 4; node.parentId = 1;
        view.treeNodes.push(node);
        node = ViewNode();
        node.id = 3; node.value = 70; node.parentId = 1;
        view.treeNodes.push(node);
        node = ViewNode();
        node.id = 4; node.value = 20; node.leftId = 5; node.parentId = 2;
        view.treeNodes.push(node);
        node = ViewNode();
        node.id = 5; node.value = 10; node.parentId = 4;
        view.treeNodes.push(node);
        view.viewType = MODEL_VIEW_TREE;
        refreshMetadata();
        message = "Mock AVL challenge tree loaded";
        events.clear();
        return true;
    }

    if (scenarioId == 13) {
        // Shared Act 1.3/1.4/1.5 fixture: root 50, height 4, diameter 6.
        // id, value, left, right, parent
        int rows[9][5] = {
            {1, 50,  2,  4, -1},
            {2, 30,  3,  6,  1},
            {3, 20,  5, -1,  2},
            {4, 70,  7,  8,  1},
            {5, 10, -1, -1,  3},
            {6, 40, -1, -1,  2},
            {7, 60, -1, -1,  4},
            {8, 90, -1,  9,  4},
            {9, 100,-1, -1,  8}
        };
        for (int i = 0; i < 9; i++) {
            ViewNode node;
            node.id = rows[i][0];
            node.value = rows[i][1];
            node.leftId = rows[i][2];
            node.rightId = rows[i][3];
            node.parentId = rows[i][4];
            view.treeNodes.push(node);
        }
        view.viewType = MODEL_VIEW_TREE;
        refreshMetadata();
        message = "Blueprint tree loaded for the height and diameter challenge";
        events.clear();
        return true;
    }

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
        traverseWalk(findRootId(), 0, events);
        message = visitSummary("Preorder", events);
    }
    else if (action == ACTION_TRAVERSE_INORDER) {
        traverseWalk(findRootId(), 1, events);
        message = visitSummary("Inorder", events);
    }
    else if (action == ACTION_TRAVERSE_POSTORDER) {
        traverseWalk(findRootId(), 2, events);
        message = visitSummary("Postorder", events);
    }
    else if (action == ACTION_TRAVERSE_LEVEL_ORDER) {
        DynArray<int> queue;
        int head = 0;
        int rootId = findRootId();
        if (rootId >= 0) queue.push(rootId);
        while (head < queue.size()) {
            int currentId = queue[head];
            head++;
            addVisit(currentId, events);
            int index = findNodeIndex(currentId);
            if (index < 0) continue;
            if (view.treeNodes[index].leftId >= 0) queue.push(view.treeNodes[index].leftId);
            if (view.treeNodes[index].rightId >= 0) queue.push(view.treeNodes[index].rightId);
        }
        message = visitSummary("Level order", events);
    }
    else if (action == ACTION_SEARCH) {
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
            }
            else if (view.treeNodes[index].value > value) {
                event.code = CMP_GREATER;
            }
            else {
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
    }
    else if (action == ACTION_INSERT) {
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
            }
            else {
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
    }
    else if (action == ACTION_DELETE) {
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
                }
                else {
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
    }
    else if (action == ACTION_RANGE_QUERY) {
        if (value > secondValue) {
            message = "Low value must not exceed high value";
            return false;
        }
        collectRange(findRootId(), value, secondValue, events);
        message = "Mock BST range query complete";
    }
    else if (action == ACTION_DIAMETER) {
        // Contract: the answer travels in EV_DONE.value (longest path in edges).
        int best = 0;
        diameterWalk(findRootId(), best);
        Event done;
        done.type = EV_DONE;
        done.value = best;
        events.add(done);
        message = "Mock diameter computed";
        return true;
    }
    else if (action == ACTION_LCA) {
        DynArray<int> firstAncestors;
        int current = value;
        while (current >= 0) {
            firstAncestors.push(current);
            int index = findNodeIndex(current);
            if (index < 0) break;
            current = view.treeNodes[index].parentId;
        }
        int secondHops = 0;
        int common = -1;
        current = secondValue;
        while (current >= 0 && common < 0) {
            for (int i = 0; i < firstAncestors.size(); i++) {
                if (firstAncestors[i] == current) {
                    common = current;
                    break;
                }
            }
            if (common >= 0) break;
            int index = findNodeIndex(current);
            if (index < 0) break;
            current = view.treeNodes[index].parentId;
            secondHops++;
        }
        if (common < 0) {
            message = "LCA endpoints are invalid";
            return false;
        }
        int firstHops = 0;
        for (int i = 0; i < firstAncestors.size(); i++) {
            if (firstAncestors[i] == common) break;
            firstHops++;
        }
        Event done;
        done.type = EV_DONE;
        done.nodeId = common;
        done.value = firstHops + secondHops;
        events.add(done);
        message = "Mock LCA computed";
        return true;
    }
    else if (action == ACTION_MIRROR) {
        if (view.viewType != MODEL_VIEW_TREE || findRootId() < 0) {
            message = "No binary tree is available to mirror";
            return false;
        }
        mirrorWalk(findRootId());
        refreshMetadata();
        message = "Mock tree mirrored";
    }
    else {
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
