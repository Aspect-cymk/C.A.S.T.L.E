#include "HandshakeScreen.h"
#include <cctype>

using namespace std;

void HandshakeScreen::setup() {
    authState = 0;
    targetCode = "";
    currentInput = "";
    maxTime = 5.0f;
    timer = maxTime;
}

void HandshakeScreen::generateCode() {
    targetCode = "";
    const char charset[] = "0123456789ABCDEF";
    for (int i = 0; i < 6; i++) targetCode += charset[GetRandomValue(0, 15)];
}

bool HandshakeScreen::isValidKeystroke(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
}

void HandshakeScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;

    if (authState == 0) {
        if (IsKeyPressed(KEY_ENTER)) {
            generateCode();
            currentInput = "";
            timer = maxTime;
            authState = 1;
        }
    }
    else if (authState == 1) {
        timer -= dt;
        if (timer <= 0.f) {
            authState = 3;
            return;
        }

        int key = GetCharPressed();
        while (key > 0) {
            char typed = toupper((char)key);
            if (isValidKeystroke(typed) && currentInput.length() < 6) currentInput += typed;
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && currentInput.length() > 0) currentInput.pop_back();

        if (currentInput.length() == 6) {
            if (currentInput == targetCode) authState = 2;
            else authState = 3;
        }
    }
    else if (authState == 2 || authState == 3) {
        if (IsKeyPressed(KEY_ENTER)) setup();
    }
}

void HandshakeScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color warnCol = { 255, 200, 50, 255 };
    Color errCol = { 255, 80, 80, 255 };
    Color okCol = { 80, 255, 80, 255 };

    DrawTextEx(font, "=== ASYNCHRONOUS HANDSHAKE PROTOCOL ===", { 50.f, 30.f }, 24, 0, mainCol);
    DrawTextEx(font, "[ESC] Back", { 50.f, 550.f }, 18, 0, GRAY);

    DrawRectangleLines(200, 120, 600, 350, Fade(mainCol, 0.5f));

    if (authState == 0) {
        DrawTextEx(font, "AWAITING AUTHENTICATION SEQUENCE", { 270.f, 250.f }, 24, 0, mainCol);
        DrawTextEx(font, "PRESS [ENTER] TO INITIATE HANDSHAKE", { 260.f, 300.f }, 24, 0, warnCol);
    }
    else if (authState == 1) {
        DrawTextEx(font, "INCOMING CHALLENGE KEY:", { 380.f, 180.f }, 20, 0, mainCol);
        DrawTextEx(font, targetCode.c_str(), { 420.f, 210.f }, 40, 0, warnCol);

        DrawTextEx(font, "ENTER RESPONSE:", { 420.f, 290.f }, 20, 0, mainCol);
        DrawTextEx(font, currentInput.c_str(), { 420.f, 320.f }, 40, 0, okCol);

        float timePct = timer / maxTime;
        Color timeCol = (timePct > 0.4f) ? mainCol : errCol;
        DrawRectangle(250, 420, (int)(500 * timePct), 10, timeCol);
        DrawRectangleLines(250, 420, 500, 10, mainCol);
    }
    else if (authState == 2) {
        DrawTextEx(font, "HANDSHAKE SUCCESSFUL", { 380.f, 250.f }, 24, 0, okCol);
        DrawTextEx(font, "SECURE UPLINK ESTABLISHED", { 355.f, 290.f }, 20, 0, mainCol);
        DrawTextEx(font, "PRESS [ENTER] TO DISCONNECT", { 350.f, 380.f }, 18, 0, GRAY);
    }
    else if (authState == 3) {
        DrawTextEx(font, "HANDSHAKE FAILED", { 400.f, 250.f }, 24, 0, errCol);
        DrawTextEx(font, "CONNECTION SEVERED", { 390.f, 290.f }, 20, 0, errCol);
        DrawTextEx(font, "PRESS [ENTER] TO RETRY", { 380.f, 380.f }, 18, 0, GRAY);
    }
}