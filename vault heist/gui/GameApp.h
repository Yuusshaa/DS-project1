#pragma once

#include <string>
#include <raylib.h>
#include "GameModel.h"

struct GuiNodePosition {
    int id;
    float x;
    float y;

    GuiNodePosition() {
        id = -1;
        x = 0.0f;
        y = 0.0f;
    }
};

class GameApp {
private:
    GameModel& model;
    ModelView view;
    EventLog events;
    DynArray<int> expectedRoute;
    DynArray<GuiNodePosition> positions;
    std::string message;
    int windowWidth;
    int windowHeight;
    int currentEvent;
    int highlightedNodeId;
    int expectedIndex;
    int alarmSteps;
    int alarmCapacity;
    int wrongClicks;
    int stars;
    int score;
    bool challengeActive;
    bool challengeFinished;
    float eventTimer;

    void processInput();
    void update(float deltaTime);
    void draw();
    void runAction(int action, int value, int secondValue);
    void startTraversalChallenge(int action);
    void handleNodeClick(int id);
    std::string traversalName(int action);
    void refreshView();
    void rebuildLayout();
    void assignPositions(int nodeId, int depth, int& cursor);
    int findNodeIndex(int id);
    int findPositionIndex(int id);
    void drawTree();
    void drawControls();
    void drawEventLog();
    std::string describeEvent(Event& event);

public:
    GameApp(GameModel& gameModel);
    void run();
};
