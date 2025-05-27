#include <string.h>
#include "stm32f10x.h"
#include "ff.h"

/* Ð?nh nghia chân CS */
#define SD_CS_PORT    GPIOB
#define SD_CS_PIN     GPIO_Pin_12

/* Ð?i tu?ng FatFs */
FATFS fs;         /* Filesystem object */
FIL file;         /* File object */
FRESULT fr;       /* FatFs result code */

/* C?u hình GPIO cho SPI2 */
void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable clock cho GPIOB và SPI2 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* C?u hình PB13 (SCK), PB14 (MISO), PB15 (MOSI) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* C?u hình PB12 (CS) */
    GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SD_CS_PORT, &GPIO_InitStructure);

    /* Ð?t CS ? m?c cao m?c d?nh */
    GPIO_SetBits(SD_CS_PORT, SD_CS_PIN);
}

/* C?u hình SPI2 */
void SPI2_Configuration(void) {
    SPI_InitTypeDef SPI_InitStructure;

    /* C?u hình SPI2 ? t?c d? th?p ban d?u (36MHz/256 ˜ 140kHz) */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
}

/* G?i và nh?n byte qua SPI2 */
uint8_t SPI2_SendByte(uint8_t byte) {
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, byte);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}

/* Ch?n/b? ch?n CS */
void SD_CS_Select(void) {
    GPIO_ResetBits(SD_CS_PORT, SD_CS_PIN);
}

void SD_CS_Deselect(void) {
    GPIO_SetBits(SD_CS_PORT, SD_CS_PIN);
}

/* Hàm delay don gi?n (dùng t?m, có th? thay b?ng timer) */
void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 72000; i++); /* Gi? d?nh 72MHz */
}

/* Hàm tang t?c SPI sau khi kh?i t?o */
void SPI2_SetHighSpeed(void) {
    SPI_Cmd(SPI2, DISABLE);
    SPI2->CR1 &= 0xFFC7; /* Xóa bits prescaler */
    SPI2->CR1 |= SPI_BaudRatePrescaler_4; /* 36MHz/4 = 9MHz */
    SPI_Cmd(SPI2, ENABLE);
}
void RCC_Init(void) 
{
	// Step 1: Kích ho?t HSE (High-Speed External clock)
	RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY)) {
      // Ch? d?n khi HSE s?n sàng
  }

  // Step 2: Thi?t l?p Flash memory
  FLASH->ACR |= FLASH_ACR_PRFTBE;    // B?t prefetch buffer
  while (!(FLASH->ACR & FLASH_ACR_PRFTBS)) {
      // Ch? d?n khi prefetch buffer du?c b?t
  }
  FLASH->ACR &= ~FLASH_ACR_LATENCY;  // Xóa bits latency cu
  FLASH->ACR |= FLASH_ACR_LATENCY_2; // Thi?t l?p d? tr? 2 wait states

	// Step 3: C?u hình PLL (Phase-Locked Loop)
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);  // Xóa c?u hình cu
  RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9); // HSE làm ngu?n PLL, nhân v?i 9

	// Step 4: C?u hình các b? chia t?n h? th?ng
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // Xóa c?u hình cu
  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1    // AHB prescaler: không chia
              | RCC_CFGR_PPRE1_DIV2   // APB1 prescaler: chia 2
              | RCC_CFGR_PPRE2_DIV1); // APB2 prescaler: không chia

  // Step 5: Kích ho?t PLL
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY)) {
      // Ch? d?n khi PLL s?n sàng
  }

	// Step 6: Chuy?n clock h? th?ng sang PLL
  RCC->CFGR &= ~RCC_CFGR_SW;          // Xóa c?u hình ngu?n clock cu
  RCC->CFGR |= RCC_CFGR_SW_PLL;       // Ch?n PLL làm ngu?n clock h? th?ng
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
      // Ch? d?n khi PLL du?c s? d?ng làm clock h? th?ng
  }
}
int main(void) {
    /* Kh?i t?o h? th?ng clock (72MHz) */
    RCC_Init();

    /* C?u hình GPIO và SPI */
    GPIO_Configuration();
    SPI2_Configuration();

    /* Mount filesystem */
    fr = f_mount(&fs, "", 1); /* 1: Mount ngay l?p t?c */
    if (fr == FR_NO_FILESYSTEM) {
        /* N?u không có h? th?ng t?p, format th? thành FAT32 */
        BYTE work[FF_MAX_SS]; /* Buffer cho f_mkfs */
        MKFS_PARM opt = {
            .fmt = FM_FAT32,    /* Ð?nh d?ng FAT32 */
            .n_fat = 2,         /* S? b?ng FAT (thu?ng là 2) */
            .align = 0,         /* Can ch?nh m?c d?nh */
            .n_root = 0,        /* S? entry thu m?c g?c (0 d? t? d?ng) */
            .au_size = 0        /* Kích thu?c cluster (0 d? t? d?ng) */
        };
        fr = f_mkfs("", &opt, work, sizeof(work));
        if (fr == FR_OK) {
            fr = f_mount(&fs, "", 1); /* Mount l?i sau khi format */
        }
    }

    if (fr != FR_OK) {
        while (1); /* L?i mount/format */
    }

    /* Tang t?c SPI sau khi kh?i t?o thành công */
    SPI2_SetHighSpeed();

    /* T?o và ghi file */
    fr = f_open(&file, "hello.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (fr == FR_OK) {
        const char *text = "Xin chao tu STM32 voi FatFs!\r\n";
        UINT bytesWritten;
        fr = f_write(&file, text, strlen(text), &bytesWritten);
        if (fr == FR_OK) {
            f_close(&file);
        } else {
            while (1); /* L?i ghi file */
        }
    } else {
        while (1); /* L?i m? file */
    }

    /* Ð?c l?i file d? ki?m tra */
    fr = f_open(&file, "hello.txt", FA_READ);
    if (fr == FR_OK) {
        char buffer[50];
        UINT bytesRead;
        fr = f_read(&file, buffer, sizeof(buffer) - 1, &bytesRead);
        if (fr == FR_OK) {
            buffer[bytesRead] = '\0'; /* Thêm ký t? k?t thúc chu?i */
            /* Có th? g?i buffer qua UART d? ki?m tra n?u c?n */
        }
        f_close(&file);
    }

    /* Vòng l?p chính */
    while (1) {
        /* Thêm code ?ng d?ng n?u c?n */
    }
}
