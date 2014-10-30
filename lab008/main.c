
#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"
#define GPIO0_DIR            				(*(unsigned long*)0x2009C003)
#define GPIO0_SET            				(*(unsigned long*)0x2009C018)
#define GPIO0_CLR            				(*(unsigned long*)0x2009C01C)

void vTaskFunction( void *pvParameters );
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
@brief : vordonnaceur 
@detail : fonction permmetant de visualis� l'ordonnanceur sur les pin de sortie pour cela on recup�re les valeur de la tache �x�cute
					avec le switch of switchOn et ainsi on peut visualis� les passage d'une tache a une autre avec par exemple un oscilo 
*/
void vOrdonnaceur(void)
{
	
}
