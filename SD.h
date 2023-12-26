#ifndef SD_H_
#define SD_H_

#include "config.h"
#include <stdio.h>

/** @brief	Macro to convert a SD command ID into a SD card command.
 * @details In SPI mode, each command for the SD card consists of an 8-bit
 * command with the last bit set to 0 and the penultimate bit 1.
 */
#define SD_ID_TO_CMD(ID) (0x40 | ID)

/** @brief SD card states.
 */
typedef enum
{
    SD_STATE_INSERT  = 0x00, /**< SD card insert */
    SD_STATE_REMOVED = 0x01, /**< SD card removed */
} SD_State_t;

/** @brief SD card types.
 */
typedef enum
{
    SD_VER_UNKNOWN = 0x00, /**< Unknown card type */
    SD_MMC         = 0x01, /**< Multimedia card */
    SD_VER_1_STD   = 0x02, /**< Version 1 SD card */
    SD_VER_2_STD   = 0x03, /**< Version 2 SD card with less than 4 GB memory */
    SD_VER_2_HI    = 0x04, /**< Version 2 SD card with large memory */
} SD_CardType_t;

/** @brief SD speed classes.
 */
typedef enum
{
    SD_CLASS_0 = 0x00, /**< Class 0 */
    SD_CLASS_2 = 0x01, /**< Class 2 */
    SD_CLASS_4 = 0x02, /**< Class 4 */
    SD_CLASS_6 = 0x03, /**< Class 6 */
} SD_Class_t;

/** @brief SD card communication errors.
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

/** @brief SD card OCR object.
 *			NOTE: Please check
 *http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf if you
 *need additional information.
 */
typedef struct
{
    uint8_t Busy : 1;      /**< Busy flag
                                0 = busy
                                1 = ready */
    uint8_t Capacity : 1;  /**< Card capacity
                                0 = SD memory card
                                1 = SDHC memory card */
    uint8_t Reserved : 6;  /**< Reserved */
    uint8_t Vcc36 : 1;     /**< Vdd 3.6 - 3.5 V */
    uint8_t Vcc35 : 1;     /**< Vdd 3.5 - 3.4 V */
    uint8_t Vcc34 : 1;     /**< Vdd 3.4 - 3.3 V */
    uint8_t Vcc33 : 1;     /**< Vdd 3.3 - 3.2 V */
    uint8_t Vcc32 : 1;     /**< Vdd 3.2 - 3.1 V */
    uint8_t Vcc31 : 1;     /**< Vdd 3.1 - 3.0 V */
    uint8_t Vcc30 : 1;     /**< Vdd 3.0 - 2.9 V */
    uint8_t Vcc29 : 1;     /**< Vdd 2.9 - 2.8 V */
    uint8_t Vcc28 : 1;     /**< Vdd 2.8 - 2.7 V */
    uint8_t Vcc27 : 1;     /**< Vdd 2.7 - 2.6 V */
    uint8_t Vcc26 : 1;     /**< Vdd 2.6 - 2.5 V */
    uint8_t Vcc25 : 1;     /**< Vdd 2.5 - 2.4 V */
    uint8_t Vcc24 : 1;     /**< Vdd 2.4 - 2.3 V */
    uint8_t Vcc23 : 1;     /**< Vdd 2.3 - 2.2 V */
    uint8_t Vcc22 : 1;     /**< Vdd 2.2 - 2.1 V */
    uint8_t Vcc21 : 1;     /**< Vdd 2.1 - 2.0 V */
    uint8_t Vcc20 : 1;     /**< Vdd 2.0 - 1.9 V */
    uint8_t Vcc19 : 1;     /**< Vdd 1.9 - 1.8 V */
    uint8_t Vcc18 : 1;     /**< Vdd 1.8 - 1.7 V */
    uint8_t Vcc17 : 1;     /**< Vdd 1.7 - 1.6 V */
    uint8_t Reserved1 : 4; /**< Reserved */
} __attribute__((packed)) SD_OCR_t;

/** @brief SD card CID object.
 *			NOTE: Please check
 *http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf if you
 *need additional information.
 */
typedef struct
{
    uint8_t  MID;           /**< Manufacturer ID */
    uint16_t OID;           /**< OEM ID */
    uint8_t  PNM[5];        /**< Product name */
    uint8_t  PRV;           /**< Product revision */
    uint32_t PSN;           /**< Product serial number */
    uint8_t  Reserved0 : 3; /**< Reserved */
    uint16_t MDT0 : 11;     /**< Manufacturing date */
    uint8_t  CRC0 : 6;      /**< CRC */
    uint8_t  Reserved1 : 1; /**< Reserved */
} __attribute__((packed)) SD_CID_t;

/** @brief SD card CSD object.
 *			NOTE: Please check
 *http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf if you
 *need additional information.
 */
typedef struct
{
    uint8_t CSD_STRUCTURE : 2;       /**< CSD structure */
    uint8_t Reserved : 6;            /**< Reserved */
    uint8_t TAAC;                    /**< Data read access time 1 */
    uint8_t NSAC;                    /**< Data read access time 2
                                          Time in clock cycles (Value * 100) */
    uint8_t  TRAN_SPEED;             /**< Max. data transfer rate */
    uint16_t CCC : 12;               /**< Card command classes */
    uint8_t  READ_BL_LEN : 4;        /**< Max. read data block length */
    uint8_t  READ_BL_PARTIAL : 1;    /**< Partial blocks for read allowed */
    uint8_t  WRITE_BLK_MISALIGN : 1; /**< Write block misalignment */
    uint8_t  READ_BLK_MISALIGN : 1;  /**< Read block misalignment */
    uint8_t  DSR_IMPL : 1;           /**< DSR implemented */
    uint8_t  Reserved1 : 2;          /**< Reserved */
    uint16_t C_SIZE : 12;            /**< Device size */
    uint8_t  VDD_R_CURR_MIN : 3;     /**< Max. read current Vdd min */
    uint8_t  VDD_R_CURR_MAX : 3;     /**< Max. read current Vdd max */
    uint8_t  VDD_W_CURR_MIN : 3;     /**< Max. write current Vdd min */
    uint8_t  VDD_W_CURR_MAX : 3;     /**< Max. write current Vdd max */
    uint8_t  C_SIZE_MULT : 3;        /**< Device size multiplier */
    uint8_t  ERASE_BLK_EN : 1;       /**< Erase single block enable */
    uint8_t  SECTOR_SIZE : 7;        /**< Erase sector size */
    uint8_t  WP_GRP_SIZE : 7;        /**< Write protect group size */
    uint8_t  WP_GRP_ENABLE : 1;      /**< Write protect group enable */
    uint8_t  Reserved2 : 2;          /**< Reserved */
    uint8_t  R2W_FACTOR : 3;         /**< Write speed factor */
    uint8_t  WRITE_BL_LEN : 4;       /**< Max. write data block length */
    uint8_t  WRITE_BL_PARTIAL : 1;   /**< Partial blocks for write allowed */
    uint8_t  Reserved3 : 5;          /**< Reserved */
    uint8_t  FILE_FORMAT_GRP : 1;    /**< File format group */
    uint8_t  COPY : 1;               /**< Copy flag (OTP) */
    uint8_t  PERM_WRITE_PROTECT : 1; /**< Permanent write protection */
    uint8_t  TMP_WRITE_PROTECT : 1;  /**< Temporary write protection */
    uint8_t  FILE_FORMAT : 2;        /**< File format */
    uint8_t  Reserved4 : 2;          /**< Reserved */
    uint8_t  CHECK : 7;              /**< CRC checksum */
    uint8_t  Reserved5 : 1;          /**< Reserved */
} __attribute__((packed)) SD_CSD_t;

/** @brief SD card status object.
 *			NOTE: Please check
 *http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf if you
 *need additional information.
 */
typedef struct
{
    uint8_t DAT_BUS_WIDTH : 2;  /**< Data bus width
                                     00h : 1 bit
                                     01h : Reserved
                                     10h : 4 bit
                                     11h : Reserved */
    uint8_t SECURED_MODE : 1;   /**< Secure mode enabled
                                     0h : Disabled
                                     1h : Enabled */
    uint16_t Reserved : 13;     /**< Reserved */
    uint16_t SD_CARD_TYPE;      /**< SD card type
                                     00xxh : SD Memory cards as defined in
                                             Physical Spec Ver 1.01 - 2.00
                                     0000h : Regular SD card
                                     0001h : SD ROM card */
    uint32_t SIZE_OF_PROT_AREA; /**< Size of protected area */
    uint8_t  SPEED_CLASS;       /**< Speed class */
    uint8_t  PERFORMANCE_MOVE;  /**< Performance move */
    uint8_t  AU_SIZE : 4;       /**< Number of AUs to be erased at a time */
    uint8_t  Reserved1 : 4;     /**< Reserved */
    uint16_t ERASE_SIZE;        /**< Erase size of the SD card */
    uint8_t  ERASE_TIMEOUT : 6; /**< Erase timeout */
    uint8_t  ERASE_OFFSET : 2;  /**< Erase offset */
    uint8_t  Zero[11];          /**< All zero */
    uint8_t  Reserved3[39];     /**< Reserved for manufacturer */
} __attribute__((packed)) SD_Status_t;

/** @brief			Initialize the SD card interface and the SD card.
 *  @param Config	Pointer to SPI master configuration object for the SD card
 * interface
 *  @return		Error code
 */
const SD_Error_t SD_Init();

/** @brief	Abort all pending write processes.
 */
void SD_Sync(void);

/** @brief			Read the CSD register (16 bytes) of the SD card.
 *  @param Buffer	Pointer to CSD data
 *  @return		Error code
 */
const SD_Error_t SD_GetCSD(SD_CSD_t* CSD);

/** @brief		Read the CID register (16 bytes) of the SD card.
 *  @param CID	Pointer to CID data
 *  @return	Error code
 */
const SD_Error_t SD_GetCID(SD_CID_t* CID);

/** @brief		Read the OCR register (4 bytes) of the SD card.
 *  @param OCR	Pointer to OCR data
 *  @return	Error code
 */
const SD_Error_t SD_GetOCR(SD_OCR_t* OCR);

/** @brief			Read the SD card status (64 bytes).
 *  @param Status	Pointer to status data
 *  @return		Error code
 */
const SD_Error_t SD_GetStatus(SD_Status_t* Status);

/** @brief			Get the sector count of the SD card.
 *  @param Sectors	Pointer to sector count
 *  @return		Error code
 */
const SD_Error_t SD_GetSectors(uint32_t* Sectors);

/** @brief				Get the erase block size of the SD card.
 *  @param EraseSize	Pointer to erase size
 *  @return			Error code
 */
const SD_Error_t SD_GetEraseBlockSize(uint16_t* EraseSize);

/** @brief		Get the card type of the SD card.
 *  @return	SD card type
 */
const SD_CardType_t SD_GetCardType(void);

/** @brief			Read a single data block from the SD card.
 *  @param Address	Start block
 *  @param Buffer	Pointer to data
 *  @return		Error code
 */
const SD_Error_t SD_ReadDataBlock(const uint32_t Address, uint8_t* Buffer);

/** @brief			Read multiple data blocks from the SD card.
 *  @param Address	Start block
 *  @param Blocks	Data blocks
 *  @param Buffer	Pointer to data
 *  @return		Error code
 */
const SD_Error_t SD_ReadDataBlocks(const uint32_t Address, const uint32_t Blocks, uint8_t* Buffer);

/** @brief			Write a single data block to the SD card.
 *  @param Address	Start block
 *  @param Buffer	Pointer to data
 *  @return		Error code
 */
const SD_Error_t SD_WriteDataBlock(const uint32_t Address, const uint8_t* Buffer);

/** @brief			Write multiple data blocks to the SD card.
 *  @param Address	Start block
 *  @param Blocks	Data blocks
 *  @param Buffer	Pointer to data
 *  @return		Error code
 */
const SD_Error_t SD_WriteDataBlocks(const uint32_t Address, const uint32_t Blocks,
                                    const uint8_t* Buffer);

#endif