#include "SD.h"
#include "SPI.h"

/** @defgroup SD
 *  @{
 */
/** @defgroup SD-Commands
 *  SD card commands.
 *  @{
 */
#define SD_CMD_GO_IDLE              0
#define SD_CMD_SEND_OP_COND         1
#define SD_CMD_ALL_SEND_CID         2
#define SD_CMD_SEND_RELATIVE_ADDR   3
#define SD_CMD_SET_DSR              4
#define SD_CMD_SWITCH_FUNC          6
#define SD_CMD_SELECT_DESELECT_CARD 7
#define SD_CMD_IF_COND              8
#define SD_CMD_SEND_CSD             9
#define SD_CMD_SEND_CID             10
#define SD_CMD_STOP_TRANSMISSION    12
#define SD_CMD_SEND_STATUS          13
#define SD_CMD_GO_INACTIVE          15
#define SD_CMD_SET_BLOCKLEN         16
#define SD_CMD_READ_SINGLE_BLOCK    17
#define SD_CMD_READ_MULTIPLE_BLOCK  18
#define SD_CMD_WRITE_SINGLE_BLOCK   24
#define SD_CMD_WRITE_MULTIPLE_BLOCK 25
#define SD_CMD_PROGRAMM_CSD         27
#define SD_CMD_SET_WRITE_PROT       28
#define SD_CMD_SET_CLR_PROT         29
#define SD_CMD_SET_SEND_PROT        30
#define SD_CMD_ERASE_WR_BLK_START   32
#define SD_CMD_ERASE_WR_BLK_END     33
#define SD_CMD_ERASE                38
#define SD_CMD_APP_SEND_OP_COND     41
#define SD_CMD_LOCK_UNLOCK          42
#define SD_CMD_APP_CMD              55
#define SD_CMD_READ_OCR             58

#define SD_CMD_ACMD13 (0x80 | SD_CMD_SEND_STATUS)
#define SD_CMD_ACMD41 (0x80 | SD_CMD_APP_SEND_OP_COND)
/** @} */ // end of Commands

/** @defgroup SD-Token
 *  SD card command tokens.
 *  @{
 */
#define SD_TOKEN_DATA_CMD25 0xFC /**< Data token for CMD25 */
#define SD_TOKEN_STOP       0xFD /**< Stop token for CMD25 */
#define SD_TOKEN_DATA       0xFE /**< Data token for CMD17/28/24 */
/** @} */                        // end of Token

/** @defgroup SD-Errors
 *  SD card error codes.
 *  @{
 */
#define SD_STATE_SUCCESSFULL     0x00 /**< No error */
#define SD_STATE_IDLE            0x01 /**< SD card enters idle state */
#define SD_STATE_RESET           0x02 /**< Reset state */
#define SD_STATE_ILLEGAL_COMMAND 0x04 /**< Illegal command error */
#define SD_STATE_CRC_ERROR       0x08 /**< CRC error */
#define SD_STATE_ERASE_ERROR     0x10 /**< Error during erase operation */
#define SD_STATE_ADDRESS_ERROR   0x20 /**< Address error */
#define SD_STATE_PARAMETER_ERROR 0x40 /**< Command parameter error */
/** @} */                             // end of Errors
/** @} */                             // end of SD

/**
 * @brief	Select the SD card
 * @link
 * https://kampi.gitbook.io/avr/sd-card-support-for-avr#a-deeper-look-into-the-sd-card
 */
static void SD_select()
{
    SPI_transmit(0xff);
    CS_CLEAR();
}

/**
 * @brief	Deselect the SD card
 * @details It is recommended to generate at least 80 additional clock cycles on
 * the SPI after unselecting the SD card to allow the SD card to complete its
 * internal processes.
 * @link
 * https://kampi.gitbook.io/avr/sd-card-support-for-avr#a-deeper-look-into-the-sd-card
 */
static void SD_Deselect(void)
{
    CS_SET();

    for (uint8_t i = 0; i < 10; i++)
    {
        SPI_transmit(0xff);
    }
}

/** @brief		Perform a software reset with the SD card.
 *  @return		Error code
 */
static const SD_Error_t SD_SoftwareReset(void)
{
    uint8_t Repeat = 0x00;

    for (uint8_t i = 0; i < 10; i++)
    {
        SPI_transmit(0xFF);
    }

    while (SD_SendCommand(SD_ID_TO_CMD(SD_CMD_GO_IDLE), 0x00) != SD_STATE_IDLE)
    {
        if (Repeat++ == 10)
        {
            SD_Deselect();

            return SD_NO_RESPONSE;
        }
    }

    SD_Deselect();

    return SD_SUCCESSFULL;
}