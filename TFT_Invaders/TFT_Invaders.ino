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

//Pins:
#define LEFT 2    //Left Button
#define MIDDLE 4  //Middle Button
#define RIGHT 7   //Right Button
#define BUZZ 5    //Buzzer / Speaker


//Global variables:
//Options marked with "Changeable" can be customized:

int invaders[] = {4,4,4,4,4};                  //Changeable
int invadersLev1[] = {4,4,4,4,4};              //This must be the same as invaders[]
int score = 0;
int level = 1;
int PlayerX = TFTwidth/2;
int PlayerY = TFTheight-15;
int PlayerSize = 8;                             //Changeable
int invadersX = 0;
int invadersY = 0;
int ArrSize = sizeof(invaders) / sizeof(int);
int PlayerMoveAmount = 4;                       //Changeable
int InvadersMoveAmount = 4;                     //Changeable
int InvaderSize = 6;                            //Changeable
unsigned int LaserColor = YELLOW;               //Changeable
unsigned int InvadersColor = RED;               //Changeable
unsigned int PlayerColor = GREEN;               //Changeable
unsigned int ScoreColor = WHITE;                //Changeable
unsigned int ScoreBackroundColor = BLUE;        //Changeable
unsigned int BackroundColor = BLACK;            //Changeable
unsigned int WinColor = GREEN;                  //Changeable
unsigned int LoseColor = RED;                   //Changeable


//Functions:
void intro(){
  tft.fillScreen(GREEN);
  tft.setCursor(10,50);
  tft.setTextColor(BLUE, BLACK);
  tft.setTextSize(2.5);
  tft.print("TFT       INVADERS");
  delay(1000);
}
  
void winAnimation(){
    delay(100);
    tft.fillScreen(WinColor);
    delay(100);
    tft.fillScreen(BackroundColor);
}

void loseAnimation(){
  tft.fillScreen(LoseColor);
  tft.setCursor(10,50);
  tft.setTextColor(BLUE, BLACK);
  tft.setTextSize(2.5);
  tft.print("GAME      OVER");
  delay(500);
  tft.setCursor(0,90);
  tft.setTextSize(2);
  tft.println("Score: ");
  tft.println(score);
  tft.println("Level: ");
  tft.println(level);
}
  
void drawInvaders(int x,int y){
  int num;
  x++;
  y++;
  for(int a=0; a<ArrSize; a++){
    num=invaders[a];
    for(int b=0; b<num; b++){
      tft.drawRect(x, y+((InvaderSize+1)*b), InvaderSize, InvaderSize, InvadersColor);
    }
    x=x+(InvaderSize+1);
  }
}

void drawPlayer(){
  tft.fillTriangle(PlayerX-PlayerSize, PlayerY, PlayerX, PlayerY-PlayerSize, PlayerX+PlayerSize, PlayerY, PlayerColor);  
}

void drawInfo(){
  tft.setCursor(0,(TFTheight-10));
  tft.setTextColor(ScoreColor, ScoreBackroundColor);
  tft.setTextSize(1);
  tft.print("Score: ");
  tft.print(score);
  tft.print(" Level: ");
  tft.print(level);
}

int getColumn(int InvX, int x){
  for(int a=0; a<ArrSize; a++){
    if(x>=InvX+((InvaderSize+1)*a) && x<=InvX+((InvaderSize+1)*(a+1))){
      return a;
      break;
    }
  }
}

boolean isLevelDone(){
  int check = 0;
  for(int a=0; a<ArrSize; a++){
    if(invaders[a]==0){
      check=check+1;
    }
  }
  if(check==ArrSize){
    return true;
  }
  else{
    return false;
  }
}

boolean isGameOver(){
  int check = 0;
  for(int a=0; a<ArrSize; a++){
    if(invaders[a]>=check){
      check=invaders[a];
    }
  }
  if(invadersY>=TFTheight-(check*(InvaderSize+1)+17+PlayerSize)){
    return true;
  }
  else{
    return false;
  }
}
  
void setup(void) {
    pinMode(LEFT, INPUT); //Initialize pins
    pinMode(MIDDLE, INPUT);
    pinMode(RIGHT, INPUT);
    pinMode(BUZZ, OUTPUT);
    tft.initR(INITR_BLACKTAB); //Initialize the screen
    intro();
    tft.fillScreen(BackroundColor);
    invadersX=0;
    invadersY=0;
}

void loop(){
  if(isGameOver()==false){ //while the game is not over
  
    //Player's movements
    if(digitalRead(LEFT)==HIGH ){
      PlayerX-=PlayerMoveAmount;
    }
    if(digitalRead(RIGHT)==HIGH ){
      PlayerX+=PlayerMoveAmount;
    }
    if(digitalRead(MIDDLE)==HIGH ){
      tone(BUZZ, 'F', 100);                                             //Play a sound
      int column=getColumn(invadersX, PlayerX);                         //Find which column of invaders that the player is aiming at
      tft.drawFastVLine(PlayerX, 0, PlayerY-PlayerSize, LaserColor);    //Display the laser
      if(column<ArrSize){                                               //If the column is valid (within the formation of invaders)
        if(invaders[column]>0){                                         //If there are invaders in that column
          invaders[column]-=1;                                          //Remove one invader from the column
          score+=5;                                                     //Increment the score
        }
      }
    }
  
    //Invaders' movements
    invadersX+=InvadersMoveAmount;                        //Move the invaders left
    if(invadersX>=(TFTwidth-(ArrSize*(InvaderSize+1)))){  //If the invasers are at the edge of the screen
      invadersY+=7;                                       //Move the invaders down
      invadersX=0;                                        //Move the invaders back all the way
    }
    
    //Drawing to screen
    drawInvaders(invadersX,invadersY);    //Dispay invaders
    drawPlayer();                         //Display Player
    drawInfo();                           //Display Score and level
    tft.fillScreen(BackroundColor);       //Blank everything
    
    
    if(isLevelDone()==true){              //Check if level is done
      winAnimation();
      level=level+1;
      invadersX=0;
      invadersY=0;
      for(int a=0; a<ArrSize; a++){
        if(invaders[a]<=15){
          invaders[a]=4+level;
        }
      }
    }
  }
  else{
    loseAnimation();
    delay(5000);
    //Reset the game
    intro();
    tft.fillScreen(BackroundColor);
    invadersX=0;
    invadersY=0;
    score=0;
    level=1;
    for(int a=0; a<ArrSize; a++){
      invaders[a] = invadersLev1[a];
    }
  }
}
