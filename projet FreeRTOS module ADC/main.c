/*Auteur : laplace yohann
Code pour l'adc*/

/*définitions des includes*/ 
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "USER.h"
/* definition variable */
#define UART0  LPC_UART0
#define ADC	 LPC_ADC
/*prototype de fonction*/
void ADC_IRQHandler(void);
void UART0_IRQHandler(void);
void adc_init(void);
void uart_init(void);

/*Constante pour la fil de message*/
xQueueHandle xAdcQueue,xUartQueue;
xAdcStruct_t valeur;
static ADC_CLOCK_SETUP_T ADCSetup;

/*Tache de l'ADC*/
static void vADCTask(void *pvParameters)
{
	portBASE_TYPE xStatus = pdPASS;
	xAdcStruct_t xAdcReceive;
	uint8_t uAdcReel;
	adc_init();

    while (1)
		{
        xStatus = xQueueReceive(xAdcQueue,&xAdcReceive,portMAX_DELAY);
				
        if(xStatus == pdPASS) 
					
				{
					uAdcReel = ((xAdcReceive.valueADC*5)/16);
					xQueueSend(xUartQueue,&uAdcReel,0);
        }
				vTaskDelay(100);
    }
}

static void vUartSend(void *pvParameters)
{
portBASE_TYPE xStatus;
xUartStruct_t xReceive;
	
	uart_init();
  Chip_UART_SendBlocking(UART0,"coucou",8);
	for( ;; )
	{
		
		xStatus = xQueueReceive( xUartQueue,&xReceive, portMAX_DELAY );//lecture bloquante.
		if( xStatus == pdPASS )
		{
			Chip_UART_SendByte(LPC_UART0,xReceive.valueUART);
		}
		vTaskDelay(150);
	}
}
/*programme principal*/
int main(void)
{
	  /*creation file de message*/
	  xAdcQueue = xQueueCreate(1, sizeof(xAdcStruct_t));
		xUartQueue = xQueueCreate(1,sizeof(xUartStruct_t));

    xTaskCreate(vADCTask,"vTaskAdc",100, NULL, 1,NULL);
		xTaskCreate(vUartSend,"vUartSend",100,NULL,2,NULL);

    // Démarrage du scheduler
    vTaskStartScheduler();
    
    while(1);
}

void ADC_IRQHandler(void)
{
	  uint8_t usAdcValue = 0;

	  NVIC_ClearPendingIRQ(ADC_IRQn);
	  Chip_ADC_ReadByte(LPC_ADC, (ADC_CH2), &usAdcValue);
    xQueueSendFromISR(xAdcQueue,&usAdcValue,0);
	
}
/**
*@brief : fonction initialisation de l'ADC
*@param : aucun parametre
*@retur : ne retourne pas.
*/
void adc_init(void)
{
	/*initialisation de l'ADC*/
	Chip_ADC_Init(ADC, &ADCSetup);
	Chip_ADC_EnableChannel(ADC, ADC_CH2, ENABLE);
	/*interruption ADC enable*/
	Chip_ADC_Int_SetChannelCmd(ADC,ADC_CH2,ENABLE);
	NVIC_SetPriority(ADC_IRQn,5);
	NVIC_EnableIRQ(ADC_IRQn);
	
}
/**
*@brief : fonction initialisation de l'UART
*@param : aucun parametre
*@retur : ne retourne pas.
*/
void uart_init(void)
{
	Chip_UART_Init(UART0);
	Chip_UART_SetBaud(UART0, 115200);
	Chip_UART_ConfigData(UART0, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_TXEnable(UART0);
}
