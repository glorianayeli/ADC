#include <18F4620.h>
#DEVICE ADC=10//no es necesario si se trabaja a 10bits
#include <stdlib.h>
#fuses HS,NOWDT,NOPROTECT,PUT,BROWNOUT,NOLVP,NOMCLR
#use delay(clock=16000000)


#define __DEBUG_SERIAL__ //Si comentas esta linea se deshabilita el debug por serial y el PIN_C6 puede ser usado en forma digital I/O

#ifdef __DEBUG_SERIAL__
   #define TX_232        PIN_C6
   #define RX_232        PIN_C7
   #use RS232(BAUD=9600, XMIT=TX_232, BITS=8,PARITY=N, STOP=1,UART1,RCV=RX_232)
   #use fast_io(c)
#endif

#define  ValorConversionTemperatura 0.48828125
#define  ValorConversionPresionAtmosferica .1124144673
#define  ValorConversionHumedadRelativa .09775171065

int TiempoEsperaADC=0, FlagADC=0, FlagEstadoPuerto=1,FlagSerial=0;
long UltimoValorADC=0;
float ValorConversion=0;

#int_timer0
void timer_0() 
{ 
    set_timer0(6);
    TiempoEsperaADC++;
}
#int_AD
void ADC()
{
   UltimoValorADC=read_adc(ADC_READ_ONLY);
   FlagADC=1;
}
#INT_RDA
void isrRDA(void)
{
   FlagSerial=1;
}
void main(void)
{
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_AD);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_16|RTCC_8_BIT);
   set_timer0(6);
   set_tris_a(0x07);
   setup_adc_ports(AN0_TO_AN2);
   setup_adc(adc_clock_internal);
   while(TRUE)
   {
         if(FlagEstadoPuerto==1&&FlagSerial==1)
         {
            set_adc_channel(0);
            if(TiempoEsperaADC>=200&&FlagADC==1)
            {
               ValorConversion=UltimoValorADC*ValorConversionTemperatura;
               printf("Temperatura %f\r",ValorConversion);
               FlagADC=0;
               TiempoEsperaADC=0;
               FlagEstadoPuerto=2;
               FlagSerial=0;
            }
         }
         if(FlagEstadoPuerto==2&&FlagSerial==1)
         {
            set_adc_channel(1);
            if(TiempoEsperaADC>=200&&FlagADC==1)
            {
               ValorConversion=(UltimoValorADC*ValorConversionPresionAtmosferica)+898;
               printf("Presion Atmosferica %f\r",ValorConversion);
               FlagADC=0;
               TiempoEsperaADC=0;
               FlagEstadoPuerto=3;
               FlagSerial=0;
            }
         }
         if(FlagEstadoPuerto==3&&FlagSerial==1)
         {
            set_adc_channel(2);
            if(TiempoEsperaADC>=200&&FlagADC==1)
            {
               ValorConversion=UltimoValorADC*ValorConversionHumedadRelativa;
               printf("Humedad Relativa %f\r",ValorConversion);
               FlagADC=0;
               TiempoEsperaADC=0;
               FlagEstadoPuerto=1;
               FlagSerial=0;
            }
         }
         read_adc(ADC_START_ONLY);
   }
}

