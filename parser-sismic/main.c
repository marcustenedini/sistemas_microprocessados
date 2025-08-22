#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TAMANHO_CAMPOS 12
#define TAMANHO_BUFFER 1000
#define INVALIDO "inválido"
#define NO_KMH 1.852

void uartPrintTerminal(char *str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }
}

void config_uart() {
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
       UCA0IE |= UCRXIE;
       // Configure M0, M1, and AUX pins
       P3DIR |= BIT0 | BIT1;
       P3OUT &= ~BIT0;
       P3OUT &= ~BIT1;

       P3DIR &= ~BIT5;
       P3REN |= BIT5;
       P3OUT |= BIT5;
}

typedef struct {
    char tempo[10];
    char status[10];
    char latitude[20];
    char lat_dir[2];
    char longitude[20];
    char lon_dir[2];
    char velocidade[10];
    char data[10];
} GPRMCData;

void format_tempo(char *tempo_atual, char *tempo_formatado) {
    if (strlen(tempo_atual) < 6) {
        strcpy(tempo_formatado, INVALIDO);
        return;
    }
    snprintf(tempo_formatado, 9, "%.2s:%.2s:%.2s", tempo_atual, tempo_atual + 2, tempo_atual + 4);
}

void format_data(char *date_atual, char *data_formatada) {
    if (strlen(date_atual) < 6) {
        strcpy(data_formatada, INVALIDO);
        return;
    }
    snprintf(data_formatada, 11, "%.2s/%.2s/%.2s", date_atual, date_atual + 2, date_atual +4); // dd/mm/aa
}

void format_velocidade(char *velocidade_atual, char *velocidade_convertida) {
    double velocidade_nos = atof(velocidade_atual);
    double velocidade_kmh = velocidade_nos * NO_KMH;
    if (velocidade_nos > 0) {
        snprintf(velocidade_convertida, 10, "%.2f km/h", velocidade_kmh);
    } else {
        strcpy(velocidade_convertida, INVALIDO);
    }
}
void parse_gprmc(const char *sentence, GPRMCData *data) {
    char copy[130];
    strcpy(copy, sentence);
    char *campos[TAMANHO_CAMPOS];
    int index = 0;
    char *start = copy;

    // Extrai os campos, considerando campos vazios
    while (*start != '\0' && index < TAMANHO_CAMPOS) {
        char *end = strchr(start, ',');  // Encontra a próxima vírgula
        if (end == NULL) {
            // Último campo (não há mais vírgulas)
            campos[index++] = start;
            break;
        }
        // Substitui a vírgula por um terminador nulo
        *end = '\0';

        // Adiciona o campo ao array
        campos[index++] = start;

        // Avança para o próximo campo
        start = end + 1;

        // Se houver duas vírgulas seguidas, adiciona uma string vazia
        if (*start == ',') {
            campos[index++] = "";
        }
    }

    // Preenche a struct
    if (index > 1 && campos[1][0]) {
        strcpy(data->tempo, campos[1]);
    } else {
        strcpy(data->tempo, INVALIDO);
    }
    if (index > 2 && campos[2][0]) {
        strcpy(data->status, campos[2]);
    } else {
        strcpy(data->status, INVALIDO);
    }
    if (index > 3 && campos[3][0]) {
        strcpy(data->latitude, campos[3]);
    } else {
        strcpy(data->latitude, INVALIDO);
    }
    if (index > 4 && campos[4][0]) {
        strcpy(data->lat_dir, campos[4]);
    } else {
        strcpy(data->lat_dir, INVALIDO);
    }
    if (index > 5 && campos[5][0]) {
        strcpy(data->longitude, campos[5]);
    } else {
        strcpy(data->longitude, INVALIDO);
    }
    if (index > 6 && campos[6][0]) {
        strcpy(data->lon_dir, campos[6]);
    } else {
        strcpy(data->lon_dir, INVALIDO);
    }
    if (index > 7 && campos[7][0]) {
        strcpy(data->velocidade, campos[7]);
    } else {
        strcpy(data->velocidade, INVALIDO);
    }
    if (index > 10 && campos[10][0]) {
        strcpy(data->data, campos[10]);
    } else {
        strcpy(data->data, INVALIDO);
    }
    // Formatação
    char tempo_at[10];
    char data_at[11];
    char velocidade_at[10];
    format_tempo(data->tempo, tempo_at); //parametro funcioma como um result
    format_data(data->data, data_at);
    format_velocidade(data->velocidade, velocidade_at);
    //preenche a struct formatada
    strcpy(data->tempo, tempo_at);
    strcpy(data->data, data_at);
    strcpy(data->velocidade, velocidade_at);
}
char buffer[TAMANHO_BUFFER];
volatile uint8_t uartReady = 0;

void main() {
    WDTCTL = WDTPW | WDTHOLD;
    config_uart();
    __enable_interrupt();
    uartPrintTerminal("LoRa Receptor Iniciado via Interrupcão!\n\r");
    //char teste[100] ="$GPRMC,170247.00,A,1541.36853,S,04749.91276,W,2.082,,180225,,,A*73";
    while (1) {
        if (uartReady) {
            uartReady = 0;
            if (strstr(/*teste*/buffer, "$GPRMC") != NULL) {

                GPRMCData data;
                parse_gprmc(buffer, &data);
                uartPrintTerminal("Tempo: "); uartPrintTerminal(data.tempo); uartPrintTerminal("\n\r");
                uartPrintTerminal("Status: "); uartPrintTerminal(data.status); uartPrintTerminal("\n\r");
                uartPrintTerminal("Latitude: "); uartPrintTerminal(data.latitude); uartPrintTerminal(data.lat_dir); uartPrintTerminal("\n\r");
                uartPrintTerminal("Longitude: "); uartPrintTerminal(data.longitude); uartPrintTerminal(data.lon_dir); uartPrintTerminal("\n\r");
                uartPrintTerminal("Velocidade: "); uartPrintTerminal(data.velocidade); uartPrintTerminal("\n\r");
                uartPrintTerminal("Data: "); uartPrintTerminal(data.data); uartPrintTerminal("\n\r");
                uartPrintTerminal("\n\r");
                _delay_cycles(1000000);
            }
            memset(buffer, 0, TAMANHO_BUFFER);
        }
    }
}

#pragma vector = USCI_A0_VECTOR
__interrupt void uart_isr0() {
    static uint16_t i = 0;
    char c = UCA0RXBUF;
    if (i < TAMANHO_BUFFER - 1) {
        buffer[i++] = c;
    }
    if (c == '\n') {
        buffer[i] = '\0';
        uartReady = 1;
        i = 0;
    }
}
