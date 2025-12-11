const int IN1 = 10;
const int IN2 = 9;
const int IN3 = 6;
const int IN4 = 5;


void setup() {
  // put your setup code here, to run once:
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Motors voorwaarts
  //Versnellen
  for (int i = 0; i <= 255; i++)
  {
    analogWrite(IN1, i);
    analogWrite(IN2, 0);
    analogWrite(IN3, i);
    analogWrite(IN4, 0);
    delay(10);
  }
  //Vertragen
  for (int i = 255; i >= 0; i--)
  {
    analogWrite(IN1, i);
    analogWrite(IN2, 0);
    analogWrite(IN3, i);
    analogWrite(IN4, 0);
    delay(10);
  }

  delay(2000);

  // Achterwaarts
  //Versnellen
  for (int i = 0; i <= 255; i++)
  {
    analogWrite(IN1, 0);
    analogWrite(IN2, i);
    analogWrite(IN3, 0);
    analogWrite(IN4, i);
    delay(10);
  }
  //Vertragen
  for (int i = 255; i >= 0; i--)
  {
    analogWrite(IN1, 0);
    analogWrite(IN2, i);
    analogWrite(IN3, 0);
    analogWrite(IN4, i);
    delay(10);
  }

  delay(2000);


}
