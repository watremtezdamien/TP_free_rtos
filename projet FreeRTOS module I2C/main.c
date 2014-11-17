/**@author : watremetz Damien 
@brief : task freeRTOS pour utilisation périphérique I2C
*/
/*declaration include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"
/*declaration fonction*/


/*variable global*/
unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];
xQueueHandle  xI2cQueue;

int main (void)
{
		xQueueCreate(1,sizeof(xI2cDatatransmit)
	
	
		for( ;; );
	
}

