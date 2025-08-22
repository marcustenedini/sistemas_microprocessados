#include <msp430.h> 
#include <stdint.h>

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
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config_mestre();
    __enable_interrupt();

    while(1){
        i2csend(address,0x08);
        __delay_cycles(1000000);
        i2csend(address,0x00);
        __delay_cycles(1000000);
        i2csend(address,0x08);
    }
   lcdInit();
   while(1){
       lcdPrint("Hello World");

   }



}
