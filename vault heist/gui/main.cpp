#include "GameApp.h"
#include "realgamemodel.h"

int main() {
    RealGameModel model;
    GameApp app(model);
    app.run();
    return 0;
}
