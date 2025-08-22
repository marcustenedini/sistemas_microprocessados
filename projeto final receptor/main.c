#include <msp430.h> 


void config_uart(){

    P3DIR |= BIT0 | BIT1; //M0 e M1
    P3OUT &= ~(BIT0 | BIT1);

    P3DIR &= ~BIT5; //AUX
    P3REN |= BIT5;
    P3OUT &= ~BIT5;//pull-down

    //9600bps
    //BRW=2^20/19200=109.226/16=6.823
    //BRF=0.823*16=13.168
    UCA1CTL1 = UCSWRST;
    UCA1CTL1 |= UCSSEL__SMCLK;

    //configurar o clock
    UCA1BRW = 6;
    UCA1MCTL = UCBRF_13 | UCOS16;

    //configurar pinos
    P4SEL |= BIT4 | BIT5;
    P3SEL |= BIT3 | BIT4;//p3.3 TX p3.4 RX
    UCA1CTL1 &= ~UCSWRST;

    UCA1IE = UCTXIE;

}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	config_uart();
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
