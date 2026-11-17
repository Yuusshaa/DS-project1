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
    DynArray<int> reconstructionExpectedSlots;
    DynArray<int> reconstructionTokens;
    DynArray<int> reconstructionPlacedSlots;
    DynArray<int> expectedCompareCodes;
    DynArray<int> skippedSubtreeRoots;
    DynArray<int> balanceFound;
    DynArray<int> enteredHeights;   // Act 1.3: answer given per postorder room, -1 = not yet
    DynArray<GuiNodePosition> positions;
    DynArray<GuiNodePosition> mirrorStartPositions;
    DynArray<GuiNodePosition> mirrorEndPositions;
    std::string message;
    std::string sandboxValueInput;
    std::string sandboxSecondInput;
    std::string sandboxSeedInput;
    std::string answerInput;        // Act 1.3: digits typed by the player
    std::string reconstructionFirstTraversal;
    std::string reconstructionSecondTraversal;
    int windowWidth;
    int windowHeight;
    int screenState; // 0=menu, 1=campaign, 2=level, 3=result, 4=sandbox, 5=endless, 6=endless result
    int activeAct;
    int currentLevelIndex;
    int currentEvent;
    int nextEvent;
    int highlightedNodeId;
    int hoveredNodeId;
    int expectedIndex;
    int searchTarget;
    int sandboxAction;
    int sandboxStructureType; // 0=Binary Tree, 1=BST, 2=AVL, 3=Max Heap
    int sandboxOperationPage; // 0=tree queries, 1=tree edits, 2=AVL, 3=heap
    int sandboxSeed;
    int sandboxRandomState;
    int endlessScore;
    int endlessLives;
    int endlessTargetAction;
    int endlessTargetKey;
    int endlessOperations;
    int endlessSeed;
    int endlessRandomState;
    int insertTarget;
    Event pendingInsertEvent;
    DynArray<Event> pendingDeleteEvents;
    DynArray<Event> rotationHistory;
    int deleteTarget;
    int pendingNodeId;              // Act 1.3: room waiting for a typed height
    int heightPhase;                // Act 1.3: 0 idle, 1 entering heights, 2 reporting diameter
    int expectedDiameter;           // Act 1.3: answer supplied by the model
    int lcaFirstNode;
    int lcaSecondNode;
    int expectedLcaNode;
    int expectedLcaPathLength;
    int lcaPhase;
    int lcaPathCurrentNode;
    int lcaPathProgress;
    int lcaOptimalSteps;
    int rotationStage;
    int rotationPhase;
    int rotationCase;
    int rotationPivotId;
    int rotationActionCount;
    int rotationActionIndex;
    int rotationExpectedActions[2];
    int rotationExpectedPivots[2];
    int rotationOptimalSteps;
    int selectedHeapIndex;
    int heapBuildSwaps;
    int heapBuildOptimalSteps;
    int heapifyStartIndex;
    int heapifyOperationCount;
    int heapifyOptimalSteps;
    int heapCompletionType;
    int heapExtractCount;
    int heapExtractOptimalSteps;
    int heapRacePhase;
    int heapRaceExtractionCount;
    int heapRaceTotalExtractions;
    int heapRaceOptimalSteps;
    int reconstructionRound;
    int reconstructionNodeCount;
    int reconstructionDragValue;
    int reconstructionDragSourceSlot;
    int reconstructionOptimalSteps;
    int avlPredictionStage;
    int avlPredictionExpected[5];
    int avlPredictionInsertValues[5];
    int avlPredictionCorrectCount;
    int avlPredictionOptimalSteps;
    int orderStatisticNodeId;
    int orderStatisticK;
    int orderStatisticRank;
    int heightOptimalSteps;         // Act 1.3: rooms + 1 diameter report
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
    bool balanceChallenge;
    bool heightChallenge;
    bool lcaChallenge;
    bool mirrorAnimationActive;
    bool rotationChallenge;
    bool heapBuildChallenge;
    bool heapifyChallenge;
    bool heapCompletionPending;
    bool heapCompletionReady;
    bool heapExtractChallenge;
    bool heapExtractWaiting;
    bool heapRaceChallenge;
    bool heapRaceWaiting;
    bool reconstructionChallenge;
    bool avlPredictionChallenge;
    bool avlPredictionWaiting;
    bool orderStatisticChallenge;
    bool animationPlaying;
    bool sandboxSecondFieldFocused;
    bool sandboxSeedFieldFocused;
    bool endlessActive;
    bool endlessPromptActive;
    bool endlessOperationWaiting;
    float eventTimer;
    float mirrorAnimationTime;
    float heapRaceSecondsRemaining;
    float heapRaceTimeLimit;
    float endlessSurvivalTime;
    float endlessPromptSeconds;
    float endlessPromptLimit;

    void processInput();
    void update(float deltaTime);
    void draw();
    void drawMainMenu();
    void drawEndlessMode();
    void drawEndlessResult();
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
    void startBalanceChallenge();
    void startHeightChallenge();
    void handleHeightClick(int id);
    void submitHeightAnswer();
    void heightPenalty(std::string text);
    void finishHeightLevel(bool answeredAll);
    void startLcaChallenge();
    void finishLcaChallenge(bool correct);
    void startMirrorChallenge();
    void finishMirrorChallenge();
    void startRotationChallenge();
    bool prepareRotationStage();
    void selectRotationAction(int action);
    void finishRotationChallenge(bool success);
    void startHeapBuildChallenge();
    void finishHeapBuildChallenge(bool success);
    void startHeapifyChallenge();
    void finishHeapifyChallenge(bool success);
    void startHeapExtractChallenge();
    void finishHeapExtractChallenge(bool success);
    void startHeapRaceChallenge();
    void finishHeapRaceChallenge(bool success);
    void startReconstructionChallenge();
    bool prepareReconstructionRound();
    void finishReconstructionChallenge(bool success);
    void handleReconstructionInput();
    bool reconstructionTokenIsPlaced(int value);
    void startAvlPredictionChallenge();
    bool prepareAvlPredictionStage();
    void chooseAvlPrediction(int predictedCase);
    void finishAvlPredictionChallenge(bool success);
    void startOrderStatisticChallenge();
    void startEndlessRun();
    void makeEndlessPrompt();
    void chooseEndlessOperation(int action);
    void missEndlessPrompt(std::string reason);
    void finishEndlessRun();
    int nextEndlessRandom();
    int enteredHeightFor(int id);
    bool isLevel(int act, int number);
    void drawHeightPanel();
    void selectDeleteCase(int selectedCase);
    void handleNodeClick(int id);
    std::string traversalName(int action);
    void refreshView();
    void copyViewData(ModelView& source, ModelView& destination);
    void rebuildLayout();
    void assignPositions(int nodeId, int depth, int& cursor);
    int findNodeIndex(int id);
    int findPositionIndex(int id);
    int findPositionIndexIn(DynArray<GuiNodePosition>& source, int id);
    bool isInSkippedSubtree(int id);
    void drawTree();
    void drawControls();
    void drawEventLog();
    void drawPlaybackControls();
    void drawSandbox();
    void selectSandboxStructure(int structureType);
    void randomizeSandbox();
    int nextSandboxRandom();
    bool sandboxContainsKey(int key);
    int sandboxOperationCount(int page);
    int sandboxOperationAt(int page, int index);
    std::string sandboxOperationLabel(int action);
    void drawHeapView();
    void drawHeapControls();
    void drawHeapifyControls();
    void drawHeapExtractControls();
    void drawHeapRaceControls();
    void drawHeapFinalControls();
    void drawReconstructionChallenge();
    void drawAvlPredictionControls();
    void executeSandboxAction(int action);
    void restartPlayback();
    void stepPlayback(int direction);
    std::string describeEvent(Event& event);

public:
    GameApp(GameModel& gameModel);
    void run();
};
