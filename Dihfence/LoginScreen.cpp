#include "LoginScreen.h"
#include <cctype>

using namespace std;

void LoginScreen::setup() {
    loginState = 0;
    userBuffer = "";
    passBuffer = "";
    challengeBuffer = "";
    failedAttempts = 0;
    lockoutTimer = 0.f;

    synChallenge = GetRandomValue(10, 50);
    expectedAck = (synChallenge * 3) + 7;
}

void LoginScreen::runFrame(float dt, int& activeScreen) {
    if (loginState == 3) {
        lockoutTimer -= dt;
        if (lockoutTimer <= 0.f) {
            setup();
        }
        return;
    }

    int key = GetCharPressed();
    while (key > 0) {
        char typed = toupper((char)key);
        if (typed >= 32 && typed <= 125) {
            if (loginState == 0 && userBuffer.length() < 15) userBuffer += typed;
            if (loginState == 1 && passBuffer.length() < 15) passBuffer += typed;
            if (loginState == 2 && challengeBuffer.length() < 5 && typed >= '0' && typed <= '9') challengeBuffer += typed;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (loginState == 0 && userBuffer.length() > 0) userBuffer.pop_back();
        if (loginState == 1 && passBuffer.length() > 0) passBuffer.pop_back();
        if (loginState == 2 && challengeBuffer.length() > 0) challengeBuffer.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (loginState == 0 && userBuffer.length() > 0) {
            loginState = 1;
        }
        else if (loginState == 1 && passBuffer.length() > 0) {
            if (userBuffer == "ADMIN" && passBuffer == "OZONE") {
                loginState = 2;
            }
            else {
                failedAttempts++;
                if (failedAttempts >= 3) {
                    loginState = 3;
                    lockoutTimer = 300.f;
                }
                else {
                    loginState = 0;
                    userBuffer = "";
                    passBuffer = "";
                }
            }
        }
        else if (loginState == 2 && challengeBuffer.length() > 0) {
            int inputAck = stoi(challengeBuffer);
            if (inputAck == expectedAck) {
                activeScreen = 0;
            }
            else {
                failedAttempts++;
                if (failedAttempts >= 3) {
                    loginState = 3;
                    lockoutTimer = 300.f;
                }
                else {
                    loginState = 0;
                    userBuffer = "";
                    passBuffer = "";
                    challengeBuffer = "";
                    synChallenge = GetRandomValue(10, 50);
                    expectedAck = (synChallenge * 3) + 7;
                }
            }
        }
    }
}

void LoginScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color errCol = { 255, 50, 50, 255 };
    Color warnCol = { 255, 200, 50, 255 };

    DrawTextEx(font, "DEPARTMENT OF DEFENSE - C2 TERMINAL", { 50.f, 50.f }, 24, 0, mainCol);
    DrawTextEx(font, "UNAUTHORIZED ACCESS IS STRICTLY PROHIBITED.", { 50.f, 80.f }, 20, 0, errCol);

    if (loginState == 3) {
        DrawTextEx(font, "SYSTEM SECURED. TERMINAL LOCKOUT ACTIVE.", { 50.f, 200.f }, 24, 0, errCol);

        int minutes = (int)lockoutTimer / 60;
        int seconds = (int)lockoutTimer % 60;
        DrawTextEx(font, TextFormat("RETRY AVAILABLE IN: %02d:%02d", minutes, seconds), { 50.f, 240.f }, 24, 0, errCol);
        return;
    }

    DrawTextEx(font, "USERNAME:", { 50.f, 200.f }, 24, 0, mainCol);
    DrawTextEx(font, userBuffer.c_str(), { 180.f, 200.f }, 24, 0, WHITE);

    if (loginState >= 1) {
        DrawTextEx(font, "PASSWORD:", { 50.f, 240.f }, 24, 0, mainCol);
        string hiddenPass(passBuffer.length(), '*');
        DrawTextEx(font, hiddenPass.c_str(), { 180.f, 240.f }, 24, 0, WHITE);
    }

    if (loginState == 2) {
        DrawTextEx(font, "=== 2FA CRYPTOGRAPHIC HANDSHAKE REQUIRED ===", { 50.f, 300.f }, 20, 0, warnCol);
        DrawTextEx(font, TextFormat("SYN CHALLENGE: %d", synChallenge), { 50.f, 330.f }, 24, 0, mainCol);
        DrawTextEx(font, "ACK RESPONSE :", { 50.f, 360.f }, 24, 0, mainCol);
        DrawTextEx(font, challengeBuffer.c_str(), { 220.f, 360.f }, 24, 0, warnCol);
        DrawTextEx(font, "HINT: ACK = (SYN * 3) + 7", { 50.f, 420.f }, 18, 0, GRAY);
    }

    if (failedAttempts > 0) {
        DrawTextEx(font, TextFormat("WARNING: %d OF 3 ATTEMPTS FAILED", failedAttempts), { 50.f, 500.f }, 20, 0, errCol);
    }

    if (loginState < 3 && ((int)(GetTime() * 2) % 2 == 0)) {
        if (loginState == 0) DrawRectangle(180 + (int)MeasureTextEx(font, userBuffer.c_str(), 24, 0).x, 200, 15, 24, mainCol);
        if (loginState == 1) DrawRectangle(180 + (int)MeasureTextEx(font, string(passBuffer.length(), '*').c_str(), 24, 0).x, 240, 15, 24, mainCol);
        if (loginState == 2) DrawRectangle(220 + (int)MeasureTextEx(font, challengeBuffer.c_str(), 24, 0).x, 360, 15, 24, warnCol);
    }
}