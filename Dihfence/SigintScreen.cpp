#include "SigintScreen.h"
#include <cmath>

using namespace std;

void SigintScreen::setup() {
    targetAmp = (float)GetRandomValue(20, 100);
    targetFreq = (float)GetRandomValue(1, 5) * 0.01f;
    targetPhase = (float)GetRandomValue(0, 314) * 0.01f;
    userAmp = 50.f;
    userFreq = 0.02f;
    userPhase = 0.f;
    isLocked = false;
    lockTimer = 0.f;
}

void SigintScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;
    if (isLocked) return;

    if (IsKeyDown(KEY_Q)) userAmp += 20.f * dt;
    if (IsKeyDown(KEY_A)) userAmp -= 20.f * dt;
    if (IsKeyDown(KEY_W)) userFreq += 0.01f * dt;
    if (IsKeyDown(KEY_S)) userFreq -= 0.01f * dt;
    if (IsKeyDown(KEY_E)) userPhase += 1.0f * dt;
    if (IsKeyDown(KEY_D)) userPhase -= 1.0f * dt;

    if (userAmp < 5.f) userAmp = 5.f;
    if (userFreq < 0.005f) userFreq = 0.005f;

    float diffAmp = abs(targetAmp - userAmp);
    float diffFreq = abs(targetFreq - userFreq);
    float diffPhase = (float)fmod(abs(targetPhase - userPhase), 3.14159f * 2.f);

    if (diffAmp < 15.f && diffFreq < 0.015f && diffPhase < 0.8f) {
        lockTimer += dt;
        if (lockTimer > 1.0f) isLocked = true;
    }
    else {
        lockTimer = 0.f;
    }
}

void SigintScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color enemyCol = { 255, 50, 50, 150 };
    Color userCol = { 100, 255, 100, 255 };

    DrawTextEx(font, "=== SIGINT TRIANGULATION ===", { 50.f, 30.f }, 24, 0, mainCol);
    DrawTextEx(font, "[Q/A] Amp | [W/S] Freq | [E/D] Phase | [ESC] Back", { 50.f, 550.f }, 18, 0, GRAY);

    DrawTextEx(font, TextFormat("AMPLITUDE: %.1f", userAmp), { 50.f, 400.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("FREQUENCY: %.3f", userFreq), { 50.f, 430.f }, 20, 0, mainCol);
    DrawTextEx(font, TextFormat("PHASE SFT: %.1f", userPhase), { 50.f, 460.f }, 20, 0, mainCol);

    if (isLocked) {
        DrawTextEx(font, "SIGNAL LOCK ACHIEVED", { 400.f, 100.f }, 24, 0, userCol);
        DrawTextEx(font, "DECRYPTED ORIGIN: SECTOR 7G [34.11N, 118.23W]", { 400.f, 130.f }, 20, 0, mainCol);
    }
    else if (lockTimer > 0.f) {
        DrawTextEx(font, "HOLD SIGNAL...", { 450.f, 100.f }, 24, 0, YELLOW);
    }

    Vector2 prevEnemy = { 100.f, 250.f };
    Vector2 prevUser = { 100.f, 250.f };

    for (float x = 100.f; x <= 900.f; x += 5.f) {
        float timeShift = GetTime() * 2.0f;
        float enemyY = 250.f + targetAmp * (float)sin(targetFreq * x + targetPhase + timeShift);
        float userY = 250.f + userAmp * (float)sin(userFreq * x + userPhase + timeShift);
        Vector2 currEnemy = { x, enemyY };
        Vector2 currUser = { x, userY };
        DrawLineEx(prevEnemy, currEnemy, 3.f, enemyCol);
        DrawLineEx(prevUser, currUser, 2.f, userCol);
        prevEnemy = currEnemy;
        prevUser = currUser;
    }
    DrawLine(100, 250, 900, 250, Fade(mainCol, 0.3f));
}