/***************************************************************************//**
  @file     SDHC.h
  @brief    Driver MCAL para SDHC
  @author   Grupo 2
 ******************************************************************************/
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "./board.h"
#include "Arduino.h"
#include "LedControl.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
uint32_t    timeNow;
uint32_t    timePrev;
uint8_t     countPressButton;
bool        pressed_yellow;
bool        pressed_red; 
bool        led_prendido;
LedControl  display8digits = LedControl(DIN, CLK, CS, 1); //Din = 12, Clck = 10, CS = 11, Number of devices = 1

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void setup() {
    pinMode(BUTTON_R, INPUT); //botones: activo bajo (presionado: 0)
    pinMode(BUTTON_Y, INPUT);
    pinMode(BUZZER, OUTPUT); //buzzer y leds: activo alto (se prenden/activan con un 1)
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    pinMode(LED6, OUTPUT);
    pinMode(LED7, OUTPUT);
    pinMode(LED8, OUTPUT);
    pinMode(LED9, OUTPUT);
    led_prendido = false;
    countPressButton = 0;

    display8digits.shutdown(0, false);
    display8digits.setIntensity(0,8);
    display8digits.clearDisplay(0);
  //Serial.begin(9600); //creo que hace falta porque se comunica por spi con el modulo de displays
}

void loop(){






    timeNow = millis();
    if (timeNow-timePrev > 2000){
        timePrev = timeNow;
        led_prendido = led_prendido ? false : true;
        digitalWrite(LED3, led_prendido ? HIGH : LOW);
        digitalWrite(LED4, led_prendido ? HIGH : LOW);
        digitalWrite(LED5, led_prendido ? HIGH : LOW);
        digitalWrite(LED6, led_prendido ? HIGH : LOW);
        digitalWrite(LED7, led_prendido ? HIGH : LOW);
        digitalWrite(LED8, led_prendido ? HIGH : LOW);
        digitalWrite(LED9, led_prendido ? HIGH : LOW);  
        if(led_prendido){
          tone(BUZZER,785); //primer beep, se prenden 3 leds
        }
        else{
          noTone(BUZZER);
        }
    }

    if(digitalRead(BUTTON_R)==LOW){
      pressed_red = true;
      digitalWrite(LED1, HIGH);
    }
    if(pressed_red == true && digitalRead(BUTTON_R) == HIGH){
      countPressButton++;
      pressed_red = false;
    }  
    
    if(digitalRead(BUTTON_Y)==LOW){
      pressed_yellow = true;
      digitalWrite(LED2, HIGH);
    }
    if(pressed_yellow == true && digitalRead(BUTTON_Y) == HIGH){
      countPressButton++;
      pressed_yellow = false;
    }
  
    for (uint8_t i = 0; i < 8; i++){
      display8digits.setDigit(0, i, countPressButton, false); //imprime por ej: 2.47 (2mins, 47seg)
    }
    


    



}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
