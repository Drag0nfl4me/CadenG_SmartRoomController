/*
 * Project 3PawnGame_RoomController
 * Description: A smart room controller that reads an active 3-pawn game to control lights and wemos
 * Author: Caden Gamache
 * Date: 03/01/2023
 */

SYSTEM_MODE(MANUAL);

#include <IoTClassroom_CNM.h>

float rowsRead [3][3];
float ead [3][3];
int i,j,currentRow,currentColor;
int pieceSide, photoRead;
int colors [] {0,62309,59086,53714,48343,45120,42971};
int redPiece,bluePiece;

void checkPiecePos ();
void rowSetup ();
void determineAdvantage ();

void setup() {

  WiFi.on();
  WiFi.setCredentials("IoTNetwork");
  WiFi.connect();
  while(WiFi.connecting()) {
  delay(1);
  }

  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(D8,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D6,OUTPUT);
  waitFor(Serial.isConnected,10000);
}

void loop() {

  checkPiecePos();
  determineAdvantage();

  if (redPiece > bluePiece) {currentColor = 3 - (redPiece - bluePiece);}
  if (redPiece < bluePiece) {currentColor = 3 + (bluePiece - redPiece);}
  if (redPiece == bluePiece) {currentColor = 3;}

  if (rowsRead[0][0] == 2 || rowsRead[0][1] == 2 || rowsRead[0][2] == 2) {
    setHue(3,false,colors[currentColor],100,255);
    delay(5000);
  }
  if (rowsRead[2][0] == 1 || rowsRead[2][1] == 1 || rowsRead[2][2] == 1) {
    setHue(3,true,21485,100,255);
    delay(5000);
  }
  else {
    setHue(3,true,colors[currentColor],100,255);
  }

  Serial.printf("\nNumRed = %i NumBlue = %i",redPiece,bluePiece);
  if (currentColor>6) {
    currentColor = 0;
  }

}

void determineAdvantage () {
  redPiece = 0;
  bluePiece = 0;
  for (j=0;j<3;j++) {
    for (i=0;i<3;i++) {
      if (rowsRead[j][i] == 1) {redPiece ++;}
      if (rowsRead[j][i] == 2) {bluePiece ++;}
    }
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

void checkPiecePos () {
  for (j=0;j<3;j++) {
    currentRow = j;
    rowSetup ();
    delay(1000);
    for (i=0;i<3;i++) {
      rowsRead [i][j] = analogRead(i+17)/903.0;
      ead [i][j] = rowsRead [i][j];
      if (rowsRead [i][j] > 1 && rowsRead [i][j] < 2) {rowsRead [i][j] = 1;}
      if (rowsRead [i][j] > 2) {rowsRead [i][j] = 2;}
      if (rowsRead [i][j] < 1) {rowsRead [i][j] = 0;}
    }
  }
  for (i=2;i>=0;i--) {
    Serial.printf("\ncol1 = %.2f col2 = %.2f col3 = %.2f",ead[i][0],ead[i][1],ead[i][2]);
  }
  for (i=2;i>=0;i--) {
    Serial.printf("\nPieceSide = %.2f PieceSide = %.2f PieceSide = %.2f",rowsRead[i][0],rowsRead[i][1],rowsRead[i][2]);
  }
}