#pragma once
#include "raylib.h"

class TerminalScreen {
public:
    virtual ~TerminalScreen() = default;
    virtual void setup() = 0;
    virtual void runFrame(float dt, int& activeScreen) = 0;
    virtual void draw(Font font) = 0;
};