#include<Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Fonts/backwards9pt7b.h>

#define BLACK    0x0000
#define BLUE     0x063F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

#define TFT_CS     9
#define TFT_RST    8
#define TFT_DC     7

#define TFT_SCLK 13
#define TFT_MOSI 10
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define POT_PIN 2
#define TRIG 5
#define ECHO 4

int val = 0, mode=0, oldmode; //0 = Temp, 1 = Accel XYZ+Temp, 2 = Gyro XYZ+Distance

bool usePing = false;

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
void setup() {
  if(usePing){
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
  }
  
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setFont(&BACKRG__9pt7b);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  randomSeed(analogRead(0));
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  //Serial.begin(9600);
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
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  int16_t dist = pulseIn(ECHO, HIGH);
  return dist*0.034/2;
}

String reverse(int16_t num) {
  String out, n = String(num);
  int len = n.length();
  for(int i=len-1; i>=0; i--){
    out += n[i];
  }
  return out;
}
  
void displayBars(int16_t x, int16_t y, int16_t z, int16_t t){
  tft.fillRect(15, 10, 130, 27, RED);
  tft.fillRect(15, 37, 130, 27, GREEN);
  tft.fillRect(15, 64, 130, 27, BLUE);
  tft.fillRect(15, 91, 130, 27, YELLOW);
  tft.setTextColor(BLACK);
  tft.setCursor(20, 33);
  tft.print(reverse(x)+" :X");
  tft.setCursor(20, 60);
  tft.print(reverse(y)+" :Y");
  tft.setCursor(20, 87);
  tft.print(reverse(z)+" :Z");
  tft.setCursor(20, 108);
  if(mode == 1){
    tft.print(reverse(t)+" :pmeT");
  } else if(usePing){
    tft.print(reverse(t)+" :)mc( tsiD");
  }
}
void verticalText(String text, int x, int y){
  for(int i=0; i<text.length(); i++){
    tft.setCursor(x, i*15+y);
    if(text[i] == 'I'){
      tft.setCursor(x+4, i*15+y);
    }
      tft.print(text[i]);
  }
}

void crosshair(int x, int y, int16_t clr){
  tft.drawFastHLine(0, y, 160, clr);
  tft.drawFastVLine(x, 0, 120, clr);
  tft.drawFastHLine(x-15, y-15, 30, clr);
  tft.drawFastHLine(x-15, y+15, 30, clr);
  tft.drawFastVLine(x-15, y-15, 8, clr);
  tft.drawFastVLine(x+15, y-15, 8, clr);
  tft.drawFastVLine(x-15, y+7, 8, clr);
  tft.drawFastVLine(x+14, y+7, 8, clr);
}
void loop() {
  /*Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ); */

  val = analogRead(POT_PIN);
  //Serial.print(" | Val = "); Serial.println(val);
  oldmode = mode;
  int x, y;
  if (val < 341){
    tft.fillRect(0, 0, 160, 120, BLACK);
    tft.drawFastHLine(148, 112, 9, BLUE);
    tft.drawFastHLine(148, 9, 9, BLUE);
    tft.drawFastVLine(148, 108, 4, BLUE);
    tft.drawFastVLine(156, 108, 4, BLUE);
    tft.drawFastVLine(148, 9, 4, BLUE);
    tft.drawFastVLine(156, 9, 4, BLUE);
    int r=255, g=0, b=0;
    int16_t clr;
    x = random(30, 131);
    y = random(30, 91);
    for(int j=0; j<3; j++){
      x = random(x-5, x+6);
      y = random(y-5, y+6);
      for(int i=0; i<=200; i=i+2){
        if(i<=100){
          r = i+155;
        } else {
          r = 355-i;
        }
        if(i%4==0){
          tft.drawFastHLine(150, 110-(i/6)-j*33, 5, BLUE);
        } else {
          tft.drawFastHLine(150, 110-(i/6)-j*33, 5, GREEN);
        }
        clr = ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
        crosshair(x, y, clr);
      }
      crosshair(x, y, BLACK);
    }
    tft.fillRect(5, 5, 120, 20, GREEN);
    tft.fillRect(155, 120, 160, 0, GREEN);
    tft.setCursor(5, 18);
    tft.print("dekcoL tegraT");
    for(int j=0; j<3; j++){
      crosshair(x, y, GREEN);
      delay(300);
      crosshair(x, y, BLUE);
      delay(300);
    }
    mode = 0;
  }
  if (val >=341 && val<=682){
    getAccel();
    displayBars(AcX, AcY, AcZ, Tmp);
    mode = 1;
  }
  if (val > 682){
    getAccel();
    if(usePing){
      displayBars(GyX, GyY, GyZ, distance());
    } else {
      displayBars(GyX, GyY, GyZ, 0);
    }
    mode = 2;
  }
  if (mode!=oldmode){
    tft.fillRect(0, 0, 160, 120, BLACK);
    if(mode == 1){
      tft.setTextColor(YELLOW);
      verticalText("ACCEL", 1, 30);
      verticalText("+TEMP", 146, 30);
    }
    if(mode == 2){
      tft.setTextColor(YELLOW);
      tft.setTextSize(0);
      verticalText("GYRO", 1, 30);
      verticalText("+DIST", 146, 30);
    }
  }
}
