/*-----------------------------------------------------------------------*/
/* Low level disk I/O module for FatFs     (C)ChaN, 2019                */
/*-----------------------------------------------------------------------*/
/* This module is customized for STM32F103C8 with SD Card via SPI2       */
/*-----------------------------------------------------------------------*/

#include "ff.h"         /* FatFs types and definitions */
#include "diskio.h"     /* Declarations of disk functions */
#include "stm32f10x.h"  /* STM32 hardware definitions */

/* Definitions of physical drive number */
#define DEV_MMC     0   /* Map SD Card to physical drive 0 */

/* External SPI functions from main.c */
extern uint8_t SPI2_SendByte(uint8_t byte);
extern void SD_CS_Select(void);
extern void SD_CS_Deselect(void);
extern void Delay_ms(uint32_t ms);

/* SD Card status */
static volatile DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    if (pdrv != DEV_MMC) {
        return STA_NOINIT;  /* Only support drive 0 (SD Card) */
    }
    return Stat;  /* Return current status */
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    uint8_t res;
    int i;

    if (pdrv != DEV_MMC) {
        return STA_NOINIT;  /* Only support drive 0 (SD Card) */
    }

    /* Deselect SD Card and send 74+ clock pulses */
    SD_CS_Deselect();
    for (i = 0; i < 10; i++) {
        SPI2_SendByte(0xFF);
    }

    /* Enter SPI mode with CMD0 */
    SD_CS_Select();
    SPI2_SendByte(0x40); /* CMD0 */
    SPI2_SendByte(0x00);
    SPI2_SendByte(0x00);
    SPI2_SendByte(0x00);
    SPI2_SendByte(0x00);
    SPI2_SendByte(0x95); /* Valid CRC for CMD0 */

    /* Wait for response (0x01 = idle state) */
    for (i = 0; i < 1000; i++) {
        res = SPI2_SendByte(0xFF);
        if (res != 0xFF) break;
    }
    SD_CS_Deselect();

    if (res == 0x01) {
        Stat &= ~STA_NOINIT; /* Clear NOINIT flag if successful */
        return 0;
    }

    return STA_NOINIT; /* Initialization failed */
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number */
    BYTE *buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res;
    int i;

    if (pdrv != DEV_MMC || !count) {
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;
    }

    /* Read each sector */
    while (count--) {
        SD_CS_Select();

        /* Send CMD17 (Read Single Block) */
        SPI2_SendByte(0x51); /* CMD17 */
        SPI2_SendByte((sector >> 24) & 0xFF);
        SPI2_SendByte((sector >> 16) & 0xFF);
        SPI2_SendByte((sector >> 8) & 0xFF);
        SPI2_SendByte(sector & 0xFF);
        SPI2_SendByte(0xFF); /* Dummy CRC */

        /* Wait for response */
        for (i = 0; i < 1000; i++) {
            res = SPI2_SendByte(0xFF);
            if (res != 0xFF) break;
        }

        if (res == 0x00) {
            /* Wait for data token (0xFE) */
            while (SPI2_SendByte(0xFF) != 0xFE);

            /* Read 512 bytes of data */
            for (i = 0; i < 512; i++) {
                buff[i] = SPI2_SendByte(0xFF);
            }

            /* Read 2-byte CRC (discard) */
            SPI2_SendByte(0xFF);
            SPI2_SendByte(0xFF);
        } else {
            SD_CS_Deselect();
            return RES_ERROR;
        }

        SD_CS_Deselect();
        buff += 512; /* Move buffer pointer to next sector */
        sector++;
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive number */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res;
    int i;

    if (pdrv != DEV_MMC || !count) {
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;
    }

    /* Write each sector */
    while (count--) {
        SD_CS_Select();

        /* Send CMD24 (Write Single Block) */
        SPI2_SendByte(0x58); /* CMD24 */
        SPI2_SendByte((sector >> 24) & 0xFF);
        SPI2_SendByte((sector >> 16) & 0xFF);
        SPI2_SendByte((sector >> 8) & 0xFF);
        SPI2_SendByte(sector & 0xFF);
        SPI2_SendByte(0xFF); /* Dummy CRC */

        /* Wait for response */
        for (i = 0; i < 1000; i++) {
            res = SPI2_SendByte(0xFF);
            if (res != 0xFF) break;
        }

        if (res == 0x00) {
            SPI2_SendByte(0xFF); /* Dummy byte */
            SPI2_SendByte(0xFE); /* Data token */

            /* Write 512 bytes of data */
            for (i = 0; i < 512; i++) {
                SPI2_SendByte(buff[i]);
            }

            /* Write 2-byte CRC (dummy) */
            SPI2_SendByte(0xFF);
            SPI2_SendByte(0xFF);

            /* Check data response */
            res = SPI2_SendByte(0xFF);
            if ((res & 0x1F) == 0x05) { /* Data accepted */
                /* Wait for write completion */
                while (SPI2_SendByte(0xFF) == 0x00);
            } else {
                SD_CS_Deselect();
                return RES_ERROR;
            }
        } else {
            SD_CS_Deselect();
            return RES_ERROR;
        }

        SD_CS_Deselect();
        buff += 512; /* Move buffer pointer to next sector */
        sector++;
    }

    return RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive number */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    if (pdrv != DEV_MMC) {
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;
    }

    switch (cmd) {
    case CTRL_SYNC: /* Flush disk cache */
        SD_CS_Select();
        while (SPI2_SendByte(0xFF) == 0x00); /* Wait for SD Card to finish */
        SD_CS_Deselect();
        return RES_OK;

    case GET_SECTOR_SIZE: /* Get sector size */
        *(WORD*)buff = 512;
        return RES_OK;

    case GET_BLOCK_SIZE: /* Get erase block size in sectors */
        *(DWORD*)buff = 1; /* SD Card doesn't use erase blocks in SPI mode */
        return RES_OK;

    case GET_SECTOR_COUNT: /* Get number of sectors (simple estimation) */
        /* This should use CMD9 to get real capacity, but for simplicity: */
        *(DWORD*)buff = 2097152; /* Example: 1GB card (1GB / 512 bytes) */
        return RES_OK;

    default:
        return RES_PARERR;
    }
}
