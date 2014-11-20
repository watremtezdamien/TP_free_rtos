/**@author : watremetz Damien 
@brief : task freeRTOS pour utilisation périphérique I2C
*/
/*declaration include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"

/*declaration fonction*/
static void vI2cTaskTransmit(void* pvParameters);
static void vUartDabugTask( void* pvParameters);
/*variable global*/
//unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];
xQueueHandle  xI2cQueue,xUartQueue;
xUartDataReceive_t xUartData
xI2cDataTransmit_t xI2cData = {0x22,0x22};
int main (void)
{
		xI2cQueue = xQueueCreate( 1, sizeof(xI2cDataTransmit_t));
		if( xI2cQueue != NULL)
		{
			xTaskCreate(vI2cTaskTransmit, "I2cTaskTransmit", 240, &xI2cQueue, 1, NULL);
			vTaskStartScheduler();
		}
	 else
	 {
		 /*message error*/
	 }

	 for( ;; );
	
}
static void vI2cTaskTransmit(void* pvParameters)
{
	portBASE_TYPE xStatus;
	xI2cDataTransmit_t xI2cDataReadToTransmit;
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);
	/*ensemble des fonction CMSIS ou LPCOPEN pour l'initialisation com I2C*/
	I2C_XFER_T xI2cDataTransmit ;

	for ( ;; )
	{
		if(uxQueueMessagesWaiting != NULL)
		{
			xStatus = xQueueReceive(xI2cQueue, &xI2cDataReadToTransmit, 0); 
			if( xStatus != pdPASS)
			{
				xI2cDataTransmit.txBuff = xI2cDataReadToTransmit.Data;
				Chip_I2C_MasterTransfer(I2C0,&xI2cDataTransmit);
			}
		}
	} 
	
	
}

static void vUartDabugTask(void* pvParameters)
{
	
}