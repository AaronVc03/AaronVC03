#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>

#define SerialPort Serial
#define Baudrate 9600

#define MotorLeftForward 5
#define MotorLeftBackward 6
#define MotorRightForward 10
#define MotorRightBackward 9

#define RX_PIN 11  // Verbonden met TX van HC-05
#define TX_PIN 12  // Verbonden met RX van HC-05

SoftwareSerial BTSerial(RX_PIN, TX_PIN);  // RX, TX

const int Drukknop = 2; // interrupt
const int LED = 3;       // Led
const int IR = 13;       // SensorLED

volatile bool runState = false;
volatile unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50; 

SerialCommand sCmd(SerialPort);    // USB commands
SerialCommand sCmdBT(BTSerial);    // Bluetooth commands

bool run;                   
unsigned long previous, calculationTime;

const int sensor[] = {A7, A6, A5, A4, A3, A2, A1, A0};

// --- Motor debug toevoeging ---
int lastLeftPower = 0;
int lastRightPower = 0;
unsigned long lastMotorPrint = 0;


// Helper functies voor output naar beide poorten
void printBoth(String msg) {
    Serial.print(msg);
    BTSerial.print(msg);
}

void printlnBoth(String msg) {
    Serial.println(msg);
    BTSerial.println(msg);
}

struct param_t
{
  unsigned long cycleTime;
  int black[8];
  int white[8];
  float diff;
  float kp;
  int power;
  float ki;
  float kd;
} params;

int normalised[8];
float debugposition;
float output;
float error;
float iTerm = 0;
float lastErr;

void setup()
{
  pinMode(MotorLeftForward, OUTPUT);
  pinMode(MotorLeftBackward, OUTPUT);
  pinMode(MotorRightForward, OUTPUT);
  pinMode(MotorRightBackward, OUTPUT);

  SerialPort.begin(Baudrate);
  BTSerial.begin(Baudrate);

  // USB commands
  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.addCommand("run", onRun);
  sCmd.addCommand("stop", onStop);
  sCmd.setDefaultHandler(onUnknownCommand);

  // Bluetooth commands
  sCmdBT.addCommand("set", onSet);
  sCmdBT.addCommand("debug", onDebug);
  sCmdBT.addCommand("calibrate", onCalibrate);
  sCmdBT.addCommand("run", onRun);
  sCmdBT.addCommand("stop", onStop);
  sCmdBT.setDefaultHandler(onUnknownCommand);

  EEPROM_readAnything(0, params);
  printlnBoth("ready");

  pinMode(Drukknop, INPUT);
  attachInterrupt(digitalPinToInterrupt(Drukknop), Interrupt, RISING);

  pinMode(LED, OUTPUT);
  pinMode(IR, OUTPUT);
  digitalWrite(IR, HIGH);

  if (params.cycleTime == 0 || params.cycleTime == 4294967295UL) params.cycleTime = 1000;
  if (isnan(params.kp))  params.kp = 0.4;
  if (isnan(params.ki))  params.ki = 0.0;
  if (isnan(params.kd))  params.kd = 0.3;
  if (isnan(params.diff)) params.diff = 1.0;
  if (params.power <= 0 || params.power > 255) params.power = 150;
}

void loop()
{
  // --- Process USB en Bluetooth commands ---
  sCmd.readSerial();    // USB
  sCmdBT.readSerial();  // Bluetooth

  unsigned long current = micros();

  if (current - previous >= params.cycleTime)
  {
    previous = current;

    for (int i = 0; i < 8; i++)
    {
      normalised[i] = map(analogRead(sensor[i]), params.white[i], params.black[i], 0, 1000);
      normalised[i] = constrain(normalised[i], 0, 1000);
    }

    int index = 0;
    for (int i = 1; i < 8; i++)
     if (normalised[i] > normalised[index]) index = i;

    if (normalised[index] < 100)
    {
      run = false;
      stopMotors();
      return;
    }

    float position;
    if (index == 0) position = -30;
    else if (index == 7) position = 30;
    else
    {
      int sNul = normalised[index];
      int sMinEen = normalised[index-1];
      int sPlusEen = normalised[index+1];
      float b = (sPlusEen - sMinEen) / 2.0;
      float a = sPlusEen - b - sNul;
      position = -b / (2 * a);
      position += index;
      position -= 3.5;
      position *= 9.525;
    }

    debugposition = position;

    error = -position;
    output = -error * params.kp;

    iTerm += params.ki * error;
    iTerm = constrain(iTerm, -510, 510);
    output += iTerm;

    output += params.kd * (error - lastErr);
    lastErr = error;

    output = constrain(output, -510, 510);

    if (run)
    {
      int basePower = params.power;
      int powerLeft = constrain(basePower + output, 0, 255);
      int powerRight = constrain(basePower - output, 0, 255);

      analogWrite(MotorLeftForward, powerLeft);
      analogWrite(MotorLeftBackward, 0);
      analogWrite(MotorRightForward, powerRight);
      analogWrite(MotorRightBackward, 0);

    }
    else
    {
      stopMotors();
    }
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

void stopMotors()
{
  analogWrite(MotorLeftForward, 0);
  analogWrite(MotorLeftBackward, 0);
  analogWrite(MotorRightForward, 0);
  analogWrite(MotorRightBackward, 0);
}

void onUnknownCommand(char *command)
{
  printlnBoth("unknown command: \"" + String(command) + "\"");
}

void onSet()
{
  // Probeer eerst USB
  char* param = sCmd.next();
  char* value = sCmd.next();

  // Als er niks van USB is, probeer BT
  if (!param || !value) {
    param = sCmdBT.next();
    value = sCmdBT.next();
  }

  if (!param || !value) return; // geen geldige input

  if (strcmp(param, "cycle") == 0)
  {
    long newCycleTime = atol(value);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);
    params.ki *= ratio;
    params.kd /= ratio;
    params.cycleTime = newCycleTime;
  }
  else if (strcmp(param, "ki") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value) * cycleTimeInSec;
  }
  else if (strcmp(param, "kd") == 0)
  {
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value) / cycleTimeInSec;
  }
  else if (strcmp(param, "power") == 0) params.power = atol(value);
  else if (strcmp(param, "diff") == 0) params.diff = atof(value);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);

  EEPROM_writeAnything(0, params);
}


void onDebug()
{
  printlnBoth("cycle time: " + String(params.cycleTime));

  printBoth("black: ");
  for (int i = 0; i < 8; i++) { printBoth(String(params.black[i]) + " "); }
  printlnBoth("");

  printBoth("white: ");
  for (int i = 0; i < 8; i++) { printBoth(String(params.white[i]) + " "); }
  printlnBoth("");

  printBoth("Normalised: ");
  for (int i = 0; i < 8; i++) { printBoth(String(normalised[i]) + " "); }
  printlnBoth("");

  printlnBoth("position: " + String(debugposition));
  printlnBoth("Power: " + String(params.power));
  printlnBoth("diff: " + String(params.diff));
  printlnBoth("kp: " + String(params.kp));

  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  printlnBoth("Ki: " + String(params.ki / cycleTimeInSec));
  printlnBoth("Kd: " + String(params.kd * cycleTimeInSec));
  printlnBoth("RUN: " + String(run));
  printlnBoth("calculation time: " + String(calculationTime));
  calculationTime = 0;
}

void onCalibrate()
{
  char* param = sCmd.next();

  if (strcmp(param, "black") == 0)
  {
    printlnBoth("start calibrating black...");
    for (int i = 0; i < 8; i++) params.black[i] = analogRead(sensor[i]);
    printlnBoth("done");
  }
  else if (strcmp(param, "white") == 0)
  {
    printlnBoth("start calibrating white...");
    for (int i = 0; i < 8; i++) params.white[i] = analogRead(sensor[i]);
    printlnBoth("done");
  }

  EEPROM_writeAnything(0, params);
}

void onRun()
{
  run = true;
  digitalWrite(LED, HIGH);
}

void onStop()
{
  run = false;
  stopMotors();
  digitalWrite(LED, LOW);
}

void Interrupt()
{
  int buttonState = digitalRead(Drukknop);

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == HIGH) {
      runState = !runState;
      digitalWrite(LED, runState);
      run = runState;
      iTerm = 0;
    }
    lastDebounceTime = millis();
  }
}
