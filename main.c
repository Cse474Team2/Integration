#include "tiva.h"
#include "bluetooth.h"
#include <stdint.h>
#include <stdlib.h>

float measureD(void);
uint32_t counter = 0;
float distance = 0;
#define MAX_TIME 7500
#define ECHO (1U<<2) //PA2(Input)
#define TRIGGER (1U<<4) //PA4(OUTPUT)

void delay_Microsecond(uint32_t time); 

float measureD(void){ 
	GPIO_PORTA_DATA &=~TRIGGER;
	delay_Microsecond(10);
	GPIO_PORTA_DATA |= TRIGGER;
	delay_Microsecond(10);
	GPIO_PORTA_DATA &=~TRIGGER;
	counter =0;
	while((GPIO_PORTA_DATA & ECHO)==0);
	while(((GPIO_PORTA_DATA & ECHO)!=0) &(counter < MAX_TIME)) {
		counter++; 
		delay_Microsecond(1);
	} 
	distance = (float)counter*(float)0.01700000;
	return distance; 
} 
 
int main(void) 
    { 
   SYSCTL_RCGCGPIO |=(1U<<0);
   GPIO_PORTA_DIR =TRIGGER;
   GPIO_PORTA_DEN |=(ECHO)|(TRIGGER);
   
   bluetoothInit();

   while(1){
    float result = measureD();
    int length = snprintf(NULL, 0, "%f\r\n", result) + 1;
    char* str = (char *)malloc(length);
    snprintf(str, length, "%f\r\n", result);
    bluetoothSendString(str, length);
    free(str);
  }
}
 
void delay_Microsecond(uint32_t time)
{
    int i;
    SYSCTL_RCGCTIMER |= (1U<<1); 
    TIMER1_CTL =0;
    TIMER1_CFG = 0x04;
    TIMER1_TAMR = 0x02;
    TIMER1_TAILR = 16-1;
    TIMER1_ICR =0x1;
    TIMER1_CTL |= 0x01;
 
    for(i=0;i<time;i++){ 
        while((TIMER1_RIS & 0x1)==0);
        TIMER1_ICR = 0x1;
    }
 
}

