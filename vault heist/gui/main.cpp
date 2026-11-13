#include "GameApp.h"
#include "MockGameModel.h"

int main() {
    MockGameModel model;
    GameApp app(model);
    app.run();
    return 0;
}
