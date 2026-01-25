#include <Arduino.h>

#define ENA 5
#define forward 6
#define backward 7
#define joystickY 3
#define joystickX 4

int x;
int y;

void setup() {
    Serial.begin(115200);

    pinMode(ENA, OUTPUT);
    pinMode(forward, OUTPUT);
    pinMode(backward, OUTPUT);

    digitalWrite(ENA, HIGH);
    digitalWrite(forward, LOW);
    digitalWrite(backward, LOW);

    Serial.println("L298N test start");
}

void loop() {
    x = analogRead(joystickX);
    y = analogRead(joystickY);

    if (x <= 2250) {
        digitalWrite(ENA, HIGH);
        digitalWrite(forward, LOW);
        digitalWrite(backward,HIGH);
    }
    if (x >= 2350) {
        digitalWrite(ENA, HIGH);
        digitalWrite(forward, HIGH);
        digitalWrite(backward,LOW);
    }
    if (x > 2250 && x < 2350) {
        digitalWrite(ENA, LOW);
    }
    Serial.print(x);
    Serial.print("\n");
}