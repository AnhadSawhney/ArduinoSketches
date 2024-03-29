#include <Arduino.h>
#include <U8g2lib.h>

#define check_sec 600         //How many seconds to wait before next refresh
#define charge_thresh 13.8    //Voltage of charged battery
#define max_thresh 19         //Voltage of charger (no battery connected
#define min_thresh 11       //Voltage of completely discharged battery
#define dc_thresh 5         //Voltage of unconnected / floating pin

// A0 A2 A4 A6   true/HIGH [0] coil off
// 2  3  4  5
// A1 A3 A5 A7   PIN false/LOW [1] coil is on

//Digital 2,3,4,5 for switching, 10-11 for display
//A0-A7 battery 
//TwoWire (maybe sda11, scl12) for oled
//digital 6 for button

//                 PIN LOW, COIL ON|PIN HIGH, COIL OFF
volatile float charge[4][2] = {{0.0,0.0},  // A1,A0    A,E
                               {0.0,0.0},  // A3,A2    B,F
                               {0.0,0.0},  // A5,A4    C,G
                               {0.0,0.0}}; // A7,A6    D,H

/*const float coefficients[4][2] = {{1.0,1.0},  // TO IMPROVE ACCURACY OF VOLTAGE READINGS
                                  {1.0,1.0},
                                  {1.0,1.0},
                                  {1.0,1.0}};*/
                         
volatile bool state[4] = {true, true, true, true};

volatile int timesincerefresh = 0;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 11, /* data=*/ 10, /* reset=*/ U8X8_PIN_NONE); 

#define doge_width 128
#define doge_height 48
static const unsigned char doge_bits[] U8X8_PROGMEM = { //More like PROGMEME amirite
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xE0, 0x01, 0x00, 0x00, 
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 
  0xE0, 0x03, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x60, 0x00, 0xF0, 0x0F, 0x00, 0xC0, 0x07, 0x00, 0x80, 0x99, 
  0x79, 0x66, 0x06, 0x00, 0x00, 0x00, 0x60, 0x00, 0xF0, 0x3F, 0x00, 0xC0, 
  0x07, 0x00, 0x80, 0x99, 0x7D, 0x66, 0x06, 0xC0, 0x67, 0xC6, 0x63, 0x07, 
  0xF0, 0x7F, 0xC0, 0xE3, 0x0F, 0x00, 0x80, 0xDD, 0xCE, 0x76, 0x03, 0xE0, 
  0x67, 0xE6, 0xE7, 0x0F, 0xF0, 0xFF, 0xF1, 0xE3, 0x1F, 0x00, 0x80, 0xFD, 
  0xC6, 0xF6, 0x03, 0x60, 0x60, 0x66, 0xE6, 0x0C, 0xF0, 0xFF, 0x7F, 0x00, 
  0x1E, 0x00, 0x80, 0xF7, 0xC6, 0xDE, 0x03, 0xC0, 0x61, 0x36, 0x60, 0x0C, 
  0xF0, 0xFF, 0x3F, 0x7C, 0x08, 0x00, 0x80, 0x77, 0xC6, 0xDE, 0x01, 0x80, 
  0x63, 0x36, 0x60, 0x0C, 0xF0, 0xFF, 0x3F, 0xFE, 0x00, 0x07, 0x00, 0x77, 
  0x7C, 0xDC, 0x01, 0x00, 0x66, 0x36, 0x66, 0x0C, 0xF0, 0xFF, 0xFF, 0xFF, 
  0x00, 0x0F, 0x00, 0x23, 0x38, 0x8C, 0x00, 0xE0, 0xE7, 0xE7, 0x67, 0x0C, 
  0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 
  0xC3, 0xC7, 0x63, 0x0C, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x1E, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 
  0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xE0, 0xFF, 0xFC, 0xFF, 0x01, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x7F, 0xF8, 0xF9, 0x03, 0x38, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F, 0xFC, 0xF0, 
  0xE3, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0xF0, 0x0F, 0x7F, 0xE0, 0xF3, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
  0x00, 0x00, 0x00, 0x00, 0xF0, 0x07, 0x3F, 0xC0, 0xF1, 0x31, 0x00, 0x9F, 
  0xFD, 0xF0, 0xEC, 0x7E, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x03, 0xDE, 0x07, 
  0xF0, 0x01, 0x80, 0x9F, 0xFF, 0xF9, 0xFD, 0x7E, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0x01, 0xEC, 0x1F, 0xE0, 0x03, 0x80, 0x81, 0x9B, 0x9D, 0xDD, 0x18, 
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0xF0, 0x3F, 0x80, 0x01, 0x00, 0x87, 
  0x99, 0x8D, 0xCD, 0x18, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0xE0, 0x3F, 
  0x00, 0xC0, 0x01, 0x8E, 0x99, 0x8D, 0x0D, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0x00, 0xC0, 0x1F, 0x00, 0xC0, 0x01, 0x98, 0x99, 0x8D, 0x0D, 0x18, 
  0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x03, 0x00, 0x80, 0x83, 0x9F, 
  0x99, 0xFD, 0x0D, 0x18, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 
  0xE0, 0x9F, 0x03, 0x8F, 0x99, 0xF9, 0x0D, 0x18, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x9F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 
  0xF8, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xF0, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x03, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x00, 
  0xF0, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xFC, 0x01, 0x00, 0x00, 0xF0, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x00, 0xF8, 0x1F, 0x03, 0x00, 
  0x00, 0x3C, 0x76, 0x78, 0x76, 0x3C, 0x3C, 0x3B, 0xFE, 0x01, 0x00, 0x70, 
  0xF8, 0x8F, 0x03, 0x00, 0x00, 0x7E, 0xFE, 0xFC, 0x7E, 0x7E, 0x7E, 0x3F, 
  0xFE, 0x01, 0x00, 0x70, 0xF8, 0x8F, 0x03, 0x00, 0x00, 0x66, 0xCE, 0xCE, 
  0x6E, 0x67, 0x67, 0x37, 0xFE, 0x03, 0x00, 0xF8, 0xFF, 0x9F, 0x03, 0x00, 
  0x00, 0x03, 0xC6, 0xC6, 0x66, 0x63, 0x7B, 0x33, 0xFF, 0x03, 0x00, 0xF8, 
  0xFF, 0x9F, 0x03, 0x00, 0x00, 0x03, 0xC6, 0xC6, 0x06, 0x63, 0x1F, 0x03, 
  0xFF, 0x03, 0x00, 0xE0, 0xFF, 0xCF, 0x03, 0x00, 0x00, 0x63, 0xC6, 0xC6, 
  0x06, 0x73, 0x07, 0x03, 0xFF, 0x07, 0x00, 0x00, 0xFF, 0xC7, 0x01, 0x00, 
  0x00, 0x7E, 0xC6, 0xFE, 0x06, 0x7F, 0x7E, 0x03, 0xFF, 0x0F, 0x00, 0x00, 
  0xF8, 0xE0, 0x00, 0x00, 0x00, 0x3C, 0xC6, 0xFC, 0x06, 0x7E, 0x3C, 0x03, 
  0xFE, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x60, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00, 
  0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 
  0xFC, 0xFF, 0x00, 0x00, 0x80, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1F, 0x00, 0x00, 0xF8, 0xFF, 0x01, 0x00, 0xF0, 0x0F, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x03, 0x00, 
  0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

#define lightning_width 6
#define lightning_height 9
static const unsigned char lightning_bits[] U8X8_PROGMEM = {
  0x18, 0x0C, 0x06, 0x03, 0x3F, 0x30, 0x18, 0x0C, 0x06, };

ISR(PCINT2_vect){
    if(timesincerefresh > 2){
        cycleBatt();
        timesincerefresh = 0;
    }
}

void cycleBatt(){
    noInterrupts();
    updateCharge();

    //update charge array and swap battery if charged
    for(int i = 0; i < 4; i++){
        if(charge[i][0] <= dc_thresh && charge[i][1] <= dc_thresh)//if both disconnected default idx#1
            state[i] = true;
        else if(charge[i][0] >= charge_thresh && charge[i][1] <= dc_thresh)//idx#0 charged, swap to unplugged
            state[i] = true;//idx#1
        else if(charge[i][0] <= dc_thresh && charge[i][1] >= charge_thresh)//idx#1 charged, swap to unplugged
            state[i] = false;//idx#0
        else if(charge[i][0] >= charge[i][1] && charge[i][0] < charge_thresh) //if idx#0 batt highest+not charged
            state[i] = false;//idx#0
        else 
            state[i] = true;//idx#1
    }

    updatePowered();
    //blinkCharged();
    interrupts();
}

void updateCharge(){
    for(uint8_t j = 0; j < 2; j++){
        //updateCharges and flip batteries
        for(uint8_t i = 0; i < 4; i++){
            charge[i][(uint8_t)(state[i])] = analogRead(i*2+state[i])*21.5/1024.f;//*coefficients[i][(uint8_t)(state[i])];
            if(charge[i][(uint8_t)(state[i])] >= max_thresh){
                charge[i][(uint8_t)(state[i])] = 0;
            }
        }
        //printVoltages();
        for(uint8_t i = 0; i < 4; i++){
            state[i] = !state[i];
        }
        updatePowered();
    }

    printVoltages();
}

void updatePowered(){
    for(int i = 0; i < 4; i++)
        digitalWrite(i+2, state[i]);
    delay(200);//allow relays to swap
}

/*void blinkCharged(){
    //blink number of charged batteries
    uint8_t tot = 0, maximum = 8;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 2; j++){
            if(charge[i][j] >= charge_thresh){
                tot++;
                digitalWrite(LED_BUILTIN, HIGH);
                delay(100);
                digitalWrite(LED_BUILTIN, LOW);
                delay(400);
            } else if(charge[i][j] < dc_thresh)//dont count disconnected
                maximum--;
        }
    //solid if all done
    if(tot >= maximum)
        digitalWrite(LED_BUILTIN, HIGH);
    else
        digitalWrite(LED_BUILTIN, LOW);
}*/

void dispVoltages(){  
    noInterrupts();  
    u8g2.clearBuffer();
    u8g2.setDrawColor(2);
    uint8_t x, h;
    char buf[9];
    for(uint8_t i = 0; i < 4; i++){
        for(uint8_t j = 0; j < 2; j++){
            //charge[i][j] = random(0, 1024);
            
            x = (i+j*4)*16;
            
            if(state[i] == j){
                  u8g2.drawXBMP(x+7, 7, lightning_width, lightning_height, lightning_bits);
            }
            if(charge[i][j] > dc_thresh && charge[i][j] < max_thresh) {
                u8g2.setCursor(x+1, 5);
                u8g2.print((char)(65+i+j*4));

                u8g2.drawHLine(x+1, 20, 4);
                u8g2.drawHLine(x+11, 20, 4);
                u8g2.drawHLine(x+4, 16, 8);
                u8g2.drawHLine(x+1, 63, 14);
                u8g2.drawVLine(x+4, 16, 4);
                u8g2.drawVLine(x+11, 16, 4);
                u8g2.drawVLine(x+1, 20, 44);
                u8g2.drawVLine(x+14, 20, 44);

                h = (int)(46.0*(charge[i][j]-min_thresh)/(charge_thresh-min_thresh));

                if(h > 46){
                  h = 46;
                }
                
                if(h > 42){
                    u8g2.drawBox(x+2, 21, 12, 42);
                    h -= 42;
                    u8g2.drawBox(x+5, 21-h, 6, h);
                } else {
                    u8g2.drawBox(x+2, 63-h, 12, h);
                }
                
                u8g2.setCursor(x, 22);
                u8g2.setFontDirection(1);
                dtostrf(charge[i][j], 4, 2, buf);
                strcat(buf, "v");
                u8g2.drawStr(x+14, 62-strlen(buf)*6, buf);
                u8g2.setFontDirection(0);
            }
        }
    }

    u8g2.drawFrame(0, 0, 127, 5);
    u8g2.drawBox(1, 1, (int)(timesincerefresh*125.0/check_sec), 3);
    
    u8g2.sendBuffer();
    interrupts();
}

void printVoltages(){
    Serial.println();
    for(int x = 0; x < 4; x++){
        Serial.print(state[x]);
        Serial.print("   ");
        Serial.print(charge[x][0]);
        Serial.print("|");
        Serial.print(charge[x][1]);
        Serial.print(", ");
        Serial.print(x*2);
        Serial.print(": ");
        Serial.print(analogRead(x*2)*21.5/1024.f);
        Serial.print(", ");
        Serial.print(x*2+1);
        Serial.print(": ");
        Serial.println(analogRead(x*2+1)*21.5/1024.f);
    }
    Serial.println();
}

void setup(){
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(6, INPUT_PULLUP);
    for(uint8_t pin = 2; pin <= 5; pin++)
        pinMode(pin, OUTPUT);

    *digitalPinToPCMSK(6) |= bit (digitalPinToPCMSKbit(6));
    PCIFR  |= bit (digitalPinToPCICRbit(6));
    PCICR  |= bit (digitalPinToPCICRbit(6));

    cycleBatt();
    
    //display setup
    u8g2.begin();
    u8g2.setBitmapMode(false /* solid */);
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.setFontPosTop();
    u8g2.setFontMode(1);
    u8g2.setFontDirection(0);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(0);
    
    //DRAW STARTUP IMAGE
    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 16, doge_width, doge_height, doge_bits);
    u8g2.sendBuffer();

    delay(1000); //so people can see it
    
    dispVoltages();
}

void loop(){
    for(int i = 0; i < check_sec; i++){
        dispVoltages();
        delay(1000);
        timesincerefresh++;
    }
    
    //every minute switch, check/switch, switch and charge update
    //flash # of charged
    cycleBatt();
    timesincerefresh = 0;
}

