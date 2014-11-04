#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "basic_io.h"

#include "ARMCM3.h"

#define mainSW_INTERRUPT_ID		( ( IRQn_Type ) 0 )

#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ( mainSW_INTERRUPT_ID )

#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ( mainSW_INTERRUPT_ID )

#define mainSOFTWARE_INTERRUPT_PRIORITY 		( 5 )

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

static void prvSetupSoftwareInterrupt( void );

void vSoftwareInterruptHandler( void );

//xSemaphoreHandle xBinarySemaphore;
xSemaphoreHandle xSemaphoreCounting;
int main( void )
{
	
   // vSemaphoreCreateBinary( xBinarySemaphore );//creation d'un semaphore binaire
		xSemaphoreCounting = xSemaphoreCreateCounting( 10, 0 );
    if( xSemaphoreCounting != NULL )
    {
    	prvSetupSoftwareInterrupt();

        xTaskCreate( vHandlerTask, "Handler", 240, NULL, 3, NULL );


        xTaskCreate( vPeriodicTask, "Periodic", 240, NULL, 1, NULL );

        vTaskStartScheduler();
    }

	for( ;; );
}

static void vHandlerTask( void *pvParameters )
{
    xSemaphoreTake( xSemaphoreCounting, 0 );

    for( ;; )
    {
		 if( xSemaphoreTake( xSemaphoreCounting, portMAX_DELAY ) == pdTRUE )
        {
        vPrintString( "Handler task - Processing event.\n" );
			  mainTRIGGER_INTERRUPT();
        mainTRIGGER_INTERRUPT();
        mainTRIGGER_INTERRUPT();
				}
			
    }
		
}

static void vPeriodicTask( void *pvParameters )
{
    for( ;; )
    {
        vTaskDelay( 500 / portTICK_RATE_MS );

        vPrintString( "Periodic task - About to generate an interrupt.\n" );
        mainTRIGGER_INTERRUPT();
        vPrintString( "Periodic task - Interrupt generated.\n\n" );
    }
}

static void prvSetupSoftwareInterrupt( void )
{
	NVIC_SetPriority( mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY );

	NVIC_EnableIRQ( mainSW_INTERRUPT_ID );
}

void vSoftwareInterruptHandler( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR( xSemaphoreCounting, &xHigherPriorityTaskWoken );

    mainCLEAR_INTERRUPT();

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
