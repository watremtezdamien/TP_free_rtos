/*auteur : watremetz damien */
#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"
#define GPIO0_DIR            				(*(unsigned long*)0x2009C000)
#define GPIO0_SET            				(*(unsigned long*)0x2009C018)
#define GPIO0_CLR            				(*(unsigned long*)0x2009C01C)
#define mainDELAY_LOOP_COUNT		(0xfffff)

void vTaskFunction( void *pvParameters );
void vTaskFunction2( void *pvParameters );
void vTaskFunction3( void *pvParameters );

void vOrdonnanceurIn(int iPin);
void vOrdonnanceurOut(int iPin);

unsigned int itask;
unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];
unsigned long ultimeIdle;
const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";
const char *pcTextForTask3 = "Task 3 is running\n";


int main(void)
{
  xTaskCreate( vTaskFunction, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );

  xTaskCreate( vTaskFunction2, "Task 2", 240, (void*)pcTextForTask2, 2, NULL );
	xTaskCreate( vTaskFunction3, "Task 2", 240, (void*)pcTextForTask3, 2, NULL );
  vTaskStartScheduler();	
	
  for(;;);
}
/**
@brief : vTaskFunction 
@detail : création d'une tache simple et permet ,ici de facilement créé plusieur tache avec la même la fonction

*/
void vTaskFunction(void *pvParameters)
{
	volatile unsigned long ul;
  char *pcTaskName;
  portTickType xLastWakeTime;

  pcTaskName = ( char * ) pvParameters;

  xLastWakeTime = xTaskGetTickCount();

  for(;;)	{
    vPrintString( pcTaskName );

    for(ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {

    }
  }
}
void vTaskFunction2(void *pvParameters)
{
	volatile unsigned long ul;
  char *pcTaskName;
  portTickType xLastWakeTime;

  pcTaskName = ( char * ) pvParameters;

  xLastWakeTime = xTaskGetTickCount();

  for(;;)	{
    vPrintString( pcTaskName );

    for(ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {

    }
  }
}
void vTaskFunction3(void *pvParameters)
{
	volatile unsigned long ul;
  char *pcTaskName;
  portTickType xLastWakeTime;

  pcTaskName = ( char * ) pvParameters;

  xLastWakeTime = xTaskGetTickCount();

  for(;;)	{
    vPrintString( pcTaskName );

    for(ul = 0; ul < mainDELAY_LOOP_COUNT; ul++) {

    }
  }
}
/**
@brief : vApplicationIdleHook 
@detail : cette fonction est un hook activé via le fichier de config de freertos il permet ainsi par exempl de compter le nombre de fois 
ou la tache idle est activé permet ainsi de mettre a jours les trace disponible par freertos
*/
void vApplicationIdleHook(void)
{
	ultimeIdle++;
}
/**
@brief : vordonnaceurIn 
@detail :  fonction permettant de voir le changement de 0 a 1 permettant de représenté le chronogramme de lordonnanceur
					en possition SWITCHIN. visualitation sur GPIO
@param : iPin permet de selectionné la pin du GPIO a activé
*/
void vOrdonnanceurIn(int iPin)
{
	GPIO0_DIR=iPin; // ne pas forcément utilisé dans cette fonction le definir dans l'init au départ gain de temps de cycle 
	GPIO0_SET =0x1;
}
/**
@brief : dérivé de la fonction vOrdonnanceurIn 
@detail : fonction permettant de voir le changement de 1 a 0 permettant de représenté le chronogramme de lordonnanceur
					en possition SWITCHOUT. visualisation sur GPIO
@param : iPin permet de selectionné la pin du GPIO a activé 
*/
void vOrdonnanceurOut(int iPin)
{
	GPIO0_DIR=iPin;
	GPIO0_CLR= 0x1;
}
