
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "basic_io.h"

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );


xQueueHandle xQueue;


int main( void )
{
	 //xQueue pointeur de la file 
    xQueue = xQueueCreate( 5, sizeof( long ) );//! creation d'une file de 5 celulle avec pour longueur le type LONG

	if( xQueue != NULL )
	{
		xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) 100, 1, NULL );
		xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) 200, 1, NULL );
		// priorit� du lecteur permet de preampter les deux tache ecrivaint pour lire les valeurs une seul case sera lu par le recepteur
		// la file ne peut pas contenir plus d'un item au vu la priorit� de la lecture 
		xTaskCreate( vReceiverTask, "Receiver", 240, NULL, 2, NULL );

		vTaskStartScheduler();
	}
	else
	{

	}
		
	for( ;; );
}
/**
@brief : tache d'envoie 
@detail : les fonctions d'ecritures ne sont pas utiliser de fa�ons cyclique mais en permannence elle lib�rer juste le cpu a la 
					a la fin de sont ex�cution.envoie de message diff�rent entre les deux taches via pvparemeter
@param : pvParameters permet d'envoyer dans la file le message entre les diff�rente tache sender 
*/
static void vSenderTask( void *pvParameters )
{
long lValueToSend;
portBASE_TYPE xStatus;

	lValueToSend = ( long ) pvParameters;

	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

		if( xStatus != pdPASS ) // message en cas d'erreur 
		{
			vPrintString( "Could not send to the queue.\r\n" );
		}

		taskYIELD(); // libere le CPU
	}
}
/**
@brief : fonction de r�ception 
@detail : la fonction de reception ne s'execute que lorsque qu'un message et recus dans la file 
					le timeout ne peut se d�clench� que dans le cas ou la tache receive a l'initialisation s'�x�cute 
					en premier 
*/
static void vReceiverTask( void *pvParameters )
{
long lReceivedValue;
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			vPrintString( "Queue should have been empty!\r\n" );
		}

		xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

		if( xStatus == pdPASS )
		{
			vPrintStringAndNumber( "Received = ", lReceivedValue );
		}
		else
		{
			vPrintString( "Could not receive from the queue.\r\n" );
		}
	}
}

void vApplicationMallocFailedHook( void )
{
	for( ;; );
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	for( ;; );
}

void vApplicationIdleHook( void )
{

}


void vApplicationTickHook( void )
{

}


