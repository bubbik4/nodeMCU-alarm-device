#ifndef RUNTIME_VARS_H
#define RUNTIME_VARS_H
#pragma once
#include <Arduino.h>


// State vars
extern float distance;
extern bool lastAlarmState;
extern bool isArmed;
extern bool isBeeping;
extern unsigned long beepStartTime;
extern int beepDuration;

// Alarm counters
extern int alarmPD;
extern int alarmPH;
extern int lastResetHour;
extern int lastAlarmPHDisplayed;

// Nightmode handling
extern bool nightModeEnabled;
extern int nmStart;
extern int nmEnd;

// Diagnostics
extern int sensorFailCount;

// Service
extern bool serviceMode;
extern bool terminalWatchdog;
extern bool sensorLog;
extern String terminalCommand;

// Timers
extern unsigned long lastWiFiCheck;
extern unsigned long lastCheckTime;

#endif