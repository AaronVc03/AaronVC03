const int buttonPin = 2;   // Druknop op pin 2
const int ledPin = 3;      // LED op pin 3

volatile bool ledState = false;  // Huidige status van de LED
volatile unsigned long lastDebounceTime = 0;  // Tijd voor debouncing
unsigned long debounceDelay = 50;  // Debounce-tijd in milliseconden

void setup() {
  pinMode(buttonPin, INPUT);  // Configureer pin 2 als ingang met pull-up weerstand
  pinMode(ledPin, OUTPUT);           // Configureer pin 3 als uitgang

  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, RISING);  // Koppel de interrupt-functie aan de opgaande flank van de drukknop
}

void loop() {
  // Doe hier andere taken die onafhankelijk zijn van de drukknopstatus
}

void buttonInterrupt() {
  // Lees de status van de drukknop
  int buttonState = digitalRead(buttonPin);

  // Debounce de drukknop met millis
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Als de knop opgaand is, wissel de status van de LED
    if (buttonState == HIGH) {
      ledState = !ledState;

      // Schakel de LED in of uit op basis van de nieuwe status
      digitalWrite(ledPin, ledState);
    }

    // Update de tijd van de laatste debouncing
    lastDebounceTime = millis();
  }
}
