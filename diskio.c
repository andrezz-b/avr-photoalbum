/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h" /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "SD.h"


/* Definitions of physical drive number for each drive */
#define DEV_MMC 0 /* Example: Map MMC/SD card to physical drive 1 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

// Disk status
static volatile DSTATUS __MMCStatus = STA_NOINIT;

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    switch (pdrv)
    {
        case DEV_MMC:
        {
            if (SD_Init() == SD_SUCCESSFULL)
            {
                __MMCStatus &= ~STA_NOINIT;

                return __MMCStatus;
            }
        }
    }

    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    switch (pdrv)
    {
        case DEV_MMC:
        {
            return __MMCStatus;
        }
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE  pdrv,   /* Physical drive nmuber to identify the drive */
                  BYTE* buff,   /* Data buffer to store read data */
                  LBA_t sector, /* Start sector in LBA */
                  UINT  count   /* Number of sectors to read */
)
{
    switch (pdrv)
    {
        case DEV_MMC:
        {
            if (__MMCStatus & STA_NOINIT)
            {
                return RES_NOTRDY;
            }

            // Read a single block
            if (count == 1)
            {
                if (SD_ReadDataBlock(sector, buff) == SD_SUCCESSFULL)
                {
                    return RES_OK;
                }
            }
            // Read multiple blocks
            else
            {
                if (SD_ReadDataBlocks(sector, count, buff) == SD_SUCCESSFULL)
                {
                    return RES_OK;
                }
            }

            return RES_ERROR;
        }
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE        pdrv,   /* Physical drive nmuber to identify the drive */
                   const BYTE* buff,   /* Data to be written */
                   LBA_t       sector, /* Start sector in LBA */
                   UINT        count   /* Number of sectors to write */
)
{
    switch (pdrv)
    {
        case DEV_MMC:
        {
            if (__MMCStatus & STA_NOINIT)
            {
                return RES_NOTRDY;
            }

            // Write a single block
            if (count == 1)
            {
                if (SD_WriteDataBlock(sector, buff) == SD_SUCCESSFULL)
                {
                    return RES_OK;
                }
            }
            // Write multiple blocks
            else
            {
                if (SD_WriteDataBlocks(sector, count, buff) == SD_SUCCESSFULL)
                {
                    return RES_OK;
                }
            }

            return RES_ERROR;
        }
    }

    return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE  pdrv, /* Physical drive nmuber (0..) */
                   BYTE  cmd,  /* Control code */
                   void* buff  /* Buffer to send/receive control data */
)
{
    BYTE* ptr = (BYTE*) buff;

    switch (pdrv)
    {
        case DEV_MMC:
        {
            switch (cmd)
            {
                case GET_BLOCK_SIZE:
                {
                    if (SD_GetEraseBlockSize((uint16_t*) ptr) == SD_SUCCESSFULL)
                    {
                        return RES_OK;
                    }

                    return RES_OK;
                }
                case GET_SECTOR_COUNT:
                {
                    if (SD_GetSectors((DWORD*) ptr) == SD_SUCCESSFULL)
                    {
                        return RES_OK;
                    }

                    return RES_ERROR;
                }
                case GET_SECTOR_SIZE:
                {
                    *(WORD*) buff = SD_BLOCK_SIZE;

                    // ToDo: Support for older SD cards with smaller sectors than 512 Byte

                    return RES_OK;
                }
                case CTRL_SYNC:
                {
                    SD_Sync();

                    return RES_OK;
                }
                default:
                {
                    return RES_PARERR;
                }
            }
        }
        default:
        {
            return RES_PARERR;
        }
    }

    return RES_PARERR;
}
