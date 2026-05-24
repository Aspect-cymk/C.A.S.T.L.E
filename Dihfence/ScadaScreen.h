#pragma once
#include "TerminalScreen.h"
#include <vector>

struct PowerNode {
    Vector2 pos;
    bool isActive;
    bool isCorrupted;
};

class ScadaScreen : public TerminalScreen {
private:
    std::vector<PowerNode> gridNodes;
    std::vector<int> currentPath;
    bool powerRestored;
    float alertTimer;

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};