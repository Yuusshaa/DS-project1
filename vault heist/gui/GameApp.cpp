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
    alarmSteps = 0;
    alarmCapacity = 10;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    challengeActive = false;
    challengeFinished = false;
    searchChallenge = false;
    insertChallenge = false;
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
    } else {
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
    expectedRoute.clear();
    events.clear();
    if (action == 0) {
        loadCurrentLevel();
    } else {
        model.performAction(action, value, secondValue, events);
    }
    message = model.statusText();
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
        scenarioId = levelManager.getLevel(currentLevelIndex).scenarioId;
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
    } else {
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
    } else {
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
        } else if (event.type == EV_INSERT) {
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
    } else {
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

void GameApp::handleNodeClick(int id) {
    if (challengeActive == false || challengeFinished == true) {
        return;
    }

    if (searchChallenge || insertChallenge) {
        if (expectedIndex < expectedRoute.size() &&
            id == expectedRoute[expectedIndex]) {
            Event event;
            event.type = EV_COMPARE;
            event.nodeId = id;
            event.value = insertChallenge ? insertTarget : searchTarget;
            event.code = expectedCompareCodes[expectedIndex];
            events.add(event);
            alarmSteps++;
            expectedIndex++;
            currentEvent = events.size() - 1;
            nextEvent = events.size();
            highlightedNodeId = id;
            eventTimer = 0.0f;
            if (expectedIndex == expectedRoute.size()) {
                if (insertChallenge && alarmSteps < alarmCapacity) {
                    events.add(pendingInsertEvent);
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
                bool targetFound = insertChallenge ||
                    expectedCompareCodes[expectedIndex - 1] == CMP_EQUAL;
                if (!targetFound || alarmSteps >= alarmCapacity) stars = 0;
                else if (wrongClicks == 0 && alarmSteps <= optimalSteps) stars = 3;
                else if (wrongClicks <= 1 && alarmSteps * 4 <= optimalSteps * 5) stars = 2;
                else stars = 1;
                score = stars > 0 ? 1000 - 10 * alarmSteps - 50 * wrongClicks : 0;
                if (score < 0) score = 0;
                message = stars > 0 ? (insertChallenge ? "Room inserted. Route complete." :
                                                        "Target found! Search complete.") :
                          targetFound ? "Alarm full. Level failed - retry." :
                                        "Target was not found. Level failed - retry.";
                levelManager.recordResult(currentLevelIndex, stars, score);
                levelManager.saveProgress("campaign_progress.dat");
                if (!insertChallenge) {
                    Event done;
                    done.type = EV_DONE;
                    events.add(done);
                }
                screenState = 3;
            } else {
                int comparison = expectedCompareCodes[expectedIndex - 1];
                if (insertChallenge) {
                    message = comparison == CMP_LESS ? "New key is larger. Continue right." :
                              "New key is smaller. Continue left.";
                } else {
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
        } else {
            message = "Wrong room. +1 alarm step. Follow the BST comparison path.";
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
            } else if (wrongClicks == 0 && alarmSteps <= optimalSteps) {
                stars = 3;
                message = "Level complete! 3 stars";
            } else if (wrongClicks <= 1 &&
                       alarmSteps * 4 <= optimalSteps * 5) {
                stars = 2;
                message = "Level complete! 2 stars";
            } else {
                stars = 1;
                message = "Level complete! 1 star";
            }
            score = 1000 - 10 * alarmSteps - 50 * wrongClicks;
            if (score < 0) score = 0;
            levelManager.recordResult(currentLevelIndex, stars, score);
            levelManager.saveProgress("campaign_progress.dat");
            Event done;
            done.type = EV_DONE;
            events.add(done);
            screenState = 3;
        } else {
            message = "Correct. Keep following the traversal order.";
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
    } else {
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
        } else if (screenState == 1) {
            screenState = 0;
        } else if (screenState == 4) {
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
            } else if (codepoint == '-' &&
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
        if (CheckCollisionPointRec(mouse, Rectangle{490, 320, 300, 58})) {
            screenState = 1;
        } else if (CheckCollisionPointRec(mouse, Rectangle{490, 394, 300, 58})) {
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
            Rectangle actBounds = Rectangle{36.0f + (act - 1) * 310.0f,
                                             122.0f, 278.0f, 72.0f};
            if (CheckCollisionPointRec(mouse, actBounds)) {
                activeAct = act;
                return;
            }
        }
        for (int row = 0; row < 5; row++) {
            Rectangle bounds = Rectangle{80, (float)(232 + row * 70), 350, 56};
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
        if (CheckCollisionPointRec(mouse, Rectangle{24, 725, 180, 44})) {
            screenState = 0;
        }
        return;
    }

    if (screenState == 3) {
        if (CheckCollisionPointRec(mouse, Rectangle{390, 560, 220, 52})) {
            runAction(0, 0, 0);
            screenState = 2;
        } else if (CheckCollisionPointRec(mouse, Rectangle{650, 560, 220, 52})) {
            screenState = 1;
        }
        return;
    }

    if (screenState == 4) {
        if (CheckCollisionPointRec(mouse, Rectangle{24, 725, 180, 44})) {
            screenState = 0;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{24, 118, 210, 40})) {
            sandboxSecondFieldFocused = false;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{24, 194, 210, 40})) {
            sandboxSecondFieldFocused = true;
            return;
        }
        if (CheckCollisionPointRec(mouse, Rectangle{24, 260, 210, 42})) {
            executeSandboxAction(ACTION_SEARCH);
        } else if (CheckCollisionPointRec(mouse, Rectangle{24, 310, 210, 42})) {
            executeSandboxAction(ACTION_INSERT);
        } else if (CheckCollisionPointRec(mouse, Rectangle{24, 360, 210, 42})) {
            executeSandboxAction(ACTION_DELETE);
        } else if (CheckCollisionPointRec(mouse, Rectangle{24, 410, 210, 42})) {
            executeSandboxAction(ACTION_RANGE_QUERY);
        } else if (CheckCollisionPointRec(mouse, Rectangle{24, 460, 210, 42})) {
            runAction(0, 0, 0);
        } else {
            for (int i = 0; i < positions.size(); i++) {
                Vector2 center = Vector2{positions[i].x, positions[i].y};
                if (CheckCollisionPointCircle(mouse, center, 31.0f)) {
                    highlightedNodeId = positions[i].id;
                    break;
                }
            }
        }
        return;
    }

    if (CheckCollisionPointRec(mouse, Rectangle{285, 82, 82, 38})) {
        restartPlayback();
        return;
    } else if (CheckCollisionPointRec(mouse, Rectangle{375, 82, 70, 38})) {
        stepPlayback(-1);
        return;
    } else if (CheckCollisionPointRec(mouse, Rectangle{453, 82, 82, 38})) {
        animationPlaying = !animationPlaying;
        return;
    } else if (CheckCollisionPointRec(mouse, Rectangle{543, 82, 70, 38})) {
        stepPlayback(1);
        return;
    }

    bool isSearchLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 1;
    bool isInsertLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 2;
    if (isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 82, 220, 42})) {
        int target = levelManager.getLevel(currentLevelIndex).targetValue;
        startSearchChallenge(target);
    } else if (isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 132, 220, 42})) {
        runAction(0, 0, 0);
    } else if (isInsertLevel && CheckCollisionPointRec(mouse, Rectangle{20, 82, 220, 42})) {
        int target = levelManager.getLevel(currentLevelIndex).targetValue;
        startInsertChallenge(target);
    } else if (isInsertLevel && CheckCollisionPointRec(mouse, Rectangle{20, 132, 220, 42})) {
        runAction(0, 0, 0);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 82, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_PREORDER);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 132, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_INORDER);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 182, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_POSTORDER);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 232, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_LEVEL_ORDER);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 282, 220, 42})) {
        runAction(ACTION_SEARCH, 60, 0);
    } else if (!isSearchLevel && CheckCollisionPointRec(mouse, Rectangle{20, 332, 220, 42})) {
        runAction(0, 0, 0);
    } else {
        for (int i = 0; i < positions.size(); i++) {
            Vector2 center = Vector2{positions[i].x, positions[i].y};
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
            Vector2 center = Vector2{positions[i].x, positions[i].y};
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
        } else {
            animationPlaying = false;
        }
        eventTimer = 0.0f;
    }
}

void GameApp::draw() {
    BeginDrawing();
    ClearBackground(Color{18, 24, 36, 255});
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

    DrawText("VAULT HEIST", 24, 22, 30, Color{240, 220, 170, 255});
    DrawText("Tree Lab - GUI mock connected through GameModel", 285, 54, 16,
             Color{175, 190, 210, 255});
    drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{62, 76, 96, 255});

    if (view.viewType == MODEL_VIEW_HEAP) {
        drawHeapView();
    } else {
        drawControls();
        drawTree();
    }
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
             Color{220, 224, 230, 255});
    EndDrawing();
}

void GameApp::drawButton(Rectangle bounds, std::string label, bool enabled) {
    Color fill = enabled ? Color{42, 57, 78, 255} : Color{31, 39, 52, 255};
    Color outline = enabled ? Color{91, 113, 143, 255} : Color{62, 70, 82, 255};
    Color text = enabled ? Color{230, 235, 242, 255} : Color{125, 133, 144, 255};
    DrawRectangleRounded(bounds, 0.12f, 6, fill);
    DrawRectangleLinesEx(bounds, 2.0f, outline);
    DrawText(label.c_str(), (int)bounds.x + 18, (int)bounds.y + 17, 20, text);
}

void GameApp::drawMainMenu() {
    DrawText("VAULT HEIST", 380, 170, 58, Color{240, 220, 170, 255});
    DrawText("A data structures heist", 480, 240, 22, Color{175, 190, 210, 255});
    drawButton(Rectangle{490, 320, 300, 58}, "Campaign", true);
    drawButton(Rectangle{490, 394, 300, 58}, "Sandbox", true);
    DrawText("Choose a mode to enter the vault.", 474, 490, 18,
             Color{150, 165, 185, 255});
    if (levelManager.count() == 0) {
        DrawText("Level data failed to load: data/levels.json", 390, 550, 18,
                 Color{220, 135, 115, 255});
    }
}

void GameApp::drawSandbox() {
    DrawText("VAULT HEIST - SANDBOX", 24, 22, 30, Color{240, 220, 170, 255});
    DrawText("Explore model operations on the demo tree", 285, 54, 16,
             Color{175, 190, 210, 255});
    drawPlaybackControls();
    DrawLine(260, 80, 260, windowHeight - 24, Color{62, 76, 96, 255});

    DrawText("VALUE", 24, 94, 14, Color{150, 175, 205, 255});
    DrawRectangle(24, 118, 210, 40, Color{25, 34, 48, 255});
    DrawRectangleLinesEx(Rectangle{24, 118, 210, 40}, 2.0f,
                         sandboxSecondFieldFocused ? Color{62, 76, 96, 255} :
                         Color{240, 220, 170, 255});
    DrawText(sandboxValueInput.c_str(), 36, 128, 18, Color{230, 235, 242, 255});

    DrawText("RANGE HIGH", 24, 170, 14, Color{150, 175, 205, 255});
    DrawRectangle(24, 194, 210, 40, Color{25, 34, 48, 255});
    DrawRectangleLinesEx(Rectangle{24, 194, 210, 40}, 2.0f,
                         sandboxSecondFieldFocused ? Color{240, 220, 170, 255} :
                         Color{62, 76, 96, 255});
    DrawText(sandboxSecondInput.c_str(), 36, 204, 18, Color{230, 235, 242, 255});

    const char* labels[5] = {"Search", "Insert", "Delete", "Range query", "Reset demo"};
    for (int i = 0; i < 5; i++) {
        drawButton(Rectangle{24, (float)(260 + i * 50), 210, 42}, labels[i], true);
    }
    drawButton(Rectangle{24, 725, 180, 44}, "Main menu", true);
    drawTree();
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
             Color{220, 224, 230, 255});
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
    } catch (...) {
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
    DrawText("CAMPAIGN MAP", 36, 28, 32, Color{240, 220, 170, 255});
    DrawText("Complete each wing to unlock the next.", 38, 68, 18,
             Color{175, 190, 210, 255});

    std::string actNames[4] = {
        "ACT 1 - Blueprint Wing", "ACT 2 - Sorted Wing",
        "ACT 3 - Architect's Wing", "ACT 4 - Heap Fortress"
    };
    for (int act = 1; act <= 4; act++) {
        Rectangle bounds = Rectangle{36.0f + (act - 1) * 310.0f,
                                     122.0f, 278.0f, 72.0f};
        drawButton(bounds, actNames[act - 1], true);
    }

    DrawText(TextFormat("ACT %i LEVELS", activeAct), 42, 207, 18,
             Color{150, 175, 205, 255});
    for (int row = 0; row < 5; row++) {
        int index = levelManager.findLevel(activeAct, row + 1);
        if (index < 0) continue;
        LevelDefinition& level = levelManager.getLevel(index);
        bool unlocked = levelManager.isUnlocked(index);
        std::string label = level.code() + "  " + level.title;
        if (levelManager.isCompleted(index)) {
            label += "  *" + std::to_string(levelManager.starsFor(index));
        }
        drawButton(Rectangle{80, (float)(232 + row * 70), 350, 56}, label, unlocked);
    }

    int previewIndex = levelManager.findLevel(activeAct, 1);
    if (previewIndex >= 0) {
        LevelDefinition& level = levelManager.getLevel(previewIndex);
        DrawText(level.objective.c_str(), 490, 258, 18, Color{205, 212, 224, 255});
        DrawText(level.conceptName.c_str(), 490, 300, 17, Color{150, 165, 185, 255});
    }
    drawButton(Rectangle{24, 725, 180, 44}, "Main menu", true);
}

void GameApp::drawResultScreen() {
    std::string resultTitle = "LEVEL RESULT";
    if (currentLevelIndex >= 0 && currentLevelIndex < levelManager.count()) {
        resultTitle = "LEVEL " + levelManager.getLevel(currentLevelIndex).code() + " RESULT";
    }
    DrawText(resultTitle.c_str(), 380, 170, 36, Color{240, 220, 170, 255});
    if (stars > 0) {
        DrawText("Vault route completed", 430, 238, 24, Color{150, 205, 160, 255});
    } else {
        DrawText("The alarm caught you", 440, 238, 24, Color{220, 135, 115, 255});
    }
    DrawText(TextFormat("Stars: %i", stars), 490, 310, 22,
             Color{240, 220, 170, 255});
    DrawText(TextFormat("Score: %i", score), 490, 350, 22,
             Color{220, 224, 230, 255});
    DrawText(TextFormat("Alarm steps: %i / %i", alarmSteps, alarmCapacity),
             490, 390, 20, Color{205, 212, 224, 255});
    DrawText(TextFormat("Wrong clicks: %i", wrongClicks), 490, 425, 20,
             Color{205, 212, 224, 255});
    drawButton(Rectangle{390, 560, 220, 52}, "Retry level", true);
    drawButton(Rectangle{650, 560, 220, 52}, "Campaign map", true);
}

void GameApp::drawControls() {
    bool isSearchLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 1;
    bool isInsertLevel = currentLevelIndex >= 0 && currentLevelIndex < levelManager.count() &&
        levelManager.getLevel(currentLevelIndex).act == 2 &&
        levelManager.getLevel(currentLevelIndex).number == 2;
    if (isSearchLevel || isInsertLevel) {
        LevelDefinition& level = levelManager.getLevel(currentLevelIndex);
        DrawText(isSearchLevel ? "ACT 2.1 - BST SEARCH" : "ACT 2.2 - BST INSERT",
                 24, 55, 16, Color{150, 175, 205, 255});
        std::string startLabel = (isSearchLevel ? "Search " : "Insert ") +
                                 std::to_string(level.targetValue);
        std::string labels[2] = {startLabel, "Reset level"};
        for (int i = 0; i < 2; i++) {
            int y = 82 + i * 50;
            drawButton(Rectangle{20, (float)y, 220, 42}, labels[i], true);
        }
    } else {
        DrawText("ACT 1.1 - TRAVERSAL", 24, 55, 16, Color{150, 175, 205, 255});
    }
    std::string labels[6] = {
        "Quiz: Preorder", "Quiz: Inorder", "Quiz: Postorder", "Quiz: Level order",
        "Search 60", "Reset level"
    };
    for (int i = 0; i < (isSearchLevel ? 0 : 6); i++) {
        int y = 82 + i * 50;
        DrawRectangleRounded(Rectangle{20, (float)y, 220, 42}, 0.15f, 6,
                             Color{42, 57, 78, 255});
        DrawRectangleLinesEx(Rectangle{20, (float)y, 220, 42}, 2.0f,
                             Color{91, 113, 143, 255});
        DrawText(labels[i].c_str(), 34, y + 12, 17, Color{230, 235, 242, 255});
    }

    DrawText("ALARM", 24, 405, 15, Color{150, 175, 205, 255});
    DrawRectangle(24, 430, 210, 14, Color{58, 66, 78, 255});
    int displayCapacity = alarmCapacity;
    if (displayCapacity < 1) displayCapacity = 10;
    int alarm = alarmSteps;
    if (alarm > displayCapacity) alarm = displayCapacity;
    int fillWidth = 210 * alarm / displayCapacity;
    DrawRectangle(24, 430, fillWidth, 14, Color{216, 117, 73, 255});
    DrawText(TextFormat("%i / %i steps", alarmSteps, displayCapacity), 24, 451, 15,
             Color{205, 212, 224, 255});
    DrawText(TextFormat("Correct: %i / %i", expectedIndex, expectedRoute.size()),
             24, 477, 15, Color{205, 212, 224, 255});
    DrawText(TextFormat("Wrong clicks: %i", wrongClicks), 24, 500, 15,
             Color{150, 165, 185, 255});
    DrawText(TextFormat("Stars: %i   Score: %i", stars, score), 24, 525, 15,
             Color{240, 220, 170, 255});
}

void GameApp::drawTree() {
    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int parentPosition = findPositionIndex(node.id);
        if (parentPosition < 0) continue;
        if (node.leftId >= 0) {
            int childPosition = findPositionIndex(node.leftId);
            if (childPosition >= 0) {
                DrawLineEx(Vector2{positions[parentPosition].x, positions[parentPosition].y},
                           Vector2{positions[childPosition].x, positions[childPosition].y},
                           3.0f, Color{92, 112, 138, 255});
            }
        }
        if (node.rightId >= 0) {
            int childPosition = findPositionIndex(node.rightId);
            if (childPosition >= 0) {
                DrawLineEx(Vector2{positions[parentPosition].x, positions[parentPosition].y},
                           Vector2{positions[childPosition].x, positions[childPosition].y},
                           3.0f, Color{92, 112, 138, 255});
            }
        }
    }

    for (int i = 0; i < view.treeNodes.size(); i++) {
        ViewNode& node = view.treeNodes[i];
        int p = findPositionIndex(node.id);
        if (p < 0) continue;
        Color fill = node.id == highlightedNodeId || node.id == hoveredNodeId
            ? Color{205, 154, 64, 255} : Color{47, 76, 112, 255};
        DrawCircleV(Vector2{positions[p].x, positions[p].y}, 29.0f, fill);
        DrawCircleLines((int)positions[p].x, (int)positions[p].y, 29.0f,
                        Color{188, 205, 226, 255});
        std::string label = std::to_string(node.value);
        DrawText(label.c_str(), (int)positions[p].x - 15,
                 (int)positions[p].y - 9, 18, Color{255, 255, 255, 255});
        if (node.id == hoveredNodeId) {
            int leftHeight = 0;
            int rightHeight = 0;
            int leftIndex = findNodeIndex(node.leftId);
            int rightIndex = findNodeIndex(node.rightId);
            if (leftIndex >= 0) leftHeight = view.treeNodes[leftIndex].height;
            if (rightIndex >= 0) rightHeight = view.treeNodes[rightIndex].height;
            int balance = leftHeight - rightHeight;
            DrawRectangle((int)positions[p].x + 35, (int)positions[p].y - 28,
                          176, 80, Color{25, 34, 48, 245});
            DrawRectangleLines((int)positions[p].x + 35,
                               (int)positions[p].y - 28, 176, 80,
                               Color{91, 113, 143, 255});
            DrawText(TextFormat("Key: %i", node.value),
                     (int)positions[p].x + 44, (int)positions[p].y - 15,
                     14, Color{230, 235, 242, 255});
            DrawText(TextFormat("Height: %i  Depth: %i", node.height,
                                positions[p].depth),
                     (int)positions[p].x + 44, (int)positions[p].y + 3,
                     12, Color{175, 190, 210, 255});
            DrawText(TextFormat("Balance: %i  Size: %i", balance, node.size),
                     (int)positions[p].x + 44, (int)positions[p].y + 20,
                     12, Color{175, 190, 210, 255});
        }
    }
}

void GameApp::drawHeapView() {
    DrawText("MAX HEAP", 24, 55, 16, Color{150, 175, 205, 255});
    DrawText(TextFormat("Active heap size: %i / %i", view.heapSize,
                        view.heapValues.size()),
             290, 90, 18, Color{205, 212, 224, 255});

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
            DrawLineEx(Vector2{parentX, parentY}, Vector2{x, y}, 3.0f,
                       Color{92, 112, 138, 255});
        }
        DrawCircleV(Vector2{x, y}, 29.0f,
                    selected ? Color{205, 154, 64, 255} : Color{47, 76, 112, 255});
        DrawCircleLines((int)x, (int)y, 29.0f, Color{188, 205, 226, 255});
        std::string label = std::to_string(view.heapValues[i]);
        DrawText(label.c_str(), (int)x - 15, (int)y - 9, 18,
                 Color{255, 255, 255, 255});
    }

    DrawText("HEAP ARRAY", 290, 635, 16, Color{150, 175, 205, 255});
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
        Color fill = selected ? Color{205, 154, 64, 255} :
            active ? Color{42, 57, 78, 255} : Color{31, 39, 52, 255};
        Color outline = active ? Color{91, 113, 143, 255} : Color{62, 70, 82, 255};
        DrawRectangle(x, 665, cellWidth, 48, fill);
        DrawRectangleLines(x, 665, cellWidth, 48, outline);
        std::string value = std::to_string(view.heapValues[i]);
        DrawText(value.c_str(), x + 8, 671, 18, Color{230, 235, 242, 255});
        DrawText(TextFormat("%i", i), x + 8, 695, 12,
                 Color{150, 165, 185, 255});
    }
}

void GameApp::drawEventLog() {
    int panelX = windowWidth - 300;
    DrawRectangle(panelX, 72, 270, 230, Color{25, 34, 48, 255});
    DrawText("EVENT LOG", panelX + 16, 88, 16, Color{170, 190, 215, 255});
    int first = events.size() - 8;
    if (first < 0) first = 0;
    int row = 0;
    for (int i = first; i < events.size(); i++) {
        Event& event = events.get(i);
        std::string line = describeEvent(event);
        Color color = i == currentEvent ? Color{240, 220, 170, 255} :
            event.type == EV_DONE
            ? Color{150, 195, 150, 255} : Color{215, 221, 232, 255};
        DrawText(line.c_str(), panelX + 16, 118 + row * 22, 14, color);
        row++;
    }
}

void GameApp::drawPlaybackControls() {
    drawButton(Rectangle{285, 82, 82, 38}, "Restart", true);
    drawButton(Rectangle{375, 82, 70, 38}, "Prev", events.size() > 0);
    drawButton(Rectangle{453, 82, 82, 38}, animationPlaying ? "Pause" : "Play", true);
    drawButton(Rectangle{543, 82, 70, 38}, "Next", events.size() > 0);
    std::string progress = "Event ";
    progress += std::to_string(currentEvent >= 0 ? currentEvent + 1 : 0);
    progress += " / ";
    progress += std::to_string(events.size());
    DrawText(progress.c_str(), 625, 94, 14, Color{150, 165, 185, 255});
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
