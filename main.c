#include <tm4c123gh6pm.h>
#include <stdint.h>
//#include <stdio.h>
#include <stdlib.h>
#include "bluetooth.h"
#define MAX_TIME 7500
#define GPIO_PORTA_DATA_R_MINE (*((volatile unsigned long *)0x400043FC))
float measureD(void);
uint32_t counter =0;
float distance=0;
#define ECHO (1U<<2) //PA2(INput)
#define TRIGGER (1U<<4) //PA4(OUTPUT)
#define BLUE_LED (1U<<2)//PF3 onboard Blue LED 

#define RCC2 (*((volatile unsigned long *)0x400FE070))
#define RCC (*((volatile unsigned long *)0x400FE060))

void delay_Microsecond(uint32_t time); 
float measureD(void){ 
    GPIO_PORTA_DATA_R_MINE &=~TRIGGER;
    delay_Microsecond(10);
    GPIO_PORTA_DATA_R_MINE |= TRIGGER;
    delay_Microsecond(10);
    GPIO_PORTA_DATA_R_MINE &=~TRIGGER;
    counter =0;
    while((GPIO_PORTA_DATA_R_MINE &ECHO)==0)    {
      //printf("hello\n");
    }
    while(((GPIO_PORTA_DATA_R_MINE &ECHO )!=0) &(counter < MAX_TIME)) 
 {
  counter++; 
  delay_Microsecond(1);
  } 
 distance = (float)counter*(float)0.1600000;
 return distance; } 
int main(void) 
    { 
   SYSCTL_RCGCGPIO_R |=(1U<<0); //Enable clock for PORTA 
   SYSCTL_RCGCGPIO_R |=(1U<<5); //Enable clock for PORTF 
   GPIO_PORTA_DIR_R =TRIGGER;
   GPIO_PORTF_DIR_R =BLUE_LED;
   GPIO_PORTA_DEN_R |=(ECHO)|(TRIGGER);
   GPIO_PORTF_DEN_R |= BLUE_LED;
   
   bluetoothInit();

   
   while(1){
    float result = measureD();
    //printf("%f\n", result);
    //printf("%f\n", result);
    
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
    SYSCTL_RCGCTIMER_R |=(1U<<1); 
        TIMER1_CTL_R=0;
    TIMER1_CFG_R=0x04;
    TIMER1_TAMR_R=0x02;
    TIMER1_TAILR_R= 16-1;
    TIMER1_ICR_R =0x1;
    TIMER1_CTL_R |=0x01;
 
    for(i=0;i<time;i++){ 
        while((TIMER1_RIS_R & 0x1)==0);
        TIMER1_ICR_R = 0x1;
    }
 
}

