#include "raylib.h"
#include "TerminalScreen.h"
#include "LoginScreen.h"
#include "MenuScreen.h"
#include "CipherScreen.h"
#include "RadarScreen.h"
#include "ScadaScreen.h"
#include "SigintScreen.h"
#include "HandshakeScreen.h"
#include "FusionScreen.h"
#include "UavMonitorScreen.h"

using namespace std;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1000, 600, "Command & Control Terminal");
    
    // OVERRIDE: Set custom castle icon for the window and taskbar
    Image windowIcon = LoadImage("cas.png"); 
    SetWindowIcon(windowIcon);
    UnloadImage(windowIcon);

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Font crtFont = LoadFontEx("VT323-Regular.ttf", 24, 0, 250);
    Shader retroShader = LoadShader(0, "crt.fs");
    RenderTexture2D screenBuffer = LoadRenderTexture(1000, 600);

    TerminalScreen* currentApp = new LoginScreen();
    currentApp->setup();

    int screenMode = -1;
    int lastMode = -1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        if (screenMode != lastMode) {
            delete currentApp;

            if (screenMode == -1) currentApp = new LoginScreen();
            else if (screenMode == 0) currentApp = new MenuScreen();
            else if (screenMode == 1) currentApp = new CipherScreen();
            else if (screenMode == 2) currentApp = new RadarScreen();
            else if (screenMode == 3) currentApp = new ScadaScreen();
            else if (screenMode == 4) currentApp = new SigintScreen();
            else if (screenMode == 5) currentApp = new HandshakeScreen();
            else if (screenMode == 6) currentApp = new FusionScreen();
            else if (screenMode == 7) currentApp = new UavMonitorScreen();

            currentApp->setup();
            lastMode = screenMode;
        }

        currentApp->runFrame(GetFrameTime(), screenMode);

        BeginTextureMode(screenBuffer);
        ClearBackground(Color{ 10, 15, 20, 255 });
        currentApp->draw(crtFont);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(retroShader);

        DrawTexturePro(
            screenBuffer.texture,
            { 0.0f, 0.0f, (float)screenBuffer.texture.width, -(float)screenBuffer.texture.height },
            { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0, 0 }, 0.0f, WHITE
        );

        EndShaderMode();
        EndDrawing();
    }

    delete currentApp;
    UnloadRenderTexture(screenBuffer);
    UnloadShader(retroShader);
    UnloadFont(crtFont);
    CloseWindow();
    return 0;
}