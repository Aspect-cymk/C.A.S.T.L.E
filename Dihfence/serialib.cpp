#include "serialib.h"
#include <windows.h> // Quarantined here so it doesn't break Raylib!
#include <iostream>

serialib::serialib() : hSerial(INVALID_HANDLE_VALUE), isConnected(false) {}

serialib::~serialib() {
    closeDevice();
}

char serialib::openDevice(const char* Device, const unsigned int Bauds) {
    // Windows requires the \\.\ prefix for COM ports higher than COM9, so we add it safely for all ports
    std::string portName = "\\\\.\\";
    portName += Device;

    // Open the hardware port and store it in our void pointer
    hSerial = (void*)CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) return -1;

    // Set the baud rate (must match the 9600 in your Arduino code)
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState((HANDLE)hSerial, &dcbSerialParams)) return -1;

    dcbSerialParams.BaudRate = Bauds;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState((HANDLE)hSerial, &dcbSerialParams)) return -1;

    // Set timeouts so the game doesn't freeze waiting for the Arduino
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts((HANDLE)hSerial, &timeouts);

    isConnected = true;
    return 1;
}

void serialib::closeDevice() {
    if (isConnected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle((HANDLE)hSerial);
        isConnected = false;
    }
}

int serialib::readString(char* receivedString, char finalChar, unsigned int maxNbBytes, const unsigned int timeOut_ms) {
    if (!isConnected) return -1;

    DWORD bytesRead;
    unsigned int index = 0;
    char tempChar;

    // Read bytes one by one until we hit the newline character (\n) from the Arduino
    while (index < maxNbBytes - 1) {
        if (ReadFile((HANDLE)hSerial, &tempChar, 1, &bytesRead, NULL) && bytesRead > 0) {
            receivedString[index] = tempChar;
            index++;
            if (tempChar == finalChar) break;
        }
        else {
            break; // Break on timeout so we don't freeze the screen
        }
    }

    receivedString[index] = '\0'; // Null-terminate the string
    return index;
}