/***************************************************************************//**
  @file     SDHC.h
  @brief    Driver MCAL para SDHC
  @author   Grupo 2
 ******************************************************************************/
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "./board.h"
#include  "Arduino.h"

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
bool        led_prendido;
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
    pinMode(BUZZER,OUTPUT); //buzzer y leds: activo alto (se prenden/activan con un 1)
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
  //Serial.begin(9600); //creo que hace falta porque se comunica por spi con el modulo de displays
}

void loop(){
    timeNow = millis();
    if (timeNow-timePrev > 3000){
        timePrev = timeNow;
        led_prendido = led_prendido ? false : true;
        digitalWrite(LED1, led_prendido ? HIGH : LOW);
        digitalWrite(LED2, led_prendido ? HIGH : LOW);
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

    if(digitalRead(BUTTON_R)==LOW)
      digitalWrite(LED1, HIGH);

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
