#ifndef SD_H_
#define SD_H_

/** @brief	Macro to convert a SD command ID into a SD card command.
 * @details In SPI mode, each command for the SD card consists of an 8-bit
 * command with the last bit set to 0 and the penultimate bit 1.
 */
#define SD_ID_TO_CMD(ID) (0x40 | ID)

/** @brief SD card communication errors.
 * @link https://github.com/Kampi/AVR/blob/master/include/Peripheral/SD/SD.h
 */
typedef enum
{
    SD_SUCCESSFULL     = 0x00, /**< No error */
    SD_NO_RESPONSE     = 0x01, /**< No response from card */
    SD_ILLEGAL_CMD     = 0x02, /**< Illegal command */
    SD_CRC_ERROR       = 0x04, /**< CRC error */
    SD_ERASE_ERROR     = 0x08, /**< Erase sequence error */
    SD_ADDRESS_ERROR   = 0x10, /**< Address error */
    SD_PARAMETER_ERROR = 0x20, /**< Parameter error */
} SD_Error_t;

#endif