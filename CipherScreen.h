#pragma once
#include "TerminalScreen.h"
#include <string>

class CipherScreen : public TerminalScreen {
private:
    int cipherType;
    std::string inputBuffer;
    std::string outputBuffer;
    bool showCheatSheet;

    bool isValidChar(char c);
    char decodeMorse(std::string chunk);
    char decodeBinary(std::string chunk);
    char decodeHex(std::string chunk);
    void processTranslation();
    std::string wrapText(std::string text, int lineLimit);

public:
    void setup() override;
    void runFrame(float dt, int& activeScreen) override;
    void draw(Font font) override;
};