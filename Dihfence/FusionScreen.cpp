#include "FusionScreen.h"
#include <cmath>

using namespace std;

void FusionScreen::setup() {
    globalThreat = 0.f;
    radarRisk = 10.f;
    scadaRisk = 5.f;
    sigintRisk = 8.f;
    authFailures = 0.f;
    cpuLoad = 15.f;
    memUsage = 22.f;
    coreTemp = 40.f;
    updateTimer = 0.f;
    threatHistory.clear();

    for (int i = 0; i < 180; i++) threatHistory.push_back(0.f);
}

void FusionScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;

    radarRisk += (GetRandomValue(-10, 10) / 10.0f) * dt * 15.f;
    scadaRisk += (GetRandomValue(-10, 10) / 10.0f) * dt * 8.f;
    sigintRisk += (GetRandomValue(-10, 10) / 10.0f) * dt * 12.f;
    authFailures += (GetRandomValue(-10, 10) / 10.0f) * dt * 5.f;

    if (radarRisk < 0.f) radarRisk = 0.f; if (radarRisk > 100.f) radarRisk = 100.f;
    if (scadaRisk < 0.f) scadaRisk = 0.f; if (scadaRisk > 100.f) scadaRisk = 100.f;
    if (sigintRisk < 0.f) sigintRisk = 0.f; if (sigintRisk > 100.f) sigintRisk = 100.f;
    if (authFailures < 0.f) authFailures = 0.f; if (authFailures > 100.f) authFailures = 100.f;

    globalThreat = (radarRisk * 0.40f) + (scadaRisk * 0.25f) + (sigintRisk * 0.25f) + (authFailures * 0.10f);

    cpuLoad = 20.f + (globalThreat * 0.6f) + (float)(sin(GetTime() * 5.0f) * 5.f);
    memUsage = 30.f + (globalThreat * 0.4f);
    coreTemp = 40.f + (cpuLoad * 0.4f);

    updateTimer += dt;
    if (updateTimer > 0.1f) {
        updateTimer = 0.f;
        threatHistory.erase(threatHistory.begin());
        threatHistory.push_back(globalThreat);
    }
}

void FusionScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color alertCol = { 255, 80, 80, 255 };
    Color warnCol = { 255, 200, 50, 255 };
    Color okCol = { 80, 255, 80, 255 };

    DrawTextEx(font, "=== SENSOR FUSION & SYSTEM TELEMETRY ===", { 50.f, 30.f }, 24, 0, mainCol);
    DrawTextEx(font, "[ESC] Back to Main Menu", { 50.f, 550.f }, 18, 0, GRAY);

    DrawRectangleLines(50, 80, 420, 180, Fade(mainCol, 0.5f));
    DrawTextEx(font, "AGGREGATED THREAT LEVEL", { 65.f, 95.f }, 20, 0, mainCol);

    Color threatCol = (globalThreat > 75.f) ? alertCol : ((globalThreat > 40.f) ? warnCol : okCol);
    DrawRectangle(65, 140, (int)(globalThreat * 3.8f), 35, Fade(threatCol, 0.6f));
    DrawRectangleLines(65, 140, 380, 35, mainCol);
    DrawTextEx(font, TextFormat("%.1f%%", globalThreat), { 65.f, 190.f }, 40, 0, threatCol);

    DrawRectangleLines(490, 80, 460, 180, Fade(mainCol, 0.5f));
    DrawTextEx(font, "ACTIVE NODE TELEMETRY", { 505.f, 95.f }, 20, 0, mainCol);

    DrawTextEx(font, TextFormat("RDR [RADAR]   RISK: %02.1f%%", radarRisk), { 505.f, 130.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("SCA [SCADA]   RISK: %02.1f%%", scadaRisk), { 505.f, 155.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("SIG [SIGINT]  RISK: %02.1f%%", sigintRisk), { 505.f, 180.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("COM [COMSEC]  FAIL: %02.1f%%", authFailures), { 505.f, 205.f }, 20, 0, mainCol);

    DrawTextEx(font, TextFormat("CPU: %.0f%%", cpuLoad), { 820.f, 130.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("MEM: %.0f%%", memUsage), { 820.f, 155.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("TMP: %.0fC", coreTemp), { 820.f, 180.f }, 20, 0, mainCol);

    DrawRectangleLines(50, 280, 900, 240, Fade(mainCol, 0.5f));
    DrawTextEx(font, "THREAT HISTORY (T-18s)", { 65.f, 295.f }, 20, 0, mainCol);

    DrawLine(50, 360, 950, 360, Fade(mainCol, 0.1f));
    DrawLine(50, 440, 950, 440, Fade(mainCol, 0.1f));

    for (int i = 0; i < threatHistory.size() - 1; i++) {
        Vector2 p1 = { 65.f + (i * 4.6f), 500.f - (threatHistory[i] * 1.8f) };
        Vector2 p2 = { 65.f + ((i + 1) * 4.6f), 500.f - (threatHistory[i + 1] * 1.8f) };
        DrawLineEx(p1, p2, 2.f, threatCol);
    }
}