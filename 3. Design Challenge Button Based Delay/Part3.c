/*
 * Part3.c
 *
 *  Created on: Feb 14, 2023
 *      Author: Luke Wilkins
 *
 *      This code will blink the LED at a rate defined by the length of a button press.
 */

#include <msp430.h>
#include <stdint.h>
void gpioInit();
void timerInit();

int hold = 0;
char timerstate = 0;

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
        if (timerstate == 0){
            TB1CTL = TBSSEL_1 | MC_2 | ID_1;
        }
        if (timerstate == 1){
            TB1CTL = TBSSEL_1 | MC_2 | ID_3;
        }
    }
}

void gpioInit(){
    // Configure RED LED on P1.0 as Output
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

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
    TB1CCR0 = 4096;                           // Triggers the timer every time variable "time" passes
    TB1CTL = TBSSEL_1 | MC_2 | ID_1 | TBCLR | TBIE;          // ACLK, cont. mode, clk divider 2, interrupt enabled
}

// LED Timer interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    if (timerstate == 0){
        P1OUT ^= BIT0;
        TB1CCR0 += 4096; // default
    }
    if (timerstate == 1){
        P1OUT ^= BIT0;
        TB1CCR0 += hold; // button hold time
    }
}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
   TB1CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE; // ACLK, cont. mode, clk divider 8
   timerstate = 1;
   while (~P2IN & BIT3){
       hold = TB1R;                             // Increase hold time while button is held down
   }
   P2IFG &= ~BIT3;                         // Clear P2.3 IFG

}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    timerstate = 0;
    TB1CCR0 = 4096;
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
}

