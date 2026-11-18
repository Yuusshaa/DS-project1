#include "VaultHeistGameplayUI.h"

#include <cstdio>

namespace {

Color Bg() { return Color{16, 24, 32, 255}; }
Color Surface() { return Color{24, 36, 42, 255}; }
Color SurfaceRaised() { return Color{34, 50, 56, 255}; }
Color Line() { return Color{51, 67, 74, 255}; }
Color LineBright() { return Color{82, 98, 106, 255}; }
Color TextMain() { return Color{245, 240, 230, 255}; }
Color Muted() { return Color{168, 179, 194, 255}; }
Color Gold() { return Color{244, 201, 93, 255}; }
Color Teal() { return Color{100, 216, 203, 255}; }
Color Coral() { return Color{255, 122, 89, 255}; }

float Min(float a, float b) { return a < b ? a : b; }
float Max(float a, float b) { return a > b ? a : b; }

void Text(const char* value, int x, int y, int size, Color color) {
    if (value != 0) DrawText(value, x, y, size, color);
}

void Panel(Rectangle bounds, Color fill, Color border, float scale) {
    DrawRectangleRounded(bounds, 0.08f, 8, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.08f, 8, 1.0f * scale, border);
}

Texture2D ReactionTexture(const VaultHeistUiState& state,
                          const VaultHeistUiAssets& assets) {
    if (state.reaction == VHG_REACTION_WRONG) return assets.catWrong;
    if (state.reaction == VHG_REACTION_NICE) return assets.catNice;
    if (state.reaction == VHG_REACTION_COMPLETE) return assets.catComplete;
    return assets.catIdle;
}

void DrawNode(const VaultHeistUiNode& node, float x, float y, float scale,
              int showReaction, Texture2D reactionTexture) {
    float radius = node.state == VHG_NODE_CURRENT ? 28.0f : 24.0f;
    Color border = LineBright();
    Color fill = Color{29, 43, 51, 255};
    Color valueColor = TextMain();

    if (node.state == VHG_NODE_CURRENT) {
        border = Gold();
        fill = Color{41, 53, 45, 255};
        valueColor = Gold();
    } else if (node.state == VHG_NODE_TARGET) {
        border = Teal();
        fill = Color{24, 52, 49, 255};
        valueColor = Teal();
    } else if (node.state == VHG_NODE_SKIPPED) {
        border = Color{113, 128, 135, 255};
        fill = Color{37, 48, 54, 255};
        valueColor = Color{176, 186, 194, 255};
    }

    if (showReaction && reactionTexture.id != 0) {
        float catSize = 56.0f * scale;
        Rectangle source = Rectangle{0, 0, (float)reactionTexture.width,
                                     (float)reactionTexture.height};
        Rectangle destination = Rectangle{x - catSize * 0.5f,
                                          y - (radius + 83.0f) * scale,
                                          catSize, catSize};
        DrawTexturePro(reactionTexture, source, destination, Vector2{0, 0},
                       0.0f, WHITE);
        DrawRectangleLinesEx(destination, 2.0f * scale,
                             state.reaction == VHG_REACTION_WRONG ? Coral() : Gold());

        const char* reactionText = 0;
        Color reactionColor = Gold();
        if (state.reaction == VHG_REACTION_WRONG) {
            reactionText = "Oops, wrong room!";
            reactionColor = Coral();
        } else if (state.reaction == VHG_REACTION_NICE) {
            reactionText = "Nice find!";
            reactionColor = Teal();
        } else if (state.reaction == VHG_REACTION_COMPLETE) {
            reactionText = "Level complete!";
            reactionColor = Teal();
        }

        if (reactionText != 0) {
            int labelWidth = MeasureText(reactionText, (int)(12 * scale));
            Rectangle badge = Rectangle{x - labelWidth * 0.5f - 10.0f * scale,
                                        y - (radius + 99.0f) * scale,
                                        labelWidth + 20.0f * scale, 22.0f * scale};
            DrawRectangleRounded(badge, 0.45f, 8, Surface());
            DrawRectangleRoundedLinesEx(badge, 0.45f, 8, 1.0f * scale, reactionColor);
            DrawText(reactionText, (int)(x - labelWidth * 0.5f),
                     (int)(badge.y + 4.0f * scale), (int)(12.0f * scale), reactionColor);
        }
    }

    DrawCircle((int)x, (int)y, radius * scale, fill);
    DrawCircleLines((int)x, (int)y, radius * scale, border);
    DrawCircleLines((int)x, (int)y, (radius - 2.0f) * scale,
                    Fade(border, 0.35f));

    char number[24];
    std::snprintf(number, sizeof(number), "%d", node.value);
    int fontSize = (int)(node.state == VHG_NODE_CURRENT ? 17.0f : 15.0f) * scale;
    int width = MeasureText(number, fontSize);
    DrawText(number, (int)(x - width * 0.5f), (int)(y - fontSize * 0.5f),
             fontSize, valueColor);

    if (node.label != 0 && node.label[0] != '\0') {
        int labelSize = (int)(9.0f * scale);
        int labelWidth = MeasureText(node.label, labelSize);
        Color labelColor = node.state == VHG_NODE_TARGET ? Teal() : Gold();
        if (node.state == VHG_NODE_SKIPPED) labelColor = Muted();
        DrawText(node.label, (int)(x - labelWidth * 0.5f),
                 (int)(y - (radius + 18.0f) * scale), labelSize, labelColor);
    }
}

void DrawTextureCover(Texture2D texture, Rectangle dest) {
    if (texture.id == 0 || texture.width <= 0 || texture.height <= 0) return;
    float sourceAspect = (float)texture.width / (float)texture.height;
    float destAspect = dest.width / dest.height;
    Rectangle source = Rectangle{0, 0, (float)texture.width, (float)texture.height};
    if (sourceAspect > destAspect) {
        float width = texture.height * destAspect;
        source.x = (texture.width - width) * 0.5f;
        source.width = width;
    } else {
        float height = texture.width / destAspect;
        source.y = (texture.height - height) * 0.5f;
        source.height = height;
    }
    DrawTexturePro(texture, source, dest, Vector2{0, 0}, 0.0f, WHITE);
}

} // namespace

void DrawVaultHeistGameplayUI(const VaultHeistUiState& state,
                              const VaultHeistUiAssets& assets) {
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float scale = Min(screenWidth / 1512.0f, screenHeight / 982.0f);
    if (scale <= 0.0f) return;
    float left = (screenWidth - 1512.0f * scale) * 0.5f;
    float top = (screenHeight - 982.0f * scale) * 0.5f;

    #define X(v) ((int)(left + (v) * scale))
    #define Y(v) ((int)(top + (v) * scale))
    #define S(v) ((int)((v) * scale))

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Bg());

    Rectangle header = Rectangle{left, top, 1512.0f * scale, 76.0f * scale};
    DrawRectangleRec(header, Color{16, 24, 32, 255});
    DrawLine(X(0), Y(75), X(1512), Y(75), Line());

    DrawRectangleRounded(Rectangle{X(28), Y(16), S(44), S(44)}, 0.2f, 8,
                         Color{37, 49, 43, 255});
    DrawRectangleRoundedLinesEx(Rectangle{X(28), Y(16), S(44), S(44)},
                                0.2f, 8, 1.0f * scale, Gold());
    Text("07", X(39), Y(28), S(18), Gold());
    Text(state.actLabel != 0 ? state.actLabel : "VAULT HEIST", X(84), Y(16), S(10), Teal());
    Text(state.levelTitle != 0 ? state.levelTitle : "Search the Vault", X(84), Y(35), S(19), TextMain());

    Rectangle objective = Rectangle{X(440), Y(12), S(620), S(52)};
    Panel(objective, Surface(), Line(), scale);
    Text("OBJECTIVE", X(458), Y(19), S(9), Muted());
    Text(state.objective != 0 ? state.objective : "Find the target treasure", X(458), Y(37), S(14), TextMain());

    Rectangle action = Rectangle{X(1305), Y(17), S(84), S(40)};
    Panel(action, Surface(), Line(), scale);
    Text(state.paused ? "RESUME" : "PAUSE", X(1322), Y(30), S(11), TextMain());
    Rectangle exit = Rectangle{X(1398), Y(17), S(84), S(40)};
    Panel(exit, Surface(), Line(), scale);
    Text("EXIT", X(1424), Y(30), S(11), Muted());

    Rectangle statusPanel = Rectangle{X(20), Y(88), S(188), S(760)};
    Rectangle playPanel = Rectangle{X(220), Y(88), S(972), S(760)};
    Rectangle eventPanel = Rectangle{X(1204), Y(88), S(288), S(760)};
    Panel(statusPanel, Surface(), Line(), scale);
    Panel(playPanel, Surface(), Line(), scale);
    Panel(eventPanel, Surface(), Line(), scale);

    // Left status rail.
    Text("HEIST STATUS", X(36), Y(108), S(11), Muted());
    Rectangle alarmCard = Rectangle{X(32), Y(140), S(164), S(104)};
    Panel(alarmCard, Color{39, 43, 40, 255}, Color{112, 79, 64, 255}, scale);
    Text("ALARM", X(45), Y(154), S(11), TextMain());
    char alarmText[16];
    std::snprintf(alarmText, sizeof(alarmText), "%d%%", state.alarmPercent);
    Text(alarmText, X(145), Y(150), S(18), state.alarmPercent >= 75 ? Coral() : Gold());
    DrawRectangleRounded(Rectangle{X(44), Y(181), S(140), S(8)}, 0.5f, 8, Line());
    float alarm = Max(0.0f, Min(100.0f, (float)state.alarmPercent));
    DrawRectangleRounded(Rectangle{X(44), Y(181), S(140) * alarm / 100.0f, S(8)},
                         0.5f, 8, alarm >= 75.0f ? Coral() : Gold());
    Text(alarm >= 75.0f ? "Critical alarm" : "Alarm rising", X(44), Y(202), S(10), Muted());

    Text("SCORE", X(36), Y(270), S(10), Muted());
    char scoreText[32];
    std::snprintf(scoreText, sizeof(scoreText), "%d", state.score);
    Text(scoreText, X(36), Y(290), S(28), Gold());
    Text("PENALTIES", X(36), Y(345), S(10), Muted());
    char penaltyText[32];
    std::snprintf(penaltyText, sizeof(penaltyText), "%d mistakes", state.penalties);
    Text(penaltyText, X(36), Y(365), S(13), state.penalties > 0 ? Coral() : TextMain());
    Text("CURRENT STEP", X(36), Y(420), S(10), Muted());
    char stepText[32];
    std::snprintf(stepText, sizeof(stepText), "%d / %d", state.currentStep, state.totalSteps);
    Text(stepText, X(36), Y(440), S(13), Teal());
    int dots = state.totalSteps < 1 ? 1 : state.totalSteps;
    for (int i = 0; i < dots; i++) {
        float dotX = 36.0f + i * (144.0f / dots);
        DrawRectangleRounded(Rectangle{X(dotX), Y(466), S(144.0f / dots - 4), S(5)},
                             0.5f, 8, i < state.currentStep ? Teal() : Line());
    }
    Text(state.prompt != 0 ? state.prompt : "Choose your next move", X(36), Y(489), S(10), Muted());

    // Main play area and faint, darkened vault background.
    Rectangle stage = Rectangle{X(221), Y(148), S(970), S(700)};
    DrawTextureCover(assets.vaultBackground, stage);
    DrawRectangleRec(stage, Color{16, 24, 32, 218});
    DrawRectangleLinesEx(stage, 1.0f * scale, Line());
    Text("VAULT MAP", X(242), Y(105), S(10), Muted());
    Text(state.prompt != 0 ? state.prompt : "Choose the correct branch", X(242), Y(122), S(16), TextMain());

    for (int i = 0; i < state.edgeCount; i++) {
        const VaultHeistUiEdge& edge = state.edges[i];
        if (edge.fromNode < 0 || edge.toNode < 0 || edge.fromNode >= state.nodeCount ||
            edge.toNode >= state.nodeCount) continue;
        const VaultHeistUiNode& from = state.nodes[edge.fromNode];
        const VaultHeistUiNode& to = state.nodes[edge.toNode];
        Vector2 start = Vector2{stage.x + from.x * stage.width, stage.y + from.y * stage.height};
        Vector2 end = Vector2{stage.x + to.x * stage.width, stage.y + to.y * stage.height};
        Color edgeColor = edge.active ? Teal() : edge.skipped ? Color{113, 128, 135, 150} : LineBright();
        if (edge.skipped) {
            DrawLineEx(start, end, 1.3f * scale, edgeColor);
        } else {
            DrawLineEx(start, end, edge.active ? 2.3f * scale : 1.6f * scale, edgeColor);
        }
    }

    Texture2D reactionTexture = ReactionTexture(state, assets);
    for (int i = 0; i < state.nodeCount; i++) {
        const VaultHeistUiNode& node = state.nodes[i];
        float nx = stage.x + node.x * stage.width;
        float ny = stage.y + node.y * stage.height;
        int showReaction = (i == state.currentNodeIndex) ||
                           (state.levelComplete && node.state == VHG_NODE_TARGET);
        DrawNode(node, nx, ny, scale, showReaction, reactionTexture);
    }

    Rectangle compare = Rectangle{stage.x + 18.0f * scale,
                                  stage.y + stage.height - 92.0f * scale,
                                  190.0f * scale, 72.0f * scale};
    Panel(compare, Color{24, 36, 42, 245}, LineBright(), scale);
    Text("CURRENT EVENT", (int)(compare.x + 12 * scale), (int)(compare.y + 10 * scale), S(9), Muted());
    Text(state.currentMessage != 0 ? state.currentMessage : "Waiting for your move",
         (int)(compare.x + 12 * scale), (int)(compare.y + 33 * scale), S(12), TextMain());

    // Event feed panel.
    Text("EVENT FEED", X(1222), Y(108), S(11), Muted());
    Rectangle currentEvent = Rectangle{X(1218), Y(140), S(260), S(190)};
    Panel(currentEvent, SurfaceRaised(), state.levelComplete ? Teal() : Gold(), scale);
    Text(state.levelComplete ? "LEVEL COMPLETE" : "CURRENT EVENT",
         X(1234), Y(158), S(9), state.levelComplete ? Teal() : Gold());
    Text(state.currentMessage != 0 ? state.currentMessage : "Choose a branch",
         X(1234), Y(194), S(15), TextMain());
    Text(state.nextAction != 0 ? state.nextAction : "Next: choose left or right",
         X(1234), Y(226), S(11), Muted());
    DrawLine(X(1234), Y(270), X(1462), Y(270), Line());
    Text("Recent events", X(1222), Y(354), S(11), Muted());
    Text("Current action and model events", X(1222), Y(380), S(10), Muted());

    // Bottom action deck.
    DrawRectangleRec(Rectangle{left, top + 850.0f * scale, 1512.0f * scale, 132.0f * scale},
                     Color{16, 24, 32, 255});
    DrawLine(X(0), Y(850), X(1512), Y(850), Line());
    Text(state.levelComplete ? "LEVEL COMPLETE" : "YOUR MOVE", X(30), Y(880), S(10), Teal());
    Text(state.levelComplete ? "Treasure recovered" : "Choose the next branch",
         X(30), Y(902), S(14), TextMain());

    Rectangle leftButton = Rectangle{X(500), Y(870), S(210), S(72)};
    Panel(leftButton, Surface(), LineBright(), scale);
    Text("A", X(519), Y(896), S(12), Muted());
    Text(state.leftChoice != 0 ? state.leftChoice : "Go left", X(555), Y(895), S(15), TextMain());
    Rectangle rightButton = Rectangle{X(722), Y(870), S(210), S(72)};
    Panel(rightButton, Gold(), Gold(), scale);
    Text("D", X(741), Y(896), S(12), Color{24, 36, 42, 255});
    Text(state.rightChoice != 0 ? state.rightChoice : "Go right", X(777), Y(895), S(15), Color{24, 36, 42, 255});

    Rectangle resetButton = Rectangle{X(1360), Y(880), S(62), S(56)};
    Panel(resetButton, Surface(), Line(), scale);
    Text("RESET", X(1370), Y(900), S(10), TextMain());
    Rectangle playbackButton = Rectangle{X(1430), Y(880), S(62), S(56)};
    Panel(playbackButton, Surface(), Line(), scale);
    Text("PLAY", X(1443), Y(900), S(10), TextMain());

    if (state.paused) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{10, 15, 18, 190});
        Rectangle pausePanel = Rectangle{screenWidth * 0.5f - 190.0f * scale,
                                         screenHeight * 0.5f - 90.0f * scale,
                                         380.0f * scale, 180.0f * scale};
        Panel(pausePanel, Surface(), LineBright(), scale);
        Text("HEIST PAUSED", (int)(pausePanel.x + 98.0f * scale),
             (int)(pausePanel.y + 45.0f * scale), S(18), Gold());
        Text("Resume when you are ready.", (int)(pausePanel.x + 66.0f * scale),
             (int)(pausePanel.y + 91.0f * scale), S(13), TextMain());
    }

    #undef X
    #undef Y
    #undef S
}
