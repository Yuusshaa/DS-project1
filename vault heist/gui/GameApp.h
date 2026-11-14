#pragma once

#include <string>
#include <raylib.h>
#include "GameModel.h"
#include "LevelManager.h"

struct GuiNodePosition {
    int id;
    float x;
    float y;
    int depth;

    GuiNodePosition() {
        id = -1;
        x = 0.0f;
        y = 0.0f;
        depth = 0;
    }
};

class GameApp {
private:
    GameModel& model;
    LevelManager levelManager;
    ModelView view;
    EventLog events;
    DynArray<int> expectedRoute;
    DynArray<int> expectedCompareCodes;
    DynArray<GuiNodePosition> positions;
    std::string message;
    int windowWidth;
    int windowHeight;
    int screenState; // 0=menu, 1=campaign, 2=level, 3=result
    int activeAct;
    int currentLevelIndex;
    int currentEvent;
    int highlightedNodeId;
    int hoveredNodeId;
    int expectedIndex;
    int searchTarget;
    int alarmSteps;
    int alarmCapacity;
    int wrongClicks;
    int stars;
    int score;
    bool challengeActive;
    bool challengeFinished;
    bool searchChallenge;
    float eventTimer;

    void processInput();
    void update(float deltaTime);
    void draw();
    void drawMainMenu();
    void drawCampaignMap();
    void drawResultScreen();
    void drawButton(Rectangle bounds, std::string label, bool enabled);
    void runAction(int action, int value, int secondValue);
    void loadCurrentLevel();
    void startTraversalChallenge(int action);
    void startSearchChallenge(int target);
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
    void drawHeapView();
    std::string describeEvent(Event& event);

public:
    GameApp(GameModel& gameModel);
    void run();
};
