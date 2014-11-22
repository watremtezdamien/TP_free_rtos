/**
@ author : Watremetz Damien 
@ detail : application PWM 
*/
  
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"
//#include "basic_io.h"
#include <stdbool.h>
/*declaration variable global*/

//unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

/* declaration fonction */
static void vPwmTaskReceiver(void* pvParameters);
 /*declarantion xqueuehandle*/
xQueueHandle xPwmQueue;
xPwmData_t xPwmData ={50,1000} ;


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
@brief : definition de la tache PWM 
@details : cette tache generer un signal PWM via une tache simple la file de message devras comporté deux valeurs 
					 la frequence et le cycle c'est cette tache qui seras l'action de la sortie 
*/
static void vPwmTaskReceiver(void* pvParameters)
{
	bool cCmpt=false;
	xPwmData_t xPwmDataReceive;
	portBASE_TYPE xStatus;
	for ( ;; )
	{
		if (uxQueueMessagesWaiting(xPwmQueue) !=0)
		{
			xStatus = xQueueReceive(xPwmQueue,&xPwmDataReceive,0);
			if (xStatus == pdPASS)
			{
				if (cCmpt==false)
				{
				cCmpt=true;
				//GPIOSET;
				//vTaskDelayUntil(,); mettre dans les parenthése les valeur pour temps allumage 

				}
				else 
				{
  				cCmpt = false;
				// GPIOCLR;
				//	vTaskDelayUntil(,); mettre dans les parenthése les valeur tmps eteind 

					
				}
			}
		}
	}
}

