#include <xc.h>
#include "main.h"
#include "switch.h"

// CONFIG
#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKIN function disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)
#pragma config LPBOR = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//ra0 - pwm1
//ra2 - input

extern struct SWITCH TOUCH1;

unsigned char fstate;
unsigned char mDIMMER,sPressTime,fsegno,tGeneral8bit;
unsigned int tPressTime;
unsigned long timer0_millis; // In the ISR

void interrupt isr(void)
{   //1mS
    if(INTCONbits.TMR0IF && INTCONbits.TMR0IE == 1){
        INTCONbits.TMR0IF = 0;
        TMR0 = 9;
        if(sPressTime){
            tPressTime++;
        }
        tGeneral8bit++;
        timer0_millis++;
    }
}


void main(void) {
    //Osc 8MHz internal
    OSCCONbits.IRCF = 6;
  //  while(OSCCONbits.HFIOFR==0);  

    ANSELA = 0x00; // DIGITAL IO

    TRISAbits.TRISA0 = 0;
    
    //PWM
    //Timer2
    T2CONbits.TOUTPS = 0b0000;
    T2CONbits.T2CKPS = 0b00;
    T2CONbits.TMR2ON = 1;
    PR2 = 0x65;        
    
    PWM1CONbits.PWM1POL = 0;
    PWM1CONbits.PWM1OUT = 1;
    PWM1CONbits.PWM1OE = 1;
    PWM1CONbits.PWM1EN = 1;
    PWM1DCH = 0;

    //Timer0
    OPTION_REGbits.T0CS = 0;          // Fosc / 4
    OPTION_REGbits.PSA = 0;           
    OPTION_REGbits.PS =  2;            //Presclar 1:8
    TMR0 = 0;
    //Attiva ISR
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;

    TRISAbits.TRISA2 = 1;

    //----------------------------
    fstate=0;        
    sPressTime=0;
    tPressTime=0;    
    fsegno=0;
    mDIMMER=102;        
    
    pulsante_INIT_debounce();
    
    while(1){
        pulsante_debounce(); 
        
        //program
        switch(fstate){
            case 0:
                //se lampada è spenta
                //attendi tasto on
                if(TOUCH1.State==0){
                    //Accendi
                    PWM1DCH=mDIMMER;
                    fstate=1;
                }
            break;
        
            case 1:
                //se lampada è accesa
                //attendi tasto rilascio
                if(TOUCH1.State==1){
                    fstate=2;
                }
            break;
        
             case 2:
                //se lampada è accesa
                //attendi pressione x spegnere o x programmazione
                if(TOUCH1.State==0){
                    fstate=3;
                    //start timer
                    sPressTime=0;
                    tPressTime=0;
                    sPressTime=1;
                }
            break;
               
             case 3:
                //se lampada è accesa
                //pressione tasto = on ,  spegnere o x programmazione
                if(TOUCH1.State==1){
                    sPressTime=0;
                    tPressTime=0;
                    fstate=4;
                    // spegni
                }
                sPressTime=0;
                if(tPressTime>1000){
                    //vai a programmazione
                    fstate=5;
                    tPressTime=0;
                    tGeneral8bit=0;
                }else{
                    sPressTime=1;
                }            
            break;
                       
             case 4:
                //se lampada è accesa
                //spegni
                 PWM1DCH=0;
                 fstate=0;
            break;
        
            case 5:
                //se lampada è accesa e in programmazione
                 if(TOUCH1.State==0){
                     //dimmera
                     if(tGeneral8bit>25){
                        tGeneral8bit=0;
                         if(fsegno==1){
                            mDIMMER++;
                           if(mDIMMER>=102){
                               mDIMMER = 102;
                           }
                        }else{
                           if(mDIMMER>3){
                            mDIMMER--;
                           }                  
                        }
                        PWM1DCH=mDIMMER;
                     }
                 }else{
                     //exit programmazione
                     if(fsegno==1){
                         fsegno=0;
                     }else{
                        fsegno=1;
                     }
                     fstate=2;
                 }               
            break;       
        }
        
        
        
        
        
        
    
    }
    
        
}



unsigned long millis(void){
	unsigned long m;
    INTCONbits.GIE = 0;     //Disable Global Intterrupt
    m = timer0_millis;
    INTCONbits.GIE = 1;     //Enable Global Intterrupt
    return m;
}
