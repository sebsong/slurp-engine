#pragma once

void* platformReadFile(const char* fileName, void* outBuffer);

void platformVibrateController(int controllerIdx, float leftMotorSpeed, float rightMotorSpeed);

void platformShutdown();