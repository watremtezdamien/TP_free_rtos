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

static void vIntegerGenerator( void *pvParameters );
static void vStringPrinter( void *pvParameters );

static void prvSetupSoftwareInterrupt( void );

void vSoftwareInterruptHandler( void );


unsigned long ulNext = 0;
unsigned long ulCount;
unsigned long ul[ 100 ];

xQueueHandle xIntegerQueue, xStringQueue;

int main( void )
{

  xIntegerQueue = xQueueCreate( 10, sizeof( unsigned long ) );
  xStringQueue = xQueueCreate( 10, sizeof( char * ) );

  prvSetupSoftwareInterrupt();

  xTaskCreate( vIntegerGenerator, "IntGen", 240, NULL, 1, NULL );

  xTaskCreate( vStringPrinter, "String", 240, NULL, 2, NULL );

  vTaskStartScheduler();

  for( ;; );
}

static void vIntegerGenerator( void *pvParameters )
{
  portTickType xLastExecutionTime;
  unsigned portLONG ulValueToSend = 0;
  int i;

  xLastExecutionTime = xTaskGetTickCount();

  for( ;; )
  {
    vTaskDelayUntil( &xLastExecutionTime, 200 / portTICK_RATE_MS );

    for( i = 0; i < 5; i++ )
    {
      xQueueSendToBack( xIntegerQueue, &ulValueToSend, 0 );
      ulValueToSend++;
    }

    vPrintString( "Generator task - About to generate an interrupt.\n" );
    mainTRIGGER_INTERRUPT();
    vPrintString( "Generator task - Interrupt generated.\n\n" );
  }
}

static void vStringPrinter( void *pvParameters )
{
  char *pcString;

  for( ;; )
  {
    xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );

    vPrintString( pcString );
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
  static unsigned long ulReceivedNumber;

  static const char *pcStrings[] =
    {
      "String 0\n",
      "String 1\n",
      "String 2\n",
      "String 3\n"
    };

  while( /* ... */ != errQUEUE_EMPTY )
  {
    ulReceivedNumber &= 0x03;
    /* ... */
  }

  mainCLEAR_INTERRUPT();

  /* ... */
}








