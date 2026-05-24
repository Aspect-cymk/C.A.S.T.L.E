#include "RadarScreen.h"
#include "serialib.h"
#include <cmath>

using namespace std;

RadarScreen::RadarScreen() {
    hardware = new serialib();
}

RadarScreen::~RadarScreen() {
    if (hasHardware) hardware->closeDevice();
    delete hardware;
}

void RadarScreen::setup() {
    sweepAngle = 0.f;
    hasHardware = false;
    nextIdTag = 1000;
    geofenceAlarm = false;
    ringPulseTimer = 0.f;
    bogeys.clear();
}

void RadarScreen::runFrame(float dt, int& activeScreen) {
    if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;

    if (IsKeyPressed(KEY_H) && !hasHardware) {
        if (hardware->openDevice("COM3", 9600) == 1) hasHardware = true;
    }

    ringPulseTimer += dt * 2.0f;
    geofenceAlarm = false;

    if (!hasHardware) {
        sweepAngle = (float)sin(GetTime() * 2.0f) * 75.0f;
        static float simTimer = 0.f;
        simTimer += dt;
        if (simTimer > 1.5f) {
            simTimer = 0.f;
            TargetData t;
            t.id = nextIdTag++;
            t.dist = 300.f;
            t.angle = (float)GetRandomValue(-75, 75) * (3.14159f / 180.f);
            t.speed = (float)GetRandomValue(5, 25);
            t.isApproaching = true;
            bogeys.push_back(t);
        }
    }
    else {
        char buffer[32];
        if (hardware->readString(buffer, '\n', 32, 10) > 0) {
            string data(buffer);
            size_t dot = data.find('.');
            if (dot != string::npos) {
                int readAngle = stoi(data.substr(0, dot));
                int readDist = stoi(data.substr(dot + 1));
                sweepAngle = readAngle - 90.f;

                if (readDist > 2 && readDist < 40) {
                    float newDist = readDist * 5.f;
                    float newAngle = readAngle * (3.14159f / 180.f);

                    bool foundMatch = false;
                    for (int i = 0; i < bogeys.size(); i++) {
                        float distDiff = abs(bogeys[i].dist - newDist);
                        float angDiff = abs(bogeys[i].angle - newAngle);

                        if (distDiff < 20.f && angDiff < 0.2f) {
                            bogeys[i].speed = (bogeys[i].dist - newDist) / dt;
                            bogeys[i].isApproaching = (bogeys[i].speed > 0);
                            bogeys[i].speed = abs(bogeys[i].speed);

                            bogeys[i].dist = newDist;
                            bogeys[i].angle = newAngle;
                            foundMatch = true;
                            break;
                        }
                    }

                    if (!foundMatch) {
                        TargetData t;
                        t.id = nextIdTag++;
                        t.dist = newDist;
                        t.angle = newAngle;
                        t.speed = 0.f;
                        t.isApproaching = true;
                        bogeys.push_back(t);
                    }
                }
            }
        }
    }

    for (int i = 0; i < bogeys.size(); i++) {
        PastPoint p;
        p.pos = bogeys[i].pos;
        p.age = 0.f;
        if (bogeys[i].trail.size() > 20) bogeys[i].trail.erase(bogeys[i].trail.begin());
        bogeys[i].trail.push_back(p);

        for (int j = 0; j < bogeys[i].trail.size(); j++) bogeys[i].trail[j].age += dt;

        if (!hasHardware) bogeys[i].dist -= (bogeys[i].speed * dt);

        bogeys[i].pos.x = 500.f + (float)cos(bogeys[i].angle) * bogeys[i].dist;
        bogeys[i].pos.y = 300.f - (float)sin(bogeys[i].angle) * bogeys[i].dist;

        if (bogeys[i].dist <= 100.f) geofenceAlarm = true;

        if (bogeys[i].dist <= 5.f) {
            bogeys.erase(bogeys.begin() + i);
            i--;
        }
    }
}

void RadarScreen::draw(Font font) {
    Color mainCol = { 100, 200, 255, 255 };
    Color alertCol = { 255, 50, 50, 255 };

    if (geofenceAlarm) DrawRectangle(0, 0, 1000, 600, Fade(RED, 0.15f));

    string hud = hasHardware ? "HARDWARE LINK ACTIVE\n" : "SIMULATION ONLY (PRESS 'H' TO LINK)\n";
    if (geofenceAlarm) hud += "\nWARNING: GEOFENCE BREACHED!";
    DrawTextEx(font, hud.c_str(), { 20.f, 20.f }, 20, 0, geofenceAlarm ? alertCol : mainCol);

    DrawTextEx(font, "ACTIVE TARGET LOG:", { 750.f, 20.f }, 20, 0, mainCol);
    int logY = 50;
    for (int i = 0; i < bogeys.size(); i++) {
        if (i > 15) break;
        string dir = bogeys[i].isApproaching ? "INBOUND" : "OUTBOUND";
        string entry = "ID-" + to_string(bogeys[i].id) + " | " + to_string((int)bogeys[i].speed) + " U/s [" + dir + "]";
        DrawTextEx(font, entry.c_str(), { 750.f, (float)logY }, 18, 0, (bogeys[i].dist < 100.f) ? alertCol : mainCol);
        logY += 20;
    }

    Vector2 center = { 500.f, 300.f };
    float pulse = ((float)sin(ringPulseTimer) + 1.0f) / 2.0f;

    DrawCircleLines((int)center.x, (int)center.y, 100, geofenceAlarm ? Fade(alertCol, 0.8f + (pulse * 0.2f)) : Fade(mainCol, 0.4f));
    DrawTextEx(font, "100m GEOFENCE", { center.x + 105.f, center.y }, 16, 0, geofenceAlarm ? alertCol : Fade(mainCol, 0.5f));
    DrawCircleLines((int)center.x, (int)center.y, 200, Fade(mainCol, 0.2f + (pulse * 0.1f)));
    DrawCircleLines((int)center.x, (int)center.y, 300, Fade(mainCol, 0.1f + (pulse * 0.1f)));

    DrawLine((int)center.x - 300, (int)center.y, (int)center.x + 300, (int)center.y, Fade(mainCol, 0.3f));
    DrawLine((int)center.x, (int)center.y - 300, (int)center.x, (int)center.y + 300, Fade(mainCol, 0.3f));

    Vector2 endPoint = { center.x + (float)cos(sweepAngle * (3.14159f / 180.f)) * 300.f, center.y + (float)sin(sweepAngle * (3.14159f / 180.f)) * 300.f };
    DrawLineEx(center, endPoint, 2.f, mainCol);

    for (int i = 0; i < bogeys.size(); i++) {
        for (int j = 0; j < bogeys[i].trail.size(); j++) {
            float fadeStr = 1.0f - (bogeys[i].trail[j].age / 2.0f);
            if (fadeStr < 0.f) fadeStr = 0.f;
            DrawCircleV(bogeys[i].trail[j].pos, 2.f, Fade(mainCol, fadeStr * 0.5f));
        }
        Color tCol = (bogeys[i].dist <= 100.f) ? alertCol : mainCol;
        DrawCircleV(bogeys[i].pos, 4.f, tCol);
        DrawTextEx(font, to_string(bogeys[i].id).c_str(), { bogeys[i].pos.x + 8.f, bogeys[i].pos.y - 8.f }, 16, 0, tCol);
    }
    DrawCircleV(center, 6.f, mainCol);
}