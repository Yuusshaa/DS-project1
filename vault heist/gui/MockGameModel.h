#pragma once

#include "GameModel.h"

// Predictable GUI stand-in. The real model will implement the same interface.
class MockGameModel : public GameModel {
private:
    ModelView view;
    std::string message;

    int findNodeIndex(int id);
    int findRootId();
    int calculateHeight(int id);
    int calculateSize(int id);
    void refreshMetadata();
    void removeNodeRecord(int id);
    void addVisit(int id, EventLog& events);
    void collectRange(int id, int low, int high, EventLog& events);

public:
    MockGameModel();
    bool loadScenario(int scenarioId, EventLog& events);
    bool performAction(int action, int value, int secondValue,
                       EventLog& events);
    void copyView(ModelView& output);
    std::string statusText();
};
