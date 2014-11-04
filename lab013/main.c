// norme misra
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "basic_io.h"

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );
typedef struct xTaskData xTaskData;
struct xTaskData
{
	int8_t uiTache;
	long lValeur;
};

xQueueHandle xQueue;


int main( void )
{
	xTaskData tTask1 = {1,100};
	xTaskData tTask2 = {2,200};
	
	 //xQueue pointeur de la file 
    xQueue = xQueueCreate( 5, sizeof( xTaskData ) );//! creation d'une file de 5 celulle avec pour longueur le type LONG

	if( xQueue != NULL )
	{
		// priorité inversé 
		xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) &tTask1 , 1, NULL );
		xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) &tTask2, 1, NULL );
		// priorité du lecteur permet de preampter les deux tache ecrivaint pour lire les valeurs une seul case sera lu par le recepteur
		// la file ne peut pas contenir plus d'un item au vu la priorité de la lecture 
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
@detail : les fonctions d'ecritures ne sont pas utiliser de façons cyclique mais en permannence elle libérer juste le cpu a la 
					a la fin de sont exécution.envoie de message différent entre les deux taches via pvparemeter
@param : pvParameters permet d'envoyer dans la file le message entre les différente tache sender 
*/
static void vSenderTask( void *pvParameters )
{
//long lValueToSend;
//xTaskData xSendData;
portBASE_TYPE xStatus;

	//xSendData.uiTache = ( xTaskData*) pvParameters; //  declaration de pv Parameters 

	for( ;; )
	{
		// pour permettre le fonctionnement avec un inverse de priorité il suffit de remplacer les temps blockant 
		// l'écriture devras avoir un temps blockant pour bloqué l'écriture quand la file sera pleine et ainsi permettre a la lecture
		// de libéré de la place pour écrire a nouveaux 
		xStatus = xQueueSendToBack( xQueue, ( xTaskData*) pvParameters, 0 );

		if( xStatus != pdPASS ) // message en cas d'erreur 
		{
			vPrintString( "Could not send to the queue.\r\n" );
		}

		taskYIELD(); // libere le CPU
	}
}
/**
@brief : fonction de réception 
@detail : la fonction de reception ne s'execute que lorsque qu'un message et recus dans la file 
					le timeout ne peut se déclenché que dans le cas ou la tache receive a l'initialisation s'éxécute 
					en premier 
*/
static void vReceiverTask( void *pvParameters )
{
xTaskData xReceiveData;
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			vPrintString( "Queue should have been empty!\r\n" );
		}

		xStatus = xQueueReceive( xQueue, &xReceiveData, xTicksToWait);//xTicksToWait ); remplacement de xTicksToWait par 0

		if( xStatus == pdPASS )
		{
			vPrintStringAndNumber( "Received number task = ", xReceiveData.uiTache );
			vPrintStringAndNumber( "Received value", xReceiveData.lValeur);
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


