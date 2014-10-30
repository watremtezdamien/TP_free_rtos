
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
    xQueue = xQueueCreate( 5, sizeof( long ) );

	if( xQueue != NULL )
	{
		xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) 100, 1, NULL );
		xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) 200, 1, NULL );

		xTaskCreate( vReceiverTask, "Receiver", 240, NULL, 2, NULL );

		vTaskStartScheduler();
	}
	else
	{

	}
		
	for( ;; );
}

static void vSenderTask( void *pvParameters )
{
long lValueToSend;
portBASE_TYPE xStatus;

	lValueToSend = ( long ) pvParameters;

	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

		if( xStatus != pdPASS )
		{
			vPrintString( "Could not send to the queue.\r\n" );
		}

		taskYIELD();
	}
}

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


