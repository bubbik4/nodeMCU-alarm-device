# Changelog

## Wersja 1.4.13 - 19.04.2024
- **Dodano zapis EEPROM** dla zmiennych alarmPH, alarmPD. W pamięci przechowywane są również **dane na temat
ostatnich 10 alarmów**.

---

## Wersja 1.4.12 - 19.04.2025
- **Optymalizacja kodu pod względem edytowalności i modularności**. Podzielono funkcję **checkAndNotify** na:
**handleDisarmed**, **handleSensorRead**, **logSensorFailure**, **logSensorDistance**, **handleAlarmStateChange**,
**handleAlarmTriggered**, **handleAlarmCleared**.

---

## Wersja 1.4.11 - 19.04.2025
- **Utworzono osobne pliki zmiennych logicznych i stałych konfiguracyjnych** (config.h, runtime_vars.h/.cpp)
- **BFixy**.

---

## Wersja 1.4.10 - 17.04.2025
- **Połączono wszystkie pliki w jeden.**
- **Dodano tryb nocny**, który od 22:00 do 6:00 wyłącza buzzer. Tryb nocny można **kontrolować przez terminal**.

---

## Wersja 1.4.9
- **Przeniesiono logikę obsługi komend terminala** do zewnętrznego pliku `terminal_control.h`.

---

## Wersja 1.4.8
- **Dodano bufor do przechowywania ostatnich odczytów z czujnika** w ciągu ostatniej minuty.

---

## Wersja 1.4.7
- **Dodano Watchdog dla pętli (Loop Watchdog)** oraz **Watchdog dla czujnika** (Sensor Watchdog).

---

## Wersja 1.4.6
- **Dodano Watchdog dla Wi-Fi**.

---

## Wersja 1.4.5
- **Dodano tryb serwisowy i terminal**.
- **Zintegrowano obsługę terminala** z aplikacją Blynk IoT.

---

## Wersja 1.4.4
- **Dodano obsługę terminala** w aplikacji Blynk IoT.
- **Dodano tryb serwisowy**.

---

## Wersja 1.4.3
- **Naprawiono problem z losowym zerowaniem zmiennej `alarmPH`**.
- **Optymalizacja aktualizacji wyświetlacza** – stworzono nową funkcję `updateDisplayOnChange()`, która zastąpiła funkcję `displayAlarmPH()`, usuwając ciągłą aktualizację i sprawdzając tylko zmiany wartości `alarmPH`.

---

## Wersje 1.4.1 i 1.4.2
- **Poprawki błędów**.
- **Dodano dźwięki uruchamiania (boot-beeps)**, aby zapewnić prawidłowy przebieg procesu uruchamiania.

---

## Wersja 1.4
- **Dodano wsparcie dla ArduinoOTA** (Over-the-Air).

---

## Wersja 1.3
- **Dodano wsparcie dla RTC (Real-Time Clock)**, synchronizowanie z Blynk.
- **Dodano biblioteki TimeLib.h** oraz **WidgetRTC.h**.

---

## Wersja 1.2.4
- **Dodano sprawdzenie poprawności działania czujnika** (sanity check).

---

## Wersja 1.2.3
- **Poprawki błędów**.

---

## Wersja 1.2.2
- **Dodano tryb oszczędzania energii**: gdy `alarmPH` jest równe 0, wyświetlacz jest wyłączany.
- **Dodano ochronę przed przepełnieniem bufora**: gdy `alarmPH` przekroczy 9, wyświetlacz pokazuje literę "E".

---

## Wersja 1.2.1
- **Dodano funkcjonalność zmiennej `alarmPH`** oraz wyświetlanie jej na wyświetlaczu. `alarmPH` jest resetowany co godzinę.

---

## Wersja 1.2
- **Dodano wsparcie dla wyświetlacza 7-segmentowego**.

---

## Wersja 1.1
- **Usunięto nieużywaną funkcję heartbeat**, zastąpiono ją powiadomieniami Blynk.
