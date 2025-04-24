#ifndef RUNTIME_VARS_H
#define RUNTIME_VARS_H
#pragma once
#include <Arduino.h>


// State vars
extern float distance;
extern bool lastAlarmState;
extern bool isArmed;
extern bool nightModeEnabled;
extern bool isBeeping;
extern unsigned long beepStartTime;
extern int beepDuration;

// Alarm counters
extern int alarmPD;
extern int alarmPH;
extern int lastResetHour;
extern int lastAlarmPHDisplayed;

// Diagnostics
extern int sensorFailCount;

// Buffer
// extern unsigned int distanceBuffer[];
// extern int distanceIndex;
// extern bool bufferFilled;

// Service
extern bool serviceMode;
extern bool terminalWatchdog;
extern bool sensorLog;
extern String terminalCommand;

// Timers
extern unsigned long lastWiFiCheck;
extern unsigned long lastCheckTime;

#endif