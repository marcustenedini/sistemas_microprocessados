#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define tamanho_buffer 1000

char buffer[tamanho_buffer];
uint8_t uartReady;

void uart_init() {

    // Configuração da UART0 (para comunicação com LoRa)
    UCA0CTL1 = UCSWRST;                      // Coloca UCA0 em reset
    UCA0CTL1 |= UCSSEL__SMCLK;               // Usa SMCLK como clock source
    UCA0BR0 = 6;                             // Configura baud rate
    UCA0BR1 = 0;
    UCA0MCTL = UCBRF_13 | UCOS16;            // Configuração de modulação
    P3SEL |= BIT3 | BIT4;                    // Define P3.3 (TX) e P3.4 (RX) como UART
    UCA0CTL1 &= ~UCSWRST;                    // Inicializa UCA0

    // Configuração da UART1 (para debug via terminal)
    UCA1CTL1 = UCSWRST;
    UCA1CTL1 |= UCSSEL__SMCLK;
    UCA1BR0 = 6;
    UCA1BR1 = 0;
    UCA1MCTL = UCBRF_13 | UCOS16;
    P4SEL |= BIT4 | BIT5;
    UCA1CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
    //UCA1IE |= UCRXIE;
    // Configuração dos pinos M0 e M1 para modo normal
    P3DIR |= BIT0 | BIT1;
    P3OUT &= ~(BIT0 | BIT1); // M0 = 0, M1 = 0
}

void uartPrintTerminal(char *str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));        // Wait for TX buffer to be ready
        UCA1TXBUF = *str++;                 // Send character
    }
}

void uartPrintLora(char *str) {
    while (*str) {
        while (!(UCA0IFG & UCTXIFG));        // Wait for TX buffer to be ready
        UCA0TXBUF = *str++;                 // Send character
    }
}

void main() {
     WDTCTL = WDTPW | WDTHOLD;  // Desativa watchdog
    uart_init();
    uartPrintTerminal("oi");
    while (1) {
            if(uartReady){
                uartReady = 0;
                uartPrintTerminal(buffer);
                memset(buffer, 0, tamanho_buffer);
     }
}
}

#define _ECHO_ON

#pragma vector = USCI_A1_VECTOR
__interrupt void uart_isr()
{
    static uint8_t i;
    uint8_t c = UCA1RXBUF;
#ifdef _ECHO_ON
    UCA1TXBUF = c;
#endif
    if(c == 0x0D)
    {
        uartReady = 1;
        buffer[i] = 0;
        i = 0;
        return;
    }
    buffer[i] = c;
    i = (i + 1) & 0xF;
}

#pragma vector = USCI_A0_VECTOR
__interrupt void uart_isr0() {
    static uint16_t i = 0;
    char c = UCA0RXBUF;
    if (i < tamanho_buffer - 1) {
        buffer[i++] = c;
    }
    if (c == '\n') {
        buffer[i] = '\0';
        uartReady = 1;
        i = 0;
    }
}
