#pragma once

#include "GameModel.h"

// Predictable GUI stand-in. The real model will implement the same interface.
class MockGameModel : public GameModel {
private:
    ModelView view;
    std::string message;

    int findNodeIndex(int id);
    void addVisit(int id, EventLog& events);

public:
    MockGameModel();
    bool loadScenario(int scenarioId, EventLog& events);
    bool performAction(int action, int value, int secondValue,
                       EventLog& events);
    void copyView(ModelView& output);
    std::string statusText();
};
