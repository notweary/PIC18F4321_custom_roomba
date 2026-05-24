/* 
 * File:   newmain.c
 * Author: Hedonism-Bot
 *
 * Created on November 25, 2024, 6:57 PM
 */

//#include <stdio.h>
//#include <stdlib.h>
#include <xc.h>
#include <pic18f4321.h>
#include "pic18f4321-Config.h"
#include "LCD.h"
#define _XTAL_FREQ 8000000
#define TRIG PORTBbits.RB0
#define ECHO PORTBbits.RB1
#define IN1 PORTAbits.RA0
#define IN2 PORTAbits.RA1
#define IN3 PORTAbits.RA2
#define IN4 PORTAbits.RA3

void Pwm_setup(int x);
void start_TMR2();
unsigned int dist_calc();


void main(void) {
    OSCCON = 0x72; // Configure the oscillator for 8 MHz
    ADCON1 = 0x0F; // Configure all pins as digital
    
    TRISBbits.RB0 = 0; // TRIG pin as output
    TRISBbits.RB1 = 1; // ECHO pin as input
    
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    T1CON = 0x30; // Prescaler 1:8, internal clock (Fosc/4)

    LCD_init(); // Initialize LCD
    TMR2 = 0;

    while(1){        
        
        unsigned int result;
        
        result = dist_calc();
        LCD_clear(); // Clear LCD
        LCD_write_string("Distance cm"); // Display "Distance cm"
        LCD_cursor_set(2, 1); // Set cursor to line 2
        LCD_write_variable(result, 1);
        __delay_ms(500);        
        
        
        while (result > 12){
            IN1 = 0;
            IN2 = 1;
            IN3 = 0;
            IN4 = 1;
            Pwm_setup(10);    
            start_TMR2();
            result = dist_calc();
        }
        
        while (result <= 12){ 
            LCD_clear();
            LCD_write_string("Too close!"); // Display "Distance cm"
            IN1 = 1;
            IN2 = 0;
            IN3 = 0;
            IN4 = 1;
            Pwm_setup(10);
            start_TMR2();
            result = dist_calc();
        }           
    }
}

unsigned int dist_calc(){
    unsigned int dist;
    unsigned int cycles;
    PIR1bits.TMR1IF = 0; // Clear Timer1 overflow flag
    TMR1 = 0; // Clear Timer1 register
    dist = 0; // Clear distance variable


    TRIG = 0; // Clear TRIG pin
    __delay_ms(10); // Wait 10ms to stabilize the sensor
    TRIG = 1; // Set TRIG pin
    __delay_us(10); // Wait 10us to generate 8 cycles of 40KHz ultrasonic waves
    TRIG = 0; // Clear TRIG pin

    T1CONbits.TMR1ON = 1; // Enable Timer1
    while(ECHO == 0); // Wait for ECHO pin to go high
    while(ECHO == 1); // Wait for ECHO pin to go low
    T1CONbits.TMR1ON = 0; // Disable Timer1

    cycles = TMR1; // Read Timer1 value
    dist = (cycles * 0.0343) / 2; // Calculate distance

    return dist;
}

void Pwm_setup(int x) {
    TRISCbits.RC2 = 0;
    TRISCbits.RC1 = 0;
    T2CON = 0x00;
    PR2 = 61;
    
    //CCP1
    CCP1CON = 0x00;
    CCPR1L = (unsigned char)((61 * x) / 10); //30
    
    
    //CCP2
    CCP2CON = 0x00;
    CCPR2L = (unsigned char)((61 * x) / 10);
    
    CCP1CON = 0x0C; //.5 lower two bits, PWM mode
    CCP2CON = 0x0C;
}

void Pwm_setup_(int x) {
    TRISCbits.RC2 = 0;
    TRISCbits.RC1 = 0;
    T2CON = 0x00;
    PR2 = 61;
    
    //CCP1
    CCP1CON = 0x00;
    CCPR1L = (unsigned char)((61 * x) / 10); //30
    
    
    //CCP2
    CCP2CON = 0x00;
    CCPR2L = (unsigned char)((61 * x) / 10);
    
    CCP1CON = 0x0C; //.5 lower two bits, PWM mode
    CCP2CON = 0x0C;
}


void start_TMR2(){
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;
    while (PIR1bits.TMR2IF == 0);
}