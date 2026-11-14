#pragma once

#include <string>
#include "dynamicarray.h"

struct LevelDefinition {
    int act;
    int number;
    int scenarioId;
    int targetValue;
    int optimalSteps;
    int alarmCapacity;
    bool implemented;
    std::string title;
    std::string conceptName;
    std::string objective;

    LevelDefinition() {
        act = 0;
        number = 0;
        scenarioId = 1;
        targetValue = 0;
        optimalSteps = 0;
        alarmCapacity = 0;
        implemented = false;
    }

    std::string code();
};

class LevelManager {
private:
    DynArray<LevelDefinition> levels;
    DynArray<int> earnedStars;
    DynArray<int> bestScores;

public:
    bool loadFromJson(std::string filePath);
    int count();
    LevelDefinition& getLevel(int index);
    int findLevel(int act, int number);
    bool isUnlocked(int index);
    bool isCompleted(int index);
    int starsFor(int index);
    int bestScoreFor(int index);
    void recordResult(int index, int stars, int score);
};
