
#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"

void vTaskFunction( void *pvParameters );

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";

int main(void)
{
  xTaskCreate( vTaskFunction, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );

  xTaskCreate( vTaskFunction, "Task 2", 240, (void*)pcTextForTask2, 2, NULL );

  vTaskStartScheduler();	
	
  for(;;);
}

void vTaskFunction(void *pvParameters)
{
  char *pcTaskName;
  portTickType xLastWakeTime;

  pcTaskName = ( char * ) pvParameters;

  xLastWakeTime = xTaskGetTickCount();

  for(;;)	{
    vPrintString( pcTaskName );

    vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_RATE_MS ) );
  }
}
