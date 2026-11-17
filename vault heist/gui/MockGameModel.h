#pragma once

#include "GameModel.h"

// Predictable GUI stand-in. The real model will implement the same interface.
class MockGameModel : public GameModel {
private:
    ModelView view;
    std::string message;
    bool avlMode;

    int findNodeIndex(int id);
    int findRootId();
    int calculateHeight(int id);
    int calculateSize(int id);
    void refreshMetadata();
    int diameterWalk(int id, int& best);
    void mirrorWalk(int id);
    void traverseWalk(int id, int mode, EventLog& events);
    std::string visitSummary(std::string name, EventLog& events);
    void removeNodeRecord(int id);
    void addVisit(int id, EventLog& events);
    void collectRange(int id, int low, int high, EventLog& events);
    bool performAvlInsert(int value, EventLog& events);

public:
    MockGameModel();
    bool loadScenario(int scenarioId, EventLog& events);
    bool performAction(int action, int value, int secondValue,
        EventLog& events);
    void copyView(ModelView& output);
    std::string statusText();
    std::string serialize() override;
    bool deserialize(const std::string& data) override;
};
