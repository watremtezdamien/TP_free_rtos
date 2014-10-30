
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#define mainMAX_MSG_LEN	( 80 )

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

static void prvPrintTask( void *pvParameters );


static void prvNewPrintString( const portCHAR *pcString );


xSemaphoreHandle xMutex;


int main( void )
{

    xMutex = xSemaphoreCreateMutex();

	srand( 567 );


	if( xMutex != NULL )
	{

		xTaskCreate( prvPrintTask, "Print1", 240, "Task 1 ******************************************\n", 1, NULL );
		xTaskCreate( prvPrintTask, "Print2", 240, "Task 2 ------------------------------------------\n", 2, NULL );

		vTaskStartScheduler();
	}


	for( ;; );
}


static void prvNewPrintString( const portCHAR *pcString )
{
    static char cBuffer[ mainMAX_MSG_LEN ];

    sprintf( cBuffer, "%s", pcString );
    printf( cBuffer );

}

static void prvPrintTask( void *pvParameters )
{
char *pcStringToPrint;

	pcStringToPrint = ( char * ) pvParameters;

	for( ;; )
	{

		prvNewPrintString( pcStringToPrint );

		vTaskDelay( ( rand() & 0x1FF )/200 );
	}
}



