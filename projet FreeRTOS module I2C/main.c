/**@author : watremetz Damien 
@brief : task freeRTOS pour utilisation périphérique I2C
*/
/*declaration include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"

#define UART0 LPC_UART0

/*declaration fonction*/
static void vI2cTaskTransmit(void* pvParameters);
static void vUartTask( void* pvParameters);
void  vUartSend(void);
/*variable global*/
//unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];
xQueueHandle  xI2cQueue,xUartQueue;
xUartDataReceive_t xUartData;
xI2cDataTransmit_t xI2cData;
int main (void)
{
		xI2cQueue = xQueueCreate( 1, sizeof(xI2cDataTransmit_t));
		xUartQueue = xQueueCreate(1, sizeof(xUartDataReceive_t));
		if( xI2cQueue != NULL)
		{
			xTaskCreate(vUartTask,"UartTask",240,NULL,2,NULL);
			xTaskCreate(vI2cTaskTransmit, "I2cTaskTransmit", 240,NULL, 1, NULL);
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
			xStatus = xQueueReceive(xI2cQueue, &xI2cDataReadToTransmit, 0); 
			if( xStatus != pdPASS)
			{
				xI2cDataTransmit.txBuff = xI2cDataReadToTransmit.Data;
			//	Chip_I2C_MasterTransfer(I2C0,&xI2cDataTransmit);
			}
		}
	taskYIELD();
			
	} 
	
	


static void vUartTask(void* pvParameters)
{
	/*intialisation uart0 pour debug */
	Chip_UART_Init(UART0);
	Chip_UART_SetBaud(UART0, 9600);
	Chip_UART_TXEnable(UART0);
	Chip_UART_IntEnable(UART0,(UART_IER_RLSINT|UART_IER_RBRINT));
	for( ;; ){
		/*lecture bloquand de la file de message uart remplis via l'interruption */
		vUartSend();
		
	}
	
}

void  vUartSend(void)
{
	const char *cDebugMessage[]={
	"Menu UART\n\r",
	"pour le driver de led envoyer une valeur entre 1 et 12\r\n",
	"valeur = "		
	};
	for(int i=0; 3;i++)
	{
	Chip_UART_SendBlocking(UART0,cDebugMessage[i],50);
	}
}
void UART0_IRQHandler(void)
{int ireceive;
 Chip_UART_Read(UART0,&xI2cData.Data,8);

 xQueueSendFromISR(xI2cQueue,xI2cData.Data,0);
}
