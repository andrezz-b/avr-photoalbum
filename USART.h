#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#include <stdbool.h>

/** @defgroup Serial
 *  @{
 */
/** @defgroup Serial-Commands
 *  Serial commands.
 *  @{
 */
#define LF   0x0A /**< Line feed */
#define CR   0x0D /**< Carriage return */
#define XON  0x11 /**< Software flow control on */
#define XOFF 0x13 /**< Software flow control off */
/** @} */         // end of Serial-Commands
/** @} */         // end of Serial

/** @brief	USART callback definition.
 */
typedef void (*USART_Callback_t)(void);

/** @brief USART parity modes.
 */
typedef enum
{
    USART_PARITY_NONE = 0x00,                  /**< No parity */
    USART_PARITY_EVEN = _BV(UPM1),             /**< Even parity */
    USART_PARITY_ODD  = _BV(UPM1) | _BV(UPM0), /**< Odd parity */
} USART_Parity_t;

/** @brief USART stop bit settings.
 */
typedef enum
{
    USART_STOP_1 = 0x00,      /**< One stop bit */
    USART_STOP_2 = _BV(USBS), /**< Two stop bits */
} USART_Stop_t;

/** @brief USART data length settings.
 */
typedef enum
{
    USART_SIZE_5 = 0x00,                                 /**< 5 data bits */
    USART_SIZE_6 = _BV(UCSZ0),                           /**< 6 data bits */
    USART_SIZE_7 = _BV(UCSZ1),                           /**< 7 data bits */
    USART_SIZE_8 = _BV(UCSZ1) | _BV(UCSZ0),              /**< 8 data bits */
} USART_Size_t;

/** @brief USART interface operation mode.
 */
typedef enum
{
    USART_MODE_ASYNCH = 0x00,       /**< UART mode */
    USART_MODE_SYNCH  = _BV(UMSEL), /**< USART mode */
} USART_DeviceMode_t;

/** @brief USART transmitter/receiver settings.
 */
typedef enum
{
    USART_DIRECTION_NONE = 0x00, /**< No transmitter and receiver enabled */
    USART_DIRECTION_TX   = _BV(TXEN), /**< Transmitter only */
    USART_DIRECTION_RX   = _BV(RXEN), /**< Receiver only */
    USART_DIRECTION_BOTH =
        _BV(RXEN) | _BV(TXEN), /**< Receiver and transmitter */
} USART_Direction_t;

/** @brief USART-SPI clock polarity.
 */
typedef enum
{
    USART_POL_RISING  = 0x00,       /**< Clock polarity rising edge */
    USART_POL_FALLING = _BV(UCPOL), /**< Clock polarity falling edge */
} USART_Polarity_t;

/** @brief USART configuration object.
 */
typedef struct
{
    USART_DeviceMode_t DeviceMode; /**< USART device mode */
    USART_Direction_t  Direction;  /**< Direction mode */
    uint32_t           Baudrate;   /**< Baudrate */
    USART_Parity_t     Parity;     /**< Parity settings */
    USART_Stop_t       Stop;       /**< Stop bit settings */
    USART_Size_t       Size;       /**< Data bit settings */
    USART_Polarity_t
         ClockPolarity; /**< Clock polarity. Only needed in synchronous mode */
    bool EnableDoubleSpeed; /**< Set #true to enable double speed */
} USART_Config_t;

/** @brief				Set the direction of the USART interface.
 *  @param Direction	USART direction
 */
static inline void USART_SetDirection(const USART_Direction_t Direction)
    __attribute__((always_inline));
static inline void USART_SetDirection(const USART_Direction_t Direction)
{
    UCSRB &= ~(_BV(RXEN) | _BV(TXEN));
    UCSRB |= Direction;
}

/** @brief		Get the direction of the USART interface.
 *  @return	USART direction
 */
static inline USART_Direction_t USART_GetDirection(void)
    __attribute__((always_inline));
static inline USART_Direction_t USART_GetDirection(void)
{
    return (USART_Direction_t) (UCSRB & (_BV(RXEN) | _BV(TXEN)));
}

/** @brief		Get a single byte from the USART interface.
 *  @return	Data byte
 */
static inline char USART_GetChar(void) __attribute__((always_inline));
static inline char USART_GetChar(void)
{
    return UDR;
}

/** @brief			Transmit a single byte with the USART interface.
 *  @param Data	Data byte
 */
static inline void USART_SendChar(const char Data)
    __attribute__((always_inline));
static inline void USART_SendChar(const char Data)
{
    // Ignore 9 bit mode if it is enabled
    if (!(UCSRB & _BV(UCSZ2)))
    {
        // Wait for empty transmit buffer, then send Data
        while (!(UCSRA & _BV(UDRE)))
            ;
        UDR = Data;
    }
    else
    {
    }
}

/** @brief			Initialize the USART interface.
 *  @param Config	Pointer to USART device configuration struct
 */
void USART_Init(USART_Config_t* Config);

/** @brief			Transmit a char array with the USART interface.
 *  @param Data	Pointer to data array
 */
void USART_Write(char* Data);

/** @brief			Transmit a complete line (including CR + LF) with the USART
 * interface.
 *  @param Data	Pointer to data array
 */
void USART_WriteLine(char* Data);

/** @brief			Transmit a decimal number with the USART interface.
 *  @param Value	Decimal number
 */
void USART_WriteDecimal(const uint32_t Value);

/** @brief				Set the baud rate of a USART interface.
 *  @param Baudrate	Baudrate for the interface
 *  @param Clock		USART module clock
 *  @param DoubleSpeed	Double speed enabled/disabled
 */
void USART_SetBaudrate(const uint32_t BaudRate, const uint32_t Clock,
                       const bool DoubleSpeed);

#endif /* USART_H_ */