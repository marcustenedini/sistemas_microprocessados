#include <msp430.h> 
#include <stdint.h>
#include <string.h>

#define tamanho_buffer 1000
void uartPrint(char * str)
{
    while(*str)
    {
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }
}

char buffer[tamanho_buffer];
uint8_t uartReady;

void main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

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
     // Configuração dos pinos M0 e M1 para modo normal
     P3DIR |= BIT0 | BIT1;
     P3OUT &= ~(BIT0 | BIT1); // M0 = 0, M1 = 0
     __enable_interrupt();

    uartPrint("LoRa Receptor Iniciado via Interrupção!\n\r");

    while(1)
    {
        if(uartReady){
            uartReady = 0;
            uartPrint(buffer);
            uartPrint("\n\r");
            memset(buffer, 0, tamanho_buffer);
        }
    }
}
/*
#define _ECHO_ON

#pragma vector = USCI_A1_VECTOR
__interrupt void uart_isr()
{
    uartPrint("Oi.\n\r");
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
*/

// Interrupção de recepção UART0
#pragma vector = USCI_A0_VECTOR
__interrupt void uart_isr0() {
    static uint16_t i = 0; // Índice do buffer
    char c = UCA0RXBUF; // Lê o caractere recebido

    // Armazena o caractere no buffer
    if (i < tamanho_buffer - 1) {
        buffer[i++] = c;
    }

    // Verifica se a mensagem terminou (caractere de nova linha)
    if (c == '\n') {
        buffer[i] = '\0'; // Finaliza a string
        uartReady = 1; // Indica que uma mensagem completa foi recebida
        i = 0; // Reseta o índice do buffer
    }
}
