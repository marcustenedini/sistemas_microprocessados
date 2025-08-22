#include <msp430.h> 
#include <stdint.h>
#include <stdio.h>
#define address 0x27

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

void config_mestre(void){

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
uint16_t adcRead(uint8_t pin){
    ADC12CTL0 &= ~ADC12ENC;

    ADC12CTL0 = ADC12SHT0_4 |
                ADC12ON;
    ADC12CTL1= ADC12SHS_0 |
                ADC12SHP  |
                ADC12CONSEQ_0;
    ADC12CTL2= ADC12RES_2;

    P6SEL |= 1 << pin;

    ADC12MCTL0 = pin;

    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;

    while(!(ADC12IFG & BIT0));
    return ADC12MEM0;

}
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
int main(void)
{
    static const uint8_t pino=4;
    config_mestre();
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	lcdInit();
	uartconfig();
	int i=1;
	char str[16];
	while(i--){
	    volatile uint16_t x = adcRead(pino);
        sprintf(str, "A%d: %03X ", i, ADC12MEM0);
	    lcdPrint(str);
	    uartPrint(str);
	}

	return 0;
}
