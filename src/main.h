/*
 * File:   main.h
 * Author: Nelson Rossi Bittencourt
 *
 * Created on 16/10/2024
 * HT12D emulator
 * Version: 0.0  (16/10/2024) Initial tests
 *          0.1  (06/11/2024) Time-out implementation
 *          0.2  (10/11/2024) Data pins implementation
 *          0.3  (11/11/2024) Portability (header files calculations), buzzer 
 *          test and bug fixes for data pins
 *          0.31 (13/11/2024) Test with sleep (needs hardware and pin mapping 
 *          changes because SLEEP needs MCLR HIGH
 *          0.32 (18/11/2024) 'set data' optimized, new logic for buzzer 
 */              

#ifndef main_h_header
#define	main_h_header

// *****************************************************************************
// *** Includes *** 
// *****************************************************************************
#include "pic12f675_conf.h"                     // PIC12F675 configuration for XC8 compiler


// *****************************************************************************
// *** Defines ***
// *****************************************************************************
#define _XTAL_FREQ    4000000                   // Used by delays staments 

// ** Operation mode **
#define BUZZER_D9                               // This mode uses D9 pin as buzzer controller
#define SLEEP_MODE                              // This mode:
                                                // 1) adds sleep mode in MCU reset and after VT timeout;
                                                // 2) removes D11 data pin (PIC12F675 needs GPIO3 high to sleep mode) and
                                                // 3) changes input (DIN) from GPIO3 to GPIO1.


// ** Hardware mapping and constants calculations **
// Note: PIC12F675 GPIO3 is always input (MCLR pin)
#define PORT              GPIO                  // Nickname for input/output PORT (PIC12F675 has only one) 

// * DIN pin *
#ifdef SLEEP_MODE
    #define DIN           (1<<1)                // DIN pin with sleep mode enabled
#else
    #define DIN           (1<<3)                // DIN pin with sleep mode disabled
#endif

#define VT                (1<<0)                // Valid Transmission pin               
   
// * Data pins *
#define D8                (1<<5)                // D8 data (MSB)
#define D9                (1<<4)                // D9 data or buzzer
#define D10               (1<<2)                // D10 data or LSB in SLEEP mode version

// * LSB data pin
#ifndef SLEEP_MODE
    #define D11           (1<<1)                // D11 data or MCLR pulled-up for sleep mode
#endif

#define TRIS_CONF         0xFF & DIN            // Data PINs (warning GPIO3 is only input, never output)


// * HT12E Parameters *
// Warning: HT12E_Fosc must be > 2000 Hz. This allows using integer variables in code.
// Change 'HT12E_Fosc' or 'time_tol' to meet our requiriments
#define HT12E_Fosc    3000                      // HT12E oscillation frequency (Hz)
#define time_tol      5                         // Time tolerance (%)

// Below parameters are calculated using 'Fosc' and 'time_tol'
#define period        (1e6/HT12E_Fosc)          // Oscillation period (us)
#define min_time      (256 * period)/1024       // "Minimum" valid time 
#define max_time      (int)(min_time * 2)       // "Maximum" valid time (conversion to 'int' here because this 'var' is used in code)

// * HT12E/D bits time definitions *
// bit HIGH -> logical level LOW by (2 * period) + logical level HIGH by (1 * period)
// bit LOW  -> logical level LOW by (1 * perido) + logical level HIFH by (2 * period))
// For HT12E_Fosc = 3000 and current TIMER0:
// 2*period = 666 us -> (256 * 666 /1024) -> 166 timer0 ticks
// 1*period = 333 us -> (256 * 333 /1024) ->  83 timer0 ticks
#define min_time_inf  (int)((min_time * (100-time_tol))/100)    // Minimum time - tolerance
#define min_time_sup  (int)((min_time * (100+time_tol))/100)    // Minimum time + tolerance
#define max_time_inf  (int)((max_time * (100-time_tol))/100)    // Maximum time - tolerance
#define max_time_sup  (int)((max_time * (100+time_tol))/100)    // Maximum time + tolerance


// * MCU as HT12D Parameters *
#define HT12D_Address   0xFF                    // Expected address code 
#define HT12D_FOSC      125                     // Oscillation frequency (MHz)

// * Valid Transmission time out *
// VT_TIMEOUT : Time with VT high after valid signal is gone:
// 2^14 clocks (datasheet)-> 2^14 * (1/125kHz) -> 131.072 ms
// Timer0 period = 1.024 ms -> time_out max = 131.072 ms / 1.024ms  = 127
#define VT_TIMEOUT      (int)(16348/(HT12D_FOSC*1.024))


// *****************************************************************************
// *** Functions prototypes ***
// *****************************************************************************
void set_data_pins(void);                       // Set data pins 

#endif