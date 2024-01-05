//#include <Arduino.h>
#include <U8g2lib.h>

#define check_sec 600         //How many seconds to wait before next refresh
#define charge_thresh 13.8    //Voltage of charged battery
#define max_thresh 19         //Voltage of charger (no battery connected
#define min_thresh 11.5       //Voltage of completely discharged battery
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

#define lightning_width 6
#define lightning_height 9
static const unsigned char lightning_bits[] U8X8_PROGMEM = {
  0x18, 0x0C, 0x06, 0x03, 0x3F, 0x30, 0x18, 0x0C, 0x06, };

ISR(PCINT2_vect){
    cycleBatt();
    timesincerefresh = 0;
}

void cycleBatt(){
    noInterrupts();
    for(uint8_t j = 0; j < 2; j++){
        //updateCharges and flip batteries
        for(uint8_t i = 0; i < 4; i++){
            charge[i][(uint8_t)(state[i])] = analogRead(i*2+state[i])*21.5/1024.f;//*coefficients[i][(uint8_t)(state[i])];
        }
        //printVoltages();
        for(uint8_t i = 0; i < 4; i++){
            state[i] = !state[i];
            digitalWrite(i+2, state[i]);
        }
        delay(200);
    }

    printVoltages();

    //update charge array and swap battery if charged
    for(int i = 0; i < 4; i++){
        if((charge[i][0] <= dc_thresh || charge[i][0] >= max_thresh) && (charge[i][1] <= dc_thresh || charge[i][1] >= max_thresh))//if both disconnected default idx#1
            state[i] = true;
        else if(charge[i][0] >= charge_thresh && (charge[i][1] <= dc_thresh || charge[i][1] >= max_thresh))//idx#0 charged, swap to unplugged
            state[i] = true;//idx#1
        else if((charge[i][0] <= dc_thresh || charge[i][0] >= max_thresh) && charge[i][1] >= charge_thresh)//idx#1 charged, swap to unplugged
            state[i] = false;//idx#0
        else if(charge[i][0] >= charge[i][1] && charge[i][0] < charge_thresh) //if idx#0 batt highest+not charged
            state[i] = false;//idx#0
        else 
            state[i] = true;//idx#1
    }

    for(int i = 0; i < 4; i++)
        digitalWrite(i+2, state[i]);
    delay(200);//allow relays to swap
    
    //blinkCharged();
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

    /*
    //DRAW STARTUP IMAGE
    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 16, doge_width, doge_height, doge_bits);
    u8g2.sendBuffer();

    delay(1000); //so people can see it

    */
    
    //dispVoltages();
}

void loop(){
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
    delay(1000);
    timesincerefresh++;
    
    if(timesincerefresh >= check_sec){
        cycleBatt();
        timesincerefresh = 0;
    }
}

