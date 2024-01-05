#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define UPBUTTON 9
#define DOWNBUTTON 10
#define LEFTBUTTON 11
#define RIGHTBUTTON 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(256, PIN, NEO_GRB + NEO_KHZ800);
uint32_t L = strip.Color(0,0,0);
uint32_t R = strip.Color(255,0,0);
uint32_t G = strip.Color(0,255,0);
uint32_t B = strip.Color(0,0,255);
uint32_t M = strip.Color(255,0,255);
uint32_t C = strip.Color(0,255,255);
uint32_t Y = strip.Color(255,255,0);
uint32_t W = strip.Color(255,255,255);
int WormLength = 3;
int WormCoords[10][2] =  {{7,7},{8,7},{9,7},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
char WormDirections[10] = {'L','L','L','L','L','L','L','L','L','L'};
int AppleX = 0;
int AppleY = 0;
int NewAppleX = 0;
int NewAppleY = 0;
char GameOverBoard[16][16]={
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'},
{'L','R','R','L','L','L','R','L','L','R','L','R','L','R','R','R'},
{'R','L','L','L','L','R','L','R','L','R','R','R','L','R','L','L'},
{'R','L','R','R','L','R','R','R','L','R','L','R','L','R','R','R'},
{'R','L','L','R','L','R','L','R','L','R','L','R','L','R','L','L'},
{'L','R','R','L','L','R','L','R','L','R','L','R','L','R','R','R'},
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'},
{'L','R','L','L','R','L','R','L','R','R','R','L','R','R','L','L'},
{'R','L','R','L','R','L','R','L','R','L','L','L','R','L','R','L'},
{'R','L','R','L','R','L','R','L','R','R','R','L','R','R','L','L'},
{'R','L','R','L','R','L','R','L','R','L','L','L','R','L','R','L'},
{'L','R','L','L','L','R','L','L','R','R','R','L','R','L','R','L'},
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'},
{'L','R','L','R','L','R','L','R','L','R','L','R','L','R','L','R'},
{'R','L','R','L','R','L','R','L','R','L','R','L','R','L','R','L'},
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'}};

char StartBoard[16][16]={
{'R','L','R','L','L','Y','L','L','G','G','L','L','C','L','C','L'},
{'R','L','R','L','Y','L','Y','L','G','L','G','L','C','C','C','L'},
{'R','L','R','L','Y','L','Y','L','G','G','L','L','C','L','C','L'},
{'R','R','R','L','Y','L','Y','L','G','L','G','L','C','L','C','L'},
{'R','L','R','L','L','Y','L','L','G','L','G','L','C','L','C','L'},
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'},
{'B','B','L','M','M','L','R','R','L','L','Y','Y','L','L','G','G'},
{'B','B','L','M','L','L','R','L','L','Y','L','L','L','G','L','L'},
{'B','L','L','M','L','L','R','R','L','L','Y','L','L','L','G','L'},
{'B','L','L','M','L','L','R','L','L','L','L','Y','L','L','L','G'},
{'B','L','L','M','L','L','R','R','L','Y','Y','L','L','G','G','L'},
{'L','L','L','L','L','L','L','L','L','L','L','L','L','L','L','L'},
{'C','C','L','B','B','B','L','L','M','L','L','L','L','G','G','G'},
{'C','L','L','L','B','L','L','M','L','M','L','R','R','L','G','L'},
{'L','C','L','L','B','L','L','M','M','M','L','R','L','L','G','L'},
{'C','C','L','L','B','L','L','M','L','M','L','R','L','L','G','L'}};

char UpdateDirections(char directions[]){
  int x=9;
  while(x>0){
    directions[x]=directions[x-1];
    x--;
  }
}

char UpdateCoords(int Coords[][2], char directions[]){
  int x=0;
  while(x<10){
    if(Coords[x][0]>0 | Coords[x][1]>0){
      switch(directions[x]){
        case 'L':
          Coords[x][0]--;
          break;
        case 'R':
          Coords[x][0]++;
          break;
        case 'U':
          Coords[x][1]--;
          break;
        case 'D':
          Coords[x][1]++;
          break;
      }
    }
    x++;
  }
}

char DrawWorm(int Coords[][2], char WormColor, char pixels[16][16]){
  int x=0;
  while(x<10){
    if(Coords[x][0]>=0 & Coords[x][1]>=0){
      pixels[Coords[x][1]][Coords[x][0]] = WormColor;
    }
    x++;
  }
  Serial.println(' ');
}

void DrawBoard(char pixels[16][16]){
  int x=1;
  while(x<16){
      for (int i=0, j = 15; i< 8; i++, j--){
    char temp = pixels[x][i];
    pixels[x][i] = pixels[x][j];
    pixels[x][j] = temp;
  }
    x+=2;
  }
  x=0;
  while(x<16){
      for (int i=0, j = 15; i< 8; i++, j--){
    char temp = pixels[x][i];
    pixels[x][i] = pixels[x][j];
    pixels[x][j] = temp;
  }
    x+=1;
  }
  int a=0;
  int b=0;
  int c=0;
  while(b<16){
    a=0;
  while(a<16){
    //Serial.print(pixels[b][a]);
    switch(pixels[b][a]){
      case 'L':
        strip.setPixelColor(c, L);
        break;
      case 'R':
        strip.setPixelColor(c, R);
        break;
      case 'G':
        strip.setPixelColor(c, G);
        break;
      case 'B':
        strip.setPixelColor(c, B);
        break;
      case 'M':
        strip.setPixelColor(c, M);
        break;
      case 'C':
        strip.setPixelColor(c, C);
        break;
      case 'Y':
        strip.setPixelColor(c, Y);
        break;
      case 'W':
        strip.setPixelColor(c, W);
        break;
    }
    a++;
    c++;
  }
  b++;
  //Serial.println(" ");
  }
  strip.show();
  //Serial.println(' ');
  x=0; //REVERSE THE PIXELS BACK
  while(x<16){
      for (int i=0, j = 15; i< 8; i++, j--){
    char temp = pixels[x][i];
    pixels[x][i] = pixels[x][j];
    pixels[x][j] = temp;
  }
    x+=2;
  }
}


void StartGame(){
  int out = 0;
  DrawBoard(StartBoard);
  while(out==0){
      if(digitalRead(UPBUTTON)==LOW | digitalRead(DOWNBUTTON)==LOW | 
      digitalRead(LEFTBUTTON)==LOW | digitalRead(RIGHTBUTTON)==LOW) {
        out=1;
       }
  }
}

void Win(char pixels[16][16]){
  int a = 0;
  int b = 0;
  while(a<16){
    b=0;
    while(b<16){
      StartBoard[a][b] = 'G';
      b++;
    }
    a++;
  }
  DrawBoard(pixels);
  delay(1000);
  a = 0;
  b = 0;
  while(a<16){
    b=0;
    while(b<16){
      StartBoard[a][b] = 'L';
      b++;
    }
    a++;
  }
  DrawBoard(pixels);
  while(1==1){};
}


void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  Serial.begin(9600);
  pinMode(UPBUTTON, INPUT_PULLUP);
  pinMode(DOWNBUTTON, INPUT_PULLUP);
  pinMode(LEFTBUTTON, INPUT_PULLUP);
  pinMode(RIGHTBUTTON, INPUT_PULLUP);
  StartGame();
  int a = 0;
  int b = 0;
  while(a<16){
    b=0;
    while(b<16){
      StartBoard[a][b] = 'L';
      b++;
    }
    a++;
  }
  randomSeed(analogRead(0));
  AppleX = (int)random(0,17);
  AppleY = (int)random(0,17);
  NewAppleX = (int)random(0,17);
  NewAppleY = (int)random(0,17);
}

void loop(){
  while(NewAppleX==AppleX & NewAppleY==AppleY){
    NewAppleX = (int)random(0,17);
    randomSeed(analogRead(0));
    NewAppleY = (int)random(0,17);
  }
  StartBoard[AppleX][AppleY]='R';
  Serial.println(AppleX);
  Serial.println(AppleY);
  while(WormCoords[0][1]!=AppleX | WormCoords[0][0]!=AppleY){
  for(int a = 0; a<10; a++){
    if(digitalRead(UPBUTTON) == LOW){
      Serial.println("Up");
      if(WormDirections[0] !='D'){
        WormDirections[0] = 'U';
      }
    }
    if(digitalRead(LEFTBUTTON) == LOW){
      Serial.println("Left");
      if(WormDirections[0] !='R'){
        WormDirections[0] = 'L';
      }
    }
    if(digitalRead(DOWNBUTTON) == LOW){
      Serial.println("Down");
      if(WormDirections[0] !='U'){
        WormDirections[0] = 'D';
      }
    }
    if(digitalRead(RIGHTBUTTON) == LOW){
      Serial.println("Right");
      if(WormDirections[0] !='L'){
        WormDirections[0] = 'R';
      }
    }
   delay(50);
 }
  Serial.print("Length");
  Serial.println(WormLength);
  UpdateCoords(WormCoords, WormDirections);
  
  UpdateDirections(WormDirections);

  DrawWorm(WormCoords, 'G', StartBoard);
  if(WormCoords[0][0] < 0 | WormCoords[0][0] > 16 | WormCoords[0][1] < 0 | WormCoords[0][1] > 16){
    DrawBoard(GameOverBoard);
    while(1==1){};
  }
  for(int i=1;  i<WormLength; i++) {
      if (WormCoords[0][0]==WormCoords[i][0] & WormCoords[0][1]==WormCoords[i][1]) {
        DrawBoard(GameOverBoard);
        while(1==1){};
      }
  }
  DrawBoard(StartBoard);
  int a = 0;
  int b = 0;
  while(a<16){
    b=0;
    while(b<16){
      StartBoard[a][b] = 'L';
      b++;
    }
    a++;
  }
    StartBoard[AppleX][AppleY]='R';
  }
  Serial.println("----------------APPLE--------------");
  switch(WormDirections[WormLength-1]){
      case 'R':
        WormCoords[WormLength][0] = WormCoords[WormLength-1][0]-1;
        WormCoords[WormLength][1] = WormCoords[WormLength-1][1];
        break;
      case 'L':
        WormCoords[WormLength][0] = WormCoords[WormLength-1][0]+1;
        WormCoords[WormLength][1] = WormCoords[WormLength-1][1];
        break;
      case 'D':
        WormCoords[WormLength][0] = WormCoords[WormLength-1][0];
        WormCoords[WormLength][1] = WormCoords[WormLength-1][1]-1;
        break;
      case 'U':
        WormCoords[WormLength][0] = WormCoords[WormLength-1][0];
        WormCoords[WormLength][1] = WormCoords[WormLength-1][1]+1;
        break;
  }
  WormLength++;
  Serial.print("Length");
  Serial.println(WormLength);
  AppleX=NewAppleX;
  AppleY=NewAppleY;
  if(WormLength == 8){
    Win(StartBoard);
  }
}




