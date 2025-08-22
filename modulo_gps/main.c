#include <msp430.h>
#include <stdint.h>
#include <string.h>

#define M0 0
#define M1 0

char buffer[1000];

void config_uart_teste() {
    // Configure UCA0 (LoRa)
    UCA0CTL1 = UCSWRST;
    UCA0CTL1 |= UCSSEL__SMCLK;
    UCA0BR0 = 6;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRF_13 | UCOS16;
    P3SEL |= BIT3 | BIT4;
    UCA0CTL1 &= ~UCSWRST;

    // Configure UCA1 (Terminal)
    UCA1CTL1 = UCSWRST;
    UCA1CTL1 |= UCSSEL__SMCLK;
    UCA1BR0 = 6;
    UCA1BR1 = 0;
    UCA1MCTL = UCBRF_13 | UCOS16;
    P4SEL |= BIT4 | BIT5;
    UCA1CTL1 &= ~UCSWRST;

    // Configure M0, M1, and AUX pins
    P3DIR |= BIT0 | BIT1;
    P3OUT &= ~(BIT0 | BIT1);

    P3DIR &= ~BIT5;
    P3REN |= BIT5;
    P3OUT |= BIT5;
}

void uartPrintTerminal(char *str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }
}

void uartPrintLora(char *str) {
    while (*str) {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = *str++;
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    config_uart_teste();
    __delay_cycles(2000000);

    while (1) {
          //uartPrintLora(buffer);
          //uartPrintLora("\n\n\r");
          uartPrintTerminal("hello");
          uartPrintTerminal("\n\n\r");

    }
}
