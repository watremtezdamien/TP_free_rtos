/**
@ author : Watremetz Damien 
@ detail : application PWM 
*/
  
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"
//#include "basic_io.h"

#include "ARMCM3.h"
/*declaration variable global*/

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

/* declaration fonction */
static void vPwmTaskReceiver(void* pvParameters);
 /*declarantion xqueuehandle*/
xQueueHandle xPwmValueQueue;
xPwmData_t xPwmData ={50,1000} ;
int main (void)
{
	xPwmValueQueue = xQueueCreate(2,sizeof(xPwmData_t)); // creation de la file de messsage pour la tache PWM
	if( xPwmValueQueue != NULL )
	{
	xTaskCreate(vPwmTaskReceiver,"PWM",240,NULL,1,NULL);
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
	unsigned char cCmpt=0;
	xPwmData_t xPwmDataReceive;
	for ( ;; )
	{
		
	}
}

