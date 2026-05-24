#include "ScadaScreen.h"
#include <cmath>

using namespace std;

void ScadaScreen::setup() {
    gridNodes.clear();
    currentPath.clear();
    powerRestored = false;
    alertTimer = 0.f;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 6; x++) {
            PowerNode n;
            n.pos = { 200.f + (x * 120.f), 150.f + (y * 100.f) };
            n.isActive = false;
            n.isCorrupted = (GetRandomValue(1, 100) < 25);
            gridNodes.push_back(n);
        }
    }

    gridNodes[0].isCorrupted = false;
    gridNodes[23].isCorrupted = false;
    currentPath.push_back(0);
}

void ScadaScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;
    if (IsKeyPressed(KEY_R)) setup();

    alertTimer += dt;
    if (powerRestored) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        mouse.x = mouse.x * (1000.0f / GetScreenWidth());
        mouse.y = mouse.y * (600.0f / GetScreenHeight());

        for (int i = 0; i < gridNodes.size(); i++) {
            float dx = mouse.x - gridNodes[i].pos.x;
            float dy = mouse.y - gridNodes[i].pos.y;

            if (sqrt(dx * dx + dy * dy) < 20.f) {
                if (gridNodes[i].isCorrupted) continue;

                int lastNodeIdx = currentPath.back();
                float distToLast = (float)sqrt(
                    pow(gridNodes[i].pos.x - gridNodes[lastNodeIdx].pos.x, 2) +
                    pow(gridNodes[i].pos.y - gridNodes[lastNodeIdx].pos.y, 2)
                );

                if (distToLast > 10.f && distToLast < 160.f) {
                    currentPath.push_back(i);
                    gridNodes[i].isActive = true;
                    if (i == 23) powerRestored = true;
                }
            }
        }
    }
}

void ScadaScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color errCol = { 255, 80, 80, 255 };
    Color okCol = { 80, 255, 80, 255 };

    DrawTextEx(font, "=== SCADA GRID ROUTING ===", { 50.f, 30.f }, 24, 0, mainCol);
    DrawTextEx(font, "[L-CLICK] Route Power   [R] Reset Grid   [ESC] Back", { 50.f, 550.f }, 18, 0, GRAY);

    if (powerRestored) {
        if ((int)(alertTimer * 2) % 2 == 0) DrawTextEx(font, "CONNECTION ESTABLISHED", { 650.f, 30.f }, 24, 0, okCol);
    }
    else {
        DrawTextEx(font, "MAIN RADAR OFFLINE - REROUTE REQUIRED", { 550.f, 30.f }, 24, 0, errCol);
    }

    for (int i = 0; i < (int)currentPath.size() - 1; i++) {
        int n1 = currentPath[i];
        int n2 = currentPath[i + 1];
        DrawLineEx(gridNodes[n1].pos, gridNodes[n2].pos, 4.f, okCol);
    }

    for (int i = 0; i < gridNodes.size(); i++) {
        if (gridNodes[i].isCorrupted) {
            DrawCircleV(gridNodes[i].pos, 15.f, Fade(errCol, 0.5f));
            DrawTextEx(font, "X", { gridNodes[i].pos.x - 5.f, gridNodes[i].pos.y - 10.f }, 20, 0, errCol);
        }
        else {
            bool isInPath = false;
            for (int p : currentPath) if (p == i) isInPath = true;
            Color nodeCol = isInPath ? okCol : mainCol;
            DrawCircleLines((int)gridNodes[i].pos.x, (int)gridNodes[i].pos.y, 15.f, nodeCol);

            if (i == 0) DrawTextEx(font, "SRC", { gridNodes[i].pos.x - 30.f, gridNodes[i].pos.y - 30.f }, 18, 0, okCol);
            if (i == 23) DrawTextEx(font, "RDR", { gridNodes[i].pos.x + 15.f, gridNodes[i].pos.y - 30.f }, 18, 0, mainCol);
        }
    }
}