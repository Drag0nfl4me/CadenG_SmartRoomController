/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/cgame/Documents/IoT/CadenG_SmartRoomController/3PawnGame_RoomController/src/3PawnGame_RoomController.ino"
/*
 * Project 3PawnGame_RoomController
 * Description: A smart room controller that reads an active 3-pawn game to control lights and wemos
 * Author: Caden Gamache
 * Date: 03/01/2023
 */

void setup();
void loop();
void drawBoard ();
void drawPieces ();
int pixelFill(int start, int end,int color);
#line 8 "c:/Users/cgame/Documents/IoT/CadenG_SmartRoomController/3PawnGame_RoomController/src/3PawnGame_RoomController.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

#include <IoTClassroom_CNM.h>
#include <Adafruit_SSD1306.h>
#include <neopixel.h>

float nominalRead [3][3];
float actualRead [3][3];
int piecePos [3][6] {
  {15, 48, 15, 31, 15, 15},
  {31, 48, 31, 31, 31, 15},
  {48, 48, 48, 31, 48, 15}
};
int i,j,k,currentRow ,currentColor, currentAngle;
int redPiece,bluePiece;
int neopixNum;
const int COLORS [] {0,62309,59086,53714,48343,45120,42971};
const int NEOCOLORS [] {0xFF0000,0xD5002B,0xAA0055,0x800080,0x5500AA,0x2B00D5,0x0000FF};

void checkPiecePos ();
void rowSetup ();
void determineAdvantage ();
void drawBoard();
void drawPieces();

Adafruit_NeoPixel neopix(12,A4,WS2812B);
Adafruit_SSD1306 display(D2);

void setup() {

  // WiFi.on();
  // WiFi.setCredentials("IoTNetwork");
  // WiFi.connect();
  // while(WiFi.connecting()) {
  // delay(1);
  // }

  neopix.setBrightness(8);
  neopix.show();

  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.clearDisplay();
  display.display();

  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(D8,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D6,OUTPUT);
}

void loop() {
  checkPiecePos();
  determineAdvantage();
  drawBoard();
  drawPieces();
  display.display();
  //victory
  if (nominalRead[0][0] == 2 || nominalRead[0][1] == 2 || nominalRead[0][2] == 2) {
    setHue(3,false,COLORS[currentColor],100,255);
    pixelFill(0,12,NEOCOLORS[6]);
    delay(5000);
  }
  if (nominalRead[2][0] == 1 || nominalRead[2][1] == 1 || nominalRead[2][2] == 1) {
    setHue(3,true,21485,100,255);
    pixelFill(0,12,NEOCOLORS[0]);
    delay(5000);
  }
  //on-going
  else {
    setHue(3,true,COLORS[currentColor],100,255);
    pixelFill(0,12,NEOCOLORS[currentColor]);
  }
}

void determineAdvantage () {
  //reset values
  redPiece = 0;
  bluePiece = 0;
  //read number of red and blue pieces
  for (j=0;j<3;j++) {
    for (i=0;i<3;i++) {
      if (nominalRead[j][i] == 1) {redPiece ++;}
      if (nominalRead[j][i] == 2) {bluePiece ++;}
    }
  }
  //adjust colors and servo angle
  if (redPiece > bluePiece) {
    currentColor = 3 - (redPiece - bluePiece); 
    currentAngle = 3 - (redPiece - bluePiece);
  }
  if (redPiece < bluePiece) {
    currentColor = 3 + (bluePiece - redPiece);
    currentAngle = 3 + (bluePiece - redPiece);
  }
  if (redPiece == bluePiece) {
    currentColor = 3;
    currentAngle = 3;
  }
  Serial.printf("\nNumRed = %i NumBlue = %i",redPiece,bluePiece);
}

void checkPiecePos () {
  //reads all piece positions and determines side
  for (j=0;j<3;j++) {
    currentRow = j;
    rowSetup ();
    for (i=0;i<3;i++) {
      nominalRead [i][j] = analogRead(i+17)/783.0;
      actualRead [i][j] = nominalRead [i][j];
      //converts to three values
      if (nominalRead [i][j] > 1 && nominalRead [i][j] < 2) {nominalRead [i][j] = 1;}
      if (nominalRead [i][j] > 2) {nominalRead [i][j] = 2;}
      if (nominalRead [i][j] < 1) {nominalRead [i][j] = 0;}
    }
  }
  for (i=2;i>=0;i--) {
    Serial.printf("\ncol1 = %.2f col2 = %.2f col3 = %.2f",actualRead[i][0],actualRead[i][1],actualRead[i][2]);
  }
  for (i=2;i>=0;i--) {
    Serial.printf("\nPieceSide = %.2f PieceSide = %.2f PieceSide = %.2f",nominalRead[i][0],nominalRead[i][1],nominalRead[i][2]);
  }
}

void rowSetup () { 
  if (currentRow == 0) {
    pinMode(D8,OUTPUT);
    pinMode(D7,INPUT);
    pinMode(D6,INPUT);
    digitalWrite(D8,HIGH);
    digitalWrite(D7,LOW);
    digitalWrite(D6,LOW);
  }
  if (currentRow == 1) { 
    pinMode(D8,INPUT);
    pinMode(D7,OUTPUT);
    pinMode(D6,INPUT);
    digitalWrite(D8,LOW);
    digitalWrite(D7,HIGH);
    digitalWrite(D6,LOW); 
  }
  if (currentRow == 2) {
    pinMode(D8,INPUT);
    pinMode(D7,INPUT);
    pinMode(D6,OUTPUT);
    digitalWrite(D8,LOW);
    digitalWrite(D7,LOW);
    digitalWrite(D6,HIGH);
  }
}

void drawBoard () {
  display.clearDisplay();
  //left edge
  display.drawLine(6,6,6,57,WHITE);
  //right edge
  display.drawLine(57,6,57,57,WHITE);
  //top edge
  display.drawLine(6,6,57,6,WHITE);
  //bottom edge
  display.drawLine(6,57,57,57,WHITE);
  //white squares
  display.fillRect(24,7,16,16,WHITE);
  display.fillRect(7,24,16,16,WHITE);
  display.fillRect(41,24,16,16,WHITE);
  display.fillRect(24,41,16,16,WHITE);

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(68,16);
  display.printf("red  = ");
  display.setCursor(68,43);
  display.printf("blue = ");
  display.fillCircle(118,18,4,WHITE);
  display.fillTriangle(118,41,114,49,122,49,WHITE);
}

void drawPieces () {
  bool whiteBlack;
  for (j=0;j<3;j++) {
    k = 0;
    for (i=0;i<3;i++) {
      whiteBlack = !whiteBlack;
      if (nominalRead [i][j] == 1) {
        display.fillCircle(piecePos[j][k],piecePos[j][k+1],4,whiteBlack);
        Serial.printf(" piece found");
      }
      if (nominalRead [i][j] == 2) {
        display.fillTriangle(piecePos[j][k],piecePos[j][k+1]-4,piecePos[j][k]-4,piecePos[j][k+1]+4,piecePos[j][k]+4,piecePos[j][k+1]+4,whiteBlack);
        Serial.printf(" piece found");
      }
      Serial.printf("\nk = %i i = %i j = %i",k,i,j);
      k+=2;
    }
  }
}

int pixelFill(int start, int end,int color) {

  for ( neopixNum = start; neopixNum < end; neopixNum ++) {

    neopix.setPixelColor(neopixNum, color);
    neopix.show();

  }
    
  return end;

}