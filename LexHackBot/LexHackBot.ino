/*
  LiquidCrystal Library - Custom Characters

 Demonstrates how to add custom characters on an LCD  display.
 The LiquidCrystal library works with all LCD displays that are
 compatible with the  Hitachi HD44780 driver. There are many of
 them out there, and you can usually tell them by the 16-pin interface.

 This sketch prints "I <heart> Arduino!" and a little dancing man
 to the LCD.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K potentiometer:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 10K poterntiometer on pin A0
 
 created 21 Mar 2011
 by Tom Igoe
 modified 11 Nov 2013
 by Scott Fitzgerald
 
 Based on Adafruit's example at
 https://github.com/adafruit/SPI_VFD/blob/master/examples/createChar/createChar.pde

 This example code is in the public domain.
 http://www.arduino.cc/en/Tutorial/LiquidCrystal

 Also useful:
 http://icontexto.com/charactercreator/

 */

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

// Pins
#define buzzer 10
#define right 6
#define left 9
#define trig 8
#define echo 7

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//LiquidCrystal(rs, enable, d4, d5, d6, d7)

void setup() {
  // initialize LCD and set up the number of columns and rows: 
  lcd.begin(20, 4);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buzzer, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void getAccel() { //Obtain Data From Accelerometer
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  //Convert Temp from raw data to fahrenheit
  Tmp = (Tmp / 340.00 + 36.53) * 1.8 + 32;
}

int16_t distance(){ //Returns cm from ultrasonic sensor
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  int16_t dist = pulseIn(echo, HIGH);
  return dist*0.034/2;
}

void printAccel(){
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("GyX: "); lcd.setCursor(5, 0); lcd.print(GyX); 
  lcd.setCursor(10, 0); lcd.print("GyY: "); lcd.setCursor(15, 0); lcd.print(GyY); 
  lcd.setCursor(0, 1); lcd.print("GyZ: "); lcd.setCursor(5, 1); lcd.print(GyZ); 
  lcd.setCursor(0, 2); lcd.print("AcX: "); lcd.setCursor(5, 2); lcd.print(AcX); 
  lcd.setCursor(10, 2); lcd.print("AcY: "); lcd.setCursor(15, 2); lcd.print(AcY); 
  lcd.setCursor(0, 3); lcd.print("AcZ: "); lcd.setCursor(5, 3); lcd.print(AcZ);
}

String query = "";
void loop() {
  //while(Serial.available()){
    String newquery = Serial.readString();
    if(newquery != ""){
      query = newquery;
    }
    Serial.println("Recieved: "+query);
    if(query.substring(0, 8) == "Distance"){ //Move forward until distance in centimeters is met
      analogWrite(left, 255);
      analogWrite(right, 255);
      int goal = query.substring(8).toInt();
      if(distance() <= goal){
        analogWrite(left, 0);
        analogWrite(right, 0);
        query = "Stop";
      }
    }
    if(query.substring(0, 7) == "Forward"){ //Example: Forward255 = full speed forward
      analogWrite(left, query.substring(7).toInt()/2);
      analogWrite(right, query.substring(7).toInt());
    }
    if(query.substring(0, 4) == "Left"){ //Example: Left255 = turn left 90 degrees at full speed
      analogWrite(left, 0);
      analogWrite(right, query.substring(4).toInt());
    }
    if(query.substring(0, 5) == "Right"){ //Example: Right255 = turn right 90 degrees at full speed
      analogWrite(right, 0);
      analogWrite(left, query.substring(5).toInt());
    }
    if(query == "Stop"){
      analogWrite(left, 0);
      analogWrite(right, 0);
    }
    if(query.substring(0, 5) == "Print"){
      lcd.setCursor(0, query[5]-'0');
      lcd.print(query.substring(6));
    }
    if(query == "Clear"){
      lcd.clear();
    }
    if(query == "Data"){
      getAccel();
      printAccel();
    }
    if(query.substring(0, 4) == "Tone"){
      tone(buzzer, 1046.5);
      delay(100);
      tone(buzzer, 1174.7);
      delay(100);
      tone(buzzer, 1318.5);
      delay(100);
      tone(buzzer, 1396.9);
      delay(100);
      tone(buzzer, 1567.9);
      delay(100);
      noTone(buzzer);
    }
  //}
}
