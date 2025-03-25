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
#define MATRICES 0
#define NUM_LEDS 192

//-------------------Buzzer
#define BUZZER  9           //pin digital pwm

//-------------------Modulo Display
#define DIN     10          //pin digital
#define CS      11          //pin digital
#define CLK     12          //pin digital
#define N_DISP  1

//-------------------Buttons
#define BUTTON_Y  18          //pin digital 18  A4
#define BUTTON_R  19          //pin digital 19  A5

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* SD_H_ */
