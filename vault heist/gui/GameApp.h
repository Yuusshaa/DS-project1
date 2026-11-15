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
    ModelView challengeStartView;
    ModelView challengeFinalView;
    EventLog events;
    DynArray<int> expectedRoute;
    DynArray<int> expectedCompareCodes;
    DynArray<int> skippedSubtreeRoots;
    DynArray<GuiNodePosition> positions;
    std::string message;
    std::string sandboxValueInput;
    std::string sandboxSecondInput;
    int windowWidth;
    int windowHeight;
    int screenState; // 0=menu, 1=campaign, 2=level, 3=result
    int activeAct;
    int currentLevelIndex;
    int currentEvent;
    int nextEvent;
    int highlightedNodeId;
    int hoveredNodeId;
    int expectedIndex;
    int searchTarget;
    int sandboxAction;
    int insertTarget;
    Event pendingInsertEvent;
    DynArray<Event> pendingDeleteEvents;
    int deleteTarget;
    int expectedDeleteCase;
    int rangeLow;
    int rangeHigh;
    int alarmSteps;
    int alarmCapacity;
    int wrongClicks;
    int stars;
    int score;
    bool challengeActive;
    bool challengeFinished;
    bool searchChallenge;
    bool insertChallenge;
    bool deleteChallenge;
    bool deleteCasePending;
    bool rangeChallenge;
    bool animationPlaying;
    bool sandboxSecondFieldFocused;
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
    void startInsertChallenge(int target);
    void startDeleteChallenge(int target);
    void startRangeChallenge(int low, int high);
    void selectDeleteCase(int selectedCase);
    void handleNodeClick(int id);
    std::string traversalName(int action);
    void refreshView();
    void copyViewData(ModelView& source, ModelView& destination);
    void rebuildLayout();
    void assignPositions(int nodeId, int depth, int& cursor);
    int findNodeIndex(int id);
    int findPositionIndex(int id);
    bool isInSkippedSubtree(int id);
    void drawTree();
    void drawControls();
    void drawEventLog();
    void drawPlaybackControls();
    void drawSandbox();
    void drawHeapView();
    void executeSandboxAction(int action);
    void restartPlayback();
    void stepPlayback(int direction);
    std::string describeEvent(Event& event);

public:
    GameApp(GameModel& gameModel);
    void run();
};
