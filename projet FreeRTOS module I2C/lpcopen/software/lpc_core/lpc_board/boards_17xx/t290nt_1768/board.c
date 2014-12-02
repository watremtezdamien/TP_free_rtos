/*
 * @brief T290-NT board file
 *
 */

#include "board.h"
#include "string.h"

#include "retarget.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static const uint8_t ledPorts[] = { 1,  1,  1};
static const uint8_t ledBits[]  = {18, 19, 20};

const uint8_t buttonPorts[]  = { 1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  0,  0,  2,  2};
const uint8_t buttonBits[]   = {21, 22, 23, 24, 25, 26, 27, 28,  2,  0, 18, 16,  4,  6};

static ADC_CLOCK_SETUP_T ADCSetup;

static SSP_ConfigFormat ssp_format;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and RTC oscillator rate */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

/*****************************************************************************
 * Private functions
 ****************************************************************************/


/*****************************************************************************
 * Public functions
 ****************************************************************************/

void Board_CAN_Init(LPC_CAN_T *pCAN)
{
    Chip_CAN_Init(pCAN, LPC_CANAF, LPC_CANAF_RAM);
    Chip_CAN_SetBitRate(pCAN, CAN_BITRATE);
    Chip_CAN_EnableInt(pCAN, CAN_IER_RIE);

    Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
    NVIC_EnableIRQ(CAN_IRQn);

}

void Board_ADC_Init(void)
{
    Chip_ADC_Init(LPC_ADC, &ADCSetup);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH3, ENABLE);
    Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, 100000);
    Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
}

void Board_Max6301_Init(Max6301Mode_t eMode)
{
    Chip_GPIO_WriteDirBit(LPC_GPIO,
                          WDG_WDS_GPIO_PORT_NUM,
                          WDG_WDS_GPIO_BIT_NUM,
                          true);

    Chip_GPIO_WriteDirBit(LPC_GPIO,
                          WDG_WDI_GPIO_PORT_NUM,
                          WDG_WDI_GPIO_BIT_NUM,
                          true);

    if(eMode == eMax6301NORMAL)
        Chip_GPIO_WritePortBit(LPC_GPIO,
                               WDG_WDS_GPIO_PORT_NUM,
                               WDG_WDS_GPIO_BIT_NUM,
                               0);
    else if (eMode == eMax6301EXTENDED)
        Chip_GPIO_WritePortBit(LPC_GPIO,
                               WDG_WDS_GPIO_PORT_NUM,
                               WDG_WDS_GPIO_BIT_NUM,
                               1);

}

void Board_Max6301_Refresh(void)
{
    Chip_GPIO_SetPinToggle(LPC_GPIO,
                           WDG_WDI_GPIO_PORT_NUM,
                           WDG_WDI_GPIO_BIT_NUM);
}

/* Initializes board LED(s) */
static void Board_LED_Init(void)
{
    int i;

    /* Pin PIO0_22 is configured as GPIO pin during SystemInit */
    /* Set the PIO_22 as output */

    for (i = 0; i < sizeof(ledBits); i++) {
        Chip_GPIO_SetPinDIROutput(LPC_GPIO, ledPorts[i], ledBits[i]);
        //Chip_GPIO_SetPinState(LPC_GPIO, ledPorts[i], ledBits[i], true);
    }
}


/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
    bool state = false;

    state = Chip_GPIO_ReadPortBit(LPC_GPIO, ledPorts[LEDNumber], ledBits[LEDNumber]);

    return state;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
    Board_LED_Set(LEDNumber, !Board_LED_Test(LEDNumber));
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
    Chip_GPIO_WritePortBit(LPC_GPIO, ledPorts[LEDNumber], ledBits[LEDNumber], (bool) On);
}



/* Initialize UART pins */
void Board_UART_Init(LPC_USART_T *pUART)
{
	/* Pin Muxing has already been done during SystemInit */
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_ENABLE)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
	
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_ENABLE)
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_ENABLE)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_ENABLE)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);

    /* Initialize LEDs */
    Board_LED_Init();
    Board_Buttons_Init();
}


/* Initialize pin muxing for SSP interface */
void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		/* Set up clock and muxing for SSP1 interface */
		/*
		 * Initialize SSP0 pins connect
		 * P0.7: SCK
		 * P0.6: SSEL
		 * P0.8: MISO
		 * P0.9: MOSI
		 */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 7, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 6, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 8, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 9, IOCON_MODE_INACT, IOCON_FUNC2);

        Chip_SSP_Init(LPC_SSP1);

        ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
        ssp_format.bits = SSP_BITS_16;
        ssp_format.clockMode = SSP_CLOCK_MODE0;
        Chip_SSP_SetFormat(LPC_SSP1,
                           ssp_format.bits,
                           ssp_format.frameFormat,
                           ssp_format.clockMode);

        Chip_SSP_Enable(LPC_SSP1);

        Chip_SSP_SetMaster(LPC_SSP1, 1);
	}
	else {
		/* Set up clock and muxing for SSP0 interface */
		/*
		 * Initialize SSP0 pins connect
		 * P0.15: SCK
		 * P0.16: SSEL
		 * P0.17: MISO
		 * P0.18: MOSI
		 */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 17, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 18, IOCON_MODE_INACT, IOCON_FUNC2);
	}
}

/* Sets up board specific I2C interface */
void Board_I2C_Init(I2C_ID_T id)
{
	switch (id) {
	case I2C0:
		Chip_IOCON_PinMux(LPC_IOCON, 0, 27, IOCON_MODE_INACT, IOCON_FUNC1);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 28, IOCON_MODE_INACT, IOCON_FUNC1);
		Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
		break;

	case I2C1:
		Chip_IOCON_PinMux(LPC_IOCON, 0, 19, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 20, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 19);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 20);
		break;

	case I2C2:
		Chip_IOCON_PinMux(LPC_IOCON, 0, 10, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 11, IOCON_MODE_INACT, IOCON_FUNC2);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 10);
		Chip_IOCON_EnableOD(LPC_IOCON, 0, 11);
		break;
	}
}

void Board_Buttons_Init(void)
{
    int i;

    for (i = 0; i < sizeof(ledBits); i++) {
        Chip_GPIO_WriteDirBit(LPC_GPIO, buttonPorts[i], buttonBits[i], false);
    }
}

void Serial_CreateStream(void *Stream)
{}



