#pragma once
#include "TerminalScreen.h"
#include <string>

class LoginScreen : public TerminalScreen {
private:
    int loginState;
    std::string userBuffer;
    std::string passBuffer;
    std::string challengeBuffer;
    int failedAttempts;

    int synChallenge;
    int expectedAck;
    float lockoutTimer;

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};