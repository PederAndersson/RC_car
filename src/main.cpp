#include <Arduino.h>

#define ENA 10
#define forward 6
#define backward 7
#define joystickY 3
#define joystickX 4
#define servo_pin 5

int x;
int y;

const int pwmDcChannel = 0;
const int pwmDcFreq = 20000;     // 20 kHz
const int pwmDcResolution = 8;  // 0–255

const int pwmServoChanel = 1;
const int pwmServofreq = 50;
const int pwmServoResolution = 16; //2^16 steg = 65535
const int maxValue = 65535;

const int center = 2300;
const int deadzone = 150;

void writeServoMicro(int joystickInput) {
    joystickInput = constrain(joystickInput, 500, 2500);
    const int period = 1000000/pwmServofreq;
    uint32_t duty = (uint32_t)(((uint64_t)joystickInput*maxValue)/period);

    ledcWrite(pwmServoChanel, duty);

}

void setup() {
    Serial.begin(115200);
    ledcSetup(pwmDcChannel, pwmDcFreq, pwmDcResolution);
    ledcAttachPin(ENA, pwmDcChannel);

    ledcSetup(pwmServoChanel, pwmServofreq, pwmServoResolution);
    ledcAttachPin(servo_pin, pwmServoChanel);

    pinMode(servo_pin,OUTPUT);
    pinMode(forward, OUTPUT);
    pinMode(backward, OUTPUT);

    digitalWrite(forward, LOW);
    digitalWrite(backward, LOW);
    analogReadResolution(12);

}

void loop() {
    x = analogRead(joystickX);
    y = analogRead(joystickY);

    int mappedInput = map(y, 0, 4095, 2000, 1000);
    int diff = x - center;
    int speed = 0;

    writeServoMicro(mappedInput);

    if (abs(diff) <= deadzone) {
        digitalWrite(forward, LOW);
        digitalWrite(backward, LOW);
        speed = 0;
    }else if (diff < 0) {
        digitalWrite(forward, HIGH);
        digitalWrite(backward, LOW);
        speed = map(-diff, deadzone, center, 0, 255);
    }else {
        digitalWrite(forward, LOW);
        digitalWrite(backward, HIGH);
        speed = map(diff, deadzone, 4095 - center, 0, 255);
    }

    speed = constrain(speed, 0, 255);
    ledcWrite(pwmDcChannel, speed);

    Serial.printf("%d\n", mappedInput);
    Serial.printf("x=%d diff=%d speed=%d\n", x, diff, speed);
    digitalWrite(servo_pin, HIGH);
    delayMicroseconds(1500);
    digitalWrite(servo_pin, LOW);
    delayMicroseconds(20000 - 1500);
}