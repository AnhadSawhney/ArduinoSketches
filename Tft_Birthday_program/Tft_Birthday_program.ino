#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Uncomment this to use option 2:
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//Visit https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
//  And https://learn.adafruit.com/1-8-tft-display/breakout-wiring-and-test
//  And http://www.amazon.com/SainSmart-Display-Interface-MicroSD-Arduino/dp/B008HWTVQ2/ref=sr_1_4?ie=UTF8&qid=1431651383&sr=8-4&keywords=tft+display
//  And http://www.arduino.cc/en/Main/ArduinoBoardMicro

//Screen Measurements:
#define TFTwidth 128
#define TFTheight 160

int Candle(int x, int y){
  tft.fillTriangle(x, y+4, x+2, y, x+4, y+4, YELLOW);
  tft.fillCircle(x+2, y+5, 2, YELLOW);
  tft.fillRect(x, y+9, 5, 15, GREEN);
}

void Countdown(){
    for(int c=5; c<0; c--){
    tft.drawChar(TFTheight-15, 15, char(c), RED, BLACK, 10);
    delay(1000);
    }
}
void setup() {
    tft.initR(INITR_BLACKTAB); //Initialize the screen
    tft.fillScreen(ST7735_BLACK);
    delay(1000);
    for(int c=5; c<0; c--){
    tft.drawChar(TFTheight-15, 15, char(c), RED, BLACK, 10);
    delay(1000);
    }
  
}

void loop() {
  Countdown();
  int height=20;
  int edge=15;
  int width=30;
  int topheight=1.25;
    tft.fillRect(width, TFTheight-(height+edge), TFTwidth-(width*2), height, BLUE);
    tft.fillRoundRect(width, TFTheight-(height-height/(topheight*2)+edge), TFTwidth-(width*2), height/topheight, height/2, BLUE);
    tft.fillRoundRect(width, TFTheight-(height+height/(topheight*2)+edge), TFTwidth-(width*2), height/topheight, height/2, MAGENTA);

  for(int a=0; a<4; a++){
     Candle(30+random(10, TFTwidth-(width*2+10)), TFTheight-(height+edge+random(22,27)));
  }
  delay(500);
  tft.fillScreen(BLACK);
  delay(500);
}
