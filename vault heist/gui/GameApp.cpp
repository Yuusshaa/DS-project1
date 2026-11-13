#include "GameApp.h"

#include <cstdio>

GameApp::GameApp(GameModel& gameModel) : model(gameModel) {
    windowWidth = 1280;
    windowHeight = 800;
    currentEvent = -1;
    highlightedNodeId = -1;
    expectedIndex = 0;
    alarmSteps = 0;
    alarmCapacity = 10;
    wrongClicks = 0;
    stars = 0;
    score = 0;
    challengeActive = false;
    challengeFinished = false;
    eventTimer = 0.0f;
    message = "Starting demo";
}

void GameApp::run() {
    InitWindow(windowWidth, windowHeight, "Vault Heist - Tree Lab");
    SetTargetFPS(60);
    runAction(0, 0, 0);

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
    expectedRoute.clear();
    events.clear();
    if (action == 0) {
        model.loadScenario(1, events);
    } else {
        model.performAction(action, value, secondValue, events);
    }
    message = model.statusText();
    alarmSteps = events.countAlarmSteps();
    wrongClicks = 0;
    stars = 0;
    score = 0;
    currentEvent = events.size() > 0 ? 0 : -1;
    highlightedNodeId = -1;
    eventTimer = 0.0f;
    refreshView();
}

std::string GameApp::traversalName(int action) {
    if (action == ACTION_TRAVERSE_PREORDER) return "preorder";
    if (action == ACTION_TRAVERSE_INORDER) return "inorder";
    if (action == ACTION_TRAVERSE_POSTORDER) return "postorder";
    return "level order";
}

void GameApp::startTraversalChallenge(int action) {
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
    alarmCapacity = (3 * expectedRoute.size() + 1) / 2 + 2;
    challengeActive = expectedRoute.size() > 0;
    challengeFinished = false;
    currentEvent = -1;
    highlightedNodeId = -1;
    message = "Act 1.1: click rooms in " + traversalName(action) + " order";
}

void GameApp::handleNodeClick(int id) {
    if (challengeActive == false || challengeFinished == true) {
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
        highlightedNodeId = id;
        eventTimer = 0.0f;

        if (expectedIndex == expectedRoute.size()) {
            challengeActive = false;
            challengeFinished = true;
            if (alarmSteps >= alarmCapacity) {
                stars = 0;
                message = "Alarm full. Level failed - reset and retry.";
            } else if (wrongClicks == 0 && alarmSteps <= expectedRoute.size()) {
                stars = 3;
                message = "Level complete! 3 stars";
            } else if (wrongClicks <= 1 &&
                       alarmSteps * 4 <= expectedRoute.size() * 5) {
                stars = 2;
                message = "Level complete! 2 stars";
            } else {
                stars = 1;
                message = "Level complete! 1 star";
            }
            score = 1000 - 10 * alarmSteps - wrongClicks;
            if (score < 0) score = 0;
            Event done;
            done.type = EV_DONE;
            events.add(done);
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
    } else {
        message = "Wrong room. +1 alarm step. Try again.";
    }
}

void GameApp::refreshView() {
    model.copyView(view);
    rebuildLayout();
}

void GameApp::processInput() {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, Rectangle{20, 82, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_PREORDER);
    } else if (CheckCollisionPointRec(mouse, Rectangle{20, 132, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_INORDER);
    } else if (CheckCollisionPointRec(mouse, Rectangle{20, 182, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_POSTORDER);
    } else if (CheckCollisionPointRec(mouse, Rectangle{20, 232, 220, 42})) {
        startTraversalChallenge(ACTION_TRAVERSE_LEVEL_ORDER);
    } else if (CheckCollisionPointRec(mouse, Rectangle{20, 282, 220, 42})) {
        runAction(ACTION_SEARCH, 60, 0);
    } else if (CheckCollisionPointRec(mouse, Rectangle{20, 332, 220, 42})) {
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
    if (currentEvent < 0 || currentEvent >= events.size()) {
        return;
    }
    eventTimer += deltaTime;
    if (eventTimer >= 0.55f) {
        Event& event = events.get(currentEvent);
        highlightedNodeId = event.nodeId;
        currentEvent++;
        if (currentEvent >= events.size()) {
            currentEvent = -1;
        }
        eventTimer = 0.0f;
    }
}

void GameApp::draw() {
    BeginDrawing();
    ClearBackground(Color{18, 24, 36, 255});
    DrawText("VAULT HEIST", 24, 22, 30, Color{240, 220, 170, 255});
    DrawText("Tree Lab - GUI mock connected through GameModel", 285, 54, 16,
             Color{175, 190, 210, 255});
    DrawLine(260, 80, 260, windowHeight - 24, Color{62, 76, 96, 255});

    drawControls();
    drawTree();
    drawEventLog();
    DrawText(message.c_str(), 285, windowHeight - 42, 18,
             Color{220, 224, 230, 255});
    EndDrawing();
}

void GameApp::drawControls() {
    DrawText("ACT 1.1 - TRAVERSAL", 24, 55, 16, Color{150, 175, 205, 255});
    std::string labels[6] = {
        "Quiz: Preorder", "Quiz: Inorder", "Quiz: Postorder", "Quiz: Level order",
        "Search 60", "Reset level"
    };
    for (int i = 0; i < 6; i++) {
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
        Color fill = node.id == highlightedNodeId
            ? Color{205, 154, 64, 255} : Color{47, 76, 112, 255};
        DrawCircleV(Vector2{positions[p].x, positions[p].y}, 29.0f, fill);
        DrawCircleLines((int)positions[p].x, (int)positions[p].y, 29.0f,
                        Color{188, 205, 226, 255});
        std::string label = std::to_string(node.value);
        DrawText(label.c_str(), (int)positions[p].x - 15,
                 (int)positions[p].y - 9, 18, Color{255, 255, 255, 255});
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
        Color color = event.type == EV_DONE
            ? Color{150, 195, 150, 255} : Color{215, 221, 232, 255};
        DrawText(line.c_str(), panelX + 16, 118 + row * 22, 14, color);
        row++;
    }
}

std::string GameApp::describeEvent(Event& event) {
    if (event.type == EV_VISIT) {
        return "Visit node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_COMPARE) {
        return "Compare at node " + std::to_string(event.nodeId);
    }
    if (event.type == EV_DONE) {
        return "Action complete";
    }
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
