/*
 * Project: HT12D emulator
 * File:    pic12f675_conf.h
 * Author:  Nelson Rossi Bittencourt
 *
 * Created on 16/10/2024 
 * Version: 0.0  (16/10/2024) 
 */ 

#ifndef PIC12F675_CONFIG_H
#define	PIC12F675_CONFIG_H

#include <xc.h>                 // Include processor files - each processor file is guarded.  

// *****************************************************************************
// *** PIC12F675 configuration word ***
// *****************************************************************************
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma switch speed            // Use the fastest switch method;

#endif	

