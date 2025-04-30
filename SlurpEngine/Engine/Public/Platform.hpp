#pragma once

#if DEBUG
struct DEBUG_FileReadResult
{
    void* fileContents;
    uint32_t sizeBytes;
};
DEBUG_FileReadResult DEBUG_platformReadFile(const char* fileName);
bool DEBUG_platformWriteFile(const char* fileName, void* fileContents, uint32_t sizeBytes);
void DEBUG_platformFreeMemory(void* memory);
void DEBUG_platformTogglePause();
#endif

void platformVibrateController(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed);

void platformShutdown();