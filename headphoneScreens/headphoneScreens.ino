#include <SPI.h>
#include <PDQ_GFX.h>        // PDQ: Core graphics library
#include "PDQ_ST7735_config.h"      // PDQ: ST7735 pins and other setup for this sketch
#include <PDQ_ST7735.h>     // PDQ: Hardware-specific driver library
PDQ_ST7735 tft;     // PDQ: create LCD object (using pins in "PDQ_ST7735_config.h")

#include "fix_fft.h"                                  //library to perfom the Fast Fourier Transform

// These are used to get information about static SRAM and flash memory sizes
extern "C" char __data_start[];    // start of SRAM data
extern "C" char _end[];     // end of SRAM data (used to check amount of SRAM this program's variables use)
extern "C" char __data_load_end[];  // end of FLASH (used to check amount of Flash this program's code and data uses)

char im[128], data[128];                              //variables for the FFT
int i = 0, val, rotation = 2, largest = 8;                                       //counters
int temp[64];
int16_t clr;
int16_t colorTable[32] = {0x83e0, 0x83e0, 0x6ca0, 0x6ca0, 0x5560, 0x3640, 0x1f00, 0x7c0, 0x723, 0x666,
0x5a9, 0x4cc, 0x40f, 0x352, 0x295, 0x295, 0x1b9, 0xfc, 0x3f, 0x181c, 0x181c,
0x3019, 0x4816, 0x6013, 0x7810, 0x900d, 0xa80a, 0xc806, 0xe003, 0xe003, 0xf800, 0xf800};
bool inverted = false;
unsigned long elapsedTime = 0, start = 0;

void setup(){
  //analogReference(DEFAULT);                           // Use default (5v) aref voltage.
  /*pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);*/
  #if defined(ST7735_RST_PIN)  // reset like Adafruit does
  FastPin<ST7735_RST_PIN>::setOutput();
  FastPin<ST7735_RST_PIN>::hi();
  FastPin<ST7735_RST_PIN>::lo();
  delay(1);
  FastPin<ST7735_RST_PIN>::hi();
  #endif
  tft.begin();      // initialize LCD
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(2);
}

void loop(){
    start = millis();
    largest = 0;
    for (i = 0; i < 128; i++) {                         //take 128 samples
      val = analogRead(A0);                             //get audio from Analog 0
      data[i] = val / 4 - 128;                          //each element of array is val/4-128
      im[i] = 0; 
    }
  
    fix_fft(data, im, 7, 0);                            //perform the FFT on data
  
    for (i = 2; i < 64; i++) {                          // In the current design, 60Hz and noise
      temp[i] = 4*sqrt(data[i] * data[i] + im[i] * im[i]);//filter out noise and hum
      if(temp[i] > 12 && temp[i] < 40){
        temp[i] *= 2;
      } else if(temp[i] < 8){
        temp[i] = 2;
      }
    }
    //temp[0] = temp[2] + 4;
    //temp[1] = temp[3] + 1;
    for (i = 0; i < 64; i++){
      tft.drawFastVLine(128-i*2, 0, temp[i], colorTable[i/2]); // draw bar graphics for freqs above 500Hz to buffer
      tft.drawFastVLine(127-i*2, 0, temp[i], colorTable[i/2]);
      //tft.fillRect(i*2, 0, 3, temp[i], colorTable[i]); 
    }
    for(i = 0; i < 32; i++){
      largest += temp[i];
    }
    
    largest /= 24;
    
    for(int a = largest; a >= 0; a -= 4){
      tft.drawRect(64-a, 64-a, a*2, a*2, ST7735_WHITE);
    }
  
    /*tft.drawRect(64-largest, 64-largest, largest*2, largest*2, ST7735_WHITE);
    tft.drawLine(64-largest, 64-largest, 64+largest, 64+largest, ST7735_WHITE);
    tft.drawLine(64+largest, 64-largest, 64-largest, 64+largest, ST7735_WHITE);*/

    elapsedTime = millis() - start;
    if(elapsedTime <= 50){
      delay(50-elapsedTime);
    }
    
   /* for(i = 0; i < 64; i++) {
      tft.drawFastVLine(128-i*2, 0, temp[i], ST7735_BLACK); // draw bar graphics for freqs above 500Hz to buffer
      tft.drawFastVLine(127-i*2, 0, temp[i], ST7735_BLACK);
    }
    
    for(int a = largest; a >= 0; a -= 4){
      tft.drawRect(64-a, 64-a, a*2, a*2, ST7735_BLACK);
    }*/
  
    /*tft.drawRect(64-largest, 64-largest, largest*2, largest*2, ST7735_BLACK);
    tft.drawLine(64-largest, 64-largest, 64+largest, 64+largest, ST7735_BLACK);
    tft.drawLine(64+largest, 64-largest, 64-largest, 64+largest, ST7735_BLACK);*/

    tft.fillScreen(ST7735_BLACK);
    
    if(largest > 40){
      //inverted = !inverted;
      //tft.invertDisplay(inverted);
      rotation = 2 - rotation;
      tft.setRotation(rotation);
    }
}

