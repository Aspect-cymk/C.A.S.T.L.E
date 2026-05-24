#pragma once
#include "TerminalScreen.h"
#include <vector>

class UavMonitorScreen : public TerminalScreen {
private:
    std::vector<float> altitudeData;
    std::vector<float> velocityData;
    float updateTimer;
    float scrollOffset;

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};