#include "GameApp.h"

#include <cstdio>

GameApp::GameApp(GameModel& gameModel) : model(gameModel) {
    windowWidth = 1280;
    windowHeight = 800;
    screenState = 0;
    activeAct = 1;
    currentLevelIndex = -1;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    hoveredNodeId = -1;
    expectedIndex = 0;
    searchTarget = 0;
    sandboxAction = ACTION_SEARCH;
    insertTarget = 0;
    deleteTarget = 0;
    pendingNodeId = -1;
    heightPhase = 0;
    expectedDiameter = 0;
    lcaFirstNode = -1;
    lcaSecondNode = -1;
    expectedLcaNode = -1;
    expectedLcaPathLength = 0;
    heightOptimalSteps = 0;
    expectedDeleteCase = 0;
    rangeLow = 0;
    rangeHigh = 0;
    alarmSteps = 0;
    alarmCapacity = 10;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    challengeActive = false;
    challengeFinished = false;
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    lcaChallenge = false;
    animationPlaying = true;
    sandboxSecondFieldFocused = false;
    eventTimer = 0.0f;
    message = "Starting demo";
}

void GameApp::run() {
    InitWindow(windowWidth, windowHeight, "Vault Heist - Tree Lab");
    SetTargetFPS(60);
    if (levelManager.loadFromJson("data/levels.json")) {
        levelManager.loadProgress("campaign_progress.dat");
        currentLevelIndex = levelManager.findLevel(1, 1);
    }
    else {
        message = "Could not load data/levels.json";
    }

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        processInput();
        update(deltaTime);
        draw();
    }

    CloseWindow();
}

void GameApp::runAction(int action, int value, int secondValue) {
    challengeActive = false;
    challengeFinished = false;
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    lcaChallenge = false;
    heightPhase = 0;
    pendingNodeId = -1;
    answerInput.clear();
    enteredHeights.clear();
    expectedRoute.clear();
    events.clear();
    if (action == 0) {
        loadCurrentLevel();
    }
    else {
        model.performAction(action, value, secondValue, events);
    }
    bool degenerationLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 5;
    message = action == 0 && degenerationLevel
        ? "Sorted deliveries create a one-sided BST. Search for the final treasure."
        : model.statusText();
    alarmSteps = events.countAlarmSteps();
    wrongClicks = 0;
    stars = 0;
    score = 0;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    eventTimer = 0.0f;
    refreshView();
}

void GameApp::loadCurrentLevel() {
    int scenarioId = 1;
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        scenarioId = level.scenarioId;
        if (level.act == 2 && level.number == 5) {
            model.loadScenario(scenarioId, events);
            for (int key = 10; key <= 70; key += 10) {
                events.clear();
                model.performAction(ACTION_INSERT, key, 0, events);
            }
            events.clear();
            return;
        }
    }
    model.loadScenario(scenarioId, events);
}

std::string GameApp::traversalName(int action) {
    if (action == ACTION_TRAVERSE_PREORDER) return "preorder";
    if (action == ACTION_TRAVERSE_INORDER) return "inorder";
    if (action == ACTION_TRAVERSE_POSTORDER) return "postorder";
    return "level order";
}

void GameApp::startTraversalChallenge(int action) {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    events.clear();
    if (model.performAction(action, 0, 0, events) == false) {
        message = "Could not start traversal challenge";
        return;
    }

    expectedRoute.clear();
    for (int i = 0; i < events.size(); i++) {
        Event& event = events.get(i);
        if (event.type == EV_VISIT) {
            expectedRoute.push(event.nodeId);
        }
    }

    events.clear();
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) optimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
        else alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    else {
        alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    challengeActive = expectedRoute.size() > 0;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Act 1.1: click rooms in " + traversalName(action) + " order";
}

void GameApp::startSearchChallenge(int target) {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    events.clear();
    if (!model.performAction(ACTION_SEARCH, target, 0, events)) {
        message = "Could not prepare the search challenge";
        return;
    }

    expectedRoute.clear();
    expectedCompareCodes.clear();
    for (int i = 0; i < events.size(); i++) {
        Event& event = events.get(i);
        if (event.type == EV_COMPARE) {
            expectedRoute.push(event.nodeId);
            expectedCompareCodes.push(event.code);
        }
    }
    events.clear();
    if (expectedRoute.size() == 0) {
        message = "The model returned no comparison steps";
        return;
    }

    searchTarget = target;
    searchChallenge = true;
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) optimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
        else alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    else {
        alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Search challenge: follow comparisons to find " +
        std::to_string(target);
}

void GameApp::startInsertChallenge(int target) {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    copyViewData(view, challengeStartView);
    EventLog plannedEvents;
    if (!model.performAction(ACTION_INSERT, target, 0, plannedEvents)) {
        message = model.statusText();
        return;
    }

    expectedRoute.clear();
    expectedCompareCodes.clear();
    pendingInsertEvent = Event();
    bool hasInsert = false;
    for (int i = 0; i < plannedEvents.size(); i++) {
        Event& event = plannedEvents.get(i);
        if (event.type == EV_COMPARE) {
            expectedRoute.push(event.nodeId);
            expectedCompareCodes.push(event.code);
        }
        else if (event.type == EV_INSERT) {
            pendingInsertEvent = event;
            hasInsert = true;
        }
    }
    if (!hasInsert || expectedRoute.size() == 0) {
        int scenarioId = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()
            ? levelManager.getLevel(currentLevelIndex).scenarioId : 1;
        EventLog resetEvents;
        model.loadScenario(scenarioId, resetEvents);
        copyViewData(challengeStartView, view);
        rebuildLayout();
        message = "Model did not provide the expected insertion events";
        return;
    }

    model.copyView(challengeFinalView);
    copyViewData(challengeStartView, view);
    rebuildLayout();
    events.clear();
    insertTarget = target;
    insertChallenge = true;
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) optimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
        else alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    else {
        alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Deliver " + std::to_string(target) +
        ": click the BST comparison path";
}

void GameApp::startDeleteChallenge(int target) {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    balanceChallenge = false;
    copyViewData(view, challengeStartView);
    int targetIndex = -1;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].value == target) {
            targetIndex = i;
            break;
        }
    }
    if (targetIndex < 0) {
        message = "The delete target is missing from this scenario";
        return;
    }
    int childCount = 0;
    if (view.treeNodes[targetIndex].leftId >= 0) childCount++;
    if (view.treeNodes[targetIndex].rightId >= 0) childCount++;
    expectedDeleteCase = childCount == 0 ? 1 : childCount == 1 ? 2 : 3;

    EventLog plannedEvents;
    if (!model.performAction(ACTION_DELETE, target, 0, plannedEvents)) {
        message = model.statusText();
        return;
    }
    expectedRoute.clear();
    expectedCompareCodes.clear();
    pendingDeleteEvents.clear();
    for (int i = 0; i < plannedEvents.size(); i++) {
        Event& event = plannedEvents.get(i);
        if (event.type == EV_COMPARE) {
            expectedRoute.push(event.nodeId);
            expectedCompareCodes.push(event.code);
        }
        else if (event.type != EV_DONE) {
            pendingDeleteEvents.push(event);
        }
    }
    if (expectedRoute.size() == 0 || pendingDeleteEvents.size() == 0) {
        int scenarioId = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()
            ? levelManager.getLevel(currentLevelIndex).scenarioId : 1;
        EventLog resetEvents;
        model.loadScenario(scenarioId, resetEvents);
        copyViewData(challengeStartView, view);
        rebuildLayout();
        message = "Model did not provide the expected delete events";
        return;
    }
    model.copyView(challengeFinalView);
    copyViewData(challengeStartView, view);
    rebuildLayout();
    events.clear();
    deleteTarget = target;
    deleteChallenge = true;
    deleteCasePending = true;
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) optimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
        else alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    else {
        alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    challengeActive = false;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Choose the deletion case for treasure " + std::to_string(target);
}

void GameApp::selectDeleteCase(int selectedCase) {
    if (!deleteChallenge || !deleteCasePending) return;
    if (selectedCase != expectedDeleteCase) {
        message = "That case does not match this node. Choose the correct case.";
        return;
    }
    deleteCasePending = false;
    challengeActive = true;
    expectedIndex = 0;
    message = "Correct case. Click the BST comparison path to delete the node.";
}

void GameApp::startRangeChallenge(int low, int high) {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    events.clear();
    skippedSubtreeRoots.clear();
    if (low > high) {
        message = "Range low value must not exceed high value";
        return;
    }
    EventLog plannedEvents;
    if (!model.performAction(ACTION_RANGE_QUERY, low, high, plannedEvents)) {
        message = model.statusText();
        return;
    }

    expectedRoute.clear();
    expectedCompareCodes.clear();
    skippedSubtreeRoots.clear();
    for (int i = 0; i < plannedEvents.size(); i++) {
        Event& event = plannedEvents.get(i);
        if (event.type == EV_VISIT) {
            expectedRoute.push(event.nodeId);
        }
        else if (event.type == EV_HIGHLIGHT_SUBTREE && event.code == HL_SKIPPED) {
            skippedSubtreeRoots.push(event.nodeId);
        }
    }
    if (expectedRoute.size() == 0) {
        message = "No rooms fall inside that range";
        return;
    }

    events.clear();
    for (int i = 0; i < plannedEvents.size(); i++) {
        Event& event = plannedEvents.get(i);
        if (event.type == EV_HIGHLIGHT_SUBTREE && event.code == HL_SKIPPED) {
            events.add(event);
        }
    }
    rangeLow = low;
    rangeHigh = high;
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) optimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
        else alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    else {
        alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    }
    rangeChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    eventTimer = 0.0f;
    highlightedNodeId = -1;
    message = "Select every room with key " + std::to_string(low) + " through " +
        std::to_string(high);
}

void GameApp::startBalanceChallenge() {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    balanceChallenge = false;
    events.clear();
    expectedRoute.clear();
    expectedCompareCodes.clear();
    balanceFound.clear();
    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int leftHeight = 0;
        int rightHeight = 0;
        int leftIndex = findNodeIndex(node.leftId);
        int rightIndex = findNodeIndex(node.rightId);
        if (leftIndex >= 0) leftHeight = view.treeNodes[leftIndex].height;
        if (rightIndex >= 0) rightHeight = view.treeNodes[rightIndex].height;
        int balance = leftHeight - rightHeight;
        if (balance > 1 || balance < -1) {
            expectedRoute.push(node.id);
            balanceFound.push(0);
        }
    }
    if (expectedRoute.size() == 0) {
        message = "No unbalanced rooms were supplied by this scenario";
        return;
    }
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    int optimalSteps = expectedRoute.size();
    alarmCapacity = (3 * optimalSteps + 1) / 2 + 2;
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
    }
    balanceChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Click every room whose balance factor is outside -1 to 1";
}

bool GameApp::isLevel(int act, int number) {
    return currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == act &&
        levelManager.getLevel(currentLevelIndex).number == number;
}

int GameApp::enteredHeightFor(int id) {
    for (int i = 0; i < expectedRoute.size(); i++) {
        if (expectedRoute[i] == id && i < enteredHeights.size()) {
            return enteredHeights[i];
        }
    }
    return -1;
}

// Act 1.3. The model supplies the postorder route and the diameter once, up front.
// The player then types every height (bottom-up) and finally the diameter.
void GameApp::startHeightChallenge() {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    deleteCasePending = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    events.clear();
    expectedRoute.clear();
    enteredHeights.clear();
    answerInput.clear();
    pendingNodeId = -1;
    heightPhase = 0;
    while (GetCharPressed() > 0) {
        // throw away keys typed before the challenge started
    }

    EventLog plannedRoute;
    if (model.performAction(ACTION_TRAVERSE_POSTORDER, 0, 0, plannedRoute) == false) {
        message = "Could not start the height challenge";
        return;
    }
    for (int i = 0; i < plannedRoute.size(); i++) {
        Event& event = plannedRoute.get(i);
        if (event.type == EV_VISIT) {
            expectedRoute.push(event.nodeId);
            enteredHeights.push(-1);
        }
    }
    if (expectedRoute.size() == 0) {
        message = "This scenario has no rooms to measure";
        return;
    }

    EventLog plannedDiameter;
    if (model.performAction(ACTION_DIAMETER, 0, 0, plannedDiameter) == false) {
        expectedRoute.clear();
        enteredHeights.clear();
        message = "The model cannot report a diameter yet";
        return;
    }
    expectedDiameter = 0;
    for (int i = 0; i < plannedDiameter.size(); i++) {
        if (plannedDiameter.get(i).type == EV_DONE) {
            expectedDiameter = plannedDiameter.get(i).value;
        }
    }

    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    heightOptimalSteps = expectedRoute.size() + 1;   // one visit per room + the diameter report
    alarmCapacity = (3 * heightOptimalSteps + 1) / 2 + 2;   // ceil(1.5 * optimal) + 2
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        if (level.optimalSteps > 0) heightOptimalSteps = level.optimalSteps;
        if (level.alarmCapacity > 0) alarmCapacity = level.alarmCapacity;
    }
    heightChallenge = true;
    heightPhase = 1;
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = true;
    eventTimer = 0.0f;
    highlightedNodeId = -1;
    message = "Click rooms in postorder (left, right, room). Type each height, leaf = 1.";
}

void GameApp::heightPenalty(std::string text) {
    alarmSteps++;
    wrongClicks++;
    if (alarmSteps >= alarmCapacity) {
        finishHeightLevel(false);
    }
    else {
        message = text;
    }
}

void GameApp::finishHeightLevel(bool answeredAll) {
    challengeActive = false;
    challengeFinished = true;
    heightPhase = 0;
    pendingNodeId = -1;
    answerInput.clear();
    if (answeredAll == false || alarmSteps >= alarmCapacity) {
        stars = 0;
        score = 0;
        message = "Alarm full. Level failed - retry.";
    }
    else {
        if (wrongClicks == 0 && alarmSteps <= heightOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= heightOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "Heights and diameter confirmed. Vault mapped!";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    Event done;
    done.type = EV_DONE;
    events.add(done);
    screenState = 3;
}

// Act 1.4: the model computes the LCA and path length. The player chooses two
// rooms and then identifies the returned ancestor; the GUI never walks links.
void GameApp::startLcaChallenge() {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    lcaChallenge = true;
    lcaFirstNode = -1;
    lcaSecondNode = -1;
    expectedLcaNode = -1;
    expectedLcaPathLength = 0;
    expectedIndex = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    events.clear();
    challengeActive = true;
    challengeFinished = false;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    alarmCapacity = 6;
    message = "Select two rooms, then click their lowest common ancestor.";
}

void GameApp::finishLcaChallenge(bool correct) {
    challengeActive = false;
    challengeFinished = true;
    if (correct && alarmSteps < alarmCapacity) {
        stars = wrongClicks == 0 ? 3 : wrongClicks == 1 ? 2 : 1;
        score = 1000 - alarmSteps * 10 - wrongClicks * 50;
        if (score < 0) score = 0;
        Event result;
        result.type = EV_DONE;
        result.nodeId = expectedLcaNode;
        result.value = expectedLcaPathLength;
        events.add(result);
        message = "Correct. Path length: " + std::to_string(expectedLcaPathLength) + " edges.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Level failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

// Act 1.5 is a model operation: ask the model to mirror, then draw its view.
void GameApp::startMirrorChallenge() {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    lcaChallenge = false;
    challengeActive = false;
    challengeFinished = false;
    events.clear();
    if (!model.performAction(ACTION_MIRROR, 0, 0, events)) {
        message = model.statusText();
        return;
    }
    refreshView();
    alarmSteps = events.countAlarmSteps();
    wrongClicks = 0;
    stars = 3;
    score = 1000 - alarmSteps * 10;
    if (score < 0) score = 0;
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    message = "Vault mirrored. Compare the new left and right branches.";
    challengeFinished = true;
    screenState = 3;
}

void GameApp::handleHeightClick(int id) {
    if (heightPhase != 1) {
        message = "All heights are in. Type the diameter and press Enter.";
        return;
    }
    if (expectedIndex < expectedRoute.size() && id == expectedRoute[expectedIndex]) {
        pendingNodeId = id;
        answerInput.clear();
        highlightedNodeId = id;
        message = "Type this room's height (leaf = 1), then press Enter.";
        return;
    }
    heightPenalty("Wrong room. +1 alarm step. Postorder: left subtree, right subtree, then the room.");
}

void GameApp::submitHeightAnswer() {
    if (heightChallenge == false || challengeActive == false) {
        return;
    }
    if (answerInput.empty()) {
        message = "Type a number first.";
        return;
    }
    int answer = 0;
    for (int i = 0; i < (int)answerInput.size(); i++) {
        answer = answer * 10 + (answerInput[i] - '0');
    }

    if (heightPhase == 1) {
        if (pendingNodeId < 0) {
            message = "Click the next room in postorder first.";
            return;
        }
        int nodeIndex = findNodeIndex(pendingNodeId);
        int correctHeight = nodeIndex >= 0 ? view.treeNodes[nodeIndex].height : -1;
        if (answer != correctHeight) {
            answerInput.clear();
            heightPenalty("Wrong height. +1 alarm step. Height = 1 + the taller child (leaf = 1).");
            return;
        }
        Event visit;
        visit.type = EV_VISIT;
        visit.nodeId = pendingNodeId;
        visit.value = view.treeNodes[nodeIndex].value;
        events.add(visit);
        Event update;
        update.type = EV_UPDATE_HEIGHT;
        update.nodeId = pendingNodeId;
        update.oldValue = 0;
        update.newValue = correctHeight;
        events.add(update);
        enteredHeights[expectedIndex] = correctHeight;
        alarmSteps++;
        expectedIndex++;
        currentEvent = events.size() - 1;
        nextEvent = events.size();
        highlightedNodeId = pendingNodeId;
        eventTimer = 0.0f;
        pendingNodeId = -1;
        answerInput.clear();
        if (alarmSteps >= alarmCapacity) {
            finishHeightLevel(false);
        }
        else if (expectedIndex == expectedRoute.size()) {
            heightPhase = 2;
            message = "All heights in. Now type the diameter: edges on the longest path between any two rooms.";
        }
        else {
            message = "Correct. Click the next room in postorder.";
        }
        return;
    }

    if (heightPhase == 2) {
        if (answer != expectedDiameter) {
            answerInput.clear();
            heightPenalty("Wrong diameter. +1 alarm step. The longest path may not pass through the root.");
            return;
        }
        alarmSteps++;
        finishHeightLevel(true);
    }
}

void GameApp::handleNodeClick(int id) {
    if (challengeActive == false || challengeFinished == true) {
        return;
    }

    if (heightChallenge) {
        handleHeightClick(id);
        return;
    }

    if (lcaChallenge) {
        if (lcaFirstNode < 0) {
            lcaFirstNode = id;
            highlightedNodeId = id;
            expectedIndex = 1;
            alarmSteps++;
            Event selected;
            selected.type = EV_VISIT;
            selected.nodeId = id;
            int index = findNodeIndex(id);
            if (index >= 0) selected.value = view.treeNodes[index].value;
            events.add(selected);
            message = "First room selected. Choose a different second room.";
        }
        else if (lcaSecondNode < 0) {
            if (id == lcaFirstNode) {
                alarmSteps++;
                wrongClicks++;
                if (alarmSteps >= alarmCapacity) finishLcaChallenge(false);
                else message = "Choose two different rooms.";
            }
            else {
                lcaSecondNode = id;
                expectedIndex = 2;
                alarmSteps++;
                Event selected;
                selected.type = EV_VISIT;
                selected.nodeId = id;
                int index = findNodeIndex(id);
                if (index >= 0) selected.value = view.treeNodes[index].value;
                events.add(selected);
                EventLog answer;
                if (!model.performAction(ACTION_LCA, lcaFirstNode, lcaSecondNode, answer)) {
                    message = model.statusText();
                    return;
                }
                expectedLcaNode = -1;
                expectedLcaPathLength = 0;
                for (int i = 0; i < answer.size(); i++) {
                    Event& event = answer.get(i);
                    if (event.type == EV_DONE) {
                        expectedLcaNode = event.nodeId;
                        expectedLcaPathLength = event.value;
                    }
                }
                if (expectedLcaNode < 0) {
                    message = "The model did not return a common ancestor.";
                    return;
                }
                message = "Now click the lowest common ancestor.";
            }
        }
        else {
            alarmSteps++;
            if (id == expectedLcaNode) finishLcaChallenge(true);
            else {
                wrongClicks++;
                highlightedNodeId = id;
                if (alarmSteps >= alarmCapacity) finishLcaChallenge(false);
                else message = "Not the lowest common ancestor. Try again.";
            }
        }
        return;
    }

    if (searchChallenge || insertChallenge || deleteChallenge) {
        if (expectedIndex < expectedRoute.size() &&
            id == expectedRoute[expectedIndex]) {
            Event event;
            event.type = EV_COMPARE;
            event.nodeId = id;
            event.value = insertChallenge ? insertTarget :
                deleteChallenge ? deleteTarget : searchTarget;
            event.code = expectedCompareCodes[expectedIndex];
            events.add(event);
            alarmSteps++;
            expectedIndex++;
            currentEvent = events.size() - 1;
            nextEvent = events.size();
            highlightedNodeId = id;
            eventTimer = 0.0f;
            if (expectedIndex == expectedRoute.size()) {
                if ((insertChallenge || deleteChallenge) && alarmSteps < alarmCapacity) {
                    if (insertChallenge) events.add(pendingInsertEvent);
                    for (int i = 0; i < pendingDeleteEvents.size(); i++) {
                        events.add(pendingDeleteEvents[i]);
                    }
                    Event done;
                    done.type = EV_DONE;
                    events.add(done);
                    copyViewData(challengeFinalView, view);
                    rebuildLayout();
                }
                challengeActive = false;
                challengeFinished = true;
                int optimalSteps = expectedRoute.size();
                if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
                    levelManager.getLevel(currentLevelIndex).optimalSteps > 0) {
                    optimalSteps = levelManager.getLevel(currentLevelIndex).optimalSteps;
                }
                bool targetFound = insertChallenge || deleteChallenge ||
                    expectedCompareCodes[expectedIndex - 1] == CMP_EQUAL;
                if (!targetFound || alarmSteps >= alarmCapacity) stars = 0;
                else if (wrongClicks == 0 && alarmSteps <= optimalSteps) stars = 3;
                else if (wrongClicks <= 1 && alarmSteps * 4 <= optimalSteps * 5) stars = 2;
                else stars = 1;
                score = stars > 0 ? 1000 - 10 * alarmSteps - 50 * wrongClicks : 0;
                if (score < 0) score = 0;
                message = stars > 0 ? (insertChallenge ? "Room inserted. Route complete." :
                    deleteChallenge ? "Node deleted. Case and path correct." :
                    "Target found! Search complete.") :
                    targetFound ? "Alarm full. Level failed - retry." :
                    "Target was not found. Level failed - retry.";
                levelManager.recordResult(currentLevelIndex, stars, score);
                levelManager.saveProgress("campaign_progress.dat");
                if (!insertChallenge && !deleteChallenge) {
                    Event done;
                    done.type = EV_DONE;
                    events.add(done);
                }
                screenState = 3;
            }
            else {
                int comparison = expectedCompareCodes[expectedIndex - 1];
                if (insertChallenge) {
                    message = comparison == CMP_LESS ? "New key is larger. Continue right." :
                        "New key is smaller. Continue left.";
                }
                else {
                    message = comparison == CMP_LESS ? "Target is larger. Continue right." :
                        comparison == CMP_GREATER ? "Target is smaller. Continue left." :
                        "Match found. Continue to confirm the route.";
                }
            }
            return;
        }

        alarmSteps++;
        wrongClicks++;
        if (alarmSteps >= alarmCapacity) {
            challengeActive = false;
            challengeFinished = true;
            stars = 0;
            score = 0;
            message = "Alarm full. Level failed - retry.";
            levelManager.recordResult(currentLevelIndex, 0, 0);
            levelManager.saveProgress("campaign_progress.dat");
            screenState = 3;
        }
        else {
            message = "Wrong room. +1 alarm step. Follow the BST comparison path.";
        }
        return;
    }

    if (balanceChallenge) {
        int foundIndex = -1;
        for (int i = 0; i < expectedRoute.size(); i++) {
            if (expectedRoute[i] == id && balanceFound[i] == 0) {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex >= 0) {
            Event event;
            event.type = EV_BALANCE_ALERT;
            event.nodeId = id;
            int nodeIndex = findNodeIndex(id);
            if (nodeIndex >= 0) {
                int leftHeight = 0;
                int rightHeight = 0;
                int leftIndex = findNodeIndex(view.treeNodes[nodeIndex].leftId);
                int rightIndex = findNodeIndex(view.treeNodes[nodeIndex].rightId);
                if (leftIndex >= 0) leftHeight = view.treeNodes[leftIndex].height;
                if (rightIndex >= 0) rightHeight = view.treeNodes[rightIndex].height;
                event.value = leftHeight - rightHeight;
            }
            events.add(event);
            balanceFound[foundIndex] = 1;
            alarmSteps++;
            expectedIndex++;
            currentEvent = events.size() - 1;
            nextEvent = events.size();
            highlightedNodeId = id;
            if (expectedIndex == expectedRoute.size()) {
                challengeActive = false;
                challengeFinished = true;
                int optimalSteps = expectedRoute.size();
                if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
                    levelManager.getLevel(currentLevelIndex).optimalSteps > 0) {
                    optimalSteps = levelManager.getLevel(currentLevelIndex).optimalSteps;
                }
                if (alarmSteps >= alarmCapacity) stars = 0;
                else if (wrongClicks == 0 && alarmSteps <= optimalSteps) stars = 3;
                else if (wrongClicks <= 1 && alarmSteps * 4 <= optimalSteps * 5) stars = 2;
                else stars = 1;
                score = stars > 0 ? 1000 - 10 * alarmSteps - 50 * wrongClicks : 0;
                if (score < 0) score = 0;
                message = stars > 0 ? "All unbalanced rooms identified." :
                    "Alarm full. Level failed - retry.";
                levelManager.recordResult(currentLevelIndex, stars, score);
                levelManager.saveProgress("campaign_progress.dat");
                Event done;
                done.type = EV_DONE;
                events.add(done);
                screenState = 3;
            }
            else {
                message = "Unbalanced room found. Keep checking the badges.";
            }
        }
        else {
            alarmSteps++;
            wrongClicks++;
            if (alarmSteps >= alarmCapacity) {
                challengeActive = false;
                challengeFinished = true;
                stars = 0;
                score = 0;
                message = "Alarm full. Level failed - retry.";
                levelManager.recordResult(currentLevelIndex, 0, 0);
                levelManager.saveProgress("campaign_progress.dat");
                screenState = 3;
            }
            else {
                message = "That room is balanced or already checked.";
            }
        }
        return;
    }

    if (expectedIndex < expectedRoute.size() &&
        id == expectedRoute[expectedIndex]) {
        Event event;
        event.type = EV_VISIT;
        event.nodeId = id;
        int nodeIndex = findNodeIndex(id);
        if (nodeIndex >= 0) {
            event.value = view.treeNodes[nodeIndex].value;
        }
        events.add(event);
        alarmSteps++;
        expectedIndex++;
        currentEvent = events.size() - 1;
        nextEvent = events.size();
        highlightedNodeId = id;
        eventTimer = 0.0f;

        if (expectedIndex == expectedRoute.size()) {
            challengeActive = false;
            challengeFinished = true;
            int optimalSteps = expectedRoute.size();
            if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
                levelManager.getLevel(currentLevelIndex).optimalSteps > 0) {
                optimalSteps = levelManager.getLevel(currentLevelIndex).optimalSteps;
            }
            if (alarmSteps >= alarmCapacity) {
                stars = 0;
                message = "Alarm full. Level failed - reset and retry.";
            }
            else if (wrongClicks == 0 && alarmSteps <= optimalSteps) {
                stars = 3;
                message = "Level complete! 3 stars";
            }
            else if (wrongClicks <= 1 &&
                alarmSteps * 4 <= optimalSteps * 5) {
                stars = 2;
                message = "Level complete! 2 stars";
            }
            else {
                stars = 1;
                message = "Level complete! 1 star";
            }
            if (rangeChallenge && stars > 0) {
                message = "Range cleared. All matching rooms found.";
            }
            score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
            if (score < 0) score = 0;
            levelManager.recordResult(currentLevelIndex, stars, score);
            levelManager.saveProgress("campaign_progress.dat");
            Event done;
            done.type = EV_DONE;
            events.add(done);
            screenState = 3;
        }
        else {
            message = rangeChallenge ? "Correct room. Keep selecting in-range rooms." :
                "Correct. Keep following the traversal order.";
        }
        return;
    }

    alarmSteps++;
    wrongClicks++;
    if (alarmSteps >= alarmCapacity) {
        challengeActive = false;
        challengeFinished = true;
        stars = 0;
        score = 0;
        message = "Alarm full. Level failed - reset and retry.";
        levelManager.recordResult(currentLevelIndex, 0, 0);
        levelManager.saveProgress("campaign_progress.dat");
        screenState = 3;
    }
    else {
        message = "Wrong room. +1 alarm step. Try again.";
    }
}

void GameApp::refreshView() {
    model.copyView(view);
    rebuildLayout();
}

void GameApp::copyViewData(ModelView& source, ModelView& destination) {
    destination.clear();
    destination.viewType = source.viewType;
    destination.heapSize = source.heapSize;
    for (int i = 0; i < source.treeNodes.size(); i++) {
        destination.treeNodes.push(source.treeNodes[i]);
    }
    for (int i = 0; i < source.heapValues.size(); i++) {
        destination.heapValues.push(source.heapValues[i]);
    }
}

void GameApp::processInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (screenState == 2 || screenState == 3) {
            if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
                activeAct = levelManager.getLevel(currentLevelIndex).act;
            }
            screenState = 1;
        }
        else if (screenState == 1) {
            screenState = 0;
        }
        else if (screenState == 4) {
            screenState = 0;
        }
        return;
    }

    if (screenState == 4) {
        int codepoint = GetCharPressed();
        while (codepoint > 0) {
            if (codepoint >= '0' && codepoint <= '9') {
                if (sandboxSecondFieldFocused) sandboxSecondInput += (char)codepoint;
                else sandboxValueInput += (char)codepoint;
            }
            else if (codepoint == '-' &&
                (sandboxSecondFieldFocused ? sandboxSecondInput : sandboxValueInput).empty()) {
                if (sandboxSecondFieldFocused) sandboxSecondInput += '-';
                else sandboxValueInput += '-';
            }
            codepoint = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            std::string& field = sandboxSecondFieldFocused ? sandboxSecondInput : sandboxValueInput;
            if (!field.empty()) field.pop_back();
        }
        if (IsKeyPressed(KEY_ENTER)) {
            executeSandboxAction(sandboxAction);
        }
    }

    if (screenState == 2) {
        if (heightChallenge && challengeActive) {
            bool accepting = heightPhase == 2 || (heightPhase == 1 && pendingNodeId >= 0);
            int codepoint = GetCharPressed();
            while (codepoint > 0) {
                if (accepting && codepoint >= '0' && codepoint <= '9' && answerInput.size() < 3) {
                    answerInput += (char)codepoint;
                }
                codepoint = GetCharPressed();
            }
            if (accepting) {
                if (IsKeyPressed(KEY_BACKSPACE) && !answerInput.empty()) answerInput.pop_back();
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                    submitHeightAnswer();
                    return;
                }
            }
        }
        if (IsKeyPressed(KEY_SPACE)) {
            animationPlaying = !animationPlaying;
            return;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            stepPlayback(-1);
            return;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            stepPlayback(1);
            return;
        }
        if (IsKeyPressed(KEY_R)) {
            restartPlayback();
            return;
        }
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    if (screenState == 0) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 490, 320, 300, 58 })) {
            screenState = 1;
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 490, 394, 300, 58 })) {
            screenState = 4;
            currentLevelIndex = -1;
            sandboxValueInput.clear();
            sandboxSecondInput.clear();
            sandboxSecondFieldFocused = false;
            runAction(0, 0, 0);
        }
        return;
    }

    if (screenState == 1) {
        for (int act = 1; act <= 4; act++) {
            Rectangle actBounds = Rectangle{ 36.0f + (act - 1) * 310.0f,
                                             122.0f, 278.0f, 72.0f };
            if (CheckCollisionPointRec(mouse, actBounds)) {
                activeAct = act;
                return;
            }
        }
        for (int row = 0; row < 5; row++) {
            Rectangle bounds = Rectangle{ 80, (float)(232 + row * 70), 350, 56 };
            if (CheckCollisionPointRec(mouse, bounds)) {
                int index = levelManager.findLevel(activeAct, row + 1);
                if (index >= 0 && levelManager.isUnlocked(index)) {
                    currentLevelIndex = index;
                    runAction(0, 0, 0);
                    screenState = 2;
                }
                return;
            }
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 725, 180, 44 })) {
            screenState = 0;
        }
        return;
    }

    if (screenState == 3) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 390, 560, 220, 52 })) {
            runAction(0, 0, 0);
            screenState = 2;
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 650, 560, 220, 52 })) {
            screenState = 1;
        }
        return;
    }

    if (screenState == 4) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 725, 180, 44 })) {
            screenState = 0;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 118, 210, 40 })) {
            sandboxSecondFieldFocused = false;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 194, 210, 40 })) {
            sandboxSecondFieldFocused = true;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 260, 210, 42 })) {
            executeSandboxAction(ACTION_SEARCH);
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 24, 310, 210, 42 })) {
            executeSandboxAction(ACTION_INSERT);
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 24, 360, 210, 42 })) {
            executeSandboxAction(ACTION_DELETE);
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 24, 410, 210, 42 })) {
            executeSandboxAction(ACTION_RANGE_QUERY);
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 24, 460, 210, 42 })) {
            runAction(0, 0, 0);
        }
        else {
            for (int i = 0; i < positions.size(); i++) {
                Vector2 center = Vector2{ positions[i].x, positions[i].y };
                if (CheckCollisionPointCircle(mouse, center, 31.0f)) {
                    highlightedNodeId = positions[i].id;
                    break;
                }
            }
        }
        return;
    }

    if (CheckCollisionPointRec(mouse, Rectangle{ 285, 82, 82, 38 })) {
        restartPlayback();
        return;
    }
    else if (CheckCollisionPointRec(mouse, Rectangle{ 375, 82, 70, 38 })) {
        stepPlayback(-1);
        return;
    }
    else if (CheckCollisionPointRec(mouse, Rectangle{ 453, 82, 82, 38 })) {
        animationPlaying = !animationPlaying;
        return;
    }
    else if (CheckCollisionPointRec(mouse, Rectangle{ 543, 82, 70, 38 })) {
        stepPlayback(1);
        return;
    }

    bool isSearchLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        (levelManager.getLevel(currentLevelIndex).number == 1 ||
            levelManager.getLevel(currentLevelIndex).number == 5);
    bool isInsertLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 2;
    bool isDeleteLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 3;
    bool isRangeLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 4;
    bool isBalanceLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 3 &&
        levelManager.getLevel(currentLevelIndex).number == 1;
    bool isHeightLevel = isLevel(1, 3);
    bool isLcaLevel = isLevel(1, 4);
    bool isMirrorLevel = isLevel(1, 5);
    if (isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        int target = levelManager.getLevel(currentLevelIndex).targetValue;
        startSearchChallenge(target);
    }
    else if (isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isInsertLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        int target = levelManager.getLevel(currentLevelIndex).targetValue;
        startInsertChallenge(target);
    }
    else if (isInsertLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isDeleteLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        int target = levelManager.getLevel(currentLevelIndex).targetValue;
        startDeleteChallenge(target);
    }
    else if (isDeleteLevel && deleteCasePending &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        selectDeleteCase(1);
    }
    else if (isDeleteLevel && deleteCasePending &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
        selectDeleteCase(2);
    }
    else if (isDeleteLevel && deleteCasePending &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 232, 220, 42 })) {
        selectDeleteCase(3);
    }
    else if (isDeleteLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 282, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isRangeLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        startRangeChallenge(level.rangeLow, level.rangeHigh);
    }
    else if (isRangeLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isBalanceLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startBalanceChallenge();
    }
    else if (isBalanceLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isHeightLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startHeightChallenge();
    }
    else if (isHeightLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isLcaLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startLcaChallenge();
    }
    else if (isLcaLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isMirrorLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startMirrorChallenge();
    }
    else if (isMirrorLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_PREORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_INORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_POSTORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 232, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_LEVEL_ORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 282, 220, 42 })) {
        runAction(ACTION_SEARCH, 60, 0);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 332, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else {
        for (int i = 0; i < positions.size(); i++) {
            Vector2 center = Vector2{ positions[i].x, positions[i].y };
            if (CheckCollisionPointCircle(mouse, center, 31.0f)) {
                handleNodeClick(positions[i].id);
                break;
            }
        }
    }
}

void GameApp::update(float deltaTime) {
    hoveredNodeId = -1;
    if (view.viewType == MODEL_VIEW_TREE) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < positions.size(); i++) {
            Vector2 center = Vector2{ positions[i].x, positions[i].y };
            if (CheckCollisionPointCircle(mouse, center, 31.0f)) {
                hoveredNodeId = positions[i].id;
                break;
            }
        }
    }

    if (!animationPlaying || events.size() == 0) {
        return;
    }
    eventTimer += deltaTime;
    if (eventTimer >= 0.55f) {
        if (nextEvent < events.size()) {
            currentEvent = nextEvent;
            Event& event = events.get(currentEvent);
            highlightedNodeId = event.nodeId;
            nextEvent++;
        }
        else {
            animationPlaying = false;
        }
        eventTimer = 0.0f;
    }
}

void GameApp::draw() {
    BeginDrawing();
    ClearBackground(Color{ 18, 24, 36, 255 });
    if (screenState == 0) {
        drawMainMenu();
        EndDrawing();
        return;
    }
    if (screenState == 1) {
        drawCampaignMap();
        EndDrawing();
        return;
    }
    if (screenState == 3) {
        drawResultScreen();
        EndDrawing();
        return;
    }
    if (screenState == 4) {
        drawSandbox();
        EndDrawing();
        return;
    }

    DrawText("VAULT HEIST", 24, 22, 30, Color{ 240, 220, 170, 255 });
    DrawText("Tree Lab - GUI mock connected through GameModel", 285, 54, 16,
        Color{ 175, 190, 210, 255 });
    drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{ 62, 76, 96, 255 });

    if (view.viewType == MODEL_VIEW_HEAP) {
        drawHeapView();
    }
    else {
        drawControls();
        drawTree();
    }
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
        Color{ 220, 224, 230, 255 });
    EndDrawing();
}

void GameApp::drawButton(Rectangle bounds, std::string label, bool enabled) {
    Color fill = enabled ? Color{ 42, 57, 78, 255 } : Color{ 31, 39, 52, 255 };
    Color outline = enabled ? Color{ 91, 113, 143, 255 } : Color{ 62, 70, 82, 255 };
    Color text = enabled ? Color{ 230, 235, 242, 255 } : Color{ 125, 133, 144, 255 };
    DrawRectangleRounded(bounds, 0.12f, 6, fill);
    DrawRectangleLinesEx(bounds, 2.0f, outline);
    DrawText(label.c_str(), (int)bounds.x + 18, (int)bounds.y + 17, 20, text);
}

void GameApp::drawMainMenu() {
    DrawText("VAULT HEIST", 380, 170, 58, Color{ 240, 220, 170, 255 });
    DrawText("A data structures heist", 480, 240, 22, Color{ 175, 190, 210, 255 });
    drawButton(Rectangle{ 490, 320, 300, 58 }, "Campaign", true);
    drawButton(Rectangle{ 490, 394, 300, 58 }, "Sandbox", true);
    DrawText("Choose a mode to enter the vault.", 474, 490, 18,
        Color{ 150, 165, 185, 255 });
    if (levelManager.count() == 0) {
        DrawText("Level data failed to load: data/levels.json", 390, 550, 18,
            Color{ 220, 135, 115, 255 });
    }
}

void GameApp::drawSandbox() {
    DrawText("VAULT HEIST - SANDBOX", 24, 22, 30, Color{ 240, 220, 170, 255 });
    DrawText("Explore model operations on the demo tree", 285, 54, 16,
        Color{ 175, 190, 210, 255 });
    drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{ 62, 76, 96, 255 });

    DrawText("VALUE", 24, 94, 14, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 118, 210, 40, Color{ 25, 34, 48, 255 });
    DrawRectangleLinesEx(Rectangle{ 24, 118, 210, 40 }, 2.0f,
        sandboxSecondFieldFocused ? Color{ 62, 76, 96, 255 } :
        Color{ 240, 220, 170, 255 });
    DrawText(sandboxValueInput.c_str(), 36, 128, 18, Color{ 230, 235, 242, 255 });

    DrawText("RANGE HIGH", 24, 170, 14, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 194, 210, 40, Color{ 25, 34, 48, 255 });
    DrawRectangleLinesEx(Rectangle{ 24, 194, 210, 40 }, 2.0f,
        sandboxSecondFieldFocused ? Color{ 240, 220, 170, 255 } :
        Color{ 62, 76, 96, 255 });
    DrawText(sandboxSecondInput.c_str(), 36, 204, 18, Color{ 230, 235, 242, 255 });

    const char* labels[5] = { "Search", "Insert", "Delete", "Range query", "Reset demo" };
    for (int i = 0; i < 5; i++) {
        drawButton(Rectangle{ 24, (float)(260 + i * 50), 210, 42 }, labels[i], true);
    }
    drawButton(Rectangle{ 24, 725, 180, 44 }, "Main menu", true);
    drawTree();
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
        Color{ 220, 224, 230, 255 });
}

void GameApp::executeSandboxAction(int action) {
    int value = 0;
    int secondValue = 0;
    try {
        if (sandboxValueInput.empty()) throw 0;
        value = std::stoi(sandboxValueInput);
        if (action == ACTION_RANGE_QUERY) {
            if (sandboxSecondInput.empty()) throw 0;
            secondValue = std::stoi(sandboxSecondInput);
        }
    }
    catch (...) {
        message = action == ACTION_RANGE_QUERY
            ? "Enter a low value and a high value first."
            : "Enter an integer value first.";
        return;
    }
    if (action == ACTION_RANGE_QUERY && value > secondValue) {
        message = "Range low value must be less than or equal to high value.";
        return;
    }
    sandboxAction = action;
    runAction(action, value, secondValue);
}

void GameApp::drawCampaignMap() {
    DrawText("CAMPAIGN MAP", 36, 28, 32, Color{ 240, 220, 170, 255 });
    DrawText("Complete each wing to unlock the next.", 38, 68, 18,
        Color{ 175, 190, 210, 255 });

    std::string actNames[4] = {
        "ACT 1 - Blueprint Wing", "ACT 2 - Sorted Wing",
        "ACT 3 - Architect's Wing", "ACT 4 - Heap Fortress"
    };
    for (int act = 1; act <= 4; act++) {
        Rectangle bounds = Rectangle{ 36.0f + (act - 1) * 310.0f,
                                     122.0f, 278.0f, 72.0f };
        drawButton(bounds, actNames[act - 1], true);
    }

    DrawText(TextFormat("ACT %i LEVELS", activeAct), 42, 207, 18,
        Color{ 150, 175, 205, 255 });
    for (int row = 0; row < 5; row++) {
        int index = levelManager.findLevel(activeAct, row + 1);
        if (index < 0) continue;
        LevelDefinition& level = levelManager.getLevel(index);
        bool unlocked = levelManager.isUnlocked(index);
        std::string label = level.code() + "  " + level.title;
        if (levelManager.isCompleted(index)) {
            label += "  *" + std::to_string(levelManager.starsFor(index));
        }
        drawButton(Rectangle{ 80, (float)(232 + row * 70), 350, 56 }, label, unlocked);
    }

    int previewIndex = levelManager.findLevel(activeAct, 1);
    if (previewIndex >= 0) {
        LevelDefinition& level = levelManager.getLevel(previewIndex);
        DrawText(level.objective.c_str(), 490, 258, 18, Color{ 205, 212, 224, 255 });
        DrawText(level.conceptName.c_str(), 490, 300, 17, Color{ 150, 165, 185, 255 });
    }
    drawButton(Rectangle{ 24, 725, 180, 44 }, "Main menu", true);
}

void GameApp::drawResultScreen() {
    std::string resultTitle = "LEVEL RESULT";
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        resultTitle = "LEVEL " + levelManager.getLevel(currentLevelIndex).code() + " RESULT";
    }
    DrawText(resultTitle.c_str(), 380, 170, 36, Color{ 240, 220, 170, 255 });
    if (stars > 0) {
        DrawText("Vault route completed", 430, 238, 24, Color{ 150, 205, 160, 255 });
    }
    else {
        DrawText("The alarm caught you", 440, 238, 24, Color{ 220, 135, 115, 255 });
    }
    DrawText(TextFormat("Stars: %i", stars), 490, 310, 22,
        Color{ 240, 220, 170, 255 });
    DrawText(TextFormat("Score: %i", score), 490, 350, 22,
        Color{ 220, 224, 230, 255 });
    DrawText(TextFormat("Alarm steps: %i / %i", alarmSteps, alarmCapacity),
        490, 390, 20, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong clicks: %i", wrongClicks), 490, 425, 20,
        Color{ 205, 212, 224, 255 });
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 5) {
        int height = 0;
        for (int i = 0; i < view.treeNodes.size(); i++) {
            if (view.treeNodes[i].parentId < 0) {
                height = view.treeNodes[i].height;
                break;
            }
        }
        DrawText(TextFormat("Corridor height: %i for %i treasures. AVL trees stay balanced.",
            height, view.treeNodes.size()),
            315, 475, 18, Color{ 175, 190, 210, 255 });
    }
    drawButton(Rectangle{ 390, 560, 220, 52 }, "Retry level", true);
    drawButton(Rectangle{ 650, 560, 220, 52 }, "Campaign map", true);
}

void GameApp::drawControls() {
    bool isSearchLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        (levelManager.getLevel(currentLevelIndex).number == 1 ||
            levelManager.getLevel(currentLevelIndex).number == 5);
    bool isInsertLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 2;
    bool isDeleteLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 3;
    bool isRangeLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 4;
    bool isBalanceLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 3 &&
        levelManager.getLevel(currentLevelIndex).number == 1;
    bool isHeightLevel = isLevel(1, 3);
    bool isLcaLevel = isLevel(1, 4);
    bool isMirrorLevel = isLevel(1, 5);
    if (isSearchLevel || isInsertLevel || isDeleteLevel || isRangeLevel || isBalanceLevel || isHeightLevel || isLcaLevel || isMirrorLevel) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        bool degenerationLevel = levelManager.getLevel(currentLevelIndex).number == 5;
        const char* heading = isSearchLevel
            ? (degenerationLevel ? "ACT 2.5 - DEGENERATION" : "ACT 2.1 - BST SEARCH") :
            isInsertLevel ? "ACT 2.2 - BST INSERT" :
            isDeleteLevel ? "ACT 2.3 - BST DELETE" :
            isRangeLevel ? "ACT 2.4 - BST RANGE" :
            isHeightLevel ? "ACT 1.3 - HEIGHT & DIAMETER" :
            isLcaLevel ? "ACT 1.4 - LOWEST COMMON ANCESTOR" :
            isMirrorLevel ? "ACT 1.5 - MIRROR THE VAULT" :
            "ACT 3.1 - BALANCE BADGES";
        DrawText(heading,
            24, 55, 16, Color{ 150, 175, 205, 255 });
        std::string startLabel = (isSearchLevel ? "Search " :
            isInsertLevel ? "Insert " :
            isDeleteLevel ? "Delete " : "Range ") +
            std::to_string(level.targetValue);
        if (isBalanceLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, "Check badges", true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        }
        else if (isHeightLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, "Start heights", true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        }
        else if (isLcaLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, "Start LCA", true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        }
        else if (isMirrorLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, "Mirror tree", true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        }
        else if (isRangeLevel) {
            startLabel = "Range " + std::to_string(level.rangeLow) + "-" +
                std::to_string(level.rangeHigh);
            drawButton(Rectangle{ 20, 82, 220, 42 }, startLabel, true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        }
        else if (isDeleteLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, startLabel, true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Leaf", deleteCasePending);
            drawButton(Rectangle{ 20, 182, 220, 42 }, "One child", deleteCasePending);
            drawButton(Rectangle{ 20, 232, 220, 42 }, "Two children", deleteCasePending);
            drawButton(Rectangle{ 20, 282, 220, 42 }, "Reset level", true);
        }
        else {
            std::string labels[2] = { startLabel, "Reset level" };
            for (int i = 0; i < 2; i++) {
                int y = 82 + i * 50;
                drawButton(Rectangle{ 20, (float)y, 220, 42 }, labels[i], true);
            }
        }
    }
    else {
        DrawText("ACT 1.1 - TRAVERSAL", 24, 55, 16, Color{ 150, 175, 205, 255 });
    }
    std::string labels[6] = {
        "Quiz: Preorder", "Quiz: Inorder", "Quiz: Postorder", "Quiz: Level order",
        "Search 60", "Reset level"
    };
    for (int i = 0; i < ((isSearchLevel || isInsertLevel || isDeleteLevel || isRangeLevel || isBalanceLevel || isHeightLevel || isLcaLevel || isMirrorLevel) ? 0 : 6); i++) {
        int y = 82 + i * 50;
        DrawRectangleRounded(Rectangle{ 20, (float)y, 220, 42 }, 0.15f, 6,
            Color{ 42, 57, 78, 255 });
        DrawRectangleLinesEx(Rectangle{ 20, (float)y, 220, 42 }, 2.0f,
            Color{ 91, 113, 143, 255 });
        DrawText(labels[i].c_str(), 34, y + 12, 17, Color{ 230, 235, 242, 255 });
    }

    if (isHeightLevel) {
        drawHeightPanel();
    }

    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    int displayCapacity = alarmCapacity;
    if (displayCapacity < 1) displayCapacity = 10;
    int alarm = alarmSteps;
    if (alarm > displayCapacity) alarm = displayCapacity;
    int fillWidth = 210 * alarm / displayCapacity;
    DrawRectangle(24, 430, fillWidth, 14, Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, displayCapacity), 24, 451, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Correct: %i / %i", expectedIndex, expectedRoute.size()),
        24, 477, 15, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong clicks: %i", wrongClicks), 24, 500, 15,
        Color{ 150, 165, 185, 255 });
    DrawText(TextFormat("Stars: %i   Score: %i", stars, score), 24, 525, 15,
        Color{ 240, 220, 170, 255 });
    if (lcaChallenge) {
        int firstIndex = findNodeIndex(lcaFirstNode);
        int secondIndex = findNodeIndex(lcaSecondNode);
        std::string selections = "Rooms: ";
        selections += firstIndex >= 0 ? std::to_string(view.treeNodes[firstIndex].value) : "?";
        selections += " and ";
        selections += secondIndex >= 0 ? std::to_string(view.treeNodes[secondIndex].value) : "?";
        DrawText(selections.c_str(), 24, 552, 14, Color{ 205, 212, 224, 255 });
        if (expectedLcaNode >= 0) {
            DrawText(TextFormat("Path: %i edges", expectedLcaPathLength), 24, 572, 14,
                Color{ 240, 220, 170, 255 });
        }
    }
}

void GameApp::drawHeightPanel() {
    Color hint = Color{ 150, 165, 185, 255 };
    Color bright = Color{ 230, 235, 242, 255 };
    DrawText("Leaf = 1. Heights go", 24, 190, 14, hint);
    DrawText("bottom-up (postorder).", 24, 208, 14, hint);
    bool asking = heightChallenge && challengeActive &&
        (heightPhase == 2 || (heightPhase == 1 && pendingNodeId >= 0));
    if (heightChallenge && challengeActive && heightPhase == 1 && pendingNodeId < 0) {
        DrawText("Click the next room.", 24, 250, 15, Color{ 240, 220, 170, 255 });
    }
    if (asking) {
        std::string label = "Diameter (edges):";
        if (heightPhase == 1) {
            int nodeIndex = findNodeIndex(pendingNodeId);
            int shownValue = nodeIndex >= 0 ? view.treeNodes[nodeIndex].value : 0;
            label = "Height of room " + std::to_string(shownValue) + ":";
        }
        DrawText(label.c_str(), 24, 240, 15, Color{ 240, 220, 170, 255 });
        DrawRectangle(24, 262, 210, 40, Color{ 25, 34, 48, 255 });
        DrawRectangleLinesEx(Rectangle{ 24, 262, 210, 40 }, 2.0f, Color{ 240, 220, 170, 255 });
        std::string shown = answerInput;
        if (((int)(GetTime() * 2.0)) % 2 == 0) shown += "_";
        DrawText(shown.c_str(), 36, 272, 20, bright);
        DrawText("Enter to submit", 24, 308, 13, hint);
    }
}

void GameApp::drawTree() {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int parentPosition = findPositionIndex(node.id);
        if (parentPosition < 0) continue;
        if (node.leftId >= 0) {
            int childPosition = findPositionIndex(node.leftId);
            if (childPosition >= 0) {
                Color edge = isInSkippedSubtree(node.leftId)
                    ? Color{ 65, 72, 82, 255 } : Color{ 92, 112, 138, 255 };
                DrawLineEx(Vector2{ positions[parentPosition].x, positions[parentPosition].y },
                    Vector2{ positions[childPosition].x, positions[childPosition].y },
                    3.0f, edge);
            }
        }
        if (node.rightId >= 0) {
            int childPosition = findPositionIndex(node.rightId);
            if (childPosition >= 0) {
                Color edge = isInSkippedSubtree(node.rightId)
                    ? Color{ 65, 72, 82, 255 } : Color{ 92, 112, 138, 255 };
                DrawLineEx(Vector2{ positions[parentPosition].x, positions[parentPosition].y },
                    Vector2{ positions[childPosition].x, positions[childPosition].y },
                    3.0f, edge);
            }
        }
    }

    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int p = findPositionIndex(node.id);
        if (p < 0) continue;
        Color fill = isInSkippedSubtree(node.id)
            ? Color{ 55, 61, 71, 255 } :
            node.id == highlightedNodeId || node.id == hoveredNodeId
            ? Color{ 205, 154, 64, 255 } : Color{ 47, 76, 112, 255 };
        DrawCircleV(Vector2{ positions[p].x, positions[p].y }, 29.0f, fill);
        DrawCircleLines((int)positions[p].x, (int)positions[p].y, 29.0f,
            Color{ 188, 205, 226, 255 });
        std::string label = std::to_string(node.value);
        DrawText(label.c_str(), (int)positions[p].x - 15,
            (int)positions[p].y - 9, 18, Color{ 255, 255, 255, 255 });
        if (heightChallenge) {
            int entered = enteredHeightFor(node.id);
            if (entered >= 0) {
                DrawText(TextFormat("h=%i", entered), (int)positions[p].x - 14,
                    (int)positions[p].y - 50, 16, Color{ 150, 205, 160, 255 });
            }
        }
        bool showBalanceBadges = currentLevelIndex >= 0 &&
            currentLevelIndex < levelManager.count() &&
            levelManager.getLevel(currentLevelIndex).act == 3 &&
            levelManager.getLevel(currentLevelIndex).number == 1;
        if (showBalanceBadges) {
            int leftHeight = 0;
            int rightHeight = 0;
            int leftIndex = findNodeIndex(node.leftId);
            int rightIndex = findNodeIndex(node.rightId);
            if (leftIndex >= 0) leftHeight = view.treeNodes[leftIndex].height;
            if (rightIndex >= 0) rightHeight = view.treeNodes[rightIndex].height;
            int balance = leftHeight - rightHeight;
            Color badge = balance > 1 || balance < -1
                ? Color{ 205, 95, 78, 255 } : Color{ 65, 145, 105, 255 };
            DrawRectangle((int)positions[p].x - 21, (int)positions[p].y + 33,
                42, 20, badge);
            DrawText(TextFormat("BF %i", balance),
                (int)positions[p].x - 18, (int)positions[p].y + 36,
                11, Color{ 255, 255, 255, 255 });
        }
        if (node.id == hoveredNodeId) {
            int leftHeight = 0;
            int rightHeight = 0;
            int leftIndex = findNodeIndex(node.leftId);
            int rightIndex = findNodeIndex(node.rightId);
            if (leftIndex >= 0) leftHeight = view.treeNodes[leftIndex].height;
            if (rightIndex >= 0) rightHeight = view.treeNodes[rightIndex].height;
            int balance = leftHeight - rightHeight;
            DrawRectangle((int)positions[p].x + 35, (int)positions[p].y - 28,
                176, 80, Color{ 25, 34, 48, 245 });
            DrawRectangleLines((int)positions[p].x + 35,
                (int)positions[p].y - 28, 176, 80,
                Color{ 91, 113, 143, 255 });
            DrawText(TextFormat("Key: %i", node.value),
                (int)positions[p].x + 44, (int)positions[p].y - 15,
                14, Color{ 230, 235, 242, 255 });
            bool hideHeight = heightChallenge && challengeActive &&
                enteredHeightFor(node.id) < 0;
            if (hideHeight) {
                // Act 1.3: do not leak the answer through the tooltip.
                DrawText(TextFormat("Height: ?  Depth: %i", positions[p].depth),
                    (int)positions[p].x + 44, (int)positions[p].y + 3,
                    12, Color{ 175, 190, 210, 255 });
                DrawText(TextFormat("Size: %i", node.size),
                    (int)positions[p].x + 44, (int)positions[p].y + 20,
                    12, Color{ 175, 190, 210, 255 });
            }
            else {
                DrawText(TextFormat("Height: %i  Depth: %i", node.height,
                    positions[p].depth),
                    (int)positions[p].x + 44, (int)positions[p].y + 3,
                    12, Color{ 175, 190, 210, 255 });
                DrawText(TextFormat("Balance: %i  Size: %i", balance, node.size),
                    (int)positions[p].x + 44, (int)positions[p].y + 20,
                    12, Color{ 175, 190, 210, 255 });
            }
        }
    }
}

void GameApp::drawHeapView() {
    DrawText("MAX HEAP", 24, 55, 16, Color{ 150, 175, 205, 255 });
    DrawText(TextFormat("Active heap size: %i / %i", view.heapSize,
        view.heapValues.size()),
        290, 90, 18, Color{ 205, 212, 224, 255 });

    int activeSize = view.heapSize;
    if (activeSize < 0) activeSize = 0;
    if (activeSize > view.heapValues.size()) activeSize = view.heapValues.size();

    int depth = 0;
    int firstAtDepth = 0;
    int lastAtDepth = 1;
    for (int i = 0; i < activeSize; i++) {
        while (i >= lastAtDepth) {
            depth++;
            firstAtDepth = lastAtDepth;
            lastAtDepth = lastAtDepth * 2 + 1;
        }
        float slots = (float)(lastAtDepth - firstAtDepth);
        float slot = (float)(i - firstAtDepth);
        float x = 790.0f + ((slot + 0.5f) / slots - 0.5f) * 820.0f;
        float y = 190.0f + depth * 112.0f;
        bool swapPartner = currentEvent >= 0 && currentEvent < events.size() &&
            events.get(currentEvent).type == EV_HEAP_SWAP &&
            i == events.get(currentEvent).otherId;
        bool selected = i == highlightedNodeId || swapPartner;

        if (i > 0) {
            int parent = (i - 1) / 2;
            int parentDepth = 0;
            int parentFirst = 0;
            int parentLast = 1;
            while (parent >= parentLast) {
                parentDepth++;
                parentFirst = parentLast;
                parentLast = parentLast * 2 + 1;
            }
            float parentSlots = (float)(parentLast - parentFirst);
            float parentSlot = (float)(parent - parentFirst);
            float parentX = 790.0f +
                ((parentSlot + 0.5f) / parentSlots - 0.5f) * 820.0f;
            float parentY = 190.0f + parentDepth * 112.0f;
            DrawLineEx(Vector2{ parentX, parentY }, Vector2{ x, y }, 3.0f,
                Color{ 92, 112, 138, 255 });
        }
        DrawCircleV(Vector2{ x, y }, 29.0f,
            selected ? Color{ 205, 154, 64, 255 } : Color{ 47, 76, 112, 255 });
        DrawCircleLines((int)x, (int)y, 29.0f, Color{ 188, 205, 226, 255 });
        std::string label = std::to_string(view.heapValues[i]);
        DrawText(label.c_str(), (int)x - 15, (int)y - 9, 18,
            Color{ 255, 255, 255, 255 });
    }

    DrawText("HEAP ARRAY", 290, 635, 16, Color{ 150, 175, 205, 255 });
    int cellWidth = 72;
    int gap = 8;
    if (activeSize > 0 && activeSize * (cellWidth + gap) > windowWidth - 320) {
        cellWidth = (windowWidth - 320) / activeSize - gap;
        if (cellWidth < 30) cellWidth = 30;
    }
    for (int i = 0; i < view.heapValues.size(); i++) {
        int x = 290 + i * (cellWidth + gap);
        bool active = i < activeSize;
        bool selected = i == highlightedNodeId ||
            (currentEvent >= 0 && currentEvent < events.size() &&
                events.get(currentEvent).type == EV_HEAP_SWAP &&
                i == events.get(currentEvent).otherId);
        Color fill = selected ? Color{ 205, 154, 64, 255 } :
            active ? Color{ 42, 57, 78, 255 } : Color{ 31, 39, 52, 255 };
        Color outline = active ? Color{ 91, 113, 143, 255 } : Color{ 62, 70, 82, 255 };
        DrawRectangle(x, 665, cellWidth, 48, fill);
        DrawRectangleLines(x, 665, cellWidth, 48, outline);
        std::string value = std::to_string(view.heapValues[i]);
        DrawText(value.c_str(), x + 8, 671, 18, Color{ 230, 235, 242, 255 });
        DrawText(TextFormat("%i", i), x + 8, 695, 12,
            Color{ 150, 165, 185, 255 });
    }
}

void GameApp::drawEventLog() {
    int panelX = windowWidth - 300;
    DrawRectangle(panelX, 72, 270, 230, Color{ 25, 34, 48, 255 });
    DrawText("EVENT LOG", panelX + 16, 88, 16, Color{ 170, 190, 215, 255 });
    int first = events.size() - 8;
    if (first < 0) first = 0;
    int row = 0;
    for (int i = first; i < events.size(); i++) {
        Event& event = events.get(i);
        std::string line = describeEvent(event);
        Color color = i == currentEvent ? Color{ 240, 220, 170, 255 } :
            event.type == EV_DONE
            ? Color{ 150, 195, 150, 255 } : Color{ 215, 221, 232, 255 };
        DrawText(line.c_str(), panelX + 16, 118 + row * 22, 14, color);
        row++;
    }
}

void GameApp::drawPlaybackControls() {
    drawButton(Rectangle{ 285, 82, 82, 38 }, "Restart", true);
    drawButton(Rectangle{ 375, 82, 70, 38 }, "Prev", events.size() > 0);
    drawButton(Rectangle{ 453, 82, 82, 38 }, animationPlaying ? "Pause" : "Play", true);
    drawButton(Rectangle{ 543, 82, 70, 38 }, "Next", events.size() > 0);
    std::string progress = "Event ";
    progress += std::to_string(currentEvent >= 0 ? currentEvent + 1 : 0);
    progress += " / ";
    progress += std::to_string(events.size());
    DrawText(progress.c_str(), 625, 94, 14, Color{ 150, 165, 185, 255 });
}

void GameApp::restartPlayback() {
    if (events.size() == 0) return;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    eventTimer = 0.0f;
    animationPlaying = true;
}

void GameApp::stepPlayback(int direction) {
    if (events.size() == 0) return;
    int index = currentEvent;
    if (index < 0) index = direction < 0 ? events.size() - 1 : 0;
    else index += direction;
    if (index < 0) index = 0;
    if (index >= events.size()) index = events.size() - 1;
    currentEvent = index;
    nextEvent = index + 1;
    highlightedNodeId = events.get(index).nodeId;
    eventTimer = 0.0f;
    animationPlaying = false;
}

std::string GameApp::describeEvent(Event& event) {
    if (event.type == EV_VISIT) {
        return "Visit node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_COMPARE) {
        std::string relation = event.code == CMP_LESS ? " < " :
            event.code == CMP_GREATER ? " > " : " = ";
        return "Node " + std::to_string(event.nodeId) + relation + "target " +
            std::to_string(event.value);
    }
    if (event.type == EV_DONE) {
        return "Action complete";
    }
    if (event.type == EV_INSERT) {
        return "Insert " + std::to_string(event.value) +
            " at node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_UNLINK) {
        return "Remove node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_SWAP_VALUE) {
        return "Swap values at nodes " + std::to_string(event.nodeId) +
            " and " + std::to_string(event.otherId);
    }
    if (event.type == EV_HIGHLIGHT_SUBTREE) {
        return event.code == HL_SKIPPED
            ? "Skip subtree at node " + std::to_string(event.nodeId)
            : "Highlight subtree at node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_UPDATE_HEIGHT) {
        return "Height of node " + std::to_string(event.nodeId) + " = " +
            std::to_string(event.newValue);
    }
    if (event.type == EV_HEAP_BUILD) return "Build max heap";
    if (event.type == EV_HEAP_SWAP) {
        return "Swap array indexes " + std::to_string(event.nodeId) +
            " and " + std::to_string(event.otherId);
    }
    if (event.type == EV_HEAPIFY_START) return "Heapify started";
    if (event.type == EV_HEAPIFY_DONE) return "Heapify complete";
    if (event.type == EV_EXTRACT_MAX) return "Extract max " + std::to_string(event.value);
    if (event.type == EV_MARK_SORTED) return "Mark sorted index " + std::to_string(event.nodeId);
    if (event.type == EV_HEAP_SIZE_CHANGE) return "Heap size: " + std::to_string(event.value);
    if (event.type == EV_HEAP_SORT_COMPLETE) return "Heap sort complete";
    return "Model event " + std::to_string(event.type);
}

void GameApp::rebuildLayout() {
    positions.clear();
    int cursor = 0;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].parentId < 0) {
            assignPositions(view.treeNodes[i].id, 0, cursor);
            break;
        }
    }
}

void GameApp::assignPositions(int nodeId, int depth, int& cursor) {
    int nodeIndex = findNodeIndex(nodeId);
    if (nodeIndex < 0) return;
    ViewNode& node = view.treeNodes[nodeIndex];
    if (node.leftId >= 0) {
        assignPositions(node.leftId, depth + 1, cursor);
    }

    GuiNodePosition position;
    position.id = nodeId;
    position.depth = depth;
    float step = 900.0f / (view.treeNodes.size() + 1);
    position.x = 310.0f + step * (cursor + 1);
    position.y = 180.0f + depth * 125.0f;
    positions.push(position);
    cursor++;

    if (node.rightId >= 0) {
        assignPositions(node.rightId, depth + 1, cursor);
    }
}

int GameApp::findNodeIndex(int id) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].id == id) return i;
    }
    return -1;
}

int GameApp::findPositionIndex(int id) {
    for (int i = 0; i < positions.size(); i++) {
        if (positions[i].id == id) return i;
    }
    return -1;
}

bool GameApp::isInSkippedSubtree(int id) {
    int currentId = id;
    while (currentId >= 0) {
        for (int i = 0; i < skippedSubtreeRoots.size(); i++) {
            if (skippedSubtreeRoots[i] == currentId) return true;
        }
        int index = findNodeIndex(currentId);
        if (index < 0) return false;
        currentId = view.treeNodes[index].parentId;
    }
    return false;
}
