/**
*@author : Watremetz Damien 
*@description : projet FreeRTOS regroupement de l'ensemble des api est liaison entre elle 
*/
/*définitions des includes*/ 
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "USER.h"
#include "pwm.h"
/* definition variable */
#define UART0  					 LPC_UART0
#define ADC	 						 LPC_ADC
#define UART0 					 LPC_UART0
#define IRQ_SELECTION 	 UART0_IRQn
#define Uart_Menu 			 98

#define Timer0      		 LPC_TIMER2
#define IRQ_Timer0  		 TIMER2_IRQn
#define StateLow    		 0
#define StateHigh   		 1
#define MatChannel0      2
#define MatToggle				 3
#define DefaultDutyCycle 50
#define ChannelPwm			 1
#define erreurMsg				 99
/*prototype de fonction*/
void ADC_IRQHandler(void);
void UART0_IRQHandler(void);
void adc_init(void);
void uart_init(void);
static void vADCTask(void *pvParameters);
static void vI2cTaskTransmit(void* pvParameters);
static void vPwmTaskReceiver(void* pvParameters);

static void vUartTask( void* pvParameters);

void UART0_IRQHandler(void);

void  vUartSendTerminal(int lValeurtransmit);
/*variable global*/
	const char cMenu[]={	"/**************Menu UART**************/\n\r"
	"pour le driver de led envoyer une valeur entre 1 et 12\r\n"
	"Appuyer sur U(up) ou D(down) pour changer la valeur de pwm\n\r"
	"valeur = \r\n"		};

	/*definition des queue*/
xQueueHandle  xI2cQueue,xUartQueue;
xQueueHandle xAdcQueue;
xQueueHandle xPwmQueue;
/*variable global pour file de message*/
xPwmData_t xPwmData ;
xUartDataReceive_t xUartData;
xI2cDataTransmit_t xI2cData;

xAdcStruct_t valeur;
static ADC_CLOCK_SETUP_T ADCSetup;

int main (void)
{

		xI2cQueue = xQueueCreate( 1, sizeof(xI2cDataTransmit_t));
		xUartQueue = xQueueCreate(1, sizeof(xUartDataReceive_t));
		xAdcQueue = xQueueCreate(1, sizeof(xAdcStruct_t));
		xPwmQueue = xQueueCreate(2,sizeof(xPwmData_t)); // creation de la file de messsage pour la tache PWM

		/*creation des taches*/
    xTaskCreate(vADCTask,"vTaskAdc",100, NULL, 1,NULL);
		xTaskCreate(vUartTask,"UartTask",240,NULL,2,NULL);
  	xTaskCreate(vI2cTaskTransmit, "I2cTaskTransmit", 240,NULL, 3, NULL);
		xTaskCreate(vPwmTaskReceiver,"PWM",240,NULL,1,NULL);
  	vTaskStartScheduler();
	 for( ;; );
	
}
/**
*@brief : definition de la tache PWM 
*@details : géneration d'une pwm via timer car le lpc1768 ne dispose pas de PWM 
* 					création d'une tache de pwm software.
*						Cette tache éxécute une pwm qui gérer de facons autonome tant que l'utilisateur
*						ne change pas la valeur de la PWMqueue la generation de la PWM est autonome via 
*						les sortie de MATCH qui permette de toggle lorsque la valeur est atteinte
*@return : ne retourne pas 
*/
static void vPwmTaskReceiver(void* pvParameters)
{
	uint32_t timer;
	uint32_t dutyCycle=50;
	xPwmData_t xPwmDataReceive;
	xUartTransmit_t xUartDataToTransmit;
	portBASE_TYPE xStatus;
	/*initialisation pin mat0 pour generation signal pwm */
	
	/*initialisation timer */
	Chip_TIMER_Init(Timer0);
	Chip_TIMER_Reset(Timer0);
	timer = Chip_Clock_GetSystemClockRate();
	
	/*initialisation des interruption pour generer la PWM*/
	Chip_TIMER_MatchEnableInt(Timer0, MatChannel0);
	
	/*initialisation pin MAt0 en sortie PWM sur deux declenchement*/
	Chip_TIMER_SetMatch(Timer0, MatChannel0, timer/12);// valeur high
	Chip_TIMER_ExtMatchControlSet(Timer0,StateLow,MatToggle,1);

	Chip_TIMER_ResetOnMatchEnable(Timer0, MatChannel0);
	Chip_TIMER_Enable(Timer0);
	
	/*activation interruption*/
	NVIC_ClearPendingIRQ(IRQ_Timer0);
	NVIC_SetPriority(IRQ_Timer0, 5);
	NVIC_EnableIRQ(IRQ_Timer0);
	for ( ;; )
	{
	
			xStatus = xQueueReceive(xPwmQueue,&xPwmDataReceive,portMAX_DELAY);
			if (xStatus == pdPASS)
			{
				if(dutyCycle<=100)
				{
					if(xPwmDataReceive.lValeurDutyCycle== 'u')
					{
						dutyCycle=dutyCycle+10;
					}
					else 
					{
						dutyCycle=dutyCycle-10;	
					}
				}
			
			else
			{
					xUartDataToTransmit.DataTransmit=erreurMsg;
					xQueueSend(xUartQueue,&xUartDataToTransmit,0);
			}
		
		}
	vTaskDelay(10);
  }	
}



void TIMER2_IRQHandler(void)
{
	Chip_TIMER_ClearMatch(Timer0,MatChannel0);//remise a zero de l'interruption match du timer
	NVIC_ClearPendingIRQ(IRQ_Timer0);
}



/**
*@brief : Tache ADC 
*@detail : initialisation de l'ADC Tache de l'adc permettant des calculs.
*/
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
				vTaskDelay(10);
    }
}

/**
*@brief : interruption ADC 
*@detail: envoie de la valeur de l'ADC sur la tache ADC via l'ecriture dans la file de message 
*/
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
*@brief : tache I2C jouant le role de GateKeeper permet un accé au periphérique qui lui sont resservé 
*@return : ne retourne pas.
*/
static void vI2cTaskTransmit(void* pvParameters)
{
	portBASE_TYPE xStatus;
	xI2cDataTransmit_t xI2cDataReadToTransmit;
	I2C_XFER_T xI2cDataTransmit ;
	xI2cDataTransmit.slaveAddr=0x22;
		/*ensemble des fonction LPCOPEN pour l'initialisation com I2C*/
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, 100000);


	for ( ;; )
	{
			xStatus = xQueueReceive(xI2cQueue, &xI2cDataReadToTransmit, portMAX_DELAY); 
			if( xStatus == pdPASS)
			{
				if((*xI2cDataReadToTransmit.Data>='0')|(*xI2cDataReadToTransmit.Data<='9')){
				xI2cDataTransmit.txBuff = (uint8_t*) xI2cDataReadToTransmit.Data;
				Chip_I2C_MasterTransfer(I2C0,&xI2cDataTransmit);
				}
			}
		vTaskDelay(10);
		}
	
			
	} 
	
	
/**
  *@brief : tache UART 
	*@detail : joue le role de task keeper pour l'envoie sur l'uart.	
	*@return : ne retourne pas.
*/

static void vUartTask(void* pvParameters)
{
	portBASE_TYPE xStatus;
	xUartTransmit_t xUartToTransmit;
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
		xStatus= xQueueReceive(xUartQueue,&xUartToTransmit,portMAX_DELAY);
		if(xStatus == pdPASS)
		{
			vUartSendTerminal(xUartToTransmit.DataTransmit);
		}
		
	 vTaskDelay(configTICK_RATE_HZ/2);
	}
	
}
/**
*@brief : fonction d'envoie sur l'uart permet selon la valeur envoyé de selectionné quel envoie chosir
*@param : iValeur permet de selectionné rapidement quel valeur envoyé ici les valeurs sont choisi 
* 				grace a un define permettant de savoir quel valeur on souhaite envoyé sur l'uart.
*@return : ne retourne rien.
*/
void  vUartSendTerminal( int lValeurtransmit)
{
	Chip_UART_SendBlocking(UART0,cMenu,sizeof(cMenu)-1);	
	if( lValeurtransmit == erreurMsg)
	{
		Chip_UART_SendBlocking(UART0,"erreur valeur max ou min pwm\n\r",8);
		
	}
	else{
		
	if(lValeurtransmit == 98){
		Chip_UART_SendBlocking(UART0,cMenu,sizeof(cMenu)-1);	
	}
	else 
	{
		
		Chip_UART_SendBlocking(UART0,(int*)lValeurtransmit,sizeof(lValeurtransmit));
	}
}
}
/**
*@brief : interruption UART
*@detail : fonction d'interruption de l'UART permet de récupérer ecris sur une file de message 
*					permettant d'agir sur une tache.
*@return :  ne retourne pas.
*/
void UART0_IRQHandler(void)
{
 int iReceive;
 char cMod;
 Chip_UART_Read(UART0, &iReceive,8);
 NVIC_ClearPendingIRQ(UART0_IRQn); // remise a zero interruption uart 
 cMod = (char) iReceive;
 if(cMod =='u' |cMod =='d')
 {
	 
	 xQueueSendFromISR(xPwmQueue,&cMod,0);
 }
 else
 {
 xQueueSendFromISR(xI2cQueue,&iReceive,0); //envoie sur la queue de i2c
 }
}
