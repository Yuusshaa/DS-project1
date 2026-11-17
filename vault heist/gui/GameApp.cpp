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
    sandboxStructureType = 1;
    sandboxOperationPage = 0;
    sandboxSeed = 1;
    sandboxRandomState = 1;
    endlessScore = 0;
    endlessLives = 3;
    endlessTargetAction = ACTION_INSERT;
    endlessTargetKey = 0;
    endlessOperations = 0;
    endlessSeed = 1;
    endlessRandomState = 1;
    insertTarget = 0;
    deleteTarget = 0;
    pendingNodeId = -1;
    heightPhase = 0;
    expectedDiameter = 0;
    lcaFirstNode = -1;
    lcaSecondNode = -1;
    expectedLcaNode = -1;
    expectedLcaPathLength = 0;
    lcaPhase = 0;
    lcaPathCurrentNode = -1;
    lcaPathProgress = 0;
    lcaOptimalSteps = 0;
    rotationStage = 0;
    rotationPhase = 0;
    rotationCase = ROT_NONE;
    rotationPivotId = -1;
    rotationActionCount = 0;
    rotationActionIndex = 0;
    rotationExpectedActions[0] = 0;
    rotationExpectedActions[1] = 0;
    rotationExpectedPivots[0] = -1;
    rotationExpectedPivots[1] = -1;
    rotationOptimalSteps = 0;
    selectedHeapIndex = -1;
    heapBuildSwaps = 0;
    heapBuildOptimalSteps = 4;
    heapifyStartIndex = -1;
    heapifyOperationCount = 0;
    heapifyOptimalSteps = 1;
    heapCompletionType = 0;
    heapExtractCount = 0;
    heapExtractOptimalSteps = 7;
    heapRacePhase = 0;
    heapRaceExtractionCount = 0;
    heapRaceTotalExtractions = 7;
    heapRaceOptimalSteps = 8;
    reconstructionRound = 0;
    reconstructionNodeCount = 0;
    reconstructionDragValue = -1;
    reconstructionDragSourceSlot = -1;
    reconstructionOptimalSteps = 13;
    avlPredictionStage = 0;
    avlPredictionExpected[0] = ROT_NONE;
    avlPredictionExpected[1] = ROT_LL;
    avlPredictionExpected[2] = ROT_RR;
    avlPredictionExpected[3] = ROT_LR;
    avlPredictionExpected[4] = ROT_RL;
    avlPredictionInsertValues[0] = 60;
    avlPredictionInsertValues[1] = 20;
    avlPredictionInsertValues[2] = 60;
    avlPredictionInsertValues[3] = 40;
    avlPredictionInsertValues[4] = 60;
    avlPredictionCorrectCount = 0;
    avlPredictionOptimalSteps = 5;
    orderStatisticNodeId = -1;
    orderStatisticK = 5;
    orderStatisticRank = -1;
    mirrorAnimationActive = false;
    mirrorAnimationTime = 0.0f;
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
    rotationChallenge = false;
    heapBuildChallenge = false;
    heapifyChallenge = false;
    heapCompletionPending = false;
    heapCompletionReady = false;
    heapExtractChallenge = false;
    heapExtractWaiting = false;
    heapRaceChallenge = false;
    heapRaceWaiting = false;
    reconstructionChallenge = false;
    avlPredictionChallenge = false;
    avlPredictionWaiting = false;
    orderStatisticChallenge = false;
    animationPlaying = true;
    sandboxSecondFieldFocused = false;
    sandboxSeedFieldFocused = false;
    endlessActive = false;
    endlessPromptActive = false;
    endlessOperationWaiting = false;
    endlessSaveAvailable = false;
    eventTimer = 0.0f;
    heapRaceSecondsRemaining = 0.0f;
    heapRaceTimeLimit = 60.0f;
    endlessSurvivalTime = 0.0f;
    endlessPromptSeconds = 0.0f;
    endlessPromptLimit = 4.5f;
    message = "Starting demo";
}

void GameApp::run() {
    InitWindow(windowWidth, windowHeight, "Vault Heist - Tree Lab");
    SetTargetFPS(60);
    loadLeaderboard();
    FILE* endlessSave = std::fopen("endless_run.dat", "r");
    endlessSaveAvailable = endlessSave != nullptr;
    if (endlessSave != nullptr) std::fclose(endlessSave);
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
    rotationChallenge = false;
    heapBuildChallenge = false;
    heapifyChallenge = false;
    heapCompletionPending = false;
    heapCompletionReady = false;
    heapExtractChallenge = false;
    heapExtractWaiting = false;
    heapRaceChallenge = false;
    heapRaceWaiting = false;
    reconstructionChallenge = false;
    avlPredictionChallenge = false;
    avlPredictionWaiting = false;
    orderStatisticChallenge = false;
    selectedHeapIndex = -1;
    if (action == 0) {
        heapBuildSwaps = 0;
        heapifyOperationCount = 0;
        heapExtractCount = 0;
        heapRaceExtractionCount = 0;
        heapRaceTotalExtractions = 0;
        heapRacePhase = 0;
        heapRaceSecondsRemaining = 0.0f;
        reconstructionRound = 0;
        reconstructionNodeCount = 0;
        reconstructionDragValue = -1;
        reconstructionDragSourceSlot = -1;
        avlPredictionStage = 0;
        avlPredictionCorrectCount = 0;
    }
    mirrorAnimationActive = false;
    mirrorAnimationTime = 0.0f;
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

void GameApp::startOrderStatisticChallenge() {
    orderStatisticChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    orderStatisticK = 5;
    orderStatisticNodeId = -1;
    orderStatisticRank = -1;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    alarmCapacity = 10;
    events.clear();
    EventLog answer;
    if (!model.performAction(ACTION_KTH, orderStatisticK, 0, answer)) {
        orderStatisticChallenge = false;
        challengeActive = false;
        message = model.statusText();
        return;
    }
    for (int i = 0; i < answer.size(); i++) {
        if (answer.get(i).type == EV_DONE) orderStatisticNodeId = answer.get(i).nodeId;
        events.add(answer.get(i));
    }
    if (orderStatisticNodeId < 0) {
        orderStatisticChallenge = false;
        challengeActive = false;
        message = "The model did not return the requested order statistic.";
        return;
    }
    currentEvent = -1;
    nextEvent = 0;
    alarmSteps = events.countAlarmSteps();
    animationPlaying = true;
    highlightedNodeId = -1;
    message = "Find the 5th smallest key. Each node shows its subtree size.";
}

int GameApp::nextEndlessRandom() {
    long long next = (long long)endlessRandomState * 48271LL % 2147483647LL;
    endlessRandomState = (int)next;
    return endlessRandomState;
}

void GameApp::startEndlessRun() {
    removeEndlessSave();
    endlessSaveAvailable = false;
    currentLevelIndex = -1;
    endlessSeed = (int)(GetTime() * 1000.0) + 1;
    long long normalizedSeed = (long long)endlessSeed % 2147483646LL;
    if (normalizedSeed <= 0) normalizedSeed += 2147483646LL;
    endlessSeed = (int)normalizedSeed;
    endlessRandomState = endlessSeed;
    endlessScore = 0;
    endlessLives = 3;
    endlessOperations = 0;
    endlessSurvivalTime = 0.0f;
    endlessPromptSeconds = 0.0f;
    endlessPromptLimit = 4.5f;
    endlessActive = true;
    endlessPromptActive = false;
    endlessOperationWaiting = false;
    events.clear();
    EventLog setup;
    if (!model.loadScenario(25, setup)) {
        endlessActive = false;
        message = model.statusText();
        screenState = 0;
        return;
    }
    screenState = 5;
    animationPlaying = false;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    eventTimer = 0.0f;
    refreshView();
    makeEndlessPrompt();
}

void GameApp::continueEndlessRun() {
    FILE* file = std::fopen("endless_run.dat", "r");
    if (file == nullptr) {
        endlessSaveAvailable = false;
        message = "No saved Endless run is available.";
        return;
    }

    char version[8] = {};
    float savedTime = 0.0f;
    float savedPromptSeconds = 0.0f;
    float savedPromptLimit = 0.0f;
    int savedLives = 0;
    int savedOperations = 0;
    int savedSeed = 0;
    int savedRandomState = 0;
    int savedAction = 0;
    int savedKey = 0;
    int savedPromptActive = 0;
    int savedOperationWaiting = 0;
    int nodeCount = 0;
    bool valid = std::fscanf(file, "%7s", version) == 1 &&
        std::fscanf(file, "%f %d %d %d %d %d %d %f %f %d %d %d",
            &savedTime, &savedLives, &savedOperations, &savedSeed,
            &savedRandomState, &savedAction, &savedKey, &savedPromptSeconds,
            &savedPromptLimit, &savedPromptActive, &savedOperationWaiting,
            &nodeCount) == 12;
    if (std::string(version) != "VHES1" || savedTime < 0.0f ||
        savedLives < 1 || savedLives > 3 || savedOperations < 0 ||
        savedRandomState <= 0 ||
        (savedAction != ACTION_INSERT && savedAction != ACTION_DELETE) ||
        savedPromptSeconds < 0.0f || savedPromptLimit < 1.0f ||
        nodeCount < 0 || nodeCount > 100) valid = false;

    DynArray<int> savedKeys;
    for (int i = 0; valid && i < nodeCount; i++) {
        int key = 0;
        if (std::fscanf(file, "%d", &key) != 1) valid = false;
        else savedKeys.push(key);
    }
    std::fclose(file);
    if (!valid) {
        message = "Saved Endless run is invalid or damaged.";
        return;
    }

    EventLog setup;
    if (!model.loadScenario(25, setup)) {
        message = model.statusText();
        return;
    }
    EventLog restoreEvents;
    for (int i = 0; i < savedKeys.size(); i++) {
        restoreEvents.clear();
        if (!model.performAction(ACTION_INSERT, savedKeys[i], 0, restoreEvents)) {
            message = "Could not rebuild the saved Endless tree.";
            return;
        }
    }

    endlessSurvivalTime = savedTime;
    endlessScore = (int)savedTime;
    endlessLives = savedLives;
    endlessOperations = savedOperations;
    endlessSeed = savedSeed;
    endlessRandomState = savedRandomState;
    endlessTargetAction = savedAction;
    endlessTargetKey = savedKey;
    endlessPromptSeconds = savedPromptSeconds;
    endlessPromptLimit = savedPromptLimit;
    endlessPromptActive = savedPromptActive != 0;
    endlessOperationWaiting = savedOperationWaiting != 0;
    endlessActive = true;
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = false;
    currentLevelIndex = -1;
    refreshView();
    screenState = 5;
    removeEndlessSave();
    endlessSaveAvailable = false;
    if (!endlessPromptActive && endlessOperationWaiting) {
        endlessOperationWaiting = false;
        makeEndlessPrompt();
    }
    else {
        message = "Saved run restored. Continue the incoming operation.";
    }
}

bool GameApp::saveEndlessRun() {
    if (!endlessActive || screenState != 5) return false;
    FILE* file = std::fopen("endless_run.dat", "w");
    if (file == nullptr) {
        message = "Could not write endless_run.dat.";
        return false;
    }

    std::fprintf(file, "VHES1\n%.3f %d %d %d %d %d %d %.3f %.3f %d %d %d\n",
        endlessSurvivalTime, endlessLives, endlessOperations, endlessSeed,
        endlessRandomState, endlessTargetAction, endlessTargetKey,
        endlessPromptSeconds, endlessPromptLimit, endlessPromptActive ? 1 : 0,
        endlessOperationWaiting ? 1 : 0, view.treeNodes.size());
    int maximumDepth = 0;
    for (int i = 0; i < positions.size(); i++) {
        if (positions[i].depth > maximumDepth) maximumDepth = positions[i].depth;
    }
    for (int depth = 0; depth <= maximumDepth; depth++) {
        for (int i = 0; i < positions.size(); i++) {
            if (positions[i].depth != depth) continue;
            int nodeIndex = findNodeIndex(positions[i].id);
            if (nodeIndex >= 0) std::fprintf(file, "%d ", view.treeNodes[nodeIndex].value);
        }
    }
    std::fprintf(file, "\n");
    bool saved = std::fclose(file) == 0;
    if (!saved) {
        message = "Could not finish writing endless_run.dat.";
        return false;
    }
    endlessSaveAvailable = true;
    message = "Endless run saved. Select Continue Endless from the main menu.";
    return true;
}

void GameApp::removeEndlessSave() {
    std::remove("endless_run.dat");
}

void GameApp::makeEndlessPrompt() {
    if (!endlessActive) return;
    int treeSize = view.treeNodes.size();
    if (treeSize < 3) endlessTargetAction = ACTION_INSERT;
    else if (treeSize >= 14) endlessTargetAction = ACTION_DELETE;
    else endlessTargetAction = nextEndlessRandom() % 2 == 0
        ? ACTION_INSERT : ACTION_DELETE;

    if (endlessTargetAction == ACTION_INSERT) {
        int attempts = 0;
        do {
            endlessTargetKey = nextEndlessRandom() % 2001 - 1000;
            attempts++;
        } while (sandboxContainsKey(endlessTargetKey) && attempts < 200);
        if (sandboxContainsKey(endlessTargetKey)) {
            missEndlessPrompt("Could not choose a unique treasure key.");
            return;
        }
    }
    else {
        if (treeSize <= 0) {
            endlessTargetAction = ACTION_INSERT;
            endlessTargetKey = nextEndlessRandom() % 2001 - 1000;
        }
        else {
            int index = nextEndlessRandom() % treeSize;
            endlessTargetKey = view.treeNodes[index].value;
        }
    }

    endlessPromptLimit = 4.5f - (endlessSurvivalTime / 25.0f) * 0.5f;
    if (endlessPromptLimit < 1.25f) endlessPromptLimit = 1.25f;
    endlessPromptSeconds = endlessPromptLimit;
    endlessPromptActive = true;
    message = std::string("Incoming: ") +
        (endlessTargetAction == ACTION_INSERT ? "INSERT " : "DELETE ") +
        std::to_string(endlessTargetKey) + ". Choose the matching operation before time runs out.";
}

void GameApp::chooseEndlessOperation(int action) {
    if (!endlessActive || !endlessPromptActive) return;
    if (action != endlessTargetAction) {
        missEndlessPrompt("Wrong operation. One life lost; a new order is coming.");
        return;
    }

    EventLog operationEvents;
    if (!model.performAction(action, endlessTargetKey, 0, operationEvents)) {
        missEndlessPrompt(model.statusText());
        return;
    }
    events.clear();
    for (int i = 0; i < operationEvents.size(); i++) events.add(operationEvents.get(i));
    endlessOperations++;
    endlessPromptActive = false;
    endlessOperationWaiting = true;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    eventTimer = 0.0f;
    animationPlaying = true;
    refreshView();
    message = "Valid " + std::string(action == ACTION_INSERT ? "insert" : "delete") +
        ". Review its events; the next order starts after playback.";
}

void GameApp::missEndlessPrompt(std::string reason) {
    if (!endlessActive) return;
    endlessLives--;
    endlessPromptActive = false;
    if (endlessLives <= 0) {
        message = reason;
        finishEndlessRun();
        return;
    }
    message = reason;
    makeEndlessPrompt();
}

void GameApp::finishEndlessRun() {
    endlessActive = false;
    endlessPromptActive = false;
    endlessOperationWaiting = false;
    endlessScore = (int)endlessSurvivalTime;
    removeEndlessSave();
    endlessSaveAvailable = false;
    recordEndlessScore();
    screenState = 6;
}

void GameApp::loadLeaderboard() {
    endlessLeaderboard.clear();
    FILE* file = std::fopen("endless_leaderboard.dat", "r");
    if (file == nullptr) return;

    int seconds = 0;
    int operations = 0;
    int seed = 0;
    while (std::fscanf(file, "%d %d %d", &seconds, &operations, &seed) == 3) {
        if (seconds < 0 || operations < 0) continue;
        EndlessRecord record{ seconds, operations, seed };
        endlessLeaderboard.push(record);
    }
    std::fclose(file);

    for (int i = 0; i < endlessLeaderboard.size(); i++) {
        for (int j = i + 1; j < endlessLeaderboard.size(); j++) {
            EndlessRecord& left = endlessLeaderboard[i];
            EndlessRecord& right = endlessLeaderboard[j];
            if (right.seconds > left.seconds ||
                (right.seconds == left.seconds && right.operations > left.operations)) {
                EndlessRecord temporary = left;
                left = right;
                right = temporary;
            }
        }
    }
    while (endlessLeaderboard.size() > 10) endlessLeaderboard.pop();
}

void GameApp::recordEndlessScore() {
    EndlessRecord record{ endlessScore, endlessOperations, endlessSeed };
    endlessLeaderboard.push(record);
    for (int i = 0; i < endlessLeaderboard.size(); i++) {
        for (int j = i + 1; j < endlessLeaderboard.size(); j++) {
            EndlessRecord& left = endlessLeaderboard[i];
            EndlessRecord& right = endlessLeaderboard[j];
            if (right.seconds > left.seconds ||
                (right.seconds == left.seconds && right.operations > left.operations)) {
                EndlessRecord temporary = left;
                left = right;
                right = temporary;
            }
        }
    }
    while (endlessLeaderboard.size() > 10) endlessLeaderboard.pop();
    saveLeaderboard();
}

void GameApp::saveLeaderboard() {
    FILE* file = std::fopen("endless_leaderboard.dat", "w");
    if (file == nullptr) return;
    for (int i = 0; i < endlessLeaderboard.size(); i++) {
        EndlessRecord& record = endlessLeaderboard[i];
        std::fprintf(file, "%d %d %d\n", record.seconds, record.operations, record.seed);
    }
    std::fclose(file);
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
    lcaPhase = 0;
    lcaPathCurrentNode = -1;
    lcaPathProgress = 0;
    lcaOptimalSteps = 0;
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
    alarmCapacity = 10;
    message = "Select two rooms, then click their lowest common ancestor.";
}

void GameApp::finishLcaChallenge(bool correct) {
    challengeActive = false;
    challengeFinished = true;
    if (correct && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= lcaOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= lcaOptimalSteps * 5) stars = 2;
        else stars = 1;
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
    copyViewData(view, challengeStartView);
    mirrorStartPositions.clear();
    mirrorEndPositions.clear();
    for (int i = 0; i < positions.size(); i++) {
        mirrorStartPositions.push(positions[i]);
    }
    if (!model.performAction(ACTION_MIRROR, 0, 0, events)) {
        message = model.statusText();
        return;
    }
    refreshView();
    copyViewData(view, challengeFinalView);
    for (int i = 0; i < positions.size(); i++) {
        mirrorEndPositions.push(positions[i]);
    }
    for (int i = 0; i < positions.size(); i++) {
        int oldIndex = findPositionIndexIn(mirrorStartPositions, positions[i].id);
        if (oldIndex >= 0) {
            positions[i].x = mirrorStartPositions[oldIndex].x;
            positions[i].y = mirrorStartPositions[oldIndex].y;
            positions[i].depth = mirrorStartPositions[oldIndex].depth;
        }
    }
    alarmSteps = events.countAlarmSteps();
    wrongClicks = 0;
    stars = 0;
    score = 0;
    mirrorAnimationTime = 0.0f;
    mirrorAnimationActive = true;
    animationPlaying = false;
    currentEvent = -1;
    nextEvent = 0;
    eventTimer = 0.0f;
    highlightedNodeId = -1;
    message = "Mirroring the vault...";
}

void GameApp::finishMirrorChallenge() {
    mirrorAnimationActive = false;
    alarmSteps = events.countAlarmSteps();
    stars = 3;
    score = 1000 - alarmSteps * 10;
    if (score < 0) score = 0;
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    challengeFinished = true;
    message = "Vault mirrored. Compare the new left and right branches.";
    screenState = 3;
}

void GameApp::startRotationChallenge() {
    searchChallenge = false;
    insertChallenge = false;
    deleteChallenge = false;
    rangeChallenge = false;
    balanceChallenge = false;
    heightChallenge = false;
    lcaChallenge = false;
    mirrorAnimationActive = false;
    rotationChallenge = true;
    rotationStage = 0;
    rotationPhase = 0;
    rotationActionIndex = 0;
    rotationHistory.clear();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    eventTimer = 0.0f;
    animationPlaying = false;
    highlightedNodeId = -1;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    rotationOptimalSteps = isLevel(3, 2) ? 4 : 6;
    alarmCapacity = (3 * rotationOptimalSteps + 1) / 2 + 2;
    challengeActive = true;
    challengeFinished = false;
    if (!prepareRotationStage()) {
        rotationChallenge = false;
        challengeActive = false;
        return;
    }
    message = "Click the lowest unbalanced room, then choose the fixing rotation.";
}

bool GameApp::prepareRotationStage() {
    int scenarioId = 31;
    if (isLevel(3, 2)) scenarioId = rotationStage == 0 ? 31 : 32;
    else scenarioId = rotationStage == 0 ? 33 : 34;

    EventLog setupEvents;
    if (!model.loadScenario(scenarioId, setupEvents)) {
        message = model.statusText();
        return false;
    }
    refreshView();

    EventLog caseEvents;
    if (!model.performAction(ACTION_ROTATION_CASE, 0, 0, caseEvents)) {
        message = model.statusText();
        return false;
    }
    rotationPivotId = -1;
    rotationCase = ROT_NONE;
    for (int i = 0; i < caseEvents.size(); i++) {
        Event& event = caseEvents.get(i);
        if (event.type == EV_DONE) {
            rotationPivotId = event.nodeId;
            rotationCase = event.value;
        }
    }
    if (rotationPivotId < 0 || rotationCase == ROT_NONE) {
        message = "This fixture does not contain a repairable rotation case.";
        return false;
    }

    rotationExpectedActions[0] = 0;
    rotationExpectedActions[1] = 0;
    rotationExpectedPivots[0] = -1;
    rotationExpectedPivots[1] = -1;
    rotationActionCount = 1;
    if (rotationCase == ROT_LL) {
        rotationExpectedActions[0] = ACTION_ROTATE_RIGHT;
        rotationExpectedPivots[0] = rotationPivotId;
    }
    else if (rotationCase == ROT_RR) {
        rotationExpectedActions[0] = ACTION_ROTATE_LEFT;
        rotationExpectedPivots[0] = rotationPivotId;
    }
    else if (rotationCase == ROT_LR || rotationCase == ROT_RL) {
        int pivotIndex = findNodeIndex(rotationPivotId);
        if (pivotIndex < 0) return false;
        int childId = rotationCase == ROT_LR
            ? view.treeNodes[pivotIndex].leftId : view.treeNodes[pivotIndex].rightId;
        int childIndex = findNodeIndex(childId);
        if (childIndex < 0) return false;
        rotationActionCount = 2;
        if (rotationCase == ROT_LR) {
            rotationExpectedActions[0] = ACTION_ROTATE_LEFT;
            rotationExpectedPivots[0] = childId;
            rotationExpectedActions[1] = ACTION_ROTATE_RIGHT;
            rotationExpectedPivots[1] = rotationPivotId;
        }
        else {
            rotationExpectedActions[0] = ACTION_ROTATE_RIGHT;
            rotationExpectedPivots[0] = childId;
            rotationExpectedActions[1] = ACTION_ROTATE_LEFT;
            rotationExpectedPivots[1] = rotationPivotId;
        }
    }
    rotationActionIndex = 0;
    rotationPhase = 0;
    challengeActive = true;
    return true;
}

void GameApp::selectRotationAction(int action) {
    if (!rotationChallenge || rotationPhase != 1 ||
        rotationActionIndex < 0 || rotationActionIndex >= rotationActionCount) return;

    if (action != rotationExpectedActions[rotationActionIndex]) {
        alarmSteps += 2;
        wrongClicks++;
        if (alarmSteps >= alarmCapacity) finishRotationChallenge(false);
        else message = "Wrong rotation. +2 alarm steps and a score penalty.";
        return;
    }

    EventLog actionEvents;
    if (!model.performAction(action, rotationExpectedPivots[rotationActionIndex], 0, actionEvents)) {
        message = model.statusText();
        return;
    }
    for (int i = 0; i < actionEvents.size(); i++) {
        rotationHistory.push(actionEvents.get(i));
    }
    events.clear();
    for (int i = 0; i < rotationHistory.size(); i++) events.add(rotationHistory[i]);
    alarmSteps++;
    rotationActionIndex++;
    refreshView();
    currentEvent = -1;
    for (int i = events.size() - 1; i >= 0; i--) {
        if (events.get(i).type == EV_ROTATE) {
            currentEvent = i;
            break;
        }
    }
    if (currentEvent < 0) currentEvent = events.size() - 1;
    nextEvent = currentEvent + 1;
    eventTimer = 0.0f;
    animationPlaying = true;

    if (rotationActionIndex < rotationActionCount) {
        message = "First rotation complete. Choose the second rotation.";
        return;
    }

    if (rotationStage == 0) {
        rotationStage = 1;
        if (!prepareRotationStage()) {
            finishRotationChallenge(false);
            return;
        }
        animationPlaying = false;
        currentEvent = -1;
        message = "First case repaired. Solve the second rotation case.";
        return;
    }
    finishRotationChallenge(true);
}

void GameApp::finishRotationChallenge(bool success) {
    rotationChallenge = false;
    challengeActive = false;
    challengeFinished = true;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= rotationOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= rotationOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "Rotation cases repaired. AVL balance restored.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Rotation level failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::startHeapBuildChallenge() {
    if (!isLevel(4, 1)) return;
    LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
    EventLog setup;
    if (!model.loadScenario(level.scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    refreshView();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    selectedHeapIndex = -1;
    heapBuildSwaps = 0;
    heapBuildOptimalSteps = level.optimalSteps > 0 ? level.optimalSteps : 4;
    alarmCapacity = level.alarmCapacity > 0 ? level.alarmCapacity :
        (3 * heapBuildOptimalSteps + 1) / 2 + 2;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    heapBuildChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    animationPlaying = false;
    message = "Select a parent cell, then its larger child. Build the max heap.";
}

void GameApp::finishHeapBuildChallenge(bool success) {
    heapBuildChallenge = false;
    challengeActive = false;
    challengeFinished = true;
    selectedHeapIndex = -1;
    if (success && !heapCompletionReady) {
        heapCompletionPending = true;
        heapCompletionType = 1;
        animationPlaying = true;
        currentEvent = -1;
        nextEvent = 0;
        eventTimer = 0.0f;
        message = "Heap property restored. Reviewing the swap events...";
        return;
    }
    heapCompletionReady = false;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= heapBuildOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= heapBuildOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "Max heap built. The largest key is at the root.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Heap construction failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::startHeapifyChallenge() {
    if (!isLevel(4, 2)) return;
    LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
    EventLog setup;
    if (!model.loadScenario(level.scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    refreshView();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    heapifyStartIndex = level.targetValue;
    heapifyOperationCount = 0;
    heapifyOptimalSteps = level.optimalSteps > 0 ? level.optimalSteps : 1;
    alarmCapacity = level.alarmCapacity > 0 ? level.alarmCapacity :
        (3 * heapifyOptimalSteps + 1) / 2 + 2;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    selectedHeapIndex = -1;
    heapifyChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    animationPlaying = false;
    message = "Find the violating parent, then click its array cell to sift down.";
}

void GameApp::finishHeapifyChallenge(bool success) {
    heapifyChallenge = false;
    challengeActive = false;
    challengeFinished = true;
    selectedHeapIndex = -1;
    if (success && !heapCompletionReady) {
        heapCompletionPending = true;
        heapCompletionType = 2;
        animationPlaying = true;
        currentEvent = -1;
        nextEvent = 0;
        eventTimer = 0.0f;
        message = "Heap property restored. Reviewing the heapify events...";
        return;
    }
    heapCompletionReady = false;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= heapifyOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= heapifyOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "Security heap restored. The max-heap property holds.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Heap repair failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::startHeapExtractChallenge() {
    if (!isLevel(4, 3)) return;
    LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
    EventLog setup;
    if (!model.loadScenario(level.scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    refreshView();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    heapExtractCount = 0;
    heapExtractChallenge = true;
    heapExtractWaiting = false;
    heapCompletionPending = false;
    heapCompletionReady = false;
    heapCompletionType = 0;
    heapExtractOptimalSteps = level.optimalSteps > 0 ? level.optimalSteps : 7;
    alarmCapacity = level.alarmCapacity > 0 ? level.alarmCapacity :
        (3 * heapExtractOptimalSteps + 1) / 2 + 2;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    challengeActive = true;
    challengeFinished = false;
    animationPlaying = false;
    message = "Extract the root maximum; watch it enter the sorted region.";
}

void GameApp::finishHeapExtractChallenge(bool success) {
    heapExtractChallenge = false;
    heapExtractWaiting = false;
    challengeActive = false;
    challengeFinished = true;
    if (success && !heapCompletionReady) {
        heapCompletionPending = true;
        heapCompletionType = 3;
        animationPlaying = true;
        currentEvent = -1;
        nextEvent = 0;
        eventTimer = 0.0f;
        message = "All maximums extracted. Reviewing the final extraction...";
        return;
    }
    heapCompletionReady = false;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= heapifyOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= heapifyOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "All treasures extracted in descending priority order.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Extraction level failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::startHeapRaceChallenge() {
    bool timedRace = isLevel(4, 4);
    bool finalVault = isLevel(4, 5);
    if (!timedRace && !finalVault) return;
    LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
    EventLog setup;
    if (!model.loadScenario(level.scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    refreshView();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    heapRacePhase = 0;
    heapRaceExtractionCount = 0;
    heapRaceTotalExtractions = view.heapSize > 0 ? view.heapSize - 1 : 0;
    heapRaceOptimalSteps = level.optimalSteps > 0 ? level.optimalSteps :
        heapRaceTotalExtractions + 1;
    heapRaceTimeLimit = timedRace ? 60.0f : 0.0f;
    heapRaceSecondsRemaining = heapRaceTimeLimit;
    heapRaceChallenge = true;
    heapRaceWaiting = false;
    heapCompletionPending = false;
    heapCompletionReady = false;
    heapCompletionType = 0;
    alarmCapacity = level.alarmCapacity > 0 ? level.alarmCapacity : 12;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    challengeActive = true;
    challengeFinished = false;
    animationPlaying = false;
    message = timedRace
        ? "Build the heap, then extract every maximum before the timer expires."
        : "Final vault: build the heap, then sort this new array without a timer.";
}

void GameApp::finishHeapRaceChallenge(bool success) {
    heapRaceChallenge = false;
    heapRaceWaiting = false;
    challengeActive = false;
    challengeFinished = true;
    if (success && !heapCompletionReady) {
        heapCompletionPending = true;
        heapCompletionType = 4;
        animationPlaying = true;
        currentEvent = -1;
        nextEvent = 0;
        eventTimer = 0.0f;
        message = "Heap sorted. Reviewing the final operation events...";
        return;
    }
    heapCompletionReady = false;
    bool withinTime = !isLevel(4, 4) || heapRaceSecondsRemaining > 0.0f;
    if (success && alarmSteps < alarmCapacity && withinTime) {
        if (wrongClicks == 0 && alarmSteps <= heapRaceOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= heapRaceOptimalSteps * 5) stars = 2;
        else stars = 1;
        int timePenalty = isLevel(4, 4)
            ? (int)(heapRaceTimeLimit - heapRaceSecondsRemaining) : 0;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks - timePenalty;
        if (score < 0) score = 0;
        message = isLevel(4, 4)
            ? "Race won. The treasures are sorted in ascending order."
            : "Final vault opened. The new array is sorted in ascending order.";
    }
    else {
        stars = 0;
        score = 0;
        message = "The guards reached the vault. Retry the heap-sort race.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::startReconstructionChallenge() {
    if (!isLevel(1, 2)) return;
    reconstructionRound = 0;
    reconstructionOptimalSteps = levelManager.getLevel(currentLevelIndex).optimalSteps;
    alarmCapacity = levelManager.getLevel(currentLevelIndex).alarmCapacity;
    if (alarmCapacity <= 0) alarmCapacity = 22;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    reconstructionChallenge = true;
    challengeActive = true;
    challengeFinished = false;
    if (!prepareReconstructionRound()) {
        reconstructionChallenge = false;
        challengeActive = false;
    }
}

bool GameApp::prepareReconstructionRound() {
    int scenarioId = reconstructionRound == 0 ? 14 : 15;
    EventLog setup;
    if (!model.loadScenario(scenarioId, setup)) {
        message = model.statusText();
        return false;
    }
    refreshView();

    EventLog firstEvents;
    EventLog secondEvents;
    int firstAction = reconstructionRound == 0
        ? ACTION_TRAVERSE_PREORDER : ACTION_TRAVERSE_POSTORDER;
    if (!model.performAction(firstAction, 0, 0, firstEvents) ||
        !model.performAction(ACTION_TRAVERSE_INORDER, 0, 0, secondEvents)) {
        message = model.statusText();
        return false;
    }
    reconstructionFirstTraversal = reconstructionRound == 0 ? "PREORDER: " : "POSTORDER: ";
    reconstructionSecondTraversal = "INORDER: ";
    for (int i = 0; i < firstEvents.size(); i++) {
        if (firstEvents.get(i).type != EV_VISIT) continue;
        if (reconstructionFirstTraversal.back() != ' ') reconstructionFirstTraversal += "  ";
        reconstructionFirstTraversal += std::to_string(firstEvents.get(i).value);
    }
    for (int i = 0; i < secondEvents.size(); i++) {
        if (secondEvents.get(i).type != EV_VISIT) continue;
        if (reconstructionSecondTraversal.back() != ' ') reconstructionSecondTraversal += "  ";
        reconstructionSecondTraversal += std::to_string(secondEvents.get(i).value);
    }

    reconstructionExpectedSlots.clear();
    reconstructionPlacedSlots.clear();
    reconstructionTokens.clear();
    for (int i = 0; i < 7; i++) {
        reconstructionExpectedSlots.push(-1);
        reconstructionPlacedSlots.push(-1);
    }
    DynArray<int> queuedIds;
    DynArray<int> queuedSlots;
    int rootId = -1;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].parentId < 0) {
            rootId = view.treeNodes[i].id;
            break;
        }
    }
    if (rootId < 0) {
        message = "Blueprint has no root room.";
        return false;
    }
    queuedIds.push(rootId);
    queuedSlots.push(0);
    int head = 0;
    reconstructionNodeCount = 0;
    while (head < queuedIds.size()) {
        int id = queuedIds[head];
        int slot = queuedSlots[head];
        head++;
        int nodeIndex = findNodeIndex(id);
        if (nodeIndex < 0 || slot >= 7) continue;
        reconstructionExpectedSlots[slot] = view.treeNodes[nodeIndex].value;
        reconstructionNodeCount++;
        if (view.treeNodes[nodeIndex].leftId >= 0) {
            queuedIds.push(view.treeNodes[nodeIndex].leftId);
            queuedSlots.push(slot * 2 + 1);
        }
        if (view.treeNodes[nodeIndex].rightId >= 0) {
            queuedIds.push(view.treeNodes[nodeIndex].rightId);
            queuedSlots.push(slot * 2 + 2);
        }
    }
    for (int i = view.treeNodes.size() - 1; i >= 0; i--) {
        reconstructionTokens.push(view.treeNodes[i].value);
    }
    reconstructionDragValue = -1;
    reconstructionDragSourceSlot = -1;
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    alarmCapacity = levelManager.getLevel(currentLevelIndex).alarmCapacity;
    if (alarmCapacity <= 0) alarmCapacity = 22;
    message = reconstructionRound == 0
        ? "Blueprint 1/2: use preorder and inorder to place each room."
        : "Blueprint 2/2: use postorder and inorder to place each room.";
    return true;
}

void GameApp::finishReconstructionChallenge(bool success) {
    reconstructionChallenge = false;
    challengeActive = false;
    challengeFinished = true;
    reconstructionDragValue = -1;
    reconstructionDragSourceSlot = -1;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= reconstructionOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= reconstructionOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "Both traversal blueprints reconstructed correctly.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. Blueprint reconstruction failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

bool GameApp::reconstructionTokenIsPlaced(int value) {
    if (value == reconstructionDragValue) return true;
    for (int i = 0; i < reconstructionPlacedSlots.size(); i++) {
        if (reconstructionPlacedSlots[i] == value) return true;
    }
    return false;
}

void GameApp::startAvlPredictionChallenge() {
    if (!isLevel(3, 4)) return;
    avlPredictionStage = 0;
    avlPredictionCorrectCount = 0;
    avlPredictionExpected[0] = ROT_NONE;
    avlPredictionExpected[1] = ROT_LL;
    avlPredictionExpected[2] = ROT_RR;
    avlPredictionExpected[3] = ROT_LR;
    avlPredictionExpected[4] = ROT_RL;
    avlPredictionInsertValues[0] = 60;
    avlPredictionInsertValues[1] = 20;
    avlPredictionInsertValues[2] = 60;
    avlPredictionInsertValues[3] = 40;
    avlPredictionInsertValues[4] = 60;
    avlPredictionOptimalSteps = 5;
    LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
    alarmCapacity = level.alarmCapacity > 0 ? level.alarmCapacity : 10;
    alarmSteps = 0;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    avlPredictionChallenge = true;
    avlPredictionWaiting = false;
    challengeActive = true;
    challengeFinished = false;
    animationPlaying = false;
    if (!prepareAvlPredictionStage()) finishAvlPredictionChallenge(false);
}

bool GameApp::prepareAvlPredictionStage() {
    if (avlPredictionStage < 0 || avlPredictionStage >= 5) return false;
    int scenarioIds[5] = { 39, 35, 36, 37, 38 };
    EventLog setup;
    if (!model.loadScenario(scenarioIds[avlPredictionStage], setup)) {
        message = model.statusText();
        return false;
    }
    refreshView();
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    animationPlaying = false;
    message = "Predict the AVL repair before inserting the shown key.";
    return true;
}

void GameApp::chooseAvlPrediction(int predictedCase) {
    if (!avlPredictionChallenge || avlPredictionWaiting ||
        avlPredictionStage < 0 || avlPredictionStage >= 5) return;
    if (predictedCase != avlPredictionExpected[avlPredictionStage]) {
        alarmSteps++;
        wrongClicks++;
        message = "Wrong prediction (+1 alarm). The candidate key has not been inserted.";
        if (alarmSteps >= alarmCapacity) finishAvlPredictionChallenge(false);
        return;
    }

    if (!model.performAction(ACTION_INSERT,
        avlPredictionInsertValues[avlPredictionStage], 0, events)) {
        message = model.statusText();
        return;
    }
    int observedCase = ROT_NONE;
    for (int i = 0; i < events.size(); i++) {
        if (events.get(i).type == EV_BALANCE_ALERT) observedCase = events.get(i).code;
    }
    if (observedCase != predictedCase) {
        message = "Model event mismatch: insertion reported a different rotation case.";
        finishAvlPredictionChallenge(false);
        return;
    }
    avlPredictionCorrectCount++;
    alarmSteps++;
    refreshView();
    currentEvent = -1;
    nextEvent = 0;
    eventTimer = 0.0f;
    animationPlaying = true;
    avlPredictionWaiting = true;
    message = "Prediction accepted. Watch the AVL insertion and repair events.";
}

void GameApp::finishAvlPredictionChallenge(bool success) {
    avlPredictionChallenge = false;
    avlPredictionWaiting = false;
    challengeActive = false;
    challengeFinished = true;
    if (success && alarmSteps < alarmCapacity) {
        if (wrongClicks == 0 && alarmSteps <= avlPredictionOptimalSteps) stars = 3;
        else if (wrongClicks <= 1 && alarmSteps * 4 <= avlPredictionOptimalSteps * 5) stars = 2;
        else stars = 1;
        score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
        if (score < 0) score = 0;
        message = "All five AVL insertion predictions were resolved.";
    }
    else {
        stars = 0;
        score = 0;
        message = "Alarm full. The AVL prediction level failed - retry.";
    }
    levelManager.recordResult(currentLevelIndex, stars, score);
    levelManager.saveProgress("campaign_progress.dat");
    screenState = 3;
}

void GameApp::handleReconstructionInput() {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startReconstructionChallenge();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            startReconstructionChallenge();
            return;
        }
        if (reconstructionChallenge && CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
            bool correct = true;
            for (int i = 0; i < reconstructionPlacedSlots.size(); i++) {
                if (reconstructionPlacedSlots[i] != reconstructionExpectedSlots[i]) correct = false;
            }
            if (!correct) {
                alarmSteps++;
                wrongClicks++;
                message = "Blueprint mismatch (+1 alarm). Check the traversal orders and empty slot.";
                if (alarmSteps >= alarmCapacity) finishReconstructionChallenge(false);
                return;
            }
            if (reconstructionRound == 0) {
                reconstructionRound = 1;
                if (!prepareReconstructionRound()) finishReconstructionChallenge(false);
                return;
            }
            finishReconstructionChallenge(true);
            return;
        }
        if (!reconstructionChallenge) return;

        const float slotX[7] = { 620, 440, 800, 350, 530, 710, 890 };
        const float slotY[7] = { 330, 455, 455, 575, 575, 575, 575 };
        for (int slot = 0; slot < 7; slot++) {
            if (reconstructionPlacedSlots[slot] >= 0 &&
                CheckCollisionPointCircle(mouse, Vector2{ slotX[slot], slotY[slot] }, 31.0f)) {
                reconstructionDragValue = reconstructionPlacedSlots[slot];
                reconstructionDragSourceSlot = slot;
                reconstructionPlacedSlots[slot] = -1;
                return;
            }
        }
        for (int i = 0; i < reconstructionTokens.size(); i++) {
            Rectangle tokenBounds{ 290.0f + i * 92.0f, 680.0f, 82.0f, 42.0f };
            if (!reconstructionTokenIsPlaced(reconstructionTokens[i]) &&
                CheckCollisionPointRec(mouse, tokenBounds)) {
                reconstructionDragValue = reconstructionTokens[i];
                reconstructionDragSourceSlot = -1;
                return;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && reconstructionDragValue >= 0) {
        const float slotX[7] = { 620, 440, 800, 350, 530, 710, 890 };
        const float slotY[7] = { 330, 455, 455, 575, 575, 575, 575 };
        int destination = -1;
        for (int slot = 0; slot < 7; slot++) {
            if (CheckCollisionPointCircle(mouse, Vector2{ slotX[slot], slotY[slot] }, 36.0f)) {
                destination = slot;
                break;
            }
        }
        if (destination >= 0) {
            int displaced = reconstructionPlacedSlots[destination];
            reconstructionPlacedSlots[destination] = reconstructionDragValue;
            if (reconstructionDragSourceSlot >= 0 && reconstructionDragSourceSlot != destination) {
                reconstructionPlacedSlots[reconstructionDragSourceSlot] = displaced;
            }
            if (reconstructionDragSourceSlot != destination) alarmSteps++;
            if (alarmSteps >= alarmCapacity) {
                finishReconstructionChallenge(false);
            }
            else message = "Room placed. Submit when every traversal position is reconstructed.";
        }
        else if (reconstructionDragSourceSlot >= 0) {
            reconstructionPlacedSlots[reconstructionDragSourceSlot] = reconstructionDragValue;
        }
        reconstructionDragValue = -1;
        reconstructionDragSourceSlot = -1;
    }
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

    if (orderStatisticChallenge) {
        alarmSteps++;
        if (id == orderStatisticNodeId) {
            int index = findNodeIndex(id);
            int key = index >= 0 ? view.treeNodes[index].value : 0;
            EventLog rankEvents;
            if (!model.performAction(ACTION_RANK, key, 0, rankEvents)) {
                message = model.statusText();
                return;
            }
            Event selected;
            selected.type = EV_VISIT;
            selected.nodeId = id;
            selected.value = key;
            events.add(selected);
            for (int i = 0; i < rankEvents.size(); i++) {
                if (rankEvents.get(i).type == EV_DONE) orderStatisticRank = rankEvents.get(i).value;
                events.add(rankEvents.get(i));
            }
            alarmSteps++;
            challengeFinished = true;
            challengeActive = false;
            stars = alarmSteps <= 4 ? 3 : alarmSteps <= 7 ? 2 : 1;
            score = stars * 100 + (wrongClicks == 0 ? 150 : 0);
            message = "Correct! " + std::to_string(orderStatisticRank) +
                " keys are less than or equal to " + std::to_string(key) + ".";
            levelManager.recordResult(currentLevelIndex, stars, score);
            levelManager.saveProgress("campaign_progress.dat");
            screenState = 3;
        }
        else {
            wrongClicks++;
            if (alarmSteps >= alarmCapacity) {
                challengeFinished = true;
                challengeActive = false;
                stars = 0;
                score = 0;
                message = "Alarm triggered. Reset and use left-subtree sizes to count ranks.";
                screenState = 3;
            }
            else message = "Wrong room. Count left subtree size + 1 before moving right.";
        }
        return;
    }

    if (heightChallenge) {
        handleHeightClick(id);
        return;
    }

    if (rotationChallenge) {
        if (rotationPhase == 0) {
            if (id == rotationPivotId) {
                rotationPhase = 1;
                alarmSteps++;
                highlightedNodeId = id;
                Event selected;
                selected.type = EV_BALANCE_ALERT;
                selected.nodeId = id;
                events.add(selected);
                rotationHistory.push(selected);
                currentEvent = events.size() - 1;
                nextEvent = events.size();
                message = rotationCase == ROT_LL ? "LL case. Choose rotate right." :
                    rotationCase == ROT_RR ? "RR case. Choose rotate left." :
                    rotationCase == ROT_LR ? "LR case. Choose the left rotation first." :
                    "RL case. Choose the right rotation first.";
            }
            else {
                alarmSteps++;
                wrongClicks++;
                if (alarmSteps >= alarmCapacity) finishRotationChallenge(false);
                else message = "Wrong pivot. Select the lowest unbalanced room.";
            }
        }
        else {
            message = "Choose a rotation using the buttons on the left.";
        }
        return;
    }

    if (lcaChallenge) {
        if (lcaPhase == 0) {
            lcaFirstNode = id;
            lcaPhase = 1;
            highlightedNodeId = id;
            alarmSteps++;
            Event selected;
            selected.type = EV_VISIT;
            selected.nodeId = id;
            int index = findNodeIndex(id);
            if (index >= 0) selected.value = view.treeNodes[index].value;
            events.add(selected);
            message = "First room selected. Choose a different second room.";
        }
        else if (lcaPhase == 1) {
            if (id == lcaFirstNode) {
                alarmSteps++;
                wrongClicks++;
                if (alarmSteps >= alarmCapacity) finishLcaChallenge(false);
                else message = "Choose two different rooms.";
            }
            else {
                lcaSecondNode = id;
                lcaPhase = 2;
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
                lcaOptimalSteps = 3 + expectedLcaPathLength;
                alarmCapacity = (3 * lcaOptimalSteps + 1) / 2 + 2;
                message = "Now click the lowest common ancestor.";
            }
        }
        else if (lcaPhase == 2) {
            alarmSteps++;
            if (id == expectedLcaNode) {
                lcaPhase = 3;
                lcaPathCurrentNode = lcaFirstNode;
                lcaPathProgress = 0;
                highlightedNodeId = id;
                Event ancestor;
                ancestor.type = EV_VISIT;
                ancestor.nodeId = id;
                int ancestorIndex = findNodeIndex(id);
                if (ancestorIndex >= 0) ancestor.value = view.treeNodes[ancestorIndex].value;
                events.add(ancestor);
                currentEvent = events.size() - 1;
                nextEvent = events.size();
                message = "Trace the route: click adjacent rooms from the first room to the second.";
            }
            else {
                wrongClicks++;
                highlightedNodeId = id;
                if (alarmSteps >= alarmCapacity) finishLcaChallenge(false);
                else message = "Not the lowest common ancestor. Try again.";
            }
        }
        else {
            alarmSteps++;
            int currentIndex = findNodeIndex(lcaPathCurrentNode);
            bool adjacent = false;
            if (currentIndex >= 0) {
                ViewNode& current = view.treeNodes[currentIndex];
                adjacent = id == current.parentId || id == current.leftId || id == current.rightId;
            }
            if (!adjacent) {
                wrongClicks++;
                if (alarmSteps >= alarmCapacity) finishLcaChallenge(false);
                else message = "That room is not connected to the current room. Stay on the path.";
            }
            else {
                lcaPathCurrentNode = id;
                lcaPathProgress++;
                highlightedNodeId = id;
                Event step;
                step.type = EV_VISIT;
                step.nodeId = id;
                int index = findNodeIndex(id);
                if (index >= 0) step.value = view.treeNodes[index].value;
                events.add(step);
                currentEvent = events.size() - 1;
                nextEvent = events.size();
                if (lcaPathProgress == expectedLcaPathLength) {
                    finishLcaChallenge(id == lcaSecondNode);
                }
                else if (lcaPathProgress > expectedLcaPathLength) {
                    finishLcaChallenge(false);
                }
                else if (alarmSteps >= alarmCapacity) {
                    finishLcaChallenge(false);
                }
                else {
                    message = "Correct step. Continue to the second room.";
                }
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
    if (screenState == 2 && heapCompletionPending && IsKeyPressed(KEY_ESCAPE)) return;
    if (screenState == 2 && heapRaceChallenge && IsKeyPressed(KEY_ESCAPE)) return;
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (mirrorAnimationActive) runAction(0, 0, 0);
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
        else if (screenState == 5 || screenState == 6) {
            endlessActive = false;
            endlessPromptActive = false;
            screenState = 0;
        }
        else if (screenState == 7) {
            screenState = 0;
        }
        return;
    }

    if (screenState == 2 && mirrorAnimationActive) return;

    if (screenState == 4) {
        int codepoint = GetCharPressed();
        while (codepoint > 0) {
            if (sandboxSeedFieldFocused && codepoint >= '0' && codepoint <= '9') {
                if (sandboxSeedInput.size() < 10) sandboxSeedInput += (char)codepoint;
            }
            else if (codepoint >= '0' && codepoint <= '9') {
                if (sandboxSecondFieldFocused) sandboxSecondInput += (char)codepoint;
                else sandboxValueInput += (char)codepoint;
            }
            else if (codepoint == '-' && sandboxSeedFieldFocused && sandboxSeedInput.empty()) {
                sandboxSeedInput += '-';
            }
            else if (codepoint == '-' &&
                (sandboxSecondFieldFocused ? sandboxSecondInput : sandboxValueInput).empty()) {
                if (sandboxSecondFieldFocused) sandboxSecondInput += '-';
                else sandboxValueInput += '-';
            }
            codepoint = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            std::string& field = sandboxSeedFieldFocused ? sandboxSeedInput :
                sandboxSecondFieldFocused ? sandboxSecondInput : sandboxValueInput;
            if (!field.empty()) field.pop_back();
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (sandboxSeedFieldFocused) randomizeSandbox();
            else executeSandboxAction(sandboxAction);
        }
    }

    if (screenState == 5) {
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

    if (screenState == 4) {
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
            undoSandboxAction();
            return;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y)) {
            redoSandboxAction();
            return;
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

    if (screenState == 2 && isLevel(1, 2)) {
        handleReconstructionInput();
        return;
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
            sandboxSeedInput.clear();
            sandboxSecondFieldFocused = false;
            sandboxSeedFieldFocused = false;
            runAction(0, 0, 0);
            selectSandboxStructure(sandboxStructureType);
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 490, 468, 300, 58 })) {
            startEndlessRun();
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 490, 542, 300, 58 })) {
            screenState = 7;
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 490, 616, 300, 58 }) &&
            endlessSaveAvailable) {
            continueEndlessRun();
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
        if (CheckCollisionPointRec(mouse, Rectangle{ 640, 82, 82, 38 })) {
            undoSandboxAction();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 730, 82, 82, 38 })) {
            redoSandboxAction();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 285, 82, 82, 38 })) {
            restartPlayback();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 375, 82, 70, 38 })) {
            stepPlayback(-1);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 453, 82, 82, 38 })) {
            animationPlaying = !animationPlaying;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 543, 82, 70, 38 })) {
            stepPlayback(1);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 725, 180, 44 })) {
            screenState = 0;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 104, 100, 36 })) {
            selectSandboxStructure(0);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 134, 104, 100, 36 })) {
            selectSandboxStructure(1);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 146, 100, 36 })) {
            selectSandboxStructure(2);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 134, 146, 100, 36 })) {
            selectSandboxStructure(3);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 215, 210, 36 })) {
            sandboxSeedFieldFocused = true;
            sandboxSecondFieldFocused = false;
            if (sandboxSeedInput.empty()) sandboxSeedInput = std::to_string(sandboxSeed);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 257, 210, 40 })) {
            randomizeSandbox();
            return;
        }
        if (sandboxOperationCount(0) > 0 &&
            CheckCollisionPointRec(mouse, Rectangle{ 24, 442, 100, 30 })) {
            sandboxOperationPage = 0;
            return;
        }
        if (sandboxOperationCount(1) > 0 &&
            CheckCollisionPointRec(mouse, Rectangle{ 134, 442, 100, 30 })) {
            sandboxOperationPage = 1;
            return;
        }
        if (sandboxOperationCount(2) > 0 &&
            CheckCollisionPointRec(mouse, Rectangle{ 24, 476, 100, 30 })) {
            sandboxOperationPage = 2;
            return;
        }
        if (sandboxOperationCount(3) > 0 &&
            CheckCollisionPointRec(mouse, Rectangle{ 134, 476, 100, 30 })) {
            sandboxOperationPage = 3;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 330, 210, 36 })) {
            sandboxSeedFieldFocused = false;
            sandboxSecondFieldFocused = false;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 398, 210, 36 })) {
            sandboxSeedFieldFocused = false;
            sandboxSecondFieldFocused = true;
            return;
        }
        for (int operation = 0; operation < sandboxOperationCount(sandboxOperationPage); operation++) {
            Rectangle bounds{ 24, (float)(512 + operation * 25), 210, 23 };
            if (CheckCollisionPointRec(mouse, bounds)) {
                int action = sandboxOperationAt(sandboxOperationPage, operation);
                if (action == 0) selectSandboxStructure(sandboxStructureType);
                else executeSandboxAction(action);
                return;
            }
        }
        for (int i = 0; i < positions.size(); i++) {
            Vector2 center = Vector2{ positions[i].x, positions[i].y };
            if (CheckCollisionPointCircle(mouse, center, 31.0f)) {
                highlightedNodeId = positions[i].id;
                break;
            }
        }
        return;
    }

    if (screenState == 5) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 285, 82, 82, 38 })) {
            restartPlayback();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 375, 82, 70, 38 })) {
            stepPlayback(-1);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 453, 82, 82, 38 })) {
            animationPlaying = !animationPlaying;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 543, 82, 70, 38 })) {
            stepPlayback(1);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 24, 440, 210, 42 })) {
            if (saveEndlessRun()) screenState = 0;
            return;
        }
        if (endlessPromptActive && !animationPlaying) {
            if (CheckCollisionPointRec(mouse, Rectangle{ 24, 282, 210, 46 })) {
                chooseEndlessOperation(ACTION_INSERT);
            }
            else if (CheckCollisionPointRec(mouse, Rectangle{ 24, 340, 210, 46 })) {
                chooseEndlessOperation(ACTION_DELETE);
            }
        }
        return;
    }

    if (screenState == 6) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 390, 560, 220, 52 })) {
            startEndlessRun();
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 650, 560, 220, 52 })) {
            screenState = 0;
        }
        else if (CheckCollisionPointRec(mouse, Rectangle{ 500, 630, 260, 48 })) {
            screenState = 7;
        }
        return;
    }

    if (screenState == 7) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 500, 700, 240, 48 })) {
            screenState = 0;
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
    bool isRotationLevel = isLevel(3, 2) || isLevel(3, 3);
    bool isAvlPredictionLevel = isLevel(3, 4);
    bool isOrderStatisticLevel = isLevel(3, 5);
    if (isOrderStatisticLevel) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) startOrderStatisticChallenge();
        else if (CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) runAction(0, 0, 0);
        else if (orderStatisticChallenge) {
            for (int i = 0; i < positions.size(); i++) {
                if (CheckCollisionPointCircle(mouse, Vector2{ positions[i].x, positions[i].y }, 31.0f)) {
                    handleNodeClick(positions[i].id);
                    return;
                }
            }
        }
        return;
    }
    if (isAvlPredictionLevel) {
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startAvlPredictionChallenge();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            runAction(0, 0, 0);
            return;
        }
        if (avlPredictionChallenge && !avlPredictionWaiting) {
            if (CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
                chooseAvlPrediction(ROT_NONE);
                return;
            }
            if (CheckCollisionPointRec(mouse, Rectangle{ 20, 232, 220, 42 })) {
                chooseAvlPrediction(ROT_LL);
                return;
            }
            if (CheckCollisionPointRec(mouse, Rectangle{ 20, 282, 220, 42 })) {
                chooseAvlPrediction(ROT_RR);
                return;
            }
            if (CheckCollisionPointRec(mouse, Rectangle{ 20, 332, 220, 42 })) {
                chooseAvlPrediction(ROT_LR);
                return;
            }
            if (CheckCollisionPointRec(mouse, Rectangle{ 20, 382, 220, 42 })) {
                chooseAvlPrediction(ROT_RL);
                return;
            }
        }
        return;
    }
    bool isHeapBuildLevel = isLevel(4, 1);
    bool isHeapifyLevel = isLevel(4, 2);
    bool isHeapRaceLevel = isLevel(4, 4) || isLevel(4, 5);
    if (isHeapRaceLevel) {
        if (heapCompletionPending || heapRaceWaiting) return;
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startHeapRaceChallenge();
            return;
        }
        if (heapRaceChallenge && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            EventLog operationEvents;
            events.clear();
            int action = heapRacePhase == 0 ? ACTION_HEAP_BUILD : ACTION_EXTRACT_MAX;
            if (!model.performAction(action, 0, 0, operationEvents)) {
                alarmSteps++;
                wrongClicks++;
                message = model.statusText();
                if (alarmSteps >= alarmCapacity) finishHeapRaceChallenge(false);
                return;
            }
            alarmSteps++;
            if (heapRacePhase == 0) {
                heapRacePhase = 1;
                message = "Max heap built. Let the events play, then extract each maximum.";
            }
            else {
                heapRaceExtractionCount++;
                message = "Maximum moved into the sorted region. Continue after playback.";
            }
            for (int i = 0; i < operationEvents.size(); i++) events.add(operationEvents.get(i));
            refreshView();
            currentEvent = -1;
            nextEvent = 0;
            eventTimer = 0.0f;
            animationPlaying = true;
            heapRaceWaiting = true;
            if (heapRacePhase == 1 && view.heapSize <= 1) finishHeapRaceChallenge(true);
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
            runAction(0, 0, 0);
            return;
        }
        return;
    }
    bool isHeapExtractLevel = isLevel(4, 3);
    if (isHeapExtractLevel) {
        if (heapCompletionPending || heapExtractWaiting) return;
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startHeapExtractChallenge();
            return;
        }
        if (heapExtractChallenge && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            EventLog operationEvents;
            events.clear();
            if (!model.performAction(ACTION_EXTRACT_MAX, 0, 0, operationEvents)) {
                alarmSteps++;
                wrongClicks++;
                message = model.statusText();
                if (alarmSteps >= alarmCapacity) finishHeapExtractChallenge(false);
                return;
            }
            heapExtractCount++;
            alarmSteps++;
            for (int i = 0; i < operationEvents.size(); i++) events.add(operationEvents.get(i));
            refreshView();
            currentEvent = -1;
            nextEvent = 0;
            eventTimer = 0.0f;
            animationPlaying = true;
            if (view.heapSize <= 1) finishHeapExtractChallenge(true);
            else {
                heapExtractWaiting = true;
                message = "Maximum extracted. Let the heapify events finish, then continue.";
            }
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
            runAction(0, 0, 0);
            return;
        }
        return;
    }
    if (isHeapifyLevel) {
        if (heapCompletionPending) return;
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startHeapifyChallenge();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            runAction(0, 0, 0);
            return;
        }
        if (heapifyChallenge && mouse.y >= 665 && mouse.y <= 713) {
            int cellWidth = 72;
            int gap = 8;
            int activeSize = view.heapSize;
            if (activeSize < 0) activeSize = 0;
            if (activeSize > view.heapValues.size()) activeSize = view.heapValues.size();
            if (activeSize > 0 && activeSize * (cellWidth + gap) > windowWidth - 320) {
                cellWidth = (windowWidth - 320) / activeSize - gap;
                if (cellWidth < 30) cellWidth = 30;
            }
            int index = ((int)mouse.x - 290) / (cellWidth + gap);
            int cellX = 290 + index * (cellWidth + gap);
            if (index >= 0 && index < activeSize && mouse.x >= cellX && mouse.x <= cellX + cellWidth) {
                if (index != heapifyStartIndex) {
                    alarmSteps++;
                    wrongClicks++;
                    message = "Wrong array cell (+1 alarm). Find a parent smaller than a child.";
                    if (alarmSteps >= alarmCapacity) finishHeapifyChallenge(false);
                    return;
                }
                selectedHeapIndex = index;
                EventLog repairEvents;
                bool repaired = model.performAction(ACTION_HEAPIFY, index, 0, repairEvents);
                if (!repaired) {
                    alarmSteps++;
                    wrongClicks++;
                    message = model.statusText();
                    if (alarmSteps >= alarmCapacity) finishHeapifyChallenge(false);
                    return;
                }
                heapifyOperationCount++;
                alarmSteps++;
                for (int i = 0; i < repairEvents.size(); i++) events.add(repairEvents.get(i));
                refreshView();
                currentEvent = events.size() - 1;
                nextEvent = events.size();
                EventLog checkEvents;
                model.performAction(ACTION_IS_MAX_HEAP, 0, 0, checkEvents);
                bool complete = false;
                for (int i = 0; i < checkEvents.size(); i++) {
                    if (checkEvents.get(i).type == EV_DONE && checkEvents.get(i).value == 1) complete = true;
                }
                if (complete) {
                    for (int i = 0; i < checkEvents.size(); i++) events.add(checkEvents.get(i));
                    finishHeapifyChallenge(true);
                }
                else message = "That subtree is sifted. Find and repair the remaining violation.";
                return;
            }
        }
        return;
    }
    if (isHeapBuildLevel) {
        if (heapCompletionPending) return;
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
            startHeapBuildChallenge();
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
            runAction(0, 0, 0);
            return;
        }
        if (heapBuildChallenge && mouse.y >= 665 && mouse.y <= 713) {
            int cellWidth = 72;
            int gap = 8;
            int activeSize = view.heapSize;
            if (activeSize < 0) activeSize = 0;
            if (activeSize > view.heapValues.size()) activeSize = view.heapValues.size();
            if (activeSize > 0 && activeSize * (cellWidth + gap) > windowWidth - 320) {
                cellWidth = (windowWidth - 320) / activeSize - gap;
                if (cellWidth < 30) cellWidth = 30;
            }
            int index = ((int)mouse.x - 290) / (cellWidth + gap);
            int cellX = 290 + index * (cellWidth + gap);
            if (index >= 0 && index < activeSize && mouse.x >= cellX && mouse.x <= cellX + cellWidth) {
                if (selectedHeapIndex < 0) {
                    selectedHeapIndex = index;
                    message = "Choose a child cell to swap with this parent.";
                }
                else if (selectedHeapIndex == index) {
                    selectedHeapIndex = -1;
                }
                else {
                    int first = selectedHeapIndex;
                    selectedHeapIndex = -1;
                    EventLog swapEvents;
                    bool swapped = model.performAction(ACTION_HEAP_SWAP, first, index, swapEvents);
                    if (!swapped) {
                        alarmSteps++;
                        wrongClicks++;
                        message = "Invalid swap (+1 alarm). Choose a parent and its larger child.";
                        if (alarmSteps >= alarmCapacity) finishHeapBuildChallenge(false);
                        return;
                    }
                    heapBuildSwaps++;
                    alarmSteps++;
                    for (int i = 0; i < swapEvents.size(); i++) events.add(swapEvents.get(i));
                    refreshView();
                    currentEvent = events.size() - 1;
                    nextEvent = events.size();
                    EventLog checkEvents;
                    model.performAction(ACTION_IS_MAX_HEAP, 0, 0, checkEvents);
                    bool complete = false;
                    for (int i = 0; i < checkEvents.size(); i++) {
                        if (checkEvents.get(i).type == EV_DONE && checkEvents.get(i).value == 1) {
                            complete = true;
                        }
                    }
                    if (complete) {
                        for (int j = 0; j < checkEvents.size(); j++) events.add(checkEvents.get(j));
                        finishHeapBuildChallenge(true);
                    }
                    else message = "Valid swap. Continue until every parent dominates its children.";
                }
                return;
            }
        }
        return;
    }
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
    else if (isRotationLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startRotationChallenge();
    }
    else if (isRotationLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        runAction(0, 0, 0);
    }
    else if (isRotationLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
        selectRotationAction(ACTION_ROTATE_LEFT);
    }
    else if (isRotationLevel && CheckCollisionPointRec(mouse, Rectangle{ 20, 232, 220, 42 })) {
        selectRotationAction(ACTION_ROTATE_RIGHT);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 82, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_PREORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 132, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_INORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 182, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_POSTORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 232, 220, 42 })) {
        startTraversalChallenge(ACTION_TRAVERSE_LEVEL_ORDER);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
        CheckCollisionPointRec(mouse, Rectangle{ 20, 282, 220, 42 })) {
        runAction(ACTION_SEARCH, 60, 0);
    }
    else if (!isSearchLevel && !isInsertLevel && !isDeleteLevel && !isRangeLevel && !isBalanceLevel && !isHeightLevel && !isLcaLevel && !isMirrorLevel && !isRotationLevel &&
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

    if (screenState == 5 && endlessActive) {
        endlessSurvivalTime += deltaTime;
        endlessScore = (int)endlessSurvivalTime;
        if (endlessPromptActive) {
            endlessPromptSeconds -= deltaTime;
            if (endlessPromptSeconds <= 0.0f) {
                endlessPromptSeconds = 0.0f;
                missEndlessPrompt("Order timed out. One life lost; a new order is coming.");
                if (screenState == 6) return;
            }
        }
    }

    if (mirrorAnimationActive) {
        mirrorAnimationTime += deltaTime;
        float progress = mirrorAnimationTime / 0.9f;
        if (progress > 1.0f) progress = 1.0f;
        float eased = progress * progress * (3.0f - 2.0f * progress);
        for (int i = 0; i < positions.size(); i++) {
            int startIndex = findPositionIndexIn(mirrorStartPositions, positions[i].id);
            int endIndex = findPositionIndexIn(mirrorEndPositions, positions[i].id);
            if (startIndex < 0 || endIndex < 0) continue;
            GuiNodePosition& start = mirrorStartPositions[startIndex];
            GuiNodePosition& end = mirrorEndPositions[endIndex];
            positions[i].x = start.x + (end.x - start.x) * eased;
            positions[i].y = start.y + (end.y - start.y) * eased;
            positions[i].depth = eased < 0.5f ? start.depth : end.depth;
        }
        if (progress >= 1.0f) {
            for (int i = 0; i < positions.size(); i++) {
                int endIndex = findPositionIndexIn(mirrorEndPositions, positions[i].id);
                if (endIndex >= 0) positions[i] = mirrorEndPositions[endIndex];
            }
            finishMirrorChallenge();
        }
    }

    if (heapRaceChallenge && isLevel(4, 4) && !heapCompletionPending) {
        heapRaceSecondsRemaining -= deltaTime;
        if (heapRaceSecondsRemaining <= 0.0f) {
            heapRaceSecondsRemaining = 0.0f;
            finishHeapRaceChallenge(false);
            return;
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
            if (heapCompletionPending) {
                heapCompletionPending = false;
                heapCompletionReady = true;
                if (heapCompletionType == 1) finishHeapBuildChallenge(true);
                else if (heapCompletionType == 2) finishHeapifyChallenge(true);
                else if (heapCompletionType == 3) finishHeapExtractChallenge(true);
                else finishHeapRaceChallenge(true);
                return;
            }
            if (heapExtractWaiting) heapExtractWaiting = false;
            if (heapRaceWaiting) heapRaceWaiting = false;
            if (avlPredictionWaiting) {
                avlPredictionWaiting = false;
                if (avlPredictionStage >= 4) finishAvlPredictionChallenge(true);
                else {
                    avlPredictionStage++;
                    if (!prepareAvlPredictionStage()) finishAvlPredictionChallenge(false);
                }
                return;
            }
            if (screenState == 5 && endlessOperationWaiting) {
                endlessOperationWaiting = false;
                makeEndlessPrompt();
                return;
            }
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
    if (screenState == 5) {
        drawEndlessMode();
        EndDrawing();
        return;
    }
    if (screenState == 6) {
        drawEndlessResult();
        EndDrawing();
        return;
    }
    if (screenState == 7) {
        drawLeaderboard();
        EndDrawing();
        return;
    }

    DrawText("VAULT HEIST", 24, 22, 30, Color{ 240, 220, 170, 255 });
    DrawText("Tree Lab - GUI mock connected through GameModel", 285, 54, 16,
        Color{ 175, 190, 210, 255 });
    bool reconstructionLevel = isLevel(1, 2);
    if (!reconstructionLevel) drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{ 62, 76, 96, 255 });

    if (reconstructionLevel) {
        drawReconstructionChallenge();
    }
    else if (view.viewType == MODEL_VIEW_HEAP) {
        if (isLevel(4, 2)) drawHeapifyControls();
        else if (isLevel(4, 3)) drawHeapExtractControls();
        else if (isLevel(4, 4)) drawHeapRaceControls();
        else if (isLevel(4, 5)) drawHeapFinalControls();
        else drawHeapControls();
        drawHeapView();
    }
    else {
        if (isLevel(3, 4)) drawAvlPredictionControls();
        else drawControls();
        if (isLevel(3, 4) && avlPredictionChallenge && avlPredictionStage >= 0 && avlPredictionStage < 5) {
            DrawText(TextFormat("PREDICT BEFORE INSERT: %i",
                avlPredictionInsertValues[avlPredictionStage]),
                290, 112, 19, Color{ 240, 220, 170, 255 });
        }
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
    drawButton(Rectangle{ 490, 468, 300, 58 }, "Endless survival", true);
    drawButton(Rectangle{ 490, 542, 300, 58 }, "Leaderboard", true);
    drawButton(Rectangle{ 490, 616, 300, 58 }, "Continue Endless", endlessSaveAvailable);
    DrawText("Choose a mode to enter the vault.", 474, 694, 18,
        Color{ 150, 165, 185, 255 });
    if (levelManager.count() == 0) {
        DrawText("Level data failed to load: data/levels.json", 390, 690, 18,
            Color{ 220, 135, 115, 255 });
    }
}

void GameApp::drawEndlessMode() {
    DrawText("VAULT HEIST - ENDLESS", 24, 22, 30, Color{ 240, 220, 170, 255 });
    DrawText("Keep the BST valid while the operation stream speeds up.",
        285, 54, 16, Color{ 175, 190, 210, 255 });
    drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{ 62, 76, 96, 255 });

    DrawText("SURVIVAL SCORE", 24, 92, 14, Color{ 150, 175, 205, 255 });
    DrawText(TextFormat("%i seconds", endlessScore), 24, 114, 22,
        Color{ 240, 220, 170, 255 });
    DrawText(TextFormat("Lives: %i / 3", endlessLives), 24, 150, 17,
        Color{ 220, 224, 230, 255 });
    DrawText(TextFormat("Valid operations: %i", endlessOperations), 24, 176, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Seed: %i", endlessSeed), 24, 200, 13,
        Color{ 150, 165, 185, 255 });

    if (endlessPromptActive) {
        DrawText("INCOMING OPERATION", 24, 232, 14,
            Color{ 240, 220, 170, 255 });
        std::string prompt = endlessTargetAction == ACTION_INSERT
            ? "Insert " : "Delete ";
        prompt += std::to_string(endlessTargetKey);
        DrawText(prompt.c_str(), 24, 252, 18, Color{ 230, 235, 242, 255 });
        DrawRectangle(24, 267, 210, 8, Color{ 58, 66, 78, 255 });
        int fill = endlessPromptLimit > 0.0f
            ? (int)(210.0f * endlessPromptSeconds / endlessPromptLimit) : 0;
        if (fill < 0) fill = 0;
        if (fill > 210) fill = 210;
        DrawRectangle(24, 267, fill, 8, Color{ 216, 117, 73, 255 });
    }
    else if (endlessOperationWaiting) {
        DrawText("Review the event playback.", 24, 246, 14,
            Color{ 205, 212, 224, 255 });
    }
    drawButton(Rectangle{ 24, 282, 210, 46 }, "Insert requested key",
        endlessPromptActive && !animationPlaying);
    drawButton(Rectangle{ 24, 340, 210, 46 }, "Delete requested key",
        endlessPromptActive && !animationPlaying);
    DrawText("Three missed orders end the run.", 24, 398, 13,
        Color{ 150, 165, 185, 255 });
    drawButton(Rectangle{ 24, 440, 210, 42 }, "Save & main menu", true);
    drawTree();
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
        Color{ 220, 224, 230, 255 });
}

void GameApp::drawEndlessResult() {
    DrawText("ENDLESS RUN OVER", 380, 170, 36, Color{ 240, 220, 170, 255 });
    DrawText("The vault doors closed.", 450, 238, 24,
        Color{ 220, 135, 115, 255 });
    DrawText(TextFormat("Survival time: %i seconds", endlessScore), 420, 315, 22,
        Color{ 240, 220, 170, 255 });
    DrawText(TextFormat("Valid operations: %i", endlessOperations), 420, 355, 20,
        Color{ 220, 224, 230, 255 });
    DrawText(TextFormat("Seed: %i", endlessSeed), 420, 392, 17,
        Color{ 150, 165, 185, 255 });
    drawButton(Rectangle{ 390, 560, 220, 52 }, "Play again", true);
    drawButton(Rectangle{ 650, 560, 220, 52 }, "Main menu", true);
    drawButton(Rectangle{ 500, 630, 260, 48 }, "View leaderboard", true);
}

void GameApp::drawLeaderboard() {
    DrawText("ENDLESS LEADERBOARD", 360, 96, 36, Color{ 240, 220, 170, 255 });
    DrawText("Rank     Time       Operations      Seed", 360, 170, 18,
        Color{ 150, 175, 205, 255 });
    if (endlessLeaderboard.empty()) {
        DrawText("No runs recorded yet. Play Endless survival to set a score.",
            330, 230, 18, Color{ 205, 212, 224, 255 });
    }
    for (int i = 0; i < endlessLeaderboard.size(); i++) {
        EndlessRecord& record = endlessLeaderboard[i];
        DrawText(TextFormat("%2i       %3i sec          %3i           %i",
            i + 1, record.seconds, record.operations, record.seed),
            360, 212 + i * 42, 18,
            i == 0 ? Color{ 240, 220, 170, 255 } : Color{ 220, 224, 230, 255 });
    }
    DrawText("Scores rank by survival time, then valid operations.", 360, 650, 16,
        Color{ 150, 165, 185, 255 });
    drawButton(Rectangle{ 500, 700, 240, 48 }, "Main menu", true);
}

void GameApp::drawSandbox() {
    DrawText("VAULT HEIST - SANDBOX", 24, 22, 30, Color{ 240, 220, 170, 255 });
    DrawText("Choose a structure and build a repeatable random setup", 285, 54, 16,
        Color{ 175, 190, 210, 255 });
    drawPlaybackControls();
    drawButton(Rectangle{ 640, 82, 82, 38 }, "Undo", !sandboxUndoHistory.empty());
    drawButton(Rectangle{ 730, 82, 82, 38 }, "Redo", !sandboxRedoHistory.empty());
    DrawLine(260, 80, 260, windowHeight - 24, Color{ 62, 76, 96, 255 });

    DrawText("STRUCTURE", 24, 82, 14, Color{ 150, 175, 205, 255 });
    const char* structureLabels[4] = { "Binary", "BST", "AVL", "Max Heap" };
    for (int i = 0; i < 4; i++) {
        float x = i % 2 == 0 ? 24.0f : 134.0f;
        float y = i < 2 ? 104.0f : 146.0f;
        Rectangle bounds{ x, y, 100, 36 };
        drawButton(bounds, structureLabels[i], true);
        if (sandboxStructureType == i) {
            DrawRectangleLinesEx(bounds, 3.0f, Color{ 240, 220, 170, 255 });
        }
    }

    DrawText("SEED", 24, 190, 14, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 215, 210, 36, Color{ 25, 34, 48, 255 });
    DrawRectangleLinesEx(Rectangle{ 24, 215, 210, 36 }, 2.0f,
        sandboxSeedFieldFocused ? Color{ 240, 220, 170, 255 } :
        Color{ 62, 76, 96, 255 });
    std::string seedLabel = sandboxSeedInput.empty()
        ? "auto / last " + std::to_string(sandboxSeed) : sandboxSeedInput;
    DrawText(seedLabel.c_str(), 36, 224, 17, Color{ 230, 235, 242, 255 });
    drawButton(Rectangle{ 24, 257, 210, 40 }, "Random setup", true);

    std::string firstInputLabel = "KEY / VALUE";
    if (sandboxAction == ACTION_HEIGHT || sandboxAction == ACTION_ROTATE_LEFT ||
        sandboxAction == ACTION_ROTATE_RIGHT) firstInputLabel = "NODE ID";
    else if (sandboxAction == ACTION_LCA) firstInputLabel = "FIRST NODE ID";
    else if (sandboxAction == ACTION_KTH) firstInputLabel = "K (1-BASED)";
    else if (sandboxAction == ACTION_HEAPIFY) firstInputLabel = "START ARRAY INDEX";
    else if (sandboxAction == ACTION_HEAP_SWAP) firstInputLabel = "PARENT ARRAY INDEX";
    DrawText(firstInputLabel.c_str(), 24, 306, 14, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 330, 210, 36, Color{ 25, 34, 48, 255 });
    DrawRectangleLinesEx(Rectangle{ 24, 330, 210, 36 }, 2.0f,
        !sandboxSeedFieldFocused && !sandboxSecondFieldFocused
            ? Color{ 240, 220, 170, 255 } : Color{ 62, 76, 96, 255 });
    DrawText(sandboxValueInput.c_str(), 36, 339, 17, Color{ 230, 235, 242, 255 });

    std::string secondInputLabel = "SECOND VALUE";
    if (sandboxAction == ACTION_RANGE_QUERY) secondInputLabel = "RANGE HIGH";
    else if (sandboxAction == ACTION_LCA) secondInputLabel = "SECOND NODE ID";
    else if (sandboxAction == ACTION_HEAP_SWAP) secondInputLabel = "CHILD ARRAY INDEX";
    DrawText(secondInputLabel.c_str(), 24, 374, 14, Color{ 150, 175, 205, 255 });
    DrawRectangle(24, 398, 210, 36, Color{ 25, 34, 48, 255 });
    DrawRectangleLinesEx(Rectangle{ 24, 398, 210, 36 }, 2.0f,
        sandboxSecondFieldFocused ? Color{ 240, 220, 170, 255 } :
        Color{ 62, 76, 96, 255 });
    DrawText(sandboxSecondInput.c_str(), 36, 407, 17, Color{ 230, 235, 242, 255 });

    const char* pageLabels[4] = { "Read", "Edit", "AVL", "Heap" };
    for (int page = 0; page < 4; page++) {
        float x = page % 2 == 0 ? 24.0f : 134.0f;
        float y = page < 2 ? 442.0f : 476.0f;
        bool available = sandboxOperationCount(page) > 0;
        Rectangle bounds{ x, y, 100, 30 };
        Color tabFill = available ? Color{ 42, 57, 78, 255 } : Color{ 31, 39, 52, 255 };
        Color tabText = available ? Color{ 230, 235, 242, 255 } : Color{ 125, 133, 144, 255 };
        DrawRectangleRounded(bounds, 0.1f, 4, tabFill);
        DrawRectangleLinesEx(bounds, 1.5f, Color{ 91, 113, 143, 255 });
        DrawText(pageLabels[page], (int)bounds.x + 12, (int)bounds.y + 7, 15, tabText);
        if (sandboxOperationPage == page && available) {
            DrawRectangleLinesEx(bounds, 3.0f, Color{ 240, 220, 170, 255 });
        }
    }
    int operations = sandboxOperationCount(sandboxOperationPage);
    for (int i = 0; i < operations; i++) {
        std::string label = sandboxOperationLabel(
            sandboxOperationAt(sandboxOperationPage, i));
        Rectangle bounds{ 24, (float)(512 + i * 25), 210, 23 };
        DrawRectangleRounded(bounds, 0.1f, 4, Color{ 42, 57, 78, 255 });
        DrawRectangleLinesEx(bounds, 1.5f, Color{ 91, 113, 143, 255 });
        DrawText(label.c_str(), 32, (int)bounds.y + 4, 13,
            Color{ 230, 235, 242, 255 });
    }
    if (operations == 0) {
        DrawText("No operations for this structure.", 24, 520, 13,
            Color{ 150, 165, 185, 255 });
    }
    drawButton(Rectangle{ 24, 725, 180, 44 }, "Main menu", true);
    if (view.viewType == MODEL_VIEW_HEAP) drawHeapView();
    else drawTree();
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
        Color{ 220, 224, 230, 255 });
}

void GameApp::selectSandboxStructure(int structureType) {
    if (structureType < 0 || structureType > 3) return;
    sandboxStructureType = structureType;
    sandboxOperationPage = structureType == 3 ? 3 : 0;
    sandboxAction = structureType == 0 ? ACTION_TRAVERSE_PREORDER :
        structureType == 3 ? ACTION_HEAP_BUILD : ACTION_SEARCH;
    int scenarioId = structureType == 0 ? 12 :
        structureType == 1 ? 2 : structureType == 2 ? 3 : 4;
    EventLog setup;
    if (!model.loadScenario(scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    clearSandboxHistory();
    sandboxValueInput.clear();
    sandboxSecondInput.clear();
    sandboxSeedFieldFocused = false;
    sandboxSecondFieldFocused = false;
    events.clear();
    alarmSteps = 0;
    wrongClicks = 0;
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    selectedHeapIndex = -1;
    animationPlaying = true;
    eventTimer = 0.0f;
    refreshView();
    message = structureType == 0 ? "Binary tree selected" :
        structureType == 1 ? "BST selected" :
        structureType == 2 ? "AVL tree selected" : "Max heap selected";
}

int GameApp::nextSandboxRandom() {
    long long next = (long long)sandboxRandomState * 48271LL % 2147483647LL;
    sandboxRandomState = (int)next;
    return sandboxRandomState;
}

bool GameApp::sandboxContainsKey(int key) {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        if (view.treeNodes[i].value == key) return true;
    }
    return false;
}

void GameApp::randomizeSandbox() {
    int enteredSeed = 0;
    if (sandboxSeedInput.empty()) {
        sandboxSeed = (int)(GetTime() * 1000.0) + 1;
    }
    else {
        try {
            enteredSeed = std::stoi(sandboxSeedInput);
            sandboxSeed = enteredSeed;
        }
        catch (...) {
            message = "Seed must be a whole number.";
            return;
        }
    }
    long long normalizedSeed = (long long)sandboxSeed % 2147483646LL;
    if (normalizedSeed <= 0) normalizedSeed += 2147483646LL;
    sandboxRandomState = (int)normalizedSeed;
    sandboxSeed = (int)normalizedSeed;
    clearSandboxHistory();
    sandboxSeedFieldFocused = false;
    sandboxSecondFieldFocused = false;
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    alarmSteps = 0;
    wrongClicks = 0;
    highlightedNodeId = -1;
    selectedHeapIndex = -1;
    int scenarioId = sandboxStructureType == 0
        ? 12 + nextSandboxRandom() % 2
        : sandboxStructureType == 1 ? 25
        : sandboxStructureType == 2 ? 3 : 4;
    EventLog setup;
    if (!model.loadScenario(scenarioId, setup)) {
        message = model.statusText();
        return;
    }
    refreshView();

    if (sandboxStructureType == 1 || sandboxStructureType == 2) {
        int targetCount = sandboxStructureType == 1 ? 8 : 5;
        int inserted = 0;
        int attempts = 0;
        while (inserted < targetCount && attempts < 200) {
            attempts++;
            int key = nextSandboxRandom() % 200 - 50;
            if (sandboxContainsKey(key)) continue;
            EventLog insertion;
            if (!model.performAction(ACTION_INSERT, key, 0, insertion)) continue;
            for (int eventIndex = 0; eventIndex < insertion.size(); eventIndex++) {
                events.add(insertion.get(eventIndex));
            }
            refreshView();
            inserted++;
        }
        if (inserted != targetCount) {
            message = "Could not generate enough unique keys; choose another seed.";
            return;
        }
    }
    else if (sandboxStructureType == 3) {
        int swaps = 0;
        bool validHeap = false;
        while (swaps < 128 && !validHeap) {
            DynArray<int> availableChildren;
            for (int child = 1; child < view.heapSize; child++) {
                int parent = (child - 1) / 2;
                if (view.heapValues[child] > view.heapValues[parent]) {
                    availableChildren.push(child);
                }
            }
            if (availableChildren.size() == 0) break;
            int child = availableChildren[nextSandboxRandom() % availableChildren.size()];
            EventLog swapEvents;
            int parent = (child - 1) / 2;
            if (!model.performAction(ACTION_HEAP_SWAP, parent, child, swapEvents)) {
                message = model.statusText();
                return;
            }
            for (int eventIndex = 0; eventIndex < swapEvents.size(); eventIndex++) {
                events.add(swapEvents.get(eventIndex));
            }
            refreshView();
            swaps++;
            EventLog check;
            model.performAction(ACTION_IS_MAX_HEAP, 0, 0, check);
            for (int eventIndex = 0; eventIndex < check.size(); eventIndex++) {
                if (check.get(eventIndex).type == EV_DONE && check.get(eventIndex).value == 1) {
                    validHeap = true;
                }
            }
        }
        if (!validHeap) {
            message = "Could not finish the random heap setup.";
            return;
        }
    }

    alarmSteps = events.countAlarmSteps();
    animationPlaying = true;
    eventTimer = 0.0f;
    refreshView();
    message = "Seed " + std::to_string(sandboxSeed) + " generated a " +
        (sandboxStructureType == 0 ? std::string("binary-tree setup") :
         sandboxStructureType == 1 ? std::string("BST setup") :
         sandboxStructureType == 2 ? std::string("AVL setup") :
         std::string("max-heap setup"));
}

void GameApp::executeSandboxAction(int action) {
    sandboxAction = action;
    bool takesValue = action == ACTION_SEARCH || action == ACTION_INSERT ||
        action == ACTION_DELETE || action == ACTION_RANGE_QUERY ||
        action == ACTION_HEIGHT || action == ACTION_LCA ||
        action == ACTION_ROTATE_LEFT || action == ACTION_ROTATE_RIGHT ||
        action == ACTION_KTH || action == ACTION_RANK ||
        action == ACTION_HEAPIFY || action == ACTION_HEAP_SWAP;
    bool takesSecondValue = action == ACTION_RANGE_QUERY ||
        action == ACTION_LCA || action == ACTION_HEAP_SWAP;
    if (!takesValue) {
        if (sandboxActionMutates(action)) pushSandboxUndoSnapshot();
        runAction(action, 0, 0);
        return;
    }

    int value = 0;
    int secondValue = 0;
    try {
        if (sandboxValueInput.empty()) throw 0;
        value = std::stoi(sandboxValueInput);
        if (takesSecondValue) {
            if (sandboxSecondInput.empty()) throw 0;
            secondValue = std::stoi(sandboxSecondInput);
        }
    }
    catch (...) {
        message = takesSecondValue
            ? "Enter both operation values first."
            : "Enter an integer value first.";
        return;
    }
    if (action == ACTION_RANGE_QUERY && value > secondValue) {
        message = "Range low value must be less than or equal to high value.";
        return;
    }
    if (sandboxActionMutates(action)) pushSandboxUndoSnapshot();
    runAction(action, value, secondValue);
}

bool GameApp::sandboxActionMutates(int action) {
    return action == ACTION_INSERT || action == ACTION_DELETE ||
        action == ACTION_MIRROR || action == ACTION_ROTATE_LEFT ||
        action == ACTION_ROTATE_RIGHT || action == ACTION_HEAP_BUILD ||
        action == ACTION_HEAPIFY || action == ACTION_EXTRACT_MAX ||
        action == ACTION_HEAP_SORT || action == ACTION_HEAP_SWAP;
}

void GameApp::pushSandboxUndoSnapshot() {
    if (sandboxUndoHistory.size() >= 20) {
        for (int i = 1; i < sandboxUndoHistory.size(); i++) {
            sandboxUndoHistory[i - 1] = sandboxUndoHistory[i];
        }
        sandboxUndoHistory.pop();
    }
    sandboxUndoHistory.push(model.serialize());
    sandboxRedoHistory.clear();
}

void GameApp::undoSandboxAction() {
    if (screenState != 4 || sandboxUndoHistory.empty()) {
        message = "No sandbox action to undo.";
        return;
    }
    std::string current = model.serialize();
    std::string previous = sandboxUndoHistory.pop();
    if (!model.deserialize(previous)) {
        sandboxUndoHistory.push(previous);
        message = model.statusText();
        return;
    }
    if (sandboxRedoHistory.size() >= 20) {
        for (int i = 1; i < sandboxRedoHistory.size(); i++) {
            sandboxRedoHistory[i - 1] = sandboxRedoHistory[i];
        }
        sandboxRedoHistory.pop();
    }
    sandboxRedoHistory.push(current);
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    selectedHeapIndex = -1;
    animationPlaying = false;
    alarmSteps = 0;
    refreshView();
    message = "Undid the last structure-changing action.";
}

void GameApp::redoSandboxAction() {
    if (screenState != 4 || sandboxRedoHistory.empty()) {
        message = "No sandbox action to redo.";
        return;
    }
    std::string current = model.serialize();
    std::string next = sandboxRedoHistory.pop();
    if (!model.deserialize(next)) {
        sandboxRedoHistory.push(next);
        message = model.statusText();
        return;
    }
    if (sandboxUndoHistory.size() >= 20) {
        for (int i = 1; i < sandboxUndoHistory.size(); i++) {
            sandboxUndoHistory[i - 1] = sandboxUndoHistory[i];
        }
        sandboxUndoHistory.pop();
    }
    sandboxUndoHistory.push(current);
    events.clear();
    currentEvent = -1;
    nextEvent = 0;
    highlightedNodeId = -1;
    selectedHeapIndex = -1;
    animationPlaying = false;
    alarmSteps = 0;
    refreshView();
    message = "Redid the sandbox action.";
}

void GameApp::clearSandboxHistory() {
    sandboxUndoHistory.clear();
    sandboxRedoHistory.clear();
}

int GameApp::sandboxOperationCount(int page) {
    if (page == 0) return sandboxStructureType == 3 ? 0 : 8;
    if (page == 1) {
        if (sandboxStructureType == 0) return 2;
        if (sandboxStructureType == 1 || sandboxStructureType == 2) return 5;
    }
    if (page == 2 && sandboxStructureType == 2) return 6;
    if (page == 3 && sandboxStructureType == 3) return 7;
    return 0;
}

int GameApp::sandboxOperationAt(int page, int index) {
    int queryActions[7] = {
        ACTION_TRAVERSE_PREORDER, ACTION_TRAVERSE_INORDER,
        ACTION_TRAVERSE_POSTORDER, ACTION_TRAVERSE_LEVEL_ORDER,
        ACTION_HEIGHT, ACTION_DIAMETER, ACTION_LCA
    };
    int treeEditActions[4] = {
        ACTION_SEARCH, ACTION_INSERT, ACTION_DELETE, ACTION_RANGE_QUERY
    };
    int avlActions[5] = {
        ACTION_ROTATION_CASE, ACTION_ROTATE_LEFT, ACTION_ROTATE_RIGHT,
        ACTION_KTH, ACTION_RANK
    };
    int heapActions[6] = {
        ACTION_HEAP_BUILD, ACTION_HEAPIFY, ACTION_EXTRACT_MAX,
        ACTION_HEAP_SORT, ACTION_IS_MAX_HEAP, ACTION_HEAP_SWAP
    };
    int operationCount = sandboxOperationCount(page);
    if (index < 0 || index >= operationCount) return 0;
    if (index == operationCount - 1) return 0;
    if (page == 0) return queryActions[index];
    if (page == 1 && sandboxStructureType == 0) return ACTION_MIRROR;
    if (page == 1) return treeEditActions[index];
    if (page == 2) return avlActions[index];
    return heapActions[index];
}

std::string GameApp::sandboxOperationLabel(int action) {
    if (action == 0) return "Reset selected structure";
    if (action == ACTION_TRAVERSE_PREORDER) return "Preorder traversal";
    if (action == ACTION_TRAVERSE_INORDER) return "Inorder traversal";
    if (action == ACTION_TRAVERSE_POSTORDER) return "Postorder traversal";
    if (action == ACTION_TRAVERSE_LEVEL_ORDER) return "Level-order traversal";
    if (action == ACTION_HEIGHT) return "Room height";
    if (action == ACTION_DIAMETER) return "Tree diameter";
    if (action == ACTION_LCA) return "Lowest common ancestor";
    if (action == ACTION_SEARCH) return "Search key";
    if (action == ACTION_INSERT) return "Insert key";
    if (action == ACTION_DELETE) return "Delete key";
    if (action == ACTION_RANGE_QUERY) return "Range query";
    if (action == ACTION_MIRROR) return "Mirror tree";
    if (action == ACTION_ROTATION_CASE) return "Detect rotation case";
    if (action == ACTION_ROTATE_LEFT) return "Rotate left at pivot";
    if (action == ACTION_ROTATE_RIGHT) return "Rotate right at pivot";
    if (action == ACTION_KTH) return "Find kth smallest";
    if (action == ACTION_RANK) return "Rank of key";
    if (action == ACTION_HEAP_BUILD) return "Build max heap";
    if (action == ACTION_HEAPIFY) return "Heapify from index";
    if (action == ACTION_EXTRACT_MAX) return "Extract maximum";
    if (action == ACTION_HEAP_SORT) return "Heap sort";
    if (action == ACTION_IS_MAX_HEAP) return "Check max-heap property";
    if (action == ACTION_HEAP_SWAP) return "Swap parent and child";
    return "Operation";
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
    bool isOrderStatisticLevel = isLevel(3, 5);
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
    bool isRotationSingle = isLevel(3, 2);
    bool isRotationDouble = isLevel(3, 3);
    bool isRotationLevel = isRotationSingle || isRotationDouble;
    if (isOrderStatisticLevel) {
        DrawText("ACT 3.5 - ORDER STATISTICS", 24, 55, 16, Color{ 150, 175, 205, 255 });
        drawButton(Rectangle{ 20, 82, 220, 42 }, "Find 5th smallest", true);
        drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
        DrawText("Click the 5th smallest", 24, 190, 14, Color{ 205, 212, 224, 255 });
        DrawText("Subtree sizes appear", 24, 210, 14, Color{ 205, 212, 224, 255 });
        DrawText("below each room.", 24, 230, 14, Color{ 205, 212, 224, 255 });
    }
    else if (isSearchLevel || isInsertLevel || isDeleteLevel || isRangeLevel || isBalanceLevel || isHeightLevel || isLcaLevel || isMirrorLevel || isRotationLevel) {
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
            isRotationSingle ? "ACT 3.2 - SINGLE ROTATIONS" :
            isRotationDouble ? "ACT 3.3 - DOUBLE ROTATIONS" :
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
        else if (isRotationLevel) {
            drawButton(Rectangle{ 20, 82, 220, 42 }, "Start rotations", true);
            drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
            drawButton(Rectangle{ 20, 182, 220, 42 }, "Rotate left", rotationChallenge && rotationPhase == 1);
            drawButton(Rectangle{ 20, 232, 220, 42 }, "Rotate right", rotationChallenge && rotationPhase == 1);
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
    for (int i = 0; i < ((isSearchLevel || isInsertLevel || isDeleteLevel || isRangeLevel || isBalanceLevel || isHeightLevel || isLcaLevel || isMirrorLevel || isRotationLevel) ? 0 : 6); i++) {
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
    if (rotationChallenge) {
        DrawText(TextFormat("Puzzle: %i / 2", rotationStage + 1), 24, 477, 15,
            Color{ 205, 212, 224, 255 });
    }
    else if (lcaChallenge) {
        DrawText(TextFormat("Path edges: %i / %i", lcaPathProgress, expectedLcaPathLength),
            24, 477, 15, Color{ 205, 212, 224, 255 });
    }
    else {
        DrawText(TextFormat("Correct: %i / %i", expectedIndex, expectedRoute.size()),
            24, 477, 15, Color{ 205, 212, 224, 255 });
    }
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
            DrawText(TextFormat("Trace: %i / %i edges", lcaPathProgress, expectedLcaPathLength),
                24, 572, 14, Color{ 240, 220, 170, 255 });
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
    float mirrorProgress = 1.0f;
    if (mirrorAnimationActive) {
        mirrorProgress = mirrorAnimationTime / 0.9f;
        if (mirrorProgress < 0.0f) mirrorProgress = 0.0f;
        if (mirrorProgress > 1.0f) mirrorProgress = 1.0f;
        mirrorProgress = mirrorProgress * mirrorProgress * (3.0f - 2.0f * mirrorProgress);
        unsigned char oldAlpha = (unsigned char)(255.0f * (1.0f - mirrorProgress));
        for (int i = 0; i < challengeStartView.treeNodes.size(); i++) {
            ViewNode& oldNode = challengeStartView.treeNodes[i];
            int oldParent = findPositionIndexIn(mirrorStartPositions, oldNode.id);
            if (oldParent < 0) continue;
            if (oldNode.leftId >= 0) {
                int oldChild = findPositionIndexIn(mirrorStartPositions, oldNode.leftId);
                if (oldChild >= 0) {
                    DrawLineEx(Vector2{ mirrorStartPositions[oldParent].x, mirrorStartPositions[oldParent].y },
                        Vector2{ mirrorStartPositions[oldChild].x, mirrorStartPositions[oldChild].y },
                        3.0f, Color{ 92, 112, 138, oldAlpha });
                }
            }
            if (oldNode.rightId >= 0) {
                int oldChild = findPositionIndexIn(mirrorStartPositions, oldNode.rightId);
                if (oldChild >= 0) {
                    DrawLineEx(Vector2{ mirrorStartPositions[oldParent].x, mirrorStartPositions[oldParent].y },
                        Vector2{ mirrorStartPositions[oldChild].x, mirrorStartPositions[oldChild].y },
                        3.0f, Color{ 92, 112, 138, oldAlpha });
                }
            }
        }
    }
    unsigned char newAlpha = mirrorAnimationActive
        ? (unsigned char)(255.0f * mirrorProgress) : 255;
    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int parentPosition = findPositionIndex(node.id);
        if (parentPosition < 0) continue;
        if (node.leftId >= 0) {
            int childPosition = findPositionIndex(node.leftId);
            if (childPosition >= 0) {
                Color edge = isInSkippedSubtree(node.leftId)
                    ? Color{ 65, 72, 82, newAlpha } : Color{ 92, 112, 138, newAlpha };
                DrawLineEx(Vector2{ positions[parentPosition].x, positions[parentPosition].y },
                    Vector2{ positions[childPosition].x, positions[childPosition].y },
                    3.0f, edge);
            }
        }
        if (node.rightId >= 0) {
            int childPosition = findPositionIndex(node.rightId);
            if (childPosition >= 0) {
                Color edge = isInSkippedSubtree(node.rightId)
                    ? Color{ 65, 72, 82, newAlpha } : Color{ 92, 112, 138, newAlpha };
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
        if (isLevel(3, 5)) {
            DrawText(TextFormat("S=%i", node.size), (int)positions[p].x - 14,
                (int)positions[p].y + 34, 15, Color{ 240, 220, 170, 255 });
        }
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
            DrawText(TextFormat("ID: %i  Key: %i", node.id, node.value),
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

void GameApp::drawAvlPredictionControls() {
    DrawText("ACT 3.4 - AUTO BALANCE", 24, 55, 15, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start prediction", true);
    drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
    bool enabled = avlPredictionChallenge && !avlPredictionWaiting;
    drawButton(Rectangle{ 20, 182, 220, 42 }, "No rotation", enabled);
    drawButton(Rectangle{ 20, 232, 220, 42 }, "LL rotation", enabled);
    drawButton(Rectangle{ 20, 282, 220, 42 }, "RR rotation", enabled);
    drawButton(Rectangle{ 20, 332, 220, 42 }, "LR rotation", enabled);
    drawButton(Rectangle{ 20, 382, 220, 42 }, "RL rotation", enabled);
    DrawText(TextFormat("Prediction: %i / 5", avlPredictionCorrectCount),
        24, 442, 14, Color{ 205, 212, 224, 255 });
    DrawText("ALARM", 24, 475, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 500, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 500, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, capacity), 24, 521, 14,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong predictions: %i", wrongClicks), 24, 546, 13,
        Color{ 205, 212, 224, 255 });
    if (!avlPredictionChallenge) {
        DrawText("Predict the repair before", 285, 112, 18,
            Color{ 240, 220, 170, 255 });
        DrawText("each new key is inserted.", 285, 138, 18,
            Color{ 205, 212, 224, 255 });
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
        bool selected = i == highlightedNodeId || i == selectedHeapIndex || swapPartner;

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
        bool selected = i == highlightedNodeId || i == selectedHeapIndex ||
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

void GameApp::drawReconstructionChallenge() {
    DrawText("ACT 1.2 - DECODE THE BLUEPRINT", 24, 55, 14,
        Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start blueprint", true);
    drawButton(Rectangle{ 20, 132, 220, 42 }, "Restart level", true);
    drawButton(Rectangle{ 20, 182, 220, 42 }, "Submit blueprint", reconstructionChallenge);
    DrawText(TextFormat("Round: %i / 2", reconstructionRound + 1), 24, 248, 15,
        Color{ 240, 220, 170, 255 });
    DrawText("Drag rooms into slots.", 24, 278, 14, Color{ 205, 212, 224, 255 });
    DrawText("Leave the unused slot empty.", 24, 298, 12,
        Color{ 175, 190, 210, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 22;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, capacity), 24, 451, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong submissions: %i", wrongClicks), 24, 478, 13,
        Color{ 205, 212, 224, 255 });

    DrawText(reconstructionFirstTraversal.c_str(), 290, 112, 17,
        Color{ 240, 220, 170, 255 });
    DrawText(reconstructionSecondTraversal.c_str(), 290, 145, 17,
        Color{ 205, 212, 224, 255 });

    const float slotX[7] = { 620, 440, 800, 350, 530, 710, 890 };
    const float slotY[7] = { 330, 455, 455, 575, 575, 575, 575 };
    for (int i = 1; i < 7; i++) {
        int parent = (i - 1) / 2;
        DrawLineEx(Vector2{ slotX[parent], slotY[parent] },
            Vector2{ slotX[i], slotY[i] }, 3.0f, Color{ 72, 88, 108, 255 });
    }
    for (int i = 0; i < 7; i++) {
        bool filled = reconstructionPlacedSlots.size() == 7 &&
            reconstructionPlacedSlots[i] >= 0;
        DrawCircleV(Vector2{ slotX[i], slotY[i] }, 30.0f,
            filled ? Color{ 47, 76, 112, 255 } : Color{ 25, 34, 48, 255 });
        DrawCircleLines((int)slotX[i], (int)slotY[i], 30.0f,
            Color{ 150, 175, 205, 255 });
        if (filled) {
            std::string value = std::to_string(reconstructionPlacedSlots[i]);
            DrawText(value.c_str(), (int)slotX[i] - 15, (int)slotY[i] - 9, 18,
                Color{ 255, 255, 255, 255 });
        }
        else {
            DrawText("?", (int)slotX[i] - 5, (int)slotY[i] - 10, 20,
                Color{ 110, 125, 145, 255 });
        }
    }

    DrawText("ROOM TOKENS", 290, 640, 15, Color{ 150, 175, 205, 255 });
    for (int i = 0; i < reconstructionTokens.size(); i++) {
        int value = reconstructionTokens[i];
        if (reconstructionTokenIsPlaced(value)) continue;
        Rectangle bounds{ 290.0f + i * 92.0f, 680.0f, 82.0f, 42.0f };
        DrawRectangleRounded(bounds, 0.12f, 5, Color{ 42, 57, 78, 255 });
        DrawRectangleLinesEx(bounds, 2.0f, Color{ 91, 113, 143, 255 });
        std::string label = std::to_string(value);
        DrawText(label.c_str(), (int)bounds.x + 18, (int)bounds.y + 11, 17,
            Color{ 230, 235, 242, 255 });
    }
    if (reconstructionDragValue >= 0 && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        DrawCircleV(mouse, 25.0f, Color{ 205, 154, 64, 255 });
        std::string label = std::to_string(reconstructionDragValue);
        DrawText(label.c_str(), (int)mouse.x - 12, (int)mouse.y - 8, 16,
            Color{ 255, 255, 255, 255 });
    }
}

void GameApp::drawHeapControls() {
    DrawText("ACT 4.1 - MAX HEAP BUILD", 24, 55, 15, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start heap build", true);
    drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
    DrawText("Select parent, then larger child.", 20, 190, 13,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Swaps: %i / %i", heapBuildSwaps, heapBuildOptimalSteps),
        20, 220, 15, Color{ 205, 212, 224, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, capacity), 24, 451, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong swaps: %i", wrongClicks), 24, 478, 15,
        Color{ 205, 212, 224, 255 });
}

void GameApp::drawHeapifyControls() {
    DrawText("ACT 4.2 - REPAIR THE HEAP", 24, 55, 15, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start heap repair", true);
    drawButton(Rectangle{ 20, 132, 220, 42 }, "Reset level", true);
    DrawText("Find a parent smaller than", 20, 190, 13, Color{ 205, 212, 224, 255 });
    DrawText("a child, then click its array", 20, 210, 13, Color{ 205, 212, 224, 255 });
    DrawText("cell to heapify that subtree.", 20, 230, 13, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Heapify operations: %i", heapifyOperationCount),
        20, 260, 14, Color{ 205, 212, 224, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, capacity), 24, 451, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Wrong selections: %i", wrongClicks), 24, 478, 15,
        Color{ 205, 212, 224, 255 });
}

void GameApp::drawHeapExtractControls() {
    DrawText("ACT 4.3 - EXTRACT MAX", 24, 55, 15, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start extraction", true);
    drawButton(Rectangle{ 20, 132, 220, 42 }, "Extract maximum",
        heapExtractChallenge && !heapExtractWaiting);
    drawButton(Rectangle{ 20, 182, 220, 42 }, "Reset level", true);
    DrawText("Each maximum moves to the", 20, 242, 13, Color{ 205, 212, 224, 255 });
    DrawText("sorted region at the right.", 20, 262, 13, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Extracted: %i / %i", heapExtractCount, heapExtractOptimalSteps),
        20, 292, 14, Color{ 205, 212, 224, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i steps", alarmSteps, capacity), 24, 451, 15,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Errors: %i", wrongClicks), 24, 478, 15,
        Color{ 205, 212, 224, 255 });
}

void GameApp::drawHeapRaceControls() {
    DrawText("ACT 4.4 - HEAP-SORT RACE", 24, 55, 14, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start the race", true);
    std::string actionLabel = heapRacePhase == 0 ? "Build max heap" : "Extract maximum";
    drawButton(Rectangle{ 20, 132, 220, 42 }, actionLabel,
        heapRaceChallenge && !heapRaceWaiting);
    drawButton(Rectangle{ 20, 182, 220, 42 }, "Reset level", true);
    DrawText("Build once, then extract", 20, 242, 13, Color{ 205, 212, 224, 255 });
    DrawText("until the array is sorted.", 20, 262, 13, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Extracted: %i / %i", heapRaceExtractionCount,
        heapRaceTotalExtractions),
        20, 292, 14, Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Time: %i sec", (int)heapRaceSecondsRemaining),
        20, 322, 14, Color{ 240, 220, 170, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i operations", alarmSteps, capacity), 24, 451, 14,
        Color{ 205, 212, 224, 255 });
}

void GameApp::drawHeapFinalControls() {
    DrawText("ACT 4.5 - FINAL VAULT", 24, 55, 15, Color{ 150, 175, 205, 255 });
    drawButton(Rectangle{ 20, 82, 220, 42 }, "Start final vault", true);
    std::string actionLabel = heapRacePhase == 0 ? "Build max heap" : "Extract maximum";
    drawButton(Rectangle{ 20, 132, 220, 42 }, actionLabel,
        heapRaceChallenge && !heapRaceWaiting);
    drawButton(Rectangle{ 20, 182, 220, 42 }, "Reset level", true);
    DrawText("New array: includes duplicates", 20, 242, 12,
        Color{ 205, 212, 224, 255 });
    DrawText("and negative values. Sort it.", 20, 261, 12,
        Color{ 205, 212, 224, 255 });
    DrawText(TextFormat("Extracted: %i / %i", heapRaceExtractionCount,
        heapRaceTotalExtractions), 20, 292, 14, Color{ 205, 212, 224, 255 });
    DrawText("ALARM", 24, 405, 15, Color{ 150, 175, 205, 255 });
    int capacity = alarmCapacity > 0 ? alarmCapacity : 10;
    int alarm = alarmSteps > capacity ? capacity : alarmSteps;
    DrawRectangle(24, 430, 210, 14, Color{ 58, 66, 78, 255 });
    DrawRectangle(24, 430, 210 * alarm / capacity, 14,
        Color{ 216, 117, 73, 255 });
    DrawText(TextFormat("%i / %i operations", alarmSteps, capacity), 24, 451, 14,
        Color{ 205, 212, 224, 255 });
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
    if (event.type == EV_ROTATE) {
        std::string direction = event.code == ROT_RR ? "left" : "right";
        return "Rotate " + direction + " at node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_BALANCE_ALERT) {
        std::string caseName = event.code == ROT_LL ? "LL" :
            event.code == ROT_RR ? "RR" :
            event.code == ROT_LR ? "LR" :
            event.code == ROT_RL ? "RL" : "unknown";
        return "AVL " + caseName + " case at node " + std::to_string(event.nodeId);
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
    if (event.type == EV_UPDATE_SIZE) {
        return "Subtree size of node " + std::to_string(event.nodeId) + " = " +
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

int GameApp::findPositionIndexIn(DynArray<GuiNodePosition>& source, int id) {
    for (int i = 0; i < source.size(); i++) {
        if (source[i].id == id) return i;
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
