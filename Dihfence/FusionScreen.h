#pragma once
#include "TerminalScreen.h"
#include <vector>

class FusionScreen : public TerminalScreen {
private:
    float globalThreat;
    float radarRisk;
    float scadaRisk;
    float sigintRisk;
    float authFailures;
    float cpuLoad;
    float memUsage;
    float coreTemp;
    std::vector<float> threatHistory;
    float updateTimer;

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};