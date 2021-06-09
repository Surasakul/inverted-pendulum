#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int angle;
int speedMotor;
unsigned long currentTime, previousTime;
double elapsedTime, error, setPoint = 170, input, lastError, output;
double cumError, rateError, Kp = 0.5, Ki = 0, Kd = 0.5;
void setup() {
Serial.begin(9600);
pinMode(3, OUTPUT); //ENB
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
pinMode(8, OUTPUT);
pinMode(7, OUTPUT);
pinMode(6, OUTPUT); //ENA
Wire.begin();
mpu.initialize();
// calibrate(); //In 20 sec, adjust min & max angle
}
void loop() {
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
angle = map(az, -17000, 17000, 0, 179);
input = angle;
output = computePID(input);
delay(10);
speedMotor = map(output, -1, 1, -255, 255);
speedMotor = abs(speedMotor);
speedMotor = map(speedMotor, 0, 255, 75, 100);
speedMotor = constrain(speedMotor, 0, 75);
if (output >= 0)
{
digitalWrite(5,LOW);
digitalWrite(4,HIGH);
analogWrite(3,speedMotor);
digitalWrite(8,LOW);
digitalWrite(7,HIGH);
analogWrite(6,speedMotor);
}

15

else
{
digitalWrite(5,HIGH);
digitalWrite(4,LOW);
analogWrite(3,speedMotor);
digitalWrite(8,HIGH);
digitalWrite(7,LOW);
analogWrite(6,speedMotor);
}
Serial.print("output= ");
Serial.print(output);
Serial.print(" ");
Serial.print("speed= ");
Serial.println(speedMotor);
}
double computePID(double inp)
{
currentTime = millis();
elapsedTime = (double)(currentTime - previousTime);
error = setPoint - inp;
cumError += error * elapsedTime;
rateError = (error - lastError)/elapsedTime;
double out = Kp*error + Ki*cumError + Kd*rateError;
lastError = error;
previousTime = currentTime;
return out;
}
