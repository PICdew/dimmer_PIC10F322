/* 
 * File:   switch.h
 * Author: michele
 * Version : 1.00
 * Created on 7 maggio 2017, 16.05
 */

#ifndef SWITCH_H
#define	SWITCH_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
#ifdef __ICCARM__
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#endif        
#ifdef __XC
#endif   
            
#define DELAY_DEBOUNCE 40
    
    struct SWITCH{
        //Debounce variable     
        uint8_t state_debounce;
        uint32_t timewaitdebounce;
        uint8_t laststato;
    
        //Signal
        uint8_t changeState;       //true a ogni cambio di stato, AZZERARE DA SOFTWARE !!!
        uint8_t State;             //Stato dello switch, ON o OFF;
    };
    
    void pulsante_INIT_debounce(void);
    void pulsante_debounce(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

