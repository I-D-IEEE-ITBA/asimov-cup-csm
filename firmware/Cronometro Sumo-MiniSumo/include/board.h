/***************************************************************************//**
  @file     board.h
  @brief    Defines for pins in board 
  @author   Grupo 2
 ******************************************************************************/
#ifndef BOARD_H_
#define BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//-------------------LEDs
#define LED1    15          //pin digital 15  A1
#define LED2    14          //pin digital 14  A0
#define LED3    2           //pin digital 2
#define LED4    3           //pin digital 3
#define LED5    4           //pin digital 4
#define LED6    5           //pin digital 5
#define LED7    6           //pin digital 6
#define LED8    7           //pin digital 7
#define LED9    8           //pin digital 8

//-------------------Pin de Matrices
#define MATRICES  6
//#define WIDTH     24  //no se usan
//#define HEIGHT    8   //no se usan
//#define NUM_LEDS (WIDTH * HEIGHT) // no se usan, segments.h define NUM_LEDS
#define PIN_RGB_DATA 6  // Pin para los datos de la tira LED RGB

//-------------------Buzzer
#define BUZZER  9           //pin digital pwm

//-------------------Modulo Display
#define SDA 18
#define SCL 19

#define DIN     10          //pin digital
#define CS      11          //pin digital
#define CLK     12          //pin digital
#define N_DISP  1

//-------------------Buttons
#define BUTTON_1  A0        // Pin analógico A0 (pin 14)
#define BUTTON_2  A1        // Pin analógico A1 (pin 15) 
#define BUTTON_3  A2        // Pin analógico A2 (pin 16)

#define BUTTON_Y  18          //pin digital 18  A4
#define BUTTON_R  19          //pin digital 19  A5

//------------------DCF
#define DCF 5

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* SD_H_ */
