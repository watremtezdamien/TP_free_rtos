
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define mainMAX_MSG_LEN	( 80 )

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

static void prvPrintTask( void *pvParameters );
static void pvPrintString(void *pvParameters);

static void prvNewPrintString( const portCHAR *pcString );

const char ctask1[2][mainMAX_MSG_LEN] ={ "Task 1 ******************************************\n",
																		     "Task 2 ------------------------------------------\n"};

xQueueHandle xQueue;


int main( void )
{

	xQueue =xQueueCreate(	1,sizeof(int));
	srand(567);


	if( xQueue != NULL )
	{

		xTaskCreate( prvPrintTask, "Print1", 240, (void*) 0, 1, NULL );
		xTaskCreate( prvPrintTask, "Print2", 240, (void*) 1, 2, NULL );
		xTaskCreate( pvPrintString, "gatekeper" , 240, NULL, 3, NULL);
		vTaskStartScheduler();
	}


	for( ;; );
}

/*fonction ecriture  d'une fonction */
static void prvNewPrintString( const portCHAR *pcString )
{ 
		
    static char cBuffer[ mainMAX_MSG_LEN ];
    sprintf( cBuffer, "%s", pcString );
    printf( cBuffer );



}
/*tache3 */
static void pvPrintString(void *pvParameters)
{
  unsigned int iBuff;
	portBASE_TYPE xStatus;
	for( ;; )
	{
	
	 xStatus = xQueueReceive(xQueue, &iBuff ,portMAX_DELAY);
		if(xStatus ==pdPASS)
		{
	   prvNewPrintString(ctask1[iBuff]);
		}
	 vTaskDelay(400);
	 
	}
}


static void prvPrintTask( void *pvParameters )
{
//  char *pcStringToPrint;

//	pcStringToPrint = ( char *) pvParameters;

	for( ;; )
	{

		xQueueSendToBack(xQueue, (unsigned int *)pvParameters,100);
		vTaskDelay( ( rand() & 0x1FF )/200 );
	}
}



