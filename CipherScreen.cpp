#include "CipherScreen.h"
#include <cctype>

using namespace std;

void CipherScreen::setup() {
    cipherType = 0;
    showCheatSheet = false;
    inputBuffer = "";
    outputBuffer = "";
}

string CipherScreen::wrapText(string text, int lineLimit) {
    string wrapped = "";
    int currentLen = 0;
    for (char c : text) {
        wrapped += c;
        currentLen++;
        if (currentLen >= lineLimit && c == ' ') {
            wrapped += '\n';
            currentLen = 0;
        }
        else if (currentLen >= lineLimit + 10) {
            wrapped += "-\n";
            currentLen = 0;
        }
    }
    return wrapped;
}

bool CipherScreen::isValidChar(char c) {
    if (cipherType == 1) return (c == '.' || c == '-' || c == ' ' || c == '/');
    if (cipherType == 2) return (c == '0' || c == '1' || c == ' ' || c == '/');
    if (cipherType == 3) return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || c == ' ' || c == '/');
    if (cipherType == 4 || cipherType == 5) return ((c >= 'A' && c <= 'Z') || c == ' ');
    return false;
}

char CipherScreen::decodeMorse(string chunk) {
    string morseDict[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--",
        "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
        "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
    };
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < 36; i++) {
        if (morseDict[i] == chunk) return letters[i];
    }
    return '?';
}

char CipherScreen::decodeBinary(string chunk) {
    int val = 0;
    for (char bit : chunk) val = (val * 2) + (bit - '0');
    return (char)val;
}

char CipherScreen::decodeHex(string chunk) {
    int val = 0;
    for (char c : chunk) {
        int num = 0;
        if (c >= '0' && c <= '9') num = c - '0';
        else if (c >= 'A' && c <= 'F') num = (c - 'A') + 10;
        val = (val * 16) + num;
    }
    return (char)val;
}

void CipherScreen::processTranslation() {
    outputBuffer = "";
    string word = "";
    inputBuffer += " ";

    for (char c : inputBuffer) {
        if (c == ' ' || c == '/') {
            if (word != "") {
                if (cipherType == 1) outputBuffer += decodeMorse(word);
                else if (cipherType == 2) outputBuffer += decodeBinary(word);
                else if (cipherType == 3) outputBuffer += decodeHex(word);
                word = "";
            }
            if (c == '/') outputBuffer += " ";
            if ((cipherType == 4 || cipherType == 5) && c == ' ') outputBuffer += " ";
        }
        else {
            if (cipherType == 4) outputBuffer += (char)(c - 3 < 'A' ? c + 26 - 3 : c - 3);
            else if (cipherType == 5) outputBuffer += (char)('Z' - (c - 'A'));
            else word += c;
        }
    }
    inputBuffer.pop_back();
}

void CipherScreen::runFrame(float dt, int& activeScreen) {
    if (cipherType == 0) {
        if (IsKeyPressed(KEY_ONE)) cipherType = 1;
        else if (IsKeyPressed(KEY_TWO)) cipherType = 2;
        else if (IsKeyPressed(KEY_THREE)) cipherType = 3;
        else if (IsKeyPressed(KEY_FOUR)) cipherType = 4;
        else if (IsKeyPressed(KEY_FIVE)) cipherType = 5;
        else if (IsKeyPressed(KEY_ESCAPE)) activeScreen = 0;
    }
    else {
        int key = GetCharPressed();
        while (key > 0) {
            char typed = toupper((char)key);
            if (isValidChar(typed)) inputBuffer += typed;
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputBuffer.length() > 0) inputBuffer.pop_back();
        if (IsKeyPressed(KEY_ENTER)) { processTranslation(); inputBuffer = ""; }
        if (IsKeyPressed(KEY_TAB)) showCheatSheet = !showCheatSheet;

        if (IsKeyPressed(KEY_ESCAPE)) {
            cipherType = 0;
            showCheatSheet = false;
            inputBuffer = "";
            outputBuffer = "";
        }
    }
}

void CipherScreen::draw(Font font) {
    Color textCol = { 100, 200, 255, 255 };
    if (cipherType == 0) {
        DrawTextEx(font, "=== COMSEC PROTOCOLS ===\n\n[1] MORSE\n[2] BINARY\n[3] HEX\n[4] CAESAR\n[5] ATBASH", { 50.f, 50.f }, 24, 0, textCol);
        DrawTextEx(font, "KEYBINDS: [1-5] Select [Esc] Back", { 20.f, 550.f }, 18, 0, GRAY);
    }
    else {
        string names[] = { "", "MORSE", "BINARY", "HEXADECIMAL", "CAESAR", "ATBASH" };
        string wrappedIn = wrapText(inputBuffer, 40);
        string wrappedOut = wrapText(outputBuffer, 40);
        string ui = "--- " + names[cipherType] + " ---\n\n INPUT:\n> " + wrappedIn + "\n\n DECRYPTED:\n> " + wrappedOut;

        DrawTextEx(font, ui.c_str(), { 50.f, 50.f }, 24, 0, textCol);
        DrawTextEx(font, "KEYBINDS: [Type] Input [Enter] Decrypt [Tab] Dict [Esc] Back", { 20.f, 550.f }, 18, 0, GRAY);

        string helperText = "";
        if (cipherType == 1) helperText = "INPUT: [.] [-] [Space] | Word Break: [/]";
        else if (cipherType == 2) helperText = "INPUT: [0] [1] [Space] | Word Break: [/]";
        else if (cipherType == 3) helperText = "INPUT: [0-9] [A-F] [Space] | Word Break: [/]";
        else if (cipherType == 4 || cipherType == 5) helperText = "INPUT: [A-Z] [Space]";

        DrawTextEx(font, helperText.c_str(), { 50.f, 400.f }, 18, 0, YELLOW);

        if (showCheatSheet) {
            DrawRectangle(450, 50, 520, 480, Fade(BLACK, 0.95f));
            DrawRectangleLines(450, 50, 520, 480, textCol);
            DrawTextEx(font, "SYSTEM DICTIONARY OVERLAY", { 470.f, 60.f }, 20, 0, YELLOW);

            int yOffset = 100;
            int xOffset = 470;

            if (cipherType == 1) {
                const char* morse[] = { "A .-", "B -...", "C -.-.", "D -..", "E .", "F ..-.", "G --.", "H ....", "I ..", "J .---", "K -.-", "L .-..", "M --", "0 -----", "1 .----", "2 ..---", "3 ...--", "4 ....-" };
                const char* morse2[] = { "N -.", "O ---", "P .--.", "Q --.-", "R .-.", "S ...", "T -", "U ..-", "V ...-", "W .--", "X -..-", "Y -.--", "Z --..", "5 .....", "6 -....", "7 --...", "8 ---..", "9 ----." };
                for (int i = 0; i < 18; i++) {
                    DrawTextEx(font, morse[i], { (float)xOffset, (float)yOffset + (i * 20) }, 18, 0, textCol);
                    DrawTextEx(font, morse2[i], { (float)xOffset + 180, (float)yOffset + (i * 20) }, 18, 0, textCol);
                }
            }
            else if (cipherType == 2 || cipherType == 3) {
                DrawTextEx(font, "ASCII CHARACTERS (HEX | BINARY)", { (float)xOffset, (float)yOffset - 25 }, 18, 0, textCol);
                const char* hexBin1[] = {
                    "A: 41 | 01000001", "B: 42 | 01000010", "C: 43 | 01000011", "D: 44 | 01000100",
                    "E: 45 | 01000101", "F: 46 | 01000110", "G: 47 | 01000111", "H: 48 | 01001000",
                    "I: 49 | 01001001", "J: 4A | 01001010", "K: 4B | 01001011", "L: 4C | 01001100", "M: 4D | 01001101"
                };
                const char* hexBin2[] = {
                    "N: 4E | 01001110", "O: 4F | 01001111", "P: 50 | 01010000", "Q: 51 | 01010001",
                    "R: 52 | 01010010", "S: 53 | 01010011", "T: 54 | 01010100", "U: 55 | 01010101",
                    "V: 56 | 01010110", "W: 57 | 01010111", "X: 58 | 01011000", "Y: 59 | 01011001", "Z: 5A | 01011010"
                };
                for (int i = 0; i < 13; i++) {
                    DrawTextEx(font, hexBin1[i], { (float)xOffset, (float)yOffset + (i * 25) }, 18, 0, textCol);
                    DrawTextEx(font, hexBin2[i], { (float)xOffset + 220, (float)yOffset + (i * 25) }, 18, 0, textCol);
                }
                DrawTextEx(font, "NOTE: Numbers 0-9 are Hex 30-39", { (float)xOffset, (float)yOffset + 340 }, 18, 0, YELLOW);
            }
            else {
                DrawTextEx(font, "CAESAR: Shifts letters backward by 3.\n(e.g., D becomes A, A loops to X)\n\nATBASH: Reverses the alphabet.\n(e.g., A becomes Z, B becomes Y)", { (float)xOffset, (float)yOffset }, 18, 0, textCol);
            }
        }
    }
}