// bubik
  #define BLYNK_TEMPLATE_ID "TMPL49nioPtKW"
  #define BLYNK_TEMPLATE_NAME "AlarmESP"
  #define BLYNK_AUTH_TOKEN "V0JkcCiTXaYufsIcupFokOQ_BM51udyw"

  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <TimeLib.h>
  #include <WidgetRTC.h>
  #include <ArduinoOTA.h>
  #include <EEPROM.h>

  #include "config.h"
  #include "runtime_vars.h"


struct AlarmRecord {
  byte day;
  byte month;
  byte hour;
  byte minute;
  byte second;
  byte reserved;
};


// setup, boot, pinmode, setup Blynk, wifi, timer, RTC
void setup() {
  Serial.begin(115200);
  Serial.end();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // i hope it turns it off
  //pinMode(ledPin, OUTPUT);
  //digitalWrite(ledPin, LOW);
  digitalWrite(buzzPin, LOW);

  for(int i = 0; i < 7; i++) {
    pinMode(segments[i], OUTPUT);
  } 
  clearDisplay();


  EEPROM.begin(512); // EEPROM init - size: 512
  loadAlarmCounters();
  updateDisplayOnChange();

  
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connecting to Blynk...");
  timer.setInterval(1000L, checkAndNotify);
  timer.setInterval(5000L, hourlyResetCheck);
  timer.setInterval(50000, checkLoopHealth);

  startBeep(500);
  rtc.begin();  // Uruchomienie zegara RTC
  Blynk.syncAll();  // Synchronizacja wszystkich pinów i czasu
  Serial.println("Synchronizing...");
  Serial.print("Starting RTC ");
  startBeep(500);
  // Pobieranie aktualnego czasu
  int currentHour = hour();
  int currentMinute = minute();
  int currentSecond = second();
  int currentDay = day();
  int currentMonth = month();
  int currentYear = year();

  // Wypisywanie czasu na Serial Monitor
  Serial.print("Current Time: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.print(currentMinute);
  Serial.print(":");
  Serial.print(currentSecond);
  Serial.print(" ");
  Serial.print(currentDay);
  Serial.print("/");
  Serial.print(currentMonth);
  Serial.print("/");
  Serial.println(currentYear);

  Serial.println("Initializing Over-The-Air connection...");
  startBeep(500);

  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA upload");
    startBeep(500);

  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd OTA upload");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setPassword((const char *)NULL); // brak hasła
  ArduinoOTA.begin();

  Serial.println("OTA ready");
  startBeep(500);

  startupDone();
}

// loop, ArduinoOTA.handle, Blynk, timer, wifi watchdog
void loop() {
  ArduinoOTA.handle(); // Obsługa aktualizacji OTA
  Blynk.run();
  timer.run();
  updateBeep();
  if (millis() - lastWiFiCheck >= wifiCheckInterval) {
    lastWiFiCheck = millis();
    checkWiFiConnection();
  }
}

// Zapis danych liczników do  EEPROM
void saveAlarmCounters() {
  EEPROM.write(0, alarmPD); // zapisuje alarmPD pod adresem 0
  EEPROM.write(1, alarmPH); // zapisuje alarmPH pod adresem 1
  EEPROM.commit(); // zatwierdzenie zmian
}

// Odczyt danych liczników z EEPROM
void loadAlarmCounters() {
  alarmPD = EEPROM.read(0);
  alarmPH = EEPROM.read(1);
}

// Zapis danych alarmów do EEPROM
void saveAlarmToHistory() {
  AlarmRecord rec;
  rec.day = day();
  rec.month = month();
  rec.hour = hour();
  rec.minute = minute();
  rec.second = second();
  rec.reserved = 0;

  int index = EEPROM.read(EEPROM_HISTORY_INDEX_ADDR);
  if(index >= EEPROM_HISTORY_MAX_RECORDS) index = 0;

  int addr = EEPROM_HISTORY_START + index * EEPROM_HISTORY_RECORD_SIZE;

  EEPROM.write(addr, rec.day);
  EEPROM.write(addr + 1, rec.month);
  EEPROM.write(addr + 2, rec.hour);
  EEPROM.write(addr + 3, rec.minute);
  EEPROM.write(addr + 4, rec.second);
  EEPROM.write(addr + 5, rec.reserved);

  index = (index + 1) % EEPROM_HISTORY_MAX_RECORDS;
  EEPROM.write(EEPROM_HISTORY_INDEX_ADDR, index);
  EEPROM.commit();
}

// Odczyt danych alarmów z EEPROM
void printAlarmHistory() {
  terminal.println("== Historia alarmów ==");
  int index = EEPROM.read(EEPROM_HISTORY_INDEX_ADDR);

  for(int i = 0; i < EEPROM_HISTORY_MAX_RECORDS; i++) {
    int realIndex = (index + i + 1) % EEPROM_HISTORY_MAX_RECORDS;
    int addr = EEPROM_HISTORY_START + realIndex * EEPROM_HISTORY_RECORD_SIZE;

    byte d = EEPROM.read(addr);
    byte m = EEPROM.read(addr + 1);
    byte h = EEPROM.read(addr + 2);
    byte min = EEPROM.read(addr + 3);
    byte sec = EEPROM.read(addr + 4);

    if (d > 0 && d <= 31 && m > 0 && m <= 12 && h < 24 && min < 60 && sec < 60) {
      terminal.printf("%02d.%02d %02d:%02d:%02d\n", d, m, h, min, sec);
    }
  }
  terminal.flush();
}

//WiFi Watchdog
void checkWiFiConnection() {
  if(serviceMode && terminalWatchdog) terminal.println("[WiFi Watchdog] Working...");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi Watchdog] Połączenie utracone. Próba ponownego połączenia...");
    if(serviceMode && terminalWatchdog) {
      terminal.println("[WiFi Watchdog] Połączenie utracone. Próba ponownego połączenia...");
      terminal.flush();
    }

    WiFi.disconnect();
    WiFi.begin(ssid, pass);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime <10000) {
      delay(500);
      Serial.print(".");
    }

    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[WiFi Watchdog] Połączenie WiFi zostało przywrócone.");
      if(serviceMode && terminalWatchdog) {
        terminal.println("[WiFi Watchdog] Połączenie WiFi zostało przywrócone.");
        terminal.flush();
      }
    } else {
      Serial.println("\n[WiFi Watchdog] Nie udało się połączyć z WiFi.");
      if(serviceMode && terminalWatchdog) {
        terminal.println("[WiFi Watchdog] Nie udało się połączyć z WiFi.");
        terminal.flush();
      }
    }
  }
}

// Funkcja mierząca – zwraca odległość od przeszkody w cm (lub -1 przy błędzie)
float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) { 
    Serial.println("No sensor answer. Ignoring read.");
    return -1; // Czujnik nie odpowiedział
  } else {
    return (duration * 0.0343) / 2;
  }
}

// Główna funkcja kontrolująca alarm – wywoływana w pętli głównej
void checkAndNotify() {
  lastCheckTime = millis();
  if (!isArmed) {
    handleDisarmed();
    return;
  }
  
  if(serviceMode && terminalWatchdog) {
    terminal.println("[Sensor Watchdog] Working...");
  }

  distance = measureDistance();

  if(!handleSensorRead()) return;

  logSensorDistance();

  bool currentAlarmState = (distance > alarmTreshold &&  distance > 0);

  if(currentAlarmState != lastAlarmState) {
    handleAlarmStateChange(currentAlarmState);
  }
  printStatus(currentAlarmState);
}

// Obsługuje rozbrojony alarm - zeruje stan i wyłącza buzzer
void handleDisarmed() {
  printStatus("disarmed");

  if(lastAlarmState) {
  lastAlarmState = false;
  Blynk.virtualWrite(V0, 0);
  }

  digitalWrite(buzzPin, LOW);
}

// Obsługuje pojedyńczy odczyt z czujnika - zarządza błedami i restartem
bool handleSensorRead() {
  if(distance < 0) {
    sensorFailCount++;
    logSensorFailure();

    if(sensorFailCount >= 5) {
      Serial.println("[Sensor Watchdog] No response. Restarting...");
      if(serviceMode && terminalWatchdog) {
        terminal.println("[Sensor Watchdog] No response. Restarting...");
      }
      ESP.restart();
    } 
    return false;
  }
  sensorFailCount = 0;
  return true;
}

// Loguje błąd odczytu czujnika - tylko na Serial i terminal, jeśli aktywne
void logSensorFailure() {
  String msg = "[Sensor Watchdog] Error on sensor read. Ignoring input. [";
  msg += sensorFailCount;
  msg += "]";

  Serial.println(msg);
  if(serviceMode && sensorLog) terminal.println(msg);
  if(serviceMode && terminalWatchdog) terminal.println(msg);
}

// Loguje ostatni pomiar z czujnika - jeśli aktywne logowanie w trybie serwisowym
void logSensorDistance() {
  if(serviceMode && sensorLog) {
    terminal.print("[Sensor read] ");
    terminal.println(distance);
  }
}

// Obsługuje zmianę stanu alarmu - deleguje do funkcji zależnych od stanu
void handleAlarmStateChange(bool currentState) {
  lastAlarmState = currentState;

  if(currentState) {
    handleAlarmTriggered();
  } else {
    handleAlarmCleared();
  }
}

// Obsługuje wykrycie alarmu - zwiększ liczniki, zapisuje ich stan i aktywuje sygnalizację
void handleAlarmTriggered() {
  alarmPD++;
  alarmPH++;
  saveAlarmCounters();
  saveAlarmToHistory();
  updateDisplayOnChange();

  Blynk.virtualWrite(V2, alarmPD);
  Blynk.virtualWrite(V0, 1);
  Serial.println("Blynk.virtualWrite(V0, 1)");

  int h = hour();
  bool nightMode = nightModeEnabled && (
  (nmStart <= nmEnd) ? (h >= nmStart && h < nmEnd)  // ex. 8–18
                     : (h >= nmStart || h < nmEnd)  // ex. 22–6
  );

  for(int i = 0; i < 10; i++) {
    if(nightMode) {
      digitalWrite(buzzPin, HIGH);
      delay(1);
      digitalWrite(buzzPin, LOW);
      delay(100);
    } else {
      digitalWrite(buzzPin, HIGH);
      delay(100);
      digitalWrite(buzzPin, LOW);
      delay(100);
    }
  }
}

// Obsługuje zanik alarmu - wyłącza buzzer i resetuje flagi
void handleAlarmCleared() {
  Blynk.virtualWrite(V0, 0);
  Serial.println("Blynk.virtualWrite(V0, 0)");
  digitalWrite(buzzPin, LOW);
}

// Loop watchdog
void checkLoopHealth() {
  if(serviceMode && terminalWatchdog) {
    terminal.println("[Loop Watchdog] Working...");
  }
  if (millis() - lastCheckTime > 60000) { // brak działania przez 60 sekund
    Serial.println("[Loop Watchdog] checkAndNotify softlock. Restarting...");
    ESP.restart();
  }
}

// Odbieranie statusu uzbrojenia z aplikacji
BLYNK_WRITE(V1) {
  int val = param.asInt();
  isArmed = (val == 1);
}
// Resetowanie alarmPH i alarmPD
BLYNK_WRITE(V3) {
  alarmPD = 0;
  alarmPH = 0;
  saveAlarmCounters();
  updateDisplayOnChange();
  Blynk.virtualWrite(V2, alarmPD);
}

// OBSŁUGA TERMINALA / TRYBU SERWISOWEGO
BLYNK_WRITE(V4) {
  terminalCommand = param.asStr();
  terminalCommand.toLowerCase();
  if(terminalCommand == "start") {
    serviceMode = true;
    confirmSound();
    terminal.println("Tryb serwisowy włączony");
  } else if(terminalCommand == "stop") {
    serviceMode = false;
    terminal.println("Tryb serwisowy wyłączony");
    exitSound();
  } else if(serviceMode) {
    if(terminalCommand == "reset") {
      alarmPH = 0;
      alarmPD = 0;
      saveAlarmCounters();
      terminal.println("Liczniki wyzerowane.");
      updateDisplayOnChange();
    } else if(terminalCommand == "help") {
        terminal.println("=== Dostępne komendy ===");
        terminal.println("help                - Pokazuje tę listę komend");
        terminal.println("simulate [n]        - Symuluje n alarmów (domyślnie 1)");
        terminal.println("calibrate           - Kalibruje czujnik (ustawia dystans referencyjny)");
        terminal.println("reset               - Resetuje stan licznika wszystkich zmiennych");
        terminal.println("status              - Pokazuje status (alarmPH, alarmPD, dystans");
        terminal.println("switch watchdog     - Zmienia stan wyświetlania logów watchdoga");
        terminal.println("nightmode on | off  - Włącza i wyłącza tryb nocny");
        terminal.println("=========================");
        terminal.flush();
    } else if(terminalCommand.startsWith("simulate")) {
      int iterations = 1; // domyślnie
      int spaceIndex = terminalCommand.indexOf(' ');
      if(spaceIndex > 0) {
        String arg = terminalCommand.substring(spaceIndex + 1);
        iterations = arg.toInt(); //jeśli nie, iterations == 0
        if(iterations <= 0) iterations = 1;
      }
      for (int i = 0; i < iterations; i++) {
        handleAlarmTriggered();
      }

      terminal.print("Symulacja alarmu wykonana ");
      terminal.print(iterations);
      terminal.println(" raz(y)");
      exitSound();
    } else if(terminalCommand == "calibrate") {
      confirmSound();
      long measured = measureDistance();
      if(measured > 0) {
        alarmTreshold = max(measured + 2, 10L);
        terminal.print("Nowy próg alarmu: ");
        terminal.print(alarmTreshold);
        terminal.println(" cm.");
      } else {
        terminal.println("Błąd pomiaru. Kalibracja anulowana.");
      }

    } else if(terminalCommand == "status") {
      terminal.println("=== SYSTEM STATUS ===");
      terminal.print("Alarmy dzisiaj: ");
      terminal.println(alarmPD);
      terminal.print("Alarmy w tej godzinie: ");
      terminal.println(alarmPH);
      terminal.print("Ostatni dystans: ");
      if(distance == 0) terminal.println("rozbrojony");
      else {
        terminal.print(distance);
        terminal.println(" cm");
      }
      terminal.println("=====================");
      terminal.flush();
    } else if(terminalCommand == "switch watchdog") {
      if(terminalWatchdog) terminal.println("[WiFi Watchdog] logi wyłączone.");
      else if(!terminalWatchdog) terminal.println("[WiFi Watchdog] logi włączone.");
      terminalWatchdog = !terminalWatchdog;
    } else if (terminalCommand.startsWith("nightmode")) {
      String arg = terminalCommand.substring(9);
      arg.trim();

      if (arg == "on") {
        nightModeEnabled = true;
        confirmSound();
        terminal.println("Tryb nocny włączony.");
      } else if (arg == "off") {
        nightModeEnabled = false;
        terminal.println("Tryb nocny wyłączony.");
        exitSound();
      } else {
        terminal.println("Użycie: nightmode on | off");
      }
    } else if(terminalCommand.startsWith("sensor log")) {
      String arg = terminalCommand.substring(10);
      arg.trim();

      if(arg == "on") {
        sensorLog = true;
        confirmSound();
        terminal.println("[Sensor log] on");
      } else if(arg == "off") {
        sensorLog = false;
        exitSound();
        terminal.println("[Sensor log] off");
      } else {
        terminal.println("Użycie: sensor log on | off");
        terminal.print("Stan: ");
        terminal.println(sensorLog);
      }
    } else if(terminalCommand == "alarm history") {
      printAlarmHistory();
    } else if(terminalCommand.startsWith("nmset")) {
      String args = terminalCommand.substring(5);
      args.trim();

      int spaceIdx = args.indexOf(" ");
      if(spaceIdx == -1) {
        terminal.println("Użycie: nmset <start_hour> <end_hour>");
        return;
      }

      String startStr = args.substring(0, spaceIdx);
      String endStr = args.substring(spaceIdx + 1);
      startStr.trim();
      endStr.trim();

      int startHour = startStr.toInt();
      int endHour = endStr.toInt();

      if((startHour >= 0 && startHour <= 23) && (endHour >= 0 && endHour <= 23)) {
        nmStart = startHour;
        nmEnd = endHour;
        terminal.print("Tryb nocny: ");
        terminal.print(nmStart);
        terminal.print(":00 - ");
        terminal.print(nmEnd);
        terminal.println(":00");
      } else {
        terminal.println("Błąd: godziny muszą być liczbami z zakresu 0-23");
      }
      terminal.flush();
    } else {
      terminal.println("Nieznana komenda.");
    }
  } else {
    terminal.println("Wpisz 'start' aby wejść w tryb serwisowy.");
  }

  terminal.flush();
}

// Obsługa połączenia z Blynk, synchronizacja stanu uzbrojenia
BLYNK_CONNECTED() {
  Serial.println("Getting last arming state...");
  Blynk.syncVirtual(V1); // Pobrać ostatni stan uzbrojenia po połączeniu
}

// Debugująca funkcja Serial.print(...)
void printStatus(bool currentAlarmState) {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Armed: ");
  Serial.print(isArmed ? "YES" : "NO");
  Serial.print(" | Alarm: ");
  Serial.println(currentAlarmState ? "ON" : "OFF");
}

// Jak nazwa mówi - zeruje wyświetlacz
void clearDisplay() { // Zerowanie wyświetlacza
  for(int i = 0; i <7; i++) {
    digitalWrite(segments[i], LOW); // wyłączenie segmentów
  }
}

// Podobnie, nazwa mówi - resetuje zmienną alarmPH
void resetAlarmPH() {
  if (alarmPH > 0) {
    alarmPH = 0;
    saveAlarmCounters();
    updateDisplayOnChange();
    confirmSound();
  }
}

// Wyświetlanie cyfry na wyświetlaczu segmentowym
void displayDigit(int digit) { 
  byte segmentsState = digits[digit]; // Pobiera odpowiednią cyfrę z tablicy.


  for (int i = 0; i <7; i++) {
    digitalWrite(segments[i], bitRead(segmentsState, 6 - i)); //Sterowanie segmentami 
  }
}

// Odświeża wyświetlacz segmentowy, kiedy zmieni się alarmPH
void updateDisplayOnChange() {
  if(alarmPH != lastAlarmPHDisplayed) {
    if(alarmPH > 9) {
      displayDigit(10); // Błąd E
    } else if(alarmPH == 0) {
      clearDisplay();
    } else {
      displayDigit(alarmPH);
    }
    lastAlarmPHDisplayed = alarmPH;
  }
}

// Sprawdza, czy jest równa godzina i wywołuje funkcje resetAlarmPH();
void hourlyResetCheck() {
  if (minute() <= 1 && hour() != lastResetHour) {
    lastResetHour = hour();
    resetAlarmPH();
    Serial.println("alarmPH został zresetowany o pełnej godzinie");
  }
}

//          ---     UX      ---
unsigned long lastToneTime = 0;
int toneCount = 0;
void startupDone() {
  if(toneCount < 4) {
    unsigned long currentMillis = millis();
    if(toneCount < 3) {
      if(currentMillis - lastToneTime >= (toneCount % 2 == 0 ? 120: 90)) {
        digitalWrite(buzzPin, toneCount % 2 == 0 ? HIGH : LOW);
        lastToneTime = currentMillis;
        if(toneCount %2 == 1) toneCount++;
      }
    } else {
      if(currentMillis - lastToneTime >= (toneCount == 3 ? 100 : 400)) {
        digitalWrite(buzzPin, HIGH);
        lastToneTime = currentMillis;
        toneCount++;
      }
    }
  }
}

void startBeep(int duration) {
  if(!isBeeping) {
    tone(buzzPin, 1000);
    beepStartTime = millis();
    beepDuration = duration;
    isBeeping = true;
  }
}

void updateBeep() {
  if(isBeeping && (millis() - beepStartTime >= beepDuration)) {
    noTone(buzzPin);
    isBeeping = false;
  }
}

void confirmSound() {
  tone(buzzPin, 1000);
  delay(100);
  noTone(buzzPin);

  delay(50);

  tone(buzzPin, 1200);
  delay(100);
  noTone(buzzPin);

  delay(50);

  tone(buzzPin, 1400);
  delay(100);
  noTone(buzzPin);
}

void exitSound() {
  tone(buzzPin, 1400);
  delay(100);
  noTone(buzzPin);
  
  delay(50);
  
  tone(buzzPin, 1200);
  delay(100);
  noTone(buzzPin);
  
  delay(50);
  
  tone(buzzPin, 1000);
  delay(100);
  noTone(buzzPin);
}



