#pragma once
#include "TerminalScreen.h"

class SigintScreen : public TerminalScreen {
private:
    float targetAmp;
    float targetFreq;
    float targetPhase;
    float userAmp;
    float userFreq;
    float userPhase;
    bool isLocked;
    float lockTimer;

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};