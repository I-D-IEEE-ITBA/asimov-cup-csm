#include "LedControl.h"
#include "board.h"
#include "../lib/segments/segments.h"
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define DEBUG 1

//LedControl display8digits = LedControl(DIN, CLK, CS, N_DISP); //Din = 12, Clck = 10, CS = 11, Number of devices = 1
//MAX7219 display8digits;
#define TRUE 1
#define FALSE 0

#define TIEMPO_COMPETENCIA_SEGS 180
#define TIEMPO_INFRACCION_SEGS 15

#define DEFAULT_BRIGHTNESS 100 // Default brightness for the display

//CONEXIONES IMPORTANTES: para el display, para Arduino Uno: VCC a +5V, GND a GND, DIN a pin 12, CS a pin 11, CLK a pin 10


const int button1   = BUTTON_1; //boton rojo arranca el cronometro de la pelea
const int button2   = BUTTON_2; //boton amarillo
const int button3   = BUTTON_3; //boton amarillo
const int led1      = LED1; //luces que se prenden en secuencia al inicio.
const int led2      = LED2; //luces que se prenden en secuencia al inicio.
const int led3      = LED3; //conectar, del modulo del transistor: hembra marron a gnd, hembra rojo a 5v arduino (ver esquematico,
const int led4      = LED4; //muesquita para arriba). Despues, pin 1 modulo transistores a 2 leds, pin 2 a 1 led, pin 3 a 1 led,
const int led5      = LED5; //pin 4 a 2 leds, pin 5 a 2 leds, pin 6 a 1 led.
const int led6      = LED6; //luces que se prenden en secuencia al inicio.
const int led7      = LED7;
const int led8      = LED8; //luces que se prenden en secuencia al inicio.
const int led9      = LED9;
const int buzzer    = BUZZER; //buzzer

const int matrices = MATRICES;

// Display Contador de Futbol DCF
CRGB leds[NUM_LEDS];
DisplayManager displayManager(leds);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(24, 8, matrices,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_RGB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(255, 255, 0), matrix.Color(0, 255, 0) };

int button1State = 0;
int button1PrevState = 0;
int button1Pressed = 0; //boton rojo presionado
int button2State = 0;
int button2PrevState = 0;
int button2Pressed = 0; //boton amarillo presionado
int button3State = 0;
int button3PrevState = 0;
int button3Pressed = 0; //boton verde presionado
unsigned long timeNow = 0;
unsigned long timePrev = 0;
unsigned long delaytime=250;

  
unsigned long timer0Init = 0; //Timer de tiempo para acomodar (cuenta hasta 30 seg).
                              //minutos y segundos. Activado, pausado y reseteado con boton Rojo.
unsigned long timer1Init = 0; //Timer de tiempo de pelea (cuenta hasta 3 min).
                              //minutos y segundos. Activado, pausado y reseteado con boton rojo.
unsigned long timer2Init = 0; //Timer de tiempo de infraccion (cuenta hasta 15 seg).
                              //segundos y centesimas de segundo. Activado, pausado y reseteado con boton amarillo.
unsigned long timerPausa = 0; //Timer de tiempo de pausa (cuenta tiempo de pausa).
unsigned long timer5Init = 0; //Timer de tiempo de inicio de 5seg.


int estado1 = 0; //0: presionar boton rojo para comenzar. 1: contando. 2: pausado, presionar boton rojo para volver a estado 0.
int estado2 = 0; //0: presionar boton amarillo para comenzar a contar. 1: contando.
int estado3 = 0; //0: presionar boton verde para comenzar a contar. 1: contando. 

int flagStart = 0;
int matriz1 = 0;
int matriz2 = 8;
int matriz3 = 16;
int primeros5 = 0;
int flagInEstado0 = 1;

//function prototypes
void writeTimer1(unsigned long timerInit);
void writeTimer2(unsigned long timerInit, bool on, int duracion);
//void writeTimerReset(int timer_id);
void writeTimerReset(void);
void irPrendiendoLeds(unsigned long timer1Init);
void ledsReset(void);
void readySetGo(void);
void finCompetencia(void);
char toChar(int num);
void prenderMatriz(int matriz, int r, int g, int b);
void apagarMatriz(int matriz);
bool showDigit(int digit, int startIdx, CRGB color);


//---------------------------------------------------------------------//
//---------------------------------------------------------------------//

void setup() {
/*
  display8digits.shutdown(0, false);
  display8digits.setIntensity(0,8);
  display8digits.clearDisplay(0);

*/
  
  pinMode(button1, INPUT_PULLUP); //botones: activo bajo (presionado: 0)
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(buzzer,OUTPUT); //buzzer y leds: activo alto (se prenden/activan con un 1)
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(led4,OUTPUT);
  pinMode(led5,OUTPUT);
  pinMode(led6,OUTPUT);
  pinMode(led7,OUTPUT);
  pinMode(led8,OUTPUT);
  pinMode(led9,OUTPUT);

  Serial.begin(9600); //creo que hace falta porque se comunica por spi con el modulo de displays

  // Initialize LED strip and display system
  FastLED.addLeds<WS2812B, PIN_RGB_DATA, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  displayManager.initialize(DEFAULT_BRIGHTNESS);


  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(60);
  matrix.setTextColor(colors[0]);
}

//---------------------------------------------------------------------//
//---------------------------------------------------------------------//

void loop() {
  timeNow = millis();
  //------------------lectura de botones cada 50ms----------------------------//
  	if (timeNow-timePrev>50) //leer botones cada 50 milisegs para evitar rebotes.
  	{
    timePrev=timeNow;
    button1PrevState = button1State;
    button2PrevState = button2State;
    button3PrevState = button3State;
    button1State = digitalRead(button1);
    button2State = digitalRead(button2);
    button3State = digitalRead(button3);

    if(button1State==LOW && button1PrevState==HIGH) //detectar flancos ascendentes: boton presionado
    {
      button1Pressed=1;
    }
    if(button2State==LOW && button2PrevState==HIGH)
    {
      button2Pressed=1;
    }
    if(button3State==LOW && button3PrevState==HIGH)
    {
      button3Pressed=1;
    }
  }
  

  //----------------------------estado 0--------------------------------------//
  //-------------------estado de reposo-sin comenzar--------------------------//
  if (estado1==0) //presionar boton rojo para comenzar
  {
    // Aca iria un flag para no reiniciar timers  
    if(flagInEstado0){
      Serial.println("Entro a estado 0. Apago Matrices");
      apagarMatriz(matriz1);
      apagarMatriz(matriz2);
      apagarMatriz(matriz3);
      if(!flagStart) {
        Serial.println("Reseteo Timer1 por primera vez");
        writeTimerReset(); //resetea el flag para que no se vuelva a ejecutar la secuencia de semaforo
      }
      flagInEstado0 = 0;
    }  
    
    estado2 = 0; //timer 2 en estado 'presionar boton amarillo para comenzar' (solo puede comenazar cuando estado1=1, 'contando').
    button2Pressed = 0;

    if (button1Pressed) //boton rojo presionado
	//----------------------------comenzar--------------------------------------//
    {
      estado1 = 3; 
      button1Pressed = 0; 
      timer0Init = millis();
      flagInEstado0 = 1;
    }
	//Elegir Apoyaron-Iniciar
	//se ejecuta una 
//----------|----------|


  }

//----------------------------estado 1--------------------------------------//
//----------------------------En pelea--------------------------------------//
  else if (estado1==1) //timer 1: contando
  {
    if(primeros5){
      Serial.println("Primeros 5 segundos");
      if((millis()-timer5Init)/1000 >= 5){
        Serial.println("Terminaron los primeros 5 segundos y se apagan matrices");
        primeros5 = 0;
        tone(buzzer, 785, 1000);
        apagarMatriz(matriz1);
        apagarMatriz(matriz2);
        apagarMatriz(matriz3);
      }
    }
    else{
      //si se pone aca apagar matriz, se ejecuta siempre que hayan pasado los 5 segundos     
    }

    writeTimer1(timer1Init); //imprime tiempo de pelea

  //irPrendiendoLeds(timer1Init); //prende leds a medida que pasa el tiempo de pelea.
	//semaforo 

	//Pelea - robots enganchados - 
    //Logica del boton 2 y timer 2
    if (estado2 == 0){ //presionar boton amarillo para comenzar
	  //No enganchados
      if (button2Pressed) //boton amarillo presionado
      {
        button2Pressed = 0;
        estado2 = 1; //pasa a 'contando enganchados'
        timer2Init = millis();
      }
    }
    else if (estado2 == 1) //timer 2: contando enganchados
    {
	  //enganchados
      writeTimer2(timer2Init, 1, 15);
      if (button2Pressed) //boton amarillo presionado
      {
		  //Desenganchas - sigue normal
        apagarMatriz(matriz1);
        apagarMatriz(matriz2);
        apagarMatriz(matriz3);
      
        button2Pressed = 0;
        estado2 = 0; //vuelve a 'presionar para comenzar'
      }
      else if ((millis()-timer2Init)/1000 >= TIEMPO_INFRACCION_SEGS) //pasaron mas de 15 seg
      {
		  //Infraccion 15 segundos
        //writeTimer2(timer2Init, 0, 0); //para que imprima el 15, y no un posible 14.99
        estado1 = 2; //pasa a 'pausado'
        timerPausa = millis();
		//Reanudar 
		//con apoyaron o iniciar
    //Cuando se reanuda debe hacer la secuencia de semaforo nuevamente mas los 
    //5 segundos con las luces verdes encendidas
      }
    }

    //Logica del boton 3 y pausado en medio del combate
    if(button3Pressed){
      button3Pressed = 0;
      estado1 = 2; //pasa a 'pausado'
      timerPausa = millis();
    }
	  
    //Uno gano el combate
    if (button1Pressed) //boton rojo presionado
    {
      button1Pressed = 0;
      estado1 = 2; //pasa a 'pausado'
      finCompetencia(); //beep y prende luces por 4 seg.
    }
    else if ((millis()-timer1Init)/1000 >= TIEMPO_COMPETENCIA_SEGS) //pasaron mas de 3 min
    {
		//Mas de 3min
      writeTimer1(timer1Init);
      estado1 = 2; //pasa a 'pausado'
      finCompetencia(); //beep y prende luces por 4 seg.
    }
  }

  //----------------------------estado 2--------------------------------------//
  else if (estado1 == 2) //timer 1: pausado
  {
    if (button3Pressed) //boton verde presionado
    {
      button3Pressed = 0;
      button1Pressed = 1;
      estado1 = 0; //pasa a 'presionar para comenzar'
    }
  }

  //------------------------estado 3--------------------------------------//
  else if (estado1==3){

    if((millis()-timer0Init)/1000 >= 30 || button1Pressed){
      readySetGo(); //hace 3 beeps cortos y 1 largo, y prende los leds de a 3.
      estado1 = 1; //pasa a 'contando'
      primeros5 = 1;
      button1Pressed = 0;
      if (!flagStart) {
        writeTimerReset(); //imprime ceros en el primer timer del DCF.
        flagStart = 1;
        timer1Init = millis(); //inicializa el timer1Init
      }
      else{
        timer1Init = timer1Init + (millis() - timerPausa); //agrega el tiempo de pausa al timer1Init
      }
      timer5Init = millis(); //inicializa el timer5Init
      
    } 
    else{
      writeTimer2(timer0Init, 1, 30);
    }
  }
}

//---------------------------------------------------------------------//
//---------------------------------------------------------------------//
//function definitions

void writeTimer1(unsigned long timerInit)
{ //Esrcibe mins (digito 7 del display: leftmost) y segs (digitos 6 y 5) del tiempo de pelea

  unsigned long timer1Seg = (millis()-timerInit)/1000;  
  unsigned long timer1Centiseg = (millis()-timerInit)/10;
  
  int mins = timer1Seg/60;
  int segs = timer1Seg - 60*mins;
  int centisegs = timer1Centiseg - 100*(timer1Centiseg/100);  

  if(centisegs > 95)
  {
    noTone(buzzer);
  }

  #if DEBUG
    Serial.print("Timer 1: ");
    Serial.print(mins);
    Serial.print(":");
    if(segs < 10) Serial.print("0");
    Serial.println(segs);
  #endif

  displayManager.updateDisplays(mins, segs);
  //display8digits.setDigit(0, 7, (byte)(mins), TRUE); //imprime por ej: 2.47 (2mins, 47seg)
  //display8digits.setDigit(0, 6, (byte)(segs/10), FALSE);
  //display8digits.setDigit(0, 5, (byte)(segs - (segs/10)*10), FALSE);
  //display8digits.setDigit(0, 4, (byte)(centisegs/10), FALSE);



  
  //display8digits.DisplayChar(7, toChar(mins), TRUE); //imprime por ej: 2.47 (2mins, 47seg)
  //display8digits.DisplayChar(6, toChar(segs/10), FALSE);
  //display8digits.DisplayChar(5, toChar(segs - (segs/10)*10), FALSE);
}

//---------------------------------------------------------------------//

void writeTimer2(unsigned long timerInit, bool on, int duracion)
{ //Esrcibe segs (digitos 3 y 2 del display) y centisegs (digitos 1 y 0) del tiempo de infraccion
  unsigned long timer2Centiseg = (millis()-timerInit)/10;
  int segs = timer2Centiseg/100;
  if(on){
    segs = duracion - segs;
  }
  
  //int centisegs = timer2Centiseg - 100*segs;

  int primerDigito = segs/10;
  int segundoDigito = segs - (segs/10)*10;

  matrix.setCursor(0, 0);
  matrix.print(':');

  matrix.setCursor(8, 0);
  matrix.print(primerDigito + '0');

  matrix.setCursor(16, 0);
  matrix.print(segundoDigito+'0');

  matrix.show();
  #if DEBUG
    Serial.print("Timer 2: ");
    Serial.print(":");
    Serial.print(primerDigito);
    Serial.print(segundoDigito);
    if(estado1 != 1){
      Serial.print('\n');
    }
  #endif
  // display8digits.setDigit(0, 3, (byte)(segs/10), FALSE);
  // display8digits.setDigit(0, 2, (byte)(segs - (segs/10)*10), TRUE);
  // display8digits.setDigit(0, 1, (byte)(centisegs/10), FALSE);
  // display8digits.setDigit(0, 0, (byte)(centisegs - (centisegs/10)*10), FALSE);

  
  //display8digits.DisplayChar(3, toChar(segs/10), FALSE); //imprime por ej: 04.59 (4segs, 59centisegs)
  //display8digits.DisplayChar(2, toChar(segs - (segs/10)*10), TRUE);
  //display8digits.DisplayChar(1, toChar(centisegs/10), FALSE);
  //display8digits.DisplayChar(0, toChar(centisegs - (centisegs/10)*10), FALSE);
}

//---------------------------------------------------------------------//

// void writeTimerReset(int timer_id)
// { //escribe ceros en los digitos 7-6-5 (timer_id=1) o 3-2-1-0 (timer_id=2) del display
//   int decimalPoint;
//   int i_init;
//   int i_end;
//   if (timer_id==1) //primer timer
//   {
//     i_init=4; //imprime ceros en displays 7-6-5
//     i_end=7;
//   }
//   else if (timer_id==2) //segundo timer
//   {
//     i_init=0; //imprime ceros en displays 3-2-1-0
//     i_end=3;
//   }
  
//   for (int i=i_init; i<=i_end; i++){
//     if(i==7 || i==2){
//       decimalPoint=TRUE;
//     }
//     else{
//       decimalPoint=FALSE;
//     }
    
//     display8digits.setDigit(0, i, (byte)0, decimalPoint);
//     //display8digits.DisplayChar(i,'0',decimalPoint);
//   }
// }

void writeTimerReset(void){  // Escribir ceros en el display contador football
  displayManager.updateDisplays(0, 0); // Resetea ambos displays a 00
  #if DEBUG
    Serial.println("Timer 1 Reset");
  #endif
}

//---------------------------------------------------------------------//
/*Apoyaron
30segundos
+
Iniciar 
*/
//Iniciar
void readySetGo(void)
{
  #if DEBUG
    Serial.println("Arranque");
  #endif
	//Secuencia de Iniciar
    tone(buzzer, 785, 1000);
	  prenderMatriz(matriz1, 255, 0, 0);
    delay(1000);
    apagarMatriz(matriz1);
	  prenderMatriz(matriz2, 255, 255, 0);
    delay(1000);
    apagarMatriz(matriz2);
	  prenderMatriz(matriz1, 0, 255, 0);
    prenderMatriz(matriz2, 0, 255, 0);
    prenderMatriz(matriz3, 0, 255, 0);  

  #if DEBUG
    Serial.println("Fin de Arranque");
  #endif


/*
	  tone(buzzer,785); //primer beep, se prenden 3 leds
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      delay (500);
      noTone(buzzer);
      delay(500);
      
      tone(buzzer,785); //segundo beep, se prenden otros 3 leds
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      delay (500);
      noTone(buzzer);
      delay(500);
      
      tone(buzzer,785); //tercer beep, se prenden ultimos 3 leds
      digitalWrite(led7, HIGH);
      digitalWrite(led8, HIGH);
      digitalWrite(led9, HIGH);
      delay (500);
      noTone(buzzer);
      delay(1000);
      
      ledsReset();
      tone(buzzer,1570); //cuarto beep, mas largo, se apagan todos los leds
	  */

}

//---------------------------------------------------------------------//

void finCompetencia(void)
{
  #if DEBUG
    Serial.println("Fin de la batalla");
  #endif
tone(buzzer, 1570, 1000);
delay(3000);
apagarMatriz(matriz1);
apagarMatriz(matriz2);
apagarMatriz(matriz3);


flagStart = 0; 
  
  
  /*

  tone(buzzer,1570);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);
  digitalWrite(led5, HIGH);
  digitalWrite(led6, HIGH);
  digitalWrite(led7, HIGH);
  digitalWrite(led8, HIGH);
  digitalWrite(led9, HIGH);
  delay (1000);
  noTone(buzzer);
  delay(3000);
  ledsReset();

*/
}

char toChar(int num)
{
  return num+'0';
}

void prenderMatriz(int matriz, int r, int g, int b){
  matrix.fillRect(matriz, 0, 8, 8, matrix.Color(r, g, b));
}


void apagarMatriz(int matriz){
  matrix.fillRect(matriz, 0, 8, 8, matrix.Color(0, 0, 0));
}

//---------------------------------------------------------------------//

//Algo Visual (agregado)
void irPrendiendoLeds(unsigned long timer1Init)
{ //para un tiempo de 180 segs de competencia, prende un par de leds
  //cada 40 segs, de izquierda a derecha. (El ultimo led se prende con
  //finCompetencia())
  unsigned long timer1Seg = (millis()-timer1Init)/1000;
  if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*8/9){
    digitalWrite(led8, HIGH);
  }
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*7/9){
    digitalWrite(led7, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*6/9){
    digitalWrite(led6, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*5/9){
    digitalWrite(led5, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*4/9){
    digitalWrite(led4, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*3/9){
    digitalWrite(led3, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*2/9){
    digitalWrite(led2, HIGH);
  } 
  else if (timer1Seg>=TIEMPO_COMPETENCIA_SEGS*1/9){
    digitalWrite(led1, HIGH);
  } 



}

//---------------------------------------------------------------------//

void ledsReset(void)
{

//Secuencia de apagado de matrices, codigo rgb(0,0,0)
//Apagar matrices
//apagarMatrizroja();
//apagarMatrizAmarilla();
//apagarMatrizVerde();

  /*
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(led7, LOW);
  digitalWrite(led8, LOW);
  digitalWrite(led9, LOW);
  */
}
