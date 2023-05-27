//Display TFT-  ILI9341 Toutch

//Biblioteca
#include "mbed.h"
#include "Arduino.h"
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include "TouchScreen_kbv_mbed.h"
#include "main.h"
#include "math.h"


// variaveis auxiliares

Ticker display;
Timer debounce;

BusOut saida (PC_4,PB_13,PB_14,PB_15,PB_1,PB_2,PB_12,PA_11);
InterruptIn botao(PC_10);
AnalogIn pot(PC_3);


int f = 100;
int sinal = 1;
float pi = 3.14159265358979323846;
int tempo;

// Configuração do Display
const PinName XP = D8, YP = A3, XM = A2, YM = D9; 
const int TS_LEFT=121,TS_RT=922,TS_TOP=82,TS_BOT=890;
DigitalInOut YPout(YP);
DigitalInOut XMout(XM);
TouchScreen_kbv ts = TouchScreen_kbv(XP, YP, XM, YM);
TSPoint_kbv tp;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Orientação  Display
uint8_t Orientation = 1;

// Tabela de Cores

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//***********************Escrita no  Display**********************************//


void frequencia(){ //Função que printa o tipo de onda e frequência atual

    tft.setCursor(50, 50); // Orientação X,Y
    tft.setTextSize(2);
    tft.printf("FREQUENCIA");
    tft.setTextSize(3);
    tft.fillRect(100, 100, 200, 40, BLACK);
    tft.fillRect(50, 150, 200, 40, BLACK);
    tft.setCursor(50, 100);
    tft.printf("f = %d Hz",f);
    tft.setCursor(50, 150);

    switch (sinal){
        case 2:
            tft.setTextColor(YELLOW);
            tft.print("QUADRADA");
            break;
        case 1:
            tft.setTextColor(GREEN);
            tft.print("SENOIDAL");
            break;
        case 3:
            tft.setTextColor(RED);
            tft.print("TRIANGULAR");
            break;
    }
    tft.setTextColor(CYAN);

}

//****************************************************************************//


// PROGRAMA PRINCIPAL ____________________________________________________________

void setup(void)
{
    //configs da tela
    tft.reset();
    tft.begin();
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);  // Fundo do Display
    tft.setTextColor(CYAN);
    tft.setTextSize(3);
    delay(1000);


    //inicia o debouncing
    debounce.start();

    //interrupções
    botao.rise(&muda);

    //display
    display.attach(frequencia,2);
    
}

void loop(){

    f = pot.read() * 100;

    switch (sinal){
        
        case 3: //Triangular
            for (int i = 0; i <= 255; i++){
                saida = i;
                tempo = int(700000/(510*f));
                wait_us(tempo);
            }

            for (int i = 254; i >= 0; i--){
                saida = i;
                tempo = int(700000/(510*f));
                wait_us(tempo);
            }
            break;
        case 2: // Quadrada
            saida = 255;
            tempo = int(1000/(2*f));
            wait_ms(tempo);
            saida = 0;
            tempo = int(1000/(2*f));
            wait_ms(tempo);

            break;
        case 1: // Senoidal
            for (int i = 0; i < 360; i++){
                saida = 127*sin(pi*i/180)+128;
                tempo = int(500000/(360*f));
                wait_us(tempo);
            }
            break;
    }
}

void muda(){ // função que muda o tipo de onda

    if(debounce.read_ms()>10){
        sinal++;
        delay(300);
        if(sinal>3){
            sinal = 1;
        }
    }
    debounce.reset();
}