#include "MenuScreen.h"

using namespace std;

void MenuScreen::setup() {}

void MenuScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ONE)) activeScreen = 1;
    if (IsKeyPressed(KEY_TWO)) activeScreen = 2;
    if (IsKeyPressed(KEY_THREE)) activeScreen = 3;
    if (IsKeyPressed(KEY_FOUR)) activeScreen = 4;
    if (IsKeyPressed(KEY_FIVE)) activeScreen = 5;
    if (IsKeyPressed(KEY_SIX)) activeScreen = 6;
    if (IsKeyPressed(KEY_SEVEN)) activeScreen = 7;
}

void MenuScreen::draw(Font font) {
    Color textCol = { 100, 200, 255, 255 };
    Color dimCol = { 150, 150, 150, 255 };

    DrawTextEx(font, "================================", { 150.f, 80.f }, 24, 0, textCol);
    DrawTextEx(font, "          C.A.S.T.L.E ", { 150.f, 110.f }, 24, 0, textCol);
    DrawTextEx(font, "================================", { 150.f, 140.f }, 24, 0, textCol);

    DrawTextEx(font, "SYSTEM MODULES ONLINE:", { 150.f, 200.f }, 24, 0, textCol);
    DrawTextEx(font, "[1] COMSEC DECRYPTER", { 150.f, 240.f }, 24, 0, textCol);
    DrawTextEx(font, "[2] TACTICAL RADAR (HIL)", { 150.f, 270.f }, 24, 0, textCol);
    DrawTextEx(font, "[3] SCADA POWER ROUTING", { 150.f, 300.f }, 24, 0, textCol);
    DrawTextEx(font, "[4] SIGINT TRIANGULATOR", { 150.f, 330.f }, 24, 0, textCol);
    DrawTextEx(font, "[5] ASYNC HANDSHAKE AUTH", { 150.f, 360.f }, 24, 0, textCol);
    DrawTextEx(font, "[6] SENSOR FUSION MONITOR", { 150.f, 390.f }, 24, 0, textCol);
    DrawTextEx(font, "[7] UAV TELEMETRY TRACKER", { 150.f, 420.f }, 24, 0, textCol);

    DrawTextEx(font, "KEYBINDS: [1-7] Select Module | [F11] Fullscreen | Window [X] to Exit", { 20.f, 550.f }, 18, 0, dimCol);
}