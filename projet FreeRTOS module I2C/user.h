#ifndef USER_H
#define USER_H
#include <stdint.h>

/*structure communication I2C transmition data en Queue*/
typedef struct xI2cDataTransmit
{
	uint8_t cSlaveAddress;
	const uint8_t *Data;
	int iTxSize;

	
}xI2cDataTransmit_t;

/*structure communication UART reception et reception UART */

typedef struct xUartDataReceive
{
	
	uint8_t uiI2cTransmit; 
}xUartDataReceive_t;




#endif /*USER_H*/
