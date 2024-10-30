#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(24, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_RGB            + NEO_KHZ800);


const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };



void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(1);
  matrix.setTextColor(colors[0]);
}

int x    = matrix.width();
int pass = 0;

//5V de funente externa 
//labo de micro
//brillo maximo 255 - 1.26A 
//brillo minimo 1 - 0.1A

//matrix ieee
//blanco
//brillo maximo 255 - 0.75A 
//brillo minimo 1 - 0.1A



void loop() {
  matrix.fillScreen( matrix.Color(255, 255, 255));
  matrix.setCursor(x, 0);
  //matrix.print(F("Hello Worldy"));//Hello World 
  if(--x < -72) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);


  
}