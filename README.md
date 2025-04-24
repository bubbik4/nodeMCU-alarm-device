# nodeMCU-alarm-devie
An alarm device controlled via Blynk app, supporting OTA updates, and featuring night mode, alarm counter, and much more


# Changelog

## Version 1.4.13 – 2024-04-19  
- **Added EEPROM storage** for `alarmPH` and `alarmPD` variables.  
- Also stores **data about the last 10 alarms**.

---

## Version 1.4.12 – 2025-04-19  
- **Code optimization for editability and modularity**.  
- The `checkAndNotify` function was split into:  
  - `handleDisarmed`  
  - `handleSensorRead`  
  - `logSensorFailure`  
  - `logSensorDistance`  
  - `handleAlarmStateChange`  
  - `handleAlarmTriggered`  
  - `handleAlarmCleared`

---

## Version 1.4.11 – 2025-04-19  
- **Separated logical variables and configuration constants** into dedicated files (`config.h`, `runtime_vars.h/.cpp`).  
- **Bug fixes**.

---

## Version 1.4.10 – 2025-04-17  
- **Added night mode**: disables the buzzer between 22:00 and 6:00.  
- Night mode can be **controlled via terminal**.

---

## Version 1.4.8  
- **Added buffer for storing sensor readings** from the last minute.

---

## Version 1.4.7  
- **Added loop watchdog** and **sensor watchdog**.

---

## Version 1.4.6  
- **Added Wi-Fi watchdog**.

---

## Version 1.4.5  
- **Added service mode and terminal interface**.  
- **Integrated terminal control** with the Blynk IoT app.

---

## Version 1.4.4  
- **Added terminal support** in the Blynk IoT app.  
- **Added service mode**.

---

## Version 1.4.3  
- **Fixed issue with random resetting of `alarmPH`**.  
- **Optimized display updates** – replaced `displayAlarmPH()` with `updateDisplayOnChange()`, which only updates when `alarmPH` changes.

---

## Versions 1.4.1 and 1.4.2  
- **Bug fixes**.  
- **Added boot beeps** to indicate successful startup sequence.

---

## Version 1.4  
- **Added support for ArduinoOTA** (Over-the-Air updates).

---

## Version 1.3  
- **Added support for RTC (Real-Time Clock)**, syncing with Blynk.  
- **Added TimeLib.h** and **WidgetRTC.h** libraries.

---

## Version 1.2.4  
- **Added sensor sanity check** to verify proper operation.

---

## Version 1.2.3  
- **Bug fixes**.

---

## Version 1.2.2  
- **Added energy-saving mode**: display is turned off when `alarmPH` is 0.  
- **Added buffer overflow protection**: when `alarmPH` exceeds 9, display shows the letter "E".

---

## Version 1.2.1  
- **Implemented `alarmPH` variable** and its display on the 7-segment screen.  
- `alarmPH` resets every hour.

---

## Version 1.2  
- **Added support for 7-segment display**.

---

## Version 1.1  
- **Removed unused heartbeat function**, replaced with Blynk notifications.

---
