/*
 * File:   main.h
 * Author: Nelson Rossi Bittencourt
 *
 * Created on 16/10/2024
 * HT12D emulator
 * Version: 0.0  (16/10/2024) Proteus testing
 *          0.1  (06/11/2024) Time-out implementation
 *          0.2  (10/11/2024) Data pins implementation
 *          0.3  (11/11/2024) Portability (header files calculations), buzzer 
 *          test and bug fixes for data pins
 *          0.31 (13/11/2024) Test with sleep (needs hardware and pin mapping 
 *          changes because SLEEP needs MCLR HIGH
 *          0.32 (18/11/2024) 'set data' optimized, new logic for buzzer 
 */                   
       

// *****************************************************************************
// *** Includes *** 
// *****************************************************************************
#include "main.h"


// *****************************************************************************
// *** Global vars ***
// *****************************************************************************
__bit  flag_analyze = 0;                    // Signals data analysis 
uint8_t measure0 = 0;                       // Time in LOW state of a bit
uint8_t measure1 = 0;                       // Time in HIGH state of a bit
uint8_t iter = 0;                           // Index for ht12_data
uint8_t bit_counter = 0;                    // Counter for bits in a message
uint8_t address_1 = 0;                      // Stores address from message
uint8_t data_1 = 0;                         // Stores current data from message
uint8_t data_pre = 0;                       // Stores data from previus message
uint8_t counter_valids = 0;                 // Counters consecutive valid messages
uint8_t time_out = 0;                       // VT time out 
uint16_t ht12_data[3] = {0x0,0x0,0x0};      // Stores 3 consecutive messages

#ifdef BUZZER_D9
uint16_t buzzer_time = 0;                     // Buzzer time
__bit buzzer_flag = 0;                       // Buzzer on/off flag
#endif

// *****************************************************************************
// *** Interrupt *** 
// *****************************************************************************
void __interrupt() int1()
{    
    static __bit valid_bit = 0;             // Indicates a valid bit 
    
    // ** Buzzer option verifiction **
#ifdef BUZZER_D9
    if (buzzer_flag)                    // Buzzer is active
    {
        if (buzzer_time > 10000)          // Buzzer counter overflows?
        {
            PORT ^=D9;                  // Invert buzzer pin
            buzzer_time = 0;            // Clears buzzer counter
        }
        else 
        {
            buzzer_time++;              // Increments buzzer counter
        }  
        TMR0 = 0;
        return;
    }
#endif
    
    
    // ** TIMER0 verification **
    if (T0IF)                               // Timer0 overflow?
    {   
        T0IF = 0;                           // Clears Timer0 overflow flag
        TMR0 = 0;                           // Resets Timer0 counter
        
        if (time_out>VT_TIMEOUT)            // VT Timeout?
        {
//            T0IE = 0;           // Disable timer0            
//            GPIF = 0;           // Clear port change interrup flag
            
            INTCON = 0x88;                  // 10001000
                                            //   |___________ TOIE 
                                            //        |______ GPIF
                    
            time_out = 0;                   // Clears VT Timeout
            iter = 0;                       // Clears ht12_data index
            bit_counter = 0;                // Clears bits in message counter
            PORT &=~VT;                     // VT pin to LOW
            flag_analyze = 0;               // Clears analysis flag
            
        #ifdef SLEEP_MODE 
            #ifndef BUZZER_D9
                SLEEP();                        // Enters in sleep mode
            #endif
        #endif
        }  
        else 
        {
            time_out++;                     // Increments VT timeout counter          
        }
    }
    
        
    // ** Input pin verification **
    if ((GPIF) && (bit_counter < 13))       // DIN changed and bit_counter < 13?
    {   
        if (PORT & DIN)                     // DIN is HIGH?
        {                        
            if (bit_counter)                // Isn't the first bit of message?
            { 
                measure0 = TMR0;            // Saves time to previous LOW level    
                TMR0 = 0;                   // Resets timer0 counter for next bit
            }
            else                            // Is first bit of message (start bit)?
            {
                measure1 = 0;               // Resets time for HIGH level 
                measure0 = max_time;        // Assures that time for LOW level corret)
                TMR0 = 0;                   // Resets timer0 counter for next bit
                T0IE = 1;                   // Enables timer0 interruption
                ht12_data[iter]= 0;         // Resets data array element
            }
        }
        else                                // DIN is LOW? (Falling edge)
        {   
            measure1 = TMR0;                // Saves time to previous HIGH level 
            TMR0 = 0;                       // Resets timer0 counter for next bit            
            valid_bit = 0;                  // Clears valid_bit
             
            // * Check if bit times for expected pattern *            
            if ((measure1 > min_time_inf) && (measure1 < min_time_sup))     // Time in HIGH indicates a bit ONE?
            {
                if (measure0 > max_time_inf && measure0 < max_time_sup)     // Time in LOW indicates a bit ONE?
                {
                    ht12_data[iter] |= (1 << (12-bit_counter));             // Change message bit to one
                    valid_bit = 1;                                          // Signs a valid bit
                }
            }
            else
            {
                if ((measure1 > max_time_inf) && (measure1 < max_time_sup)) // Time in HIGH indicates a bit ZERO?
                {
                    if (measure0 > min_time_inf && measure0 < min_time_sup) // Time in LOW indicates a bit ZERO?
                    {                        
                        valid_bit = 1;                                      // Signs a valid bit
                    }
                }
                else 
                {
                    time_out = VT_TIMEOUT + 1;                              // If not valid bit, overflows time_out                 
                }
            }       
        }
        GPIF = 0;                                                           // Clears flag for pin change
    }
    
       
    if (valid_bit)                          // Bit is valid?                             
    {
        if (bit_counter == 12)              // A full message was received?
        {
            iter++;                         // Increments message number
            bit_counter = 0;                // Resets bit_counter, for next message
        }
        else
        {
            bit_counter++;                  // Increments bit_counter, for next bit in current message
        }        
        valid_bit = 0;                      // clears valid_bit
    }   
        
    if (iter==3)                            // 3 messages are received?        
    {                                    
        GPIF = 0;                           // Clears flag for pin change        
        iter = 0;                           // Resets messages counter
        bit_counter = 0;                    // Resets bit_counter
        flag_analyze = 1;                   // Signs message analysis
        time_out = 0;                       // Resets time_out
    }
}

// *****************************************************************************
// *** Main function *** 
// *****************************************************************************
void main(void) 
{
    
    CMCON = 0x07;                           // Disable internal comparators
    T1CON = 0x00;                           // Disable timer1 
       
    // *** Timer0 configuration ***
    // 4MHz / 4 = 1MHz = 1us;
    // Prescaler = 4 -> 4us for each interrupt call
    // 256 * 4us = 1.024 ms     
    OPTION_REG = 0xD1;                      // 0b11010001
                                            //   |______________ Pull-ups disabled
                                            //    |_____________ Interrupt on rising edge
                                            //     |____________ Internal clock source
                                            //      |___________ Increment TIMER0 on high-to-low (not used, used internal clock)
                                            //       |__________ Pre-scaler for timer0
                                            //        |||_______ Pre-scaler value (0b001 => 1:4)
    
        
    INTCON= 0x88;                           // 0b10001000   (Global Interrupt enabled and Port Change Interrupt Enabled)
    ANSEL = 0x0;                            // ADC disabled 1/2       
    TRISIO = TRIS_CONF;                     // Sets pins directions    
    GPIO = 0x0;                             // Clears all pins        
    IOC = DIN;                              // Sets interrupt on change pin
    
#ifdef SLEEP_MODE
    SLEEP();                                
#endif
    
    // *** Infinite loop ***
    while(1)                                            
    {        
        if (flag_analyze)                               // Analysis required?
        {
            counter_valids = 0;                         // Resets valid messages counter
                        
            for(char a=0;a<3;++a)                       // Loop for 3 messages received
            {
                address_1 = (ht12_data[a] >> 4) & 0xFF; // Shifts address bits 
                                
                if (address_1 == HT12D_Address)         // Address of message is equals expected address?
                {
                   counter_valids++;                    // Increments valid message counter
                   data_1 = ht12_data[a] & 0xF;         // Extracts data bits from message
                   ht12_data[a]= 0;                     // Resets message to zero
                }
            }
                        
            if (counter_valids == 3)                    // All received messages are valid?
            { 
                PORT |=(VT);                            // VT pin to HIGH         
                
            
                if (data_pre!=data_1)                   // If current data is new (not equal to previous data)
                { 
                    data_pre = data_1;                  // Updates previous data
                    set_data_pins();                    // Sets data from message to pins
                }  
                
                #ifdef BUZZER_D9
                    buzzer_flag = 1;                        // In buzzer mode, sets buzzer flag
                #endif
                
            } 
            else 
            {
                PORT &=~(VT);                           // Otherwiese, sets VT pin to LOW
            }
                                    
            bit_counter = 0;                            // Clears bit counter
            flag_analyze = 0;                           // Clear analysis flag
            TMR0 = 0;                                   // Clears timer0 counter
        }        
    }
}


// *****************************************************************************
// *** Functions***
// *****************************************************************************

// Sets data value to data pins
void set_data_pins(void)
{    
    // Version 0.32 - faster than loop
    if (data_1>>3 & 1) PORT |= D8; else PORT &= ~D8;    
    #ifndef BUZZER_D9
    if (data_1>>2 & 1) PORT |= D9; else PORT &= ~D9;
    #endif
    if (data_1>>1 & 1) PORT |= D10; else PORT &= ~D10;
    #ifndef SLEEP_MODE
    if (data_1 & 1) PORT |= D11; else PORT &= ~D11;
    #endif    

}

