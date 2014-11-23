/**
*@author : Watremetz Damien 
*@detail : application PWM 
*/
  
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"
//#include <stdbool.h>

#define Timer0      LPC_TIMER0
#define Timer1      LPC_TIMER1
#define IRQ_Timer0  TIMER0_IRQn
#define StateLow    0
#define StateHigh   1
#define MatChannel0 0
#define MatToggle		3
/*declaration variable global*/

//unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

/* declaration fonction */
static void vPwmTaskReceiver(void* pvParameters);
 /*declarantion xqueuehandle*/
xQueueHandle xPwmQueue;
xPwmData_t xPwmData  ;


int main (void)
{
	xPwmQueue = xQueueCreate(2,sizeof(xPwmData_t)); // creation de la file de messsage pour la tache PWM
	if( xPwmQueue != NULL )
	{
	xTaskCreate(vPwmTaskReceiver,"PWM",240,NULL,1,NULL);
	vTaskStartScheduler();
	}
		for( ;; );
}



/**
*@brief : definition de la tache PWM 
*@details : géneration d'une pwm via timer car le lpc1768 ne dispose pas de PWM 
* 					création d'une tache de pwm software.
*						Cette tache éxécute une pwm qui gérer de facons autonome tant que l'utilisateur
*						ne change pas la valeur de la PWMqueue la generation de la PWM est autonome via 
*						les sortie de MATCH qui permette de toggle lorsque la valeur est atteinte
*@return : ne retourne pas 
*/
static void vPwmTaskReceiver(void* pvParameters)
{
	uint32_t timer;
	xPwmData_t xPwmDataReceive;
	portBASE_TYPE xStatus;
	/*initialisation timer */
	Chip_TIMER_Init(Timer0);
	Chip_TIMER_Reset(Timer0);
	timer = Chip_Clock_GetSystemClockRate();
	/*initialisation des interruption pour generer la PWM*/
	Chip_TIMER_MatchEnableInt(Timer0, 0);
	/*initialisation pin MAt0 en sortie PWM sur deux declenchement*/
	Chip_TIMER_ExtMatchControlSet(Timer0,StateLow,MatToggle,MatChannel0);
	Chip_TIMER_SetMatch(Timer0, MatChannel0, (timer / 48));
	Chip_TIMER_SetMatch(Timer0, MatChannel0, (timer / 24));
	Chip_TIMER_ResetOnMatchEnable(Timer0, 1);
	Chip_TIMER_Enable(Timer0);
	for ( ;; )
	{
		if (uxQueueMessagesWaiting(xPwmQueue) !=0)
		{
			xStatus = xQueueReceive(xPwmQueue,&xPwmDataReceive,0);
			if (xStatus == pdPASS)
			{
				


					
				
			}
		}
	}
}
void timer1_irqhandler (void)
{
	
	NVIC_ClearPendingIRQ(IRQ_Timer1);
}
void TIMER0_IRQHandler(void)
{

	NVIC_ClearPendingIRQ(IRQ_Timer0);
}