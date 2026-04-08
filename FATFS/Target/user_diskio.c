/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "main.h"
#include "spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SD_CS_LOW()     HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET)
#define SD_CS_HIGH()    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET)

/* SD Card Commands */
#define CMD0    0   /* GO_IDLE_STATE */
#define CMD1    1   /* SEND_OP_COND */
#define CMD8    8   /* SEND_IF_COND */
#define CMD9    9   /* SEND_CSD */
#define CMD10   10  /* SEND_CID */
#define CMD12   12  /* STOP_TRANSMISSION */
#define CMD16   16  /* SET_BLOCKLEN */
#define CMD17   17  /* READ_SINGLE_BLOCK */
#define CMD18   18  /* READ_MULTIPLE_BLOCK */
#define CMD23   23  /* SET_BLOCK_COUNT */
#define CMD24   24  /* WRITE_BLOCK */
#define CMD25   25  /* WRITE_MULTIPLE_BLOCK */
#define CMD41   41  /* SEND_OP_COND (ACMD) */
#define CMD55   55  /* APP_CMD */
#define CMD58   58  /* READ_OCR */

/* SD Card Types */
#define SD_TYPE_V1      0x01
#define SD_TYPE_V2      0x02
#define SD_TYPE_V2HC    0x04

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static uint8_t CardType = 0;

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

static uint8_t SPI_ReadWriteByte(uint8_t data)
{
    uint8_t rx_data = 0xFF;
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx_data, 1, HAL_MAX_DELAY);
    return rx_data;
}

static uint8_t SD_WaitReady(void)
{
    uint32_t timeout = 500000;
    while (SPI_ReadWriteByte(0xFF) != 0xFF) {
        if (--timeout == 0) return 1;
    }
    return 0;
}

static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t res;
    uint8_t n;

    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);
    SD_CS_LOW();

    if (SD_WaitReady()) return 0xFF;

    SPI_ReadWriteByte(cmd | 0x40);
    SPI_ReadWriteByte(arg >> 24);
    SPI_ReadWriteByte(arg >> 16);
    SPI_ReadWriteByte(arg >> 8);
    SPI_ReadWriteByte(arg);
    SPI_ReadWriteByte(crc);

    if (cmd == CMD12) SPI_ReadWriteByte(0xFF);

    n = 10;
    do {
        res = SPI_ReadWriteByte(0xFF);
    } while ((res & 0x80) && --n);

    return res;
}

static uint8_t SD_ReceiveData(uint8_t *buff, uint32_t len)
{
    uint8_t res;
    uint32_t timeout = 200000;

    do {
        res = SPI_ReadWriteByte(0xFF);
    } while (res != 0xFE && --timeout);

    if (res != 0xFE) return 1;

    while (len--) {
        *buff++ = SPI_ReadWriteByte(0xFF);
    }

    SPI_ReadWriteByte(0xFF); // CRC
    SPI_ReadWriteByte(0xFF);

    return 0;
}

static uint8_t SD_SendBlock(const uint8_t *buff, uint8_t token)
{
    uint8_t res;
    uint16_t i;

    if (SD_WaitReady()) return 1;

    SPI_ReadWriteByte(token);
    if (token != 0xFD) {
        for (i = 0; i < 512; i++) {
            SPI_ReadWriteByte(buff[i]);
        }
        SPI_ReadWriteByte(0xFF); // CRC
        SPI_ReadWriteByte(0xFF);
        res = SPI_ReadWriteByte(0xFF);
        if ((res & 0x1F) != 0x05) return 1;
    }
    return 0;
}

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER COD E BEGIN INIT */
    uint8_t n, cmd, ty, ocr[4];
    uint32_t i;

    if (pdrv) return STA_NOINIT;

    // Slow clock for initialization
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    HAL_SPI_Init(&hspi2);

    SD_CS_HIGH();
    for (n = 10; n; n--) SPI_ReadWriteByte(0xFF);

    ty = 0;
    if (SD_SendCmd(CMD0, 0, 0x95) == 1) {
        if (SD_SendCmd(CMD8, 0x1AA, 0x87) == 1) {
            for (n = 0; n < 4; n++) ocr[n] = SPI_ReadWriteByte(0xFF);
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
                for (i = 10000; i && SD_SendCmd(CMD55, 0, 0) <= 1 && SD_SendCmd(CMD41, 1UL << 30, 0); i--);
                if (i && SD_SendCmd(CMD58, 0, 0) == 0) {
                    for (n = 0; n < 4; n++) ocr[n] = SPI_ReadWriteByte(0xFF);
                    ty = (ocr[0] & 0x40) ? SD_TYPE_V2HC : SD_TYPE_V2;
                }
            }
        } else {
            if (SD_SendCmd(CMD55, 0, 0) <= 1 && SD_SendCmd(CMD41, 0, 0) <= 1) {
                ty = SD_TYPE_V1; cmd = CMD41;
            } else {
                ty = SD_TYPE_V1; cmd = CMD1;
            }
            for (i = 10000; i && SD_SendCmd(cmd, 0, 0); i--);
            if (!i || SD_SendCmd(CMD16, 512, 0) != 0) ty = 0;
        }
    }
    CardType = ty;
    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);

    if (ty) {
        Stat &= ~STA_NOINIT;
        // High clock for data transfer
        hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
        HAL_SPI_Init(&hspi2);
    } else {
        Stat |= STA_NOINIT;
    }

    return Stat;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    if (pdrv) return STA_NOINIT;
    return Stat;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & SD_TYPE_V2HC)) sector *= 512;

    if (count == 1) {
        if ((SD_SendCmd(CMD17, sector, 0) == 0) && (SD_ReceiveData(buff, 512) == 0)) {
            count = 0;
        }
    } else {
        if (SD_SendCmd(CMD18, sector, 0) == 0) {
            do {
                if (SD_ReceiveData(buff, 512)) break;
                buff += 512;
            } while (--count);
            SD_SendCmd(CMD12, 0, 0);
        }
    }
    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);

    return count ? RES_ERROR : RES_OK;
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & SD_TYPE_V2HC)) sector *= 512;

    if (count == 1) {
        if ((SD_SendCmd(CMD24, sector, 0) == 0) && (SD_SendBlock(buff, 0xFE) == 0)) {
            count = 0;
        }
    } else {
        if (CardType & SD_TYPE_V2) SD_SendCmd(CMD55, 0, 0);
        SD_SendCmd(CMD23, count, 0);
        if (SD_SendCmd(CMD25, sector, 0) == 0) {
            do {
                if (SD_SendBlock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (SD_SendBlock(0, 0xFD)) count = 1;
        }
    }
    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);

    return count ? RES_ERROR : RES_OK;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
    DRESULT res;
    uint8_t n, csd[16];
    uint32_t csize;

    if (pdrv) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    res = RES_ERROR;
    switch (cmd) {
        case CTRL_SYNC:
            if (SD_WaitReady() == 0) res = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            if ((SD_SendCmd(CMD9, 0, 0) == 0) && (SD_ReceiveData(csd, 16) == 0)) {
                if ((csd[0] >> 6) == 1) { // SDHC
                    csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
                    *(DWORD*)buff = csize << 10;
                } else { // SDV1 or SDV2
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = csize << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 128;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
            break;
    }

    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);

    return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */

