#include <msp430.h>
#include <stdint.h>
#include <string.h>

void uartPrint(char * str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }

char buffer[300];

void main() {
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

    // Configuração dos pinos M0 e M1 para modo normal
    P3DIR |= BIT0 | BIT1;
    P3OUT &= ~(BIT0 | BIT1); // M0 = 0, M1 = 0

    uartPrint("LoRa Receptor Iniciado via Interrupção!\n\r");

    while (1) {
        uint8_t i = 0;

        // Aguarda chegada de dados
        while (!(UCA0IFG & UCRXIFG)); // Aguarda um byte

        // Lê caracteres até encontrar '\r' ou atingir tamanho máximo do buffer
        while (i < sizeof(buffer) - 1) {
            while (!(UCA0IFG & UCRXIFG)); // Espera um novo byte
            buffer[i] = UCA0RXBUF;

            if (buffer[i] == '\n') { // Se encontrou o fim da mensagem
                buffer[i] = '\0';
                break;
            }
            i++;
        }

        buffer[i] = '\0'; // Finaliza a string corretamente

        uartPrint("Recebido: ");
        uartPrint(buffer);
        uartPrint("\n\r");

    }
}
