#include "runtime_vars.h"
#include <Arduino.h>

float distance = 0;
bool lastAlarmState = false;
bool isArmed = true;
bool nightModeEnabled = true;
bool isBeeping = false;
unsigned long beepStartTime = 0;
int beepDuration = 0;

int alarmPD = 0;
int alarmPH = 0;
int lastResetHour = -1;
int lastAlarmPHDisplayed = -1;

int sensorFailCount = 0;

//const int DIST_BUFFER_SIZE = 30;
// unsigned int distanceBuffer[DIST_BUFFER_SIZE];
// int distanceIndex = 0;
// bool bufferFilled = false;

bool serviceMode = false;
bool terminalWatchdog = false;
bool sensorLog = false;
String terminalCommand = "";

unsigned long lastWiFiCheck = 0;
unsigned long lastCheckTime = 0;
