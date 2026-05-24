#pragma once
#include "TerminalScreen.h"
#include <string>

class HandshakeScreen : public TerminalScreen {
private:
    int authState;
    std::string targetCode;
    std::string currentInput;
    float timer;
    float maxTime;

    void generateCode();
    bool isValidKeystroke(char c);

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};