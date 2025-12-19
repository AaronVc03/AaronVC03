#include <Arduino.h>
#include <SerialCommand.h>
#include <EEPROMAnything.h>
#include <SoftwareSerial.h>

/* ===================== CONFIG ===================== */

#define BAUDRATE 9600

#define LED_PIN 3
#define BUTTON_PIN 2

#define BT_RX 11
#define BT_TX 12

/* ===================== SERIAL ===================== */

SoftwareSerial BTSerial(BT_RX, BT_TX);

SerialCommand sCmd(Serial);
SerialCommand sCmdBT(BTSerial);
SerialCommand* activeCmd;

/* ===================== STRUCT ===================== */

struct Params {
  unsigned long cycleTime;   // µs
  bool debug;
};

Params params;

/* ===================== VARS ===================== */

volatile bool run = false;
volatile unsigned long lastInterrupt = 0;
const unsigned long debounceTime = 200;

unsigned long previousCycle = 0;
bool ledState = false;

/* ===================== HELPERS ===================== */

void printBoth(String msg) {
  Serial.print(msg);
  BTSerial.print(msg);
}

void printlnBoth(String msg) {
  Serial.println(msg);
  BTSerial.println(msg);
}

/* ===================== COMMANDS ===================== */

void onSet() {
  char* param = activeCmd->next();
  char* value = activeCmd->next();
  if (!param || !value) return;

  if (strcmp(param, "cycle") == 0) {
    params.cycleTime = atol(value);
    printlnBoth("cycle updated");
  }

  EEPROM_writeAnything(0, params);
}

void onDebug() {
  char* value = activeCmd->next();
  if (!value) return;

  if (strcmp(value, "on") == 0) params.debug = true;
  if (strcmp(value, "off") == 0) params.debug = false;

  EEPROM_writeAnything(0, params);
}

void onRun() {
  run = true;
  printlnBoth("RUN");
}

void onStop() {
  run = false;
  digitalWrite(LED_PIN, LOW);
  printlnBoth("STOP");
}

void onUnknown(char* cmd) {
  printlnBoth("Unknown command");
}

/* ===================== INTERRUPT ===================== */

void buttonISR() {
  if (millis() - lastInterrupt > debounceTime) {
    run = !run;
    lastInterrupt = millis();
  }
}

/* ===================== SETUP ===================== */

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  Serial.begin(BAUDRATE);
  BTSerial.begin(BAUDRATE);

  EEPROM_readAnything(0, params);

  // defaults indien EEPROM leeg
  if (params.cycleTime == 0 || params.cycleTime > 2000000) {
    params.cycleTime = 500000;
    params.debug = false;
  }

  // USB commands
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("run", onRun);
  sCmd.addCommand("stop", onStop);
  sCmd.setDefaultHandler(onUnknown);

  // Bluetooth commands
  sCmdBT.addCommand("set", onSet);
  sCmdBT.addCommand("debug", onDebug);
  sCmdBT.addCommand("run", onRun);
  sCmdBT.addCommand("stop", onStop);
  sCmdBT.setDefaultHandler(onUnknown);

  printlnBoth("System ready");
}

/* ===================== LOOP ===================== */

void loop() {

  activeCmd = &sCmd;
  sCmd.readSerial();

  activeCmd = &sCmdBT;
  sCmdBT.readSerial();

  if (!run) return;

  unsigned long now = micros();
  if (now - previousCycle >= params.cycleTime) {
    previousCycle = now;

    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);

    if (params.debug) {
      printlnBoth("LED toggle");
    }
  }
}
