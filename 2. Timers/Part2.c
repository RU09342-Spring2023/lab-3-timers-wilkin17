/*
 * Part2.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Russell Trafford
 *
 *      This code will need to change the speed of an LED between 3 different speeds by pressing a button.
 */

#include <msp430.h>

unsigned int time = 50000;                  // Initialize timer at slowest state
char setting = 0;                           // Initialize setting to start timer in slowest state

void gpioInit();
void timerInit();

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG
    __bis_SR_register(LPM3_bits | GIE);     // Enter LPM3 w/ interrupts
    while (1);

}


void gpioInit(){
    // Configure Green LED on P6.6 as Output
    P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state
    P6DIR |= BIT6;                          // Set P6.6 to output direction

    // Configure Button on P2.3 as input with pullup resistor
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES &= ~BIT3;                         // P2.3 Low --> High edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled
}

void timerInit(){
    // Timer1_B3 setup
    TB1CCTL0 = CCIE;                          // TBCCR0 interrupt enabled
    TB1CCR0 = time;                           // Triggers the timer every 50000 clock cycles
    TB1CTL = TBSSEL_1 | MC_2;                 // ACLK, continuous mode
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG
    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.
        setting++;
        if (setting == 3)
            setting = 0;
        if (setting == 0)                  // slow speed state (default)
            time = 50000;
        if (setting == 1)                  // mid speed state
            time = 25000;
        if (setting == 2)                  // fast speed state
            time = 12500;
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    // @TODO You can toggle the LED Pin in this routine and if adjust your count in CCR0.
    P6OUT ^= BIT6;                           // Toggle Green LED
    TB1CCR0 += time;                         // Add Offset to TB1CCR0 based off of the time setting defined in the button interrupt
}


