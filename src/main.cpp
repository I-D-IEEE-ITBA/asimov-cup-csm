#include <Arduino.h>

#include "LedControl.h"

LedControl display8digits = LedControl(12, 10, 11, 1); // Din = 12, Clck = 10, CS = 11, Number of devices = 1
// MAX7219 display8digits;
#define TRUE 1
#define FALSE 0

#define TIEMPO_COMPETENCIA_SEGS 180
#define TIEMPO_INFRACCION_SEGS 15

// CONEXIONES IMPORTANTES: para el display, para Arduino Uno: VCC a +5V, GND a GND, DIN a pin 12, CS a pin 11, CLK a pin 10

const int buttonY = 2; // boton amarillo
const int buttonR = 3; // boton rojo arranca el cronometro de la pelea
const int led1_2 = 4;  // luces que se prenden en secuencia al inicio.
const int led3 = 5;    // conectar, del modulo del transistor: hembra marron a gnd, hembra rojo a 5v arduino (ver esquematico,
const int led4 = 6;    // muesquita para arriba). Despues, pin 1 modulo transistores a 2 leds, pin 2 a 1 led, pin 3 a 1 led,
const int led5_6 = 7;  // pin 4 a 2 leds, pin 5 a 2 leds, pin 6 a 1 led.
const int led7_8 = 8;
const int led9 = 9;
const int buzzer = 13; // buzzer

int buttonRState = 0;
int buttonRPrevState = 0;
int buttonRPressed = 0; // boton rojo presionado
int buttonYState = 0;
int buttonYPrevState = 0;
int buttonYPressed = 0; // boton amarillo presionado
unsigned long timeNow = 0;
unsigned long timePrev = 0;
unsigned long delaytime = 250;

unsigned long timer1Init = 0; // Timer de tiempo de pelea (cuenta hasta 3 min).
                              // minutos y segundos. Activado, pausado y reseteado con boton rojo.
unsigned long timer2Init = 0; // Timer de tiempo de infraccion (cuenta hasta 15 seg).
                              // segundos y centesimas de segundo. Activado, pausado y reseteado con boton amarillo.

int estado1 = 0; // 0: presionar boton rojo para comenzar. 1: contando. 2: pausado, presionar boton rojo para volver a estado 0.
int estado2 = 0; // 0: presionar boton amarillo para comenzar a contar. 1: contando.

// function prototypes
void writeTimer1(unsigned long timerInit);
void writeTimer2(unsigned long timerInit);
void writeTimerReset(int timer_id);
void irPrendiendoLeds(unsigned long timer1Init);
void ledsReset(void);
void readySetGo(void);
void finCompetencia(void);
char toChar(int num);

//---------------------------------------------------------------------//
//---------------------------------------------------------------------//

void setup()
{

    display8digits.shutdown(0, false);
    display8digits.setIntensity(0, 8);
    display8digits.clearDisplay(0);

    pinMode(buttonR, INPUT); // botones: activo bajo (presionado: 0)
    pinMode(buttonY, INPUT);
    pinMode(buzzer, OUTPUT); // buzzer y leds: activo alto (se prenden/activan con un 1)
    pinMode(led1_2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
    pinMode(led5_6, OUTPUT);
    pinMode(led7_8, OUTPUT);
    pinMode(led9, OUTPUT);

    // Serial.begin(9600); //creo que hace falta porque se comunica por spi con el modulo de displays
}

//---------------------------------------------------------------------//
//---------------------------------------------------------------------//

void loop()
{
    timeNow = millis();
    //------------------lectura de botones cada 50ms----------------------------//

    if (timeNow - timePrev > 50) // leer botones cada 50 milisegs para evitar rebotes.
    {
        timePrev = timeNow;
        buttonRPrevState = buttonRState;
        buttonYPrevState = buttonYState;
        buttonRState = digitalRead(buttonR);
        buttonYState = digitalRead(buttonY);
        if (buttonRState == LOW && buttonRPrevState == HIGH) // detectar flancos ascendentes: boton presionado
        {
            buttonRPressed = 1;
        }
        if (buttonYState == LOW && buttonYPrevState == HIGH)
        {
            buttonYPressed = 1;
        }
    }

    //----------------------------estado 0--------------------------------------//
    if (estado1 == 0) // presionar boton rojo para comenzar
    {
        writeTimerReset(1); // imprime ceros en el primer timer.
        writeTimerReset(2); // imprime ceros en el segundo timer.
        ledsReset();        // apaga los 9 leds.
        estado2 = 0;        // timer 2 en estado 'presionar boton amarillo para comenzar' (solo puede comenazar cuando estado1=1, 'contando').
        buttonYPressed = 0;
        if (buttonRPressed) // boton rojo presionado
        {
            buttonRPressed = 0;
            readySetGo(); // hace 3 beeps cortos y 1 largo, y prende los leds de a 3.
            estado1 = 1;  // pasa a 'contando'
            timer1Init = millis();
        }
    }

    //----------------------------estado 1--------------------------------------//
    else if (estado1 == 1) // timer 1: contando
    {
        writeTimer1(timer1Init);      // imprime tiempo de pelea
        irPrendiendoLeds(timer1Init); // prende leds a medida que pasa el tiempo de pelea.

        // Logica del boton 2 y timer 2
        if (estado2 == 0)
        { // presionar boton amarillo para comenzar
            writeTimerReset(2);
            if (buttonYPressed) // boton amarillo presionado
            {
                buttonYPressed = 0;
                estado2 = 1; // pasa a 'contando'
                timer2Init = millis();
            }
        }
        else if (estado2 == 1) // timer 2: contando
        {
            writeTimer2(timer2Init);
            if (buttonYPressed) // boton amarillo presionado
            {
                buttonYPressed = 0;
                estado2 = 0; // vuelve a 'presionar para comenzar'
            }
            else if ((millis() - timer2Init) / 1000 >= TIEMPO_INFRACCION_SEGS) // pasaron mas de 15 seg
            {
                writeTimer2(timer2Init); // para que imprima el 15, y no un posible 14.99
                estado1 = 2;             // pasa a 'pausado'
                finCompetencia();        // beep y prende luces por 4 seg.
            }
        }
        if (buttonRPressed) // boton rojo presionado
        {
            buttonRPressed = 0;
            estado1 = 2;      // pasa a 'pausado'
            finCompetencia(); // beep y prende luces por 4 seg.
        }
        else if ((millis() - timer1Init) / 1000 >= TIEMPO_COMPETENCIA_SEGS) // pasaron mas de 3 min
        {
            writeTimer1(timer1Init);
            estado1 = 2;      // pasa a 'pausado'
            finCompetencia(); // beep y prende luces por 4 seg.
        }
    }

    //----------------------------estado 2--------------------------------------//
    else if (estado1 == 2) // timer 1: pausado
    {
        if (buttonRPressed) // boton rojo presionado
        {
            buttonRPressed = 0;
            estado1 = 0; // pasa a 'presionar para comenzar'
        }
    }
}

//---------------------------------------------------------------------//
//---------------------------------------------------------------------//

// function definitions
void writeTimer1(unsigned long timerInit)
{ // Esrcibe mins (digito 7 del display: leftmost) y segs (digitos 6 y 5) del tiempo de pelea

    unsigned long timer1Seg = (millis() - timerInit) / 1000;
    unsigned long timer1Centiseg = (millis() - timerInit) / 10;

    int mins = timer1Seg / 60;
    int segs = timer1Seg - 60 * mins;
    int centisegs = timer1Centiseg - 100 * (timer1Centiseg / 100);

    if (centisegs > 95)
    {
        noTone(buzzer);
    }

    display8digits.setDigit(0, 7, (byte)(mins), TRUE); // imprime por ej: 2.47 (2mins, 47seg)
    display8digits.setDigit(0, 6, (byte)(segs / 10), FALSE);
    display8digits.setDigit(0, 5, (byte)(segs - (segs / 10) * 10), FALSE);
    display8digits.setDigit(0, 4, (byte)(centisegs / 10), FALSE);

    // display8digits.DisplayChar(7, toChar(mins), TRUE); //imprime por ej: 2.47 (2mins, 47seg)
    // display8digits.DisplayChar(6, toChar(segs/10), FALSE);
    // display8digits.DisplayChar(5, toChar(segs - (segs/10)*10), FALSE);
}

//---------------------------------------------------------------------//

void writeTimer2(unsigned long timerInit)
{ // Esrcibe segs (digitos 3 y 2 del display) y centisegs (digitos 1 y 0) del tiempo de infraccion
    unsigned long timer2Centiseg = (millis() - timerInit) / 10;
    int segs = timer2Centiseg / 100;
    int centisegs = timer2Centiseg - 100 * segs;

    display8digits.setDigit(0, 3, (byte)(segs / 10), FALSE);
    display8digits.setDigit(0, 2, (byte)(segs - (segs / 10) * 10), TRUE);
    display8digits.setDigit(0, 1, (byte)(centisegs / 10), FALSE);
    display8digits.setDigit(0, 0, (byte)(centisegs - (centisegs / 10) * 10), FALSE);

    // display8digits.DisplayChar(3, toChar(segs/10), FALSE); //imprime por ej: 04.59 (4segs, 59centisegs)
    // display8digits.DisplayChar(2, toChar(segs - (segs/10)*10), TRUE);
    // display8digits.DisplayChar(1, toChar(centisegs/10), FALSE);
    // display8digits.DisplayChar(0, toChar(centisegs - (centisegs/10)*10), FALSE);
}

//---------------------------------------------------------------------//

void writeTimerReset(int timer_id)
{ // escribe ceros en los digitos 7-6-5 (timer_id=1) o 3-2-1-0 (timer_id=2) del display
    int decimalPoint;
    int i_init;
    int i_end;
    if (timer_id == 1) // primer timer
    {
        i_init = 4; // imprime ceros en displays 7-6-5
        i_end = 7;
    }
    else if (timer_id == 2) // segundo timer
    {
        i_init = 0; // imprime ceros en displays 3-2-1-0
        i_end = 3;
    }

    for (int i = i_init; i <= i_end; i++)
    {
        if (i == 7 || i == 2)
        {
            decimalPoint = TRUE;
        }
        else
        {
            decimalPoint = FALSE;
        }

        display8digits.setDigit(0, i, (byte)0, decimalPoint);
        // display8digits.DisplayChar(i,'0',decimalPoint);
    }
}

//---------------------------------------------------------------------//

void irPrendiendoLeds(unsigned long timer1Init)
{ // para un tiempo de 180 segs de competencia, prende un par de leds
    // cada 40 segs, de izquierda a derecha. (El ultimo led se prende con
    // finCompetencia())
    unsigned long timer1Seg = (millis() - timer1Init) / 1000;
    if (timer1Seg >= TIEMPO_COMPETENCIA_SEGS * 8 / 9)
    {
        digitalWrite(led7_8, HIGH);
    }
    else if (timer1Seg >= TIEMPO_COMPETENCIA_SEGS * 6 / 9)
    {
        digitalWrite(led5_6, HIGH);
    }
    else if (timer1Seg >= TIEMPO_COMPETENCIA_SEGS * 4 / 9)
    {
        digitalWrite(led3, HIGH);
        digitalWrite(led4, HIGH);
    }
    else if (timer1Seg >= TIEMPO_COMPETENCIA_SEGS * 2 / 9)
    {
        digitalWrite(led1_2, HIGH);
    }
}

//---------------------------------------------------------------------//

void ledsReset(void)
{
    digitalWrite(led1_2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5_6, LOW);
    digitalWrite(led7_8, LOW);
    digitalWrite(led9, LOW);
}

//---------------------------------------------------------------------//

void readySetGo(void)
{
    tone(buzzer, 785); // primer beep, se prenden 3 leds
    digitalWrite(led1_2, HIGH);
    digitalWrite(led3, HIGH);
    delay(500);
    noTone(buzzer);
    delay(500);

    tone(buzzer, 785); // segundo beep, se prenden otros 3 leds
    digitalWrite(led4, HIGH);
    digitalWrite(led5_6, HIGH);
    delay(500);
    noTone(buzzer);
    delay(500);

    tone(buzzer, 785); // tercer beep, se prenden ultimos 3 leds
    digitalWrite(led7_8, HIGH);
    digitalWrite(led9, HIGH);
    delay(500);
    noTone(buzzer);
    delay(1000);

    digitalWrite(led1_2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5_6, LOW);
    digitalWrite(led7_8, LOW);
    digitalWrite(led9, LOW);
    tone(buzzer, 1570); // cuarto beep, mas largo, se apagan todos los leds
}

//---------------------------------------------------------------------//

void finCompetencia(void)
{
    tone(buzzer, 1570);
    digitalWrite(led1_2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led5_6, HIGH);
    digitalWrite(led7_8, HIGH);
    digitalWrite(led9, HIGH);
    delay(1000);
    noTone(buzzer);
    delay(3000);
    digitalWrite(led1_2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5_6, LOW);
    digitalWrite(led7_8, LOW);
    digitalWrite(led9, LOW);
}

char toChar(int num)
{
    return num + '0';
}