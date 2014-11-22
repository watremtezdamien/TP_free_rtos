/**@author : watremetz Damien 
*@brief : task freeRTOS pour utilisation périphérique I2C
*/
/*declaration include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "user.h"

#define UART0 LPC_UART0
#define IRQ_SELECTION 	UART0_IRQn
#define Uart_Menu 1
/*declaration fonction*/

static void vI2cTaskTransmit(void* pvParameters);

static void vUartTask( void* pvParameters);

void UART0_IRQHandler(void);

void  vUartSendTerminal(int xvaleur);

/*variable global*/
//unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];


	const char cMenu[]={	"/**************Menu UART**************/\n\r"
	"pour le driver de led envoyer une valeur entre 1 et 12\r\n"
	"valeur = \r\n"		};
	
/*definition des queue*/
xQueueHandle  xI2cQueue,xUartQueue;
xUartDataReceive_t xUartData;
xI2cDataTransmit_t xI2cData;

int main (void)
{

		xI2cQueue = xQueueCreate( 1, sizeof(xI2cDataTransmit_t));
		xUartQueue = xQueueCreate(1, sizeof(xUartDataReceive_t));
//		if( xI2cQueue != NULL)
//		{
			xTaskCreate(vUartTask,"UartTask",240,NULL,2,NULL);
			xTaskCreate(vI2cTaskTransmit, "I2cTaskTransmit", 240,NULL, 3, NULL);
			vTaskStartScheduler();
//		}
//	 else
//	 {
//		 /*message error*/
//	 }

	 for( ;; );
	
}
static void vI2cTaskTransmit(void* pvParameters)
{
	portBASE_TYPE xStatus;
	xI2cDataTransmit_t xI2cDataReadToTransmit;
	I2C_XFER_T xI2cDataTransmit ;
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);
	/*ensemble des fonction CMSIS ou LPCOPEN pour l'initialisation com I2C*/


	for ( ;; )
	{
			xStatus = xQueueReceive(xI2cQueue, &xI2cDataReadToTransmit, portMAX_DELAY); 
			if( xStatus != pdPASS)
			{
				xI2cDataTransmit.txBuff = xI2cDataReadToTransmit.Data;
			//	Chip_I2C_MasterTransfer(I2C0,&xI2cDataTransmit);
			}
		}
	vTaskDelay(configTICK_RATE_HZ);
			
	} 
	
	


static void vUartTask(void* pvParameters)
{
	/*intialisation uart0 pour debug */
	Chip_UART_Init(UART0);
	Chip_UART_SetBaud(UART0, 115200);
	Chip_UART_ConfigData(UART0, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(UART0);
	/*paramettre interruption */
	Chip_UART_IntEnable(UART0,(UART_IER_RLSINT|UART_IER_RBRINT));
	NVIC_SetPriority(IRQ_SELECTION, 5);
	NVIC_EnableIRQ(IRQ_SELECTION);
	for( ;; ){
		/*lecture bloquand de la file de message uart remplis via l'interruption */
	
		vUartSendTerminal(Uart_Menu);

		
	 vTaskDelay(configTICK_RATE_HZ/2);
	}
	
}
/**
*@brief : fonction d'envoie sur l'uart permet selon la valeur envoyé de selectionné quel envoie chosir
*@param : iValeur permet de selectionné rapidement quel valeur envoyé ici les valeurs sont choisi 
* 				grace a un define permettant de savoir quel valeur on souhaite envoyé sur l'uart.
*@return : ne retourne rien 
*/
void  vUartSendTerminal( int ivaleur)
{
	if(ivaleur == Uart_Menu){
	Chip_UART_SendBlocking(UART0,cMenu,sizeof(cMenu)-1);
	}
	
}
/*erreur de flag pas de remise a zero de flag a corrigé aprés manger */
void UART0_IRQHandler(void)
{
 Chip_UART_Read(UART0, &xI2cData.Data,8);
 
 xQueueSendFromISR(xI2cQueue,&xI2cData,0);
 NVIC_ClearPendingIRQ(UART0_IRQn);
}
