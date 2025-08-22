#include <msp430.h> 
#include <stdint.h>

typedef struct {
    uint16_t value1;
    uint16_t value2;
} ADCResult;

ADCResult adcRead(uint8_t pin1,uint8_t pin2){
    ADCResult result;
    ADC12CTL0 &= ~ADC12ENC;

    ADC12CTL0 = ADC12SHT0_4 |
                ADC12ON;
    ADC12CTL1= ADC12SHS_0 |
                ADC12SHP  |
                ADC12CONSEQ_3;
    ADC12CTL2= ADC12RES_2;

    P6SEL |= 1 << pin1 | 1 << pin2 ;

    ADC12MCTL0 = pin1;
    ADC12MCTL1 = pin2 | ADC12EOS;

    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;

    while(!(ADC12IFG & BIT0));
    result.value1 = ADC12MEM0;
    while(!(ADC12IFG & BIT1));
    result.value2 = ADC12MEM1;

    return result;

}

int main(void)
{
    static const uint8_t pino1=4;
    static const uint8_t pino2=3;
    ADCResult adcValues;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    while(1){
        adcValues = adcRead(pino1, pino2);
        volatile uint16_t x = adcValues.value1;
        volatile uint16_t y = adcValues.value2;
    }

}
