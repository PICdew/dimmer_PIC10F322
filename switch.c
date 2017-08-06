#include "switch.h"
#include "main.h"
#include <xc.h>

struct SWITCH TOUCH1;

void pulsante_INIT_debounce(void) {
    //Read Pin      
    TOUCH1.state_debounce = 0;
    TOUCH1.laststato = PORTAbits.RA2;
    TOUCH1.timewaitdebounce = 0;
    TOUCH1.changeState = false;
    if(PORTAbits.RA2){
        TOUCH1.State = 1;
    }else{
        TOUCH1.State = 0;
    }
}


//Debounce
void pulsante_debounce(void){
    uint8_t value;
    value = PORTAbits.RA2;
   
    switch(TOUCH1.state_debounce){
        case 0:
            if(value != TOUCH1.laststato){
                 //Stato Cambiato stato
                TOUCH1.timewaitdebounce = millis();
                TOUCH1.state_debounce = 1;
            }
        break;
        
        case 1:
        #ifdef __XC
            if((millis() - TOUCH1.timewaitdebounce)>=DELAY_DEBOUNCE){
        #endif                       
                if(value != TOUCH1.laststato){
                    TOUCH1.laststato = value;
                    //Notifica Cambio Stato
                    TOUCH1.changeState = 1;
                    if(value){
                        TOUCH1.State = 1;                       
                    }else{
                        TOUCH1.State = 0;
                    }
                }
                TOUCH1.state_debounce = 0;
            }
        break;
    }   
    
  
    
}