/*
 * Project 3PawnGame_RoomController
 * Description: A smart room controller that reads an active 3-pawn game to control hue lights and wemos
 * Author: Caden Gamache
 * Date: 03/01/2023 - 03/09/2023
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

#include <IoTClassroom_CNM.h>
#include <Adafruit_SSD1306.h>
#include <neopixel.h>

float actualRead [3][3];  //untouched array storing the volages read
float nominalRead [3][3]; //rounded array based on actual read
//the cordinates for the center of every square. x (even columns 0,2,4) and y (odd columns 1,3,5)
int piecePos [3][6] {
  {15, 48, 15, 31, 15, 15},
  {31, 48, 31, 31, 31, 15},
  {48, 48, 48, 31, 48, 15}
};
const int COLORS [] {0,62309,59086,53714,48343,45120,42971}; //gradient from red to blue (HSV)
const int NEOCOLORS [] {0xFF0000,0xD5002B,0xAA0055,0x800080,0x5500AA,0x2B00D5,0x0000FF}; //gradient from red to blue (RGB)
int i,j,k;
int currentRow,currentColor; //variables called in multiple functions
int redPiece,bluePiece; //stores the number of red (1's) or blue (2's) pieces
int wemo1 = 2,wemo2 = 3;

void checkPiecePos ();
void rowSetup ();
void determineAdvantage ();
void drawBoard();
void drawPieces();

Adafruit_NeoPixel neopix(12,A4,WS2812B);
Adafruit_SSD1306 display(D2);


void setup() {

  WiFi.on();
  WiFi.setCredentials("IoTNetwork");
  WiFi.connect();
  //forces the system to wait for the wifi to connect
  while(WiFi.connecting()) {
  delay(1);
  }

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
  //adjusts hues if either side achieves victory
  if (nominalRead[0][0] == 2 || nominalRead[0][1] == 2 || nominalRead[0][2] == 2) {
    for (i=0;i<7;i++) {
      setHue(i,true,COLORS[6],255,255);
    }
    pixelFill(0,12,NEOCOLORS[6]);
    switchON(wemo1);
    delay(5000);
  }
  if (nominalRead[2][0] == 1 || nominalRead[2][1] == 1 || nominalRead[2][2] == 1) {
    for (i=0;i<7;i++) {
      setHue(i,true,COLORS[0],255,255);
    }
    pixelFill(0,12,NEOCOLORS[0]);
    switchON(wemo2);
    delay(5000);
  }
  //on-going (adjust hues based on the active game)
  else {
    for (i=0;i<7;i++) {
      setHue(i,true,COLORS[currentColor],100,255);
    }
    switchOFF(wemo1);
    switchOFF(wemo2);
    pixelFill(0,12,NEOCOLORS[currentColor]);
  }
}
//looks through the nominal read and adds a red or blue piece if the values match
void determineAdvantage () {
  //reset values
  redPiece = 0;
  bluePiece = 0;
  //read the number of red and blue pieces
  for (j=0;j<3;j++) {
    for (i=0;i<3;i++) {
      if (nominalRead[j][i] == 1) {redPiece ++;}
      if (nominalRead[j][i] == 2) {bluePiece ++;}
    }
  }
  //adjust colors
  if (redPiece > bluePiece) {
    currentColor = 3 - (redPiece - bluePiece); 
  }
  if (redPiece < bluePiece) {
    currentColor = 3 + (bluePiece - redPiece);
  }
  if (redPiece == bluePiece) {
    currentColor = 3;
  }
  Serial.printf("\nNumRed = %i NumBlue = %i",redPiece,bluePiece);
}
//reads the voltage of every square and stores them in an array. It then converts the stored floats into 3 values
void checkPiecePos () {
  //reads all piece positions and determines their side
  for (j=0;j<3;j++) {
    currentRow = j;
    rowSetup ();
    for (i=0;i<3;i++) {
      nominalRead [i][j] = analogRead(i+17)/723.0;
      actualRead [i][j] = nominalRead [i][j];
      //converts to three values
      if (nominalRead [i][j] > 1 && nominalRead [i][j] < 2) {nominalRead [i][j] = 1;}
      if (nominalRead [i][j] > 2) {nominalRead [i][j] = 2;}
      if (nominalRead [i][j] < 1) {nominalRead [i][j] = 0;}
    }
  }
  //these for loops flip the array values for some clarity when reading the serial monitor
  for (i=2;i>=0;i--) {
    Serial.printf("\ncol1 = %.2f col2 = %.2f col3 = %.2f",actualRead[i][0],actualRead[i][1],actualRead[i][2]);
  }
  for (i=2;i>=0;i--) {
    Serial.printf("\nPieceSide = %.2f PieceSide = %.2f PieceSide = %.2f",nominalRead[i][0],nominalRead[i][1],nominalRead[i][2]);
  }
}
//switches the D pins between input/output and high/low turning one on at a time (setting to low is required for consistent reading)
void rowSetup () { 
  //first column
  if (currentRow == 0) {
    pinMode(D8,OUTPUT);
    pinMode(D7,INPUT);
    pinMode(D6,INPUT);
    digitalWrite(D8,HIGH);
    digitalWrite(D7,LOW);
    digitalWrite(D6,LOW);
  }
  //second column
  if (currentRow == 1) { 
    pinMode(D8,INPUT);
    pinMode(D7,OUTPUT);
    pinMode(D6,INPUT);
    digitalWrite(D8,LOW);
    digitalWrite(D7,HIGH);
    digitalWrite(D6,LOW); 
  }
  //third column
  if (currentRow == 2) {
    pinMode(D8,INPUT);
    pinMode(D7,INPUT);
    pinMode(D6,OUTPUT);
    digitalWrite(D8,LOW);
    digitalWrite(D7,LOW);
    digitalWrite(D6,HIGH);
  }
}
//resets the board on the display (without showing changes)
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
  //text and symbols
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(68,16);
  display.printf("red  = ");
  display.setCursor(68,43);
  display.printf("blue = ");
  display.fillCircle(118,18,4,WHITE);
  display.fillTriangle(118,41,114,49,122,49,WHITE);
}
//reads the nominal array and puts a circle or triangle in the correct square
void drawPieces () {
  //ensures the color swaps between black and white with every square
  bool whiteBlack;
  for (j=0;j<3;j++) {
    k = 0;
    for (i=0;i<3;i++) {
      whiteBlack = !whiteBlack;
      //checks if there is a red piece in the nominal read array and puts it in the correct square
      if (nominalRead [i][j] == 1) {
        display.fillCircle(piecePos[j][k],piecePos[j][k+1],4,whiteBlack);
        Serial.printf(" piece found");
      }
      //checks if there is a blue piece in the nominal read array and puts it in the correct square
      if (nominalRead [i][j] == 2) {
        display.fillTriangle(piecePos[j][k],piecePos[j][k+1]-4,piecePos[j][k]-4,piecePos[j][k+1]+4,piecePos[j][k]+4,piecePos[j][k+1]+4,whiteBlack);
        Serial.printf(" piece found");
      }
      Serial.printf("\nk = %i i = %i j = %i",k,i,j);
      //k increase keeps x and y cordinates seperated in the array
      k+=2;
    }
  }
}
//takes a start and end position and runs through a for loop until all pixels are on
void pixelFill(int start, int end,int color) {
  int neopixNum;
  for ( neopixNum = start; neopixNum < end; neopixNum ++) {
    neopix.setPixelColor(neopixNum, color);
    neopix.show();
  } 
}