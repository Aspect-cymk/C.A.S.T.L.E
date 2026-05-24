#pragma once
#include "TerminalScreen.h"
#include <vector>
#include <string>

class serialib;

struct PastPoint {
    Vector2 pos;
    float age;
};

struct TargetData {
    int id;
    Vector2 pos;
    float dist;
    float angle;
    float speed;
    bool isApproaching;
    std::vector<PastPoint> trail;
};

class RadarScreen : public TerminalScreen {
private:
    serialib* hardware;
    bool hasHardware;
    float sweepAngle;
    std::vector<TargetData> bogeys;

    int nextIdTag;
    bool geofenceAlarm;
    float ringPulseTimer;

public:
    RadarScreen();
    ~RadarScreen();

    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};