/*
 * Part3.c
 *
 *  Created on: Feb 14, 2023
 *      Author: Luke Wilkins
 *
 *      This code will blink the LED at a rate defined by the length of a button press.
 */

#include <msp430.h>
void gpioInit();
void timerInit();

const unsigned int default_time = 1024; // Default time: 0.25s
unsigned int time = default_time; // Blink timer
unsigned int hold = default_time; // Default hold time
char count = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();

    timerInit();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    P2IFG &= ~BIT3;                         // P2.3 IFG cleared
    P4IFG &= ~BIT1;                         // P4.1 IFG cleared

    __bis_SR_register(GIE);                 // Enter LPM3 w/interrupt
    while(1){
        if (count == 1){
            hold++;
        }
        if (count == 0){
            TB1CCR0 = time;
        }
    }
}

void gpioInit(){
    // Configure RED LED on P1.0 as Output
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    P6OUT &= ~BIT6; // temp indicator light
    P6DIR |= BIT6;
    P6OUT &= ~BIT6;

    // Configure Button on P2.3 as input with pullup resistor
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES &= ~BIT3;                         // P2.3 Low --> High edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    // Configure Button on P4.1 as input with pullup resistor
    P4OUT |= BIT1;                          // Configure P4.1 as pulled-up
    P4REN |= BIT1;                          // P4.1 pull-up register enable
    P4IES &= ~BIT1;                         // P4.1 Low --> High edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled
}

void timerInit(){
    // Timer1_B3 setup
    TB1CCTL0 = CCIE;                          // TBCCR0 interrupt enabled
    TB1CCR0 = time;                           // Triggers the timer every time variable "time" passes
    TB1CTL = TBSSEL_1 | MC_1 | ID_3;          // ACLK, up mode, clk divider 8
}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if (!(P2IN & BIT3)) // Checks if the interrupt was triggered off a falling edge.
    {
        hold = 0;
        count = 1;
        P6OUT |= BIT6;
        P2IES &= ~BIT3; // Swaps the interrupt to check for low to high edge (rising)
    }

    else if (P2IN & BIT3)       // Checks if the interrupt was triggered off a rising edge.
      {
          count = 0;
          time = hold;
          P6OUT &= ~BIT6;
          P2IES |= BIT3;     // Swaps the interrupt to check for high to low edge (falling)
      }
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    hold = 0;
    P6OUT |= BIT6;
    time = default_time;                    // Reset time back to default
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
}

// LED Timer interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    P1OUT ^= BIT0;                           // Toggle Red LED
}
