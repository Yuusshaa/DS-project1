#pragma once

#include "raylib.h"

// Integer constants keep this module compatible with the project's no-enums rule.
#define VHG_NODE_NORMAL 0
#define VHG_NODE_CURRENT 1
#define VHG_NODE_TARGET 2
#define VHG_NODE_SKIPPED 3

#define VHG_REACTION_IDLE 0
#define VHG_REACTION_WRONG 1
#define VHG_REACTION_NICE 2
#define VHG_REACTION_COMPLETE 3

struct VaultHeistUiNode {
    int value;
    float x; // Normalized tree-stage coordinate, 0.0 to 1.0.
    float y; // Normalized tree-stage coordinate, 0.0 to 1.0.
    int state;
    const char* label;
};

struct VaultHeistUiEdge {
    int fromNode;
    int toNode;
    int skipped;
    int active;
};

struct VaultHeistUiAssets {
    Texture2D vaultBackground;
    Texture2D catIdle;
    Texture2D catWrong;
    Texture2D catNice;
    Texture2D catComplete;
};

struct VaultHeistUiState {
    const char* actLabel;
    const char* levelTitle;
    const char* objective;
    const char* prompt;
    const char* currentMessage;
    const char* nextAction;
    const char* leftChoice;
    const char* rightChoice;
    const VaultHeistUiNode* nodes;
    int nodeCount;
    const VaultHeistUiEdge* edges;
    int edgeCount;
    int currentNodeIndex;
    int reaction; // Use VHG_REACTION_*; the caller controls reaction timing.
    int alarmPercent;
    int score;
    int penalties;
    int currentStep;
    int totalSteps;
    int levelComplete;
    int paused;
};

// Draw-only UI. The caller owns texture loading, input, model state, and timers.
void DrawVaultHeistGameplayUI(const VaultHeistUiState& state,
                              const VaultHeistUiAssets& assets);
