#include "UavMonitorScreen.h"
#include <cmath>

using namespace std;

void UavMonitorScreen::setup() {
    altitudeData.clear();
    velocityData.clear();
    updateTimer = 0.f;
    scrollOffset = 0.f;

    for (int i = 0; i < 160; i++) {
        altitudeData.push_back(15000.f);
        velocityData.push_back(200.f);
    }
}

void UavMonitorScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;

    scrollOffset += dt * 50.f;
    updateTimer += dt;

    if (updateTimer > 0.05f) {
        updateTimer = 0.f;

        float currentAlt = altitudeData.back();
        float currentVel = velocityData.back();

        float altDrift = (15000.f - currentAlt) * 0.02f + (float)(sin(GetTime() * 0.5f) * 10.f);
        float velDrift = (200.f - currentVel) * 0.05f + (float)(cos(GetTime() * 0.3f) * 2.f);

        if (GetRandomValue(0, 1000) > 995) {
            altDrift -= GetRandomValue(300, 800);
            velDrift += GetRandomValue(20, 50);
        }

        float newAlt = currentAlt + altDrift;
        float newVel = currentVel + velDrift;

        if (newAlt < 0.f) newAlt = 0.f;
        if (newAlt > 19500.f) newAlt = 19500.f;

        if (newVel < 50.f) newVel = 50.f;
        if (newVel > 480.f) newVel = 480.f;

        altitudeData.erase(altitudeData.begin());
        altitudeData.push_back(newAlt);

        velocityData.erase(velocityData.begin());
        velocityData.push_back(newVel);
    }
}

void UavMonitorScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };     
    Color altCol = { 80, 255, 150, 255 };
    Color velCol = { 255, 200, 50, 255 };

    DrawTextEx(font, "=== UAV FLIGHT TELEMETRY LINK ===", { 50.f, 30.f }, 24, 0, mainCol);
    DrawTextEx(font, "[ESC] Back", { 50.f, 550.f }, 18, 0, GRAY);

    float currentAlt = altitudeData.back();
    float currentVel = velocityData.back();

    DrawTextEx(font, TextFormat("ALTITUDE : %.1f FT (MSL)", currentAlt), { 50.f, 80.f }, 20, 0, altCol);
    DrawTextEx(font, TextFormat("AIRSPEED : %.1f KTS", currentVel), { 50.f, 105.f }, 20, 0, velCol);

    if (currentAlt < 5000.f) {
        DrawTextEx(font, "WARNING: LOW ALTITUDE. TERRAIN COLLISION IMMINENT.", { 400.f, 80.f }, 20, 0, RED);
    }

    int graphX = 50;
    int graphY = 150;
    int graphW = 900;
    int graphH = 350;

    DrawRectangleLines(graphX, graphY, graphW, graphH, Fade(mainCol, 0.5f));

    for (int i = 0; i < 18; i++) {
        int gridX = graphX + ((i * 50) - (int)scrollOffset % 50);
        if (gridX > graphX && gridX < graphX + graphW) {
            DrawLine(gridX, graphY, gridX, graphY + graphH, Fade(mainCol, 0.1f));
        }
    }

    for (int i = 0; i < altitudeData.size() - 1; i++) {
        float x1 = graphX + (i * 5.62f);
        float x2 = graphX + ((i + 1) * 5.62f);

        float altY1 = (graphY + graphH) - ((altitudeData[i] / 20000.f) * graphH);
        float altY2 = (graphY + graphH) - ((altitudeData[i + 1] / 20000.f) * graphH);

        float velY1 = (graphY + graphH) - ((velocityData[i] / 500.f) * graphH);
        float velY2 = (graphY + graphH) - ((velocityData[i + 1] / 500.f) * graphH);

        DrawLineEx({ x1, altY1 }, { x2, altY2 }, 2.f, altCol);
        DrawLineEx({ x1, velY1 }, { x2, velY2 }, 2.f, velCol);
    }
}