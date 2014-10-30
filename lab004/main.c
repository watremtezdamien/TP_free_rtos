#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"

#define mainDELAY_LOOP_COUNT		(0xfffff)

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

int main(void)
{
	const char *tache = "tache 1\n";
	const char *tache2 = "tache 2\n";
  xTaskCreate(vTask1, "Task 1", 200,(void*)tache , 1, NULL);

  xTaskCreate(vTask1, "Task 2", 200,(void*)tache2, 2 , NULL); // changer la prioriter attention a configmax_prioritie

  vTaskStartScheduler();

  for(;;);
}

void vTask1(void *pvParameters)
{

   const char *pcTaskName = (const char*) pvParameters;//"Task 1 is running\n";
  volatile unsigned long ul;

  for(;;) {
    vPrintString(pcTaskName);
		vTaskDelay(50 /portTICK_PERIOD_MS);
//    for(ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {

//    }
  }
}


