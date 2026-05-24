#pragma once
#include <string>

// Custom serial driver using a void pointer to completely hide windows.h from Raylib
class serialib {
private:
    void* hSerial;
    bool isConnected;

public:
    serialib();
    ~serialib();

    char openDevice(const char* Device, const unsigned int Bauds);
    void closeDevice();
    int readString(char* receivedString, char finalChar, unsigned int maxNbBytes, const unsigned int timeOut_ms);
};