/*auteur : watremetz damien */
#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"
#define GPIO0_DIR            				(*(unsigned long*)0x2009C000)
#define GPIO0_SET            				(*(unsigned long*)0x2009C018)
#define GPIO0_CLR            				(*(unsigned long*)0x2009C01C)

void vTaskFunction( void *pvParameters );
void vOrdonnanceurIn(int iPin);
void vOrdonnanceurOut(int iPin);

unsigned int itask;
unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];
unsigned long ultimeIdle;
const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";

int main(void)
{
  xTaskCreate( vTaskFunction, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );

  xTaskCreate( vTaskFunction, "Task 2", 240, (void*)pcTextForTask2, 2, NULL );

  vTaskStartScheduler();	
	
  for(;;);
}
/**
@brief : vTaskFunction 
@detail : cr�ation d'une tache simple et permet ,ici de facilement cr�� plusieur tache avec la m�me la fonction

*/
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
/**
@brief : vApplicationIdleHook 
@detail : cette fonction est un hook activ� via le fichier de config de freertos il permet ainsi par exempl de compter le nombre de fois 
ou la tache idle est activ� permet ainsi de mettre a jours les trace disponible par freertos
*/
void vApplicationIdleHook(void)
{
	ultimeIdle++;
}
/**
@brief : vordonnaceurIn 
@detail :  fonction permettant de voir le changement de 0 a 1 permettant de repr�sent� le chronogramme de lordonnanceur
					en possition SWITCHIN. visualitation sur GPIO
@param : iPin permet de selectionn� la pin du GPIO a activ�
*/
void vOrdonnanceurIn(int iPin)
{
	GPIO0_DIR=iPin;
	GPIO0_SET =0x1;
}
/**
@brief : d�riv� de la fonction vOrdonnanceurIn 
@detail : fonction permettant de voir le changement de 1 a 0 permettant de repr�sent� le chronogramme de lordonnanceur
					en possition SWITCHOUT. visualisation sur GPIO
@param : iPin permet de selectionn� la pin du GPIO a activ� 
*/
void vOrdonnanceurOut(int iPin)
{
	GPIO0_DIR=iPin;
	GPIO0_CLR= 0x1;
}
