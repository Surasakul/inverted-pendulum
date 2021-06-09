#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx , valy , valz;
char rd;
int prevVal;
int pin11 = 11 , pin10 = 10 ;
int val1 , val2 ;
int valgy1 = 0 , valgy2 = 0;
byte countS = 0; //03
int recOmegaI[10]; //05
int omegaI = 0;
int zeromegaI = 0;//06
long thetaI = 0; //07
long sumPower = 0; //08
long sumSumP = 0; //09
const int kAngle = 50; //10
const int kOmega = 500; //11
const long kSpeed = 60; //12
const long kDistance = 20; //13
long powerScale; //14
int power; //15
long vE5 = 0; //16
long xE5 = 0; //17
int ry; //DL2
long R; //DL3
void setup () { //18
Wire.begin();
Serial.begin(38400);
pinMode(4, OUTPUT); //20
pinMode(5, OUTPUT); //20-a
pinMode(6, OUTPUT); //21
pinMode(7, OUTPUT);
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
for ( int i = 0 ; i < 10 ; i++ ) { recOmegaI[i] = 0; } //25 ("int" is added instead of line 2 omitted.)
pinMode(10, OUTPUT); //DL18 (These 8 lines, DL18-DL25, are added in this version.)
digitalWrite(10, HIGH); //DL19
Serial.println("Initialize MPU");
mpu.initialize();
Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");

delay(300); //26
// training(); // (This line is omitted in this version.)
// MsTimer2::set(5, chkAndCtl); // (This line is omitted in ver.2.0 and the later.)
// MsTimer2::start(); // (This line is omitted in ver.2.0 and the later.)
} //30
void loop () { //31
chkAndCtl(); // NL1 (This line is added in ver.2.0 and the later.)
if ( power > 0 ) { //32
analogWrite( 6, power );
digitalWrite( 5, HIGH );
digitalWrite( 4, LOW ); //35
analogWrite( 9, power );
digitalWrite( 7, HIGH );
digitalWrite( 8, LOW );
} else {
analogWrite( 6, - power ); //40
digitalWrite( 5, LOW );
digitalWrite( 4, HIGH );
analogWrite( 9, - power );
digitalWrite( 7, LOW );
digitalWrite( 8, HIGH ); //45
}
// delayMicroseconds(3600); // NL2 (This is omitted in this version.)
Serial.println(power) ;
}
//void training(){ //48 (These 7 lines, 48-54, are omitted in this version.)
// delay (1000);
// for ( i = 0 ; i < 500 ; i++ ){ //50
// zeroOmegaI = zeroOmegaI + analogRead(A5);
// }
// zeroOmegaI = zeroOmegaI / i;
//} //54
void chkAndCtl() { //55
// omegaI = 0; // NL3 (These 6 lines, NL3-NL8, are omitted in this version.)
// for ( i = 0 ; i < 10 ; i++ ) { //NL4
// omegaI = omegaI + analogRead(A5) - zeroOmegaI; //NL5
// delayMicroseconds(10); //NL6
// } //NL7
// omegaI = omegaI / 10; //NL8
//DL26 (These 7 lines, DL26-DL32, are added in this version.)
zeromegaI=0;
for ( int i = 0 ; i < 45 ; i++ ) { //DL27 ("int" is added instead of line 2 omitted.)
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
valx = map(ax, -17000, 17000, 0, 179) -70;
zeromegaI = valx + zeromegaI;
Serial.println(power) ;
Serial.println(valx) ;

delayMicroseconds(90); //DL30
} //DL31
omegaI = zeromegaI/45 ; //DL32
// omegaI = analogRead(A5) - zeroOmegaI; //56 (This line is omitted in ver.2.0 and the later.)
if ( abs( omegaI ) < 8 ) { omegaI = 0; } //57 (The lower bound is less than 2 in this version.)
recOmegaI[0] = omegaI;
thetaI = thetaI + omegaI;
countS = 0; //60
for ( int i = 0 ; i < 10 ; i++ ) { // ("int" is added instead of line 2 omitted.)
if ( abs( recOmegaI[i] ) < 4 ) { countS++; } //62 (The lower bound is less than 4 in this version.)
}
if ( countS > 9 ) {
thetaI = 0; //65
vE5 = 0;
xE5 = 0;
sumPower = 0;
sumSumP = 0;
} //70
for ( int i = 9 ; i > 0 ; i-- ) { recOmegaI[ i ] = recOmegaI[ i-1 ]; } // ("int" is added instead of line 2 omitted.)
powerScale = ( kAngle * thetaI / 100 ) + ( kOmega * omegaI / 100 ) + ( kSpeed * vE5 / 1000 ) + ( kDistance
* xE5 / 1000 ); //72
power = max ( min ( 95 * powerScale / 100 ,50 ) , -50 );
if (power>0){power+30;}
if (power<0){power-30;}
sumPower = sumPower + power;
sumSumP = sumSumP + sumPower; //75
vE5 = sumPower; //76a
xE5 = sumSumP / 1000; } //78
