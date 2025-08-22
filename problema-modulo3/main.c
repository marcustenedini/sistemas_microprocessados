#include <msp430.h> 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define address 0x27 //lcd
#define MAX_CHANNELS 8

uint8_t i2csend(uint8_t addr,uint8_t byte){
    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;

    while(!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = byte;

    while(UCB0CTL1 & UCTXSTT);

    if(UCB0IFG & UCNACKIFG){
        UCB0CTL1 |= UCTXSTP;
        while(UCB0CTL1 & UCTXSTP);
        return 1;
    }
    while(!(UCB0IFG & UCTXIFG));
    UCB0CTL1 |= UCTXSTP;
    while(UCB0CTL1 & UCTXSTP);
    return 0;
}

uint8_t lcdWriteNibble(uint8_t nibble, uint8_t isChar){
    uint8_t byte = isChar | (nibble << 4);
    i2csend(address,byte);
    i2csend(address,byte | 0x04);
    i2csend(address,byte);

    return 0;
}

uint8_t lcdWriteByte(uint8_t byte, uint8_t isChar){
    uint8_t lsb = byte & 0x0F;
    uint8_t msb = (byte >> 4) & 0x0F;
    lcdWriteNibble(msb,isChar);
    lcdWriteNibble(lsb,isChar);

    return 0;
}
//lcd
void lcdInit(void){
    lcdWriteNibble(0x3,0);
    lcdWriteNibble(0x3,0);
    lcdWriteNibble(0x3,0);
    lcdWriteNibble(0x2,0);
    lcdWriteByte(0x0C,0);
    lcdWriteByte(0x01,0);
    __delay_cycles(153000);
}

void lcdPrint(char *str){
    while(*str)
        lcdWriteByte(*str++,1);
}
void config_mestre_lcd(void){

    UCB0CTL1 |= UCSWRST;

    UCB0CTL0 |= UCMST | UCMODE_3| UCSYNC ;
    UCB0CTL1 |= UCTR |UCSSEL__SMCLK;
    UCB0BRW = 100;

    P3SEL |= BIT0;
    P3REN |= BIT0;
    P3OUT |= BIT0;

    P3SEL |= BIT1;
    P3REN |= BIT1;
    P3OUT |= BIT1;

    UCB0CTL1 &= ~UCSWRST;
}
//conversor AD
void configurarTimerADC(uint8_t n) {

    TA0CTL = MC__UP | TASSEL__SMCLK;
    TA0CCR0 = (1000000/200) - 1;
    TA0CCR1 =  10 - 1;   //tempo que sampcom deve ficar ativado(arredondei para 3) é 1.95 na realidade
    TA0CCTL1 = OUTMOD_7;
}

typedef struct {
    uint16_t values[MAX_CHANNELS];
} ADCResult;

uint8_t numChannels;  // Guarda o número de canais configurados

void adcConfig(uint8_t* pins, uint8_t n) {
    numChannels = n;  // Salva número de canais

    ADC12CTL0 &= ~ADC12ENC;  // Desabilita ADC para configuração

    ADC12CTL0 = ADC12SHT0_4 |  // Tempo de amostragem
                ADC12ON;       // Liga ADC

    ADC12CTL1 = ADC12SHS_1 |   // Disparo pelo Timer
                ADC12SHP  |    // Sample-and-hold
                ADC12SSEL_0 |  // Clock MODOSC (~5MHz)
                ADC12CONSEQ_3; // Conversão sequencial

    ADC12CTL2 = ADC12RES_2;    // Resolução de 12 bits

    // Configura os pinos e os canais ADC
    uint8_t i = 0;
    //while (i < n) {
    //    P6SEL |= (1 << i);  // Habilita o pino como entrada ADC
    //    ADC12MCTL[i] = i;   // Configura canal ADC
    //    i++;
    //}
    P6SEL |= BIT1;
    ADC12MCTL0 = 1;
    //ADC12MCTL[n - 1] |= ADC12EOS; // Marca o último canal com End of Sequence (EOS)
    ADC12MCTL0 |= ADC12EOS;
    ADC12CTL0 |= ADC12ENC; // Habilita ADC
}

uint8_t adcRead() {
    uint8_t result;

    ADC12CTL0 |= ADC12SC;  // Inicia conversão
    ADC12CTL0 &= ~ADC12SC; // Desativa disparo (modo sequencial)

    uint8_t i = 0;

    // Aguarda e coleta os valores convertidos
    //while (i < numChannels) {
      //  while (!(ADC12IFG & (1 << i))); // Espera conversão terminar
        result = ADC12MEM0; // Lê o valor do ADC
        //i++;
    //}


    return result;
}


//uart
void uartconfig(){

    //19200bps
    //BRW=2^20/19200=54.613/16=3.413
    //BRF=0.413*16=6.6
    UCA1CTL1 = UCSWRST;
    UCA1CTL1 |= UCSSEL__SMCLK;

    //configurar o clock
    UCA1BRW = 3;
    UCA1MCTL = UCBRF_3 | UCOS16;


    //configurar pinos
    P4SEL |= BIT4 | BIT5;

    UCA1CTL1 &= ~UCSWRST;

    UCA1IE = UCTXIE | UCRXIE;

}

void uartPrint(char *str){

    while(*str){
        while(UCA1IFG & UCTXIFG){
            UCA1TXBUF = *str++;
        }
    }
}

uint8_t buffer[16];
uint8_t uartReady;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	//configurar timer para adc
	//amostrar canais
	//receber a qualquer momento n que altera a quantidade de n canais
	//imprimir via uart e lcd os canais
	// Configurações iniciais
	config_mestre_lcd();
    uartconfig();
    lcdInit();

	    // Configuração do ADC
    uint8_t pins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // Canais A0 a A7
    adcConfig(pins, 0);
    while(1)
    {
        if (uartReady) {
            uint8_t numChannels = atoi((char*)buffer); // Converte o buffer para número
            if (numChannels > 0 && numChannels <= MAX_CHANNELS) {
                adcConfig(pins, numChannels);
            }
            uartReady = 0; // Reset da flag
        }

        uint8_t result = adcRead();
        char str[16];
        uint8_t i = 0;

        while (i < 2) {
            sprintf(str, "A%d: %03X ", i, ADC12MEM[i]);

            lcdPrint(str);
            uartPrint(str);
            i++;
        }
    }
	return 0;
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
