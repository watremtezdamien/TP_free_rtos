/*
 * @brief T290-NT board file
 *
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_T290NT_1768 MEL T290NT board software API functions
 * @ingroup LPCOPEN_17XX_BOARD_T290NT
 * The board support software API functions provide some simple abstracted
 * functions used across multiple LPCOpen board examples. See @ref BOARD_COMMON_API
 * for the functions defined by this board support layer.<br>
 * @{
 */

/** @defgroup BOARD_T290NT_1768_OPTIONS BOARD: T290NT board build options
 * This board has options that configure its operation at build-time.<br>
 * @{
 */

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
    out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
    semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
    Board_UARTPutSTR functions.
 */
#define DEBUG_UART LPC_UART0

/**
 * @}
 */

/* Board name */
#define BOARD_T290NT

//! Sélection de la vitesse CAN
/*! @note Couvre l'exigence REQ_DSL_200 */
#define CAN_BITRATE     eBITRATE_125KBPS

/**
 * LED defines
 */
typedef enum {
    eLED_SEQ            = 0x00,
    eLED_INOP           = 0x01,
    eLED_VOY_RES        = 0x02
} Led_t;

typedef enum {
    eBUTTON_LEFT            = 0x00,
    eBUTTON_RIGHT           = 0x01,
    eBUTTON_TEST            = 0x02,
    eBUTTON_PAGE            = 0x03,
    eBUTTON_LINE            = 0x04,
    eBUTTON_CD0             = 0x05,
    eBUTTON_CD1             = 0x06,
    eBUTTON_VALIDATION      = 0x07,
    eBUTTON_JETTISON        = 0x08,
    eBUTTON_JETTISON_BIS    = 0x09,
    eBUTTON_SHOT            = 0x0A,
    eBUTTON_SHOT_BIS        = 0x0B,
    eBUTTON_BP_RES          = 0x0C,
    eBUTTON_STDBY           = 0x0D,
} Button_t;

#define VALID_VOY_GPIO_PORT_NUM         2
#define VALID_VOY_GPIO_BIT_NUM          13

#define WDG_WDS_GPIO_PORT_NUM           1
#define WDG_WDS_GPIO_BIT_NUM            10

#define WDG_WDI_GPIO_PORT_NUM           1
#define WDG_WDI_GPIO_BIT_NUM            31

//! Liste des choix d'adresses CAN CCU
typedef enum {
    eCCU1    = 0,
    eCCU2    = 1
} CanAdress_t;

//! Sélection de l'adresse CAN à utiliser
/*! @note Couvre l'exigence REQ_DSL_200 */
#define t290ntCAN_ADDR        eT290ntCCU1


//! Liste des vitesses CAN utilisables
typedef enum {
    eBITRATE_125KBPS     = 125000,
    eBITRATE_250KBPS     = 250000,
    eBITRATE_500KBPS     = 500000,
    eBITRATE_1MBPS       = 1000000
} Bitrate_t;


//! Mode d'utilisation du MAX6301
typedef enum {
    eMax6301NORMAL       = 0,    //!< Mode normal
    eMax6301EXTENDED     = 1     //!< Mode extended
} Max6301Mode_t;

void Board_Max6301_Refresh(void);

void Board_Max6301_Init(Max6301Mode_t eMode);

void Board_ADC_Init(void);

void Board_CAN_Init(LPC_CAN_T *pCAN);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: ID of I2C peripheral
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_FAST_MODE_PLUS);
}

/**
 * @brief	Disables I2C Fast plus mode and enable normal mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_IOCON_SetI2CPad(LPC_IOCON, I2CPADCFG_STD_MODE);
}

/**
 * @brief	Initialize buttons on the board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Create Serial Stream
 * @param	Stream	: Pointer to stream
 * @return	Nothing
 */
void Serial_CreateStream(void *Stream);


/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
