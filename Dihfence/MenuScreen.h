#pragma once
#include "TerminalScreen.h"

class MenuScreen : public TerminalScreen {
public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};