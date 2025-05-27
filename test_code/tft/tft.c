#include <tft.h>

// Kích thu?c màn hình
volatile uint16_t LCD_HEIGHT = 320;
volatile uint16_t LCD_WIDTH  = 240;

// Hàm kh?i t?o Timer3
void Timer3_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // B?t clock cho TIM3

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // Chia t?n s? 72 MHz xu?ng còn 1 MHz (1 tick = 1 µs)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // Giá tr? t?i da
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM3, ENABLE); // B?t Timer3
}

// Hàm delay theo micro giây (µs)
void Timer3_Delay_us(uint16_t us) {
    TIM3->CNT = 0; // Reset b? d?m
    while (TIM3->CNT < us); // Ð?i d?n khi d?t s? micro giây mong mu?n
}

// Hàm delay theo mili giây (ms)
void Timer3_Delay_ms(uint16_t ms) {
    while (ms--) {
        Timer3_Delay_us(1000); // M?i l?n delay 1000 µs = 1 ms
    }
}

// Hàm SPI t? do?n code d?u tiên
void Spi1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
    
    SPI_Cmd(SPI1, ENABLE);
    
    GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

uint8_t Spi1_SendByte(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

void Spi1_SendBuffer(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        Spi1_SendByte(buffer[i]);
    }
}

// Hàm ILI9341 co b?n
void ILI9341_SPI_Tx(uint8_t data)
{
    Spi1_SendByte(data);
}

void ILI9341_SPI_TxBuffer(uint8_t *buffer, uint16_t len)
{
    Spi1_SendBuffer(buffer, len);
}

void ILI9341_WriteCommand(uint8_t cmd)
{
    ILI9341_DC_LOW();
    ILI9341_CS_LOW();
    ILI9341_SPI_Tx(cmd);
    ILI9341_CS_HIGH();
}

void ILI9341_WriteData(uint8_t data)
{
    ILI9341_DC_HIGH();
    ILI9341_CS_LOW();
    ILI9341_SPI_Tx(data);
    ILI9341_CS_HIGH();
}

void ILI9341_WriteBuffer(uint8_t *buffer, uint16_t len)
{
    ILI9341_DC_HIGH();
    ILI9341_CS_LOW();
    ILI9341_SPI_TxBuffer(buffer, len);
    ILI9341_CS_HIGH();
}

void ILI9341_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t buffer[4];
    buffer[0] = x1 >> 8;
    buffer[1] = x1;
    buffer[2] = x2 >> 8;
    buffer[3] = x2;

    ILI9341_WriteCommand(0x2A);
    ILI9341_WriteBuffer(buffer, sizeof(buffer));

    buffer[0] = y1 >> 8;
    buffer[1] = y1;
    buffer[2] = y2 >> 8;
    buffer[3] = y2;

    ILI9341_WriteCommand(0x2B);
    ILI9341_WriteBuffer(buffer, sizeof(buffer));

    ILI9341_WriteCommand(0x2C);
}

void ILI9341_Reset(void)
{
    ILI9341_RST_LOW();
    Timer3_Delay_ms(10); // 10ms
    ILI9341_CS_LOW();
    Timer3_Delay_ms(10); // 10ms
    ILI9341_RST_HIGH();
    ILI9341_CS_HIGH();
}

void ILI9341_Enable(void)
{
    ILI9341_RST_HIGH();
}

void ILI9341_Init(void)
{
    Timer3_Init(); // Kh?i t?o Timer3 tru?c khi dùng delay
    ILI9341_Enable();
    ILI9341_Reset();

    ILI9341_WriteCommand(0x01); // SOFTWARE RESET
    Timer3_Delay_ms(10); // 10ms

    ILI9341_WriteCommand(0xCB); // POWER CONTROL A
    ILI9341_WriteData(0x39);
    ILI9341_WriteData(0x2C);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x34);
    ILI9341_WriteData(0x02);

    ILI9341_WriteCommand(0xCF); // POWER CONTROL B
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0xC1);
    ILI9341_WriteData(0x30);

    ILI9341_WriteCommand(0xE8); // DRIVER TIMING CONTROL A
    ILI9341_WriteData(0x85);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x78);

    ILI9341_WriteCommand(0xEA); // DRIVER TIMING CONTROL B
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x00);

    ILI9341_WriteCommand(0xED); // POWER ON SEQUENCE CONTROL
    ILI9341_WriteData(0x64);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x12);
    ILI9341_WriteData(0x81);

    ILI9341_WriteCommand(0xF7); // PUMP RATIO CONTROL
    ILI9341_WriteData(0x20);

    ILI9341_WriteCommand(0xC0); // POWER CONTROL, VRH[5:0]
    ILI9341_WriteData(0x23);

    ILI9341_WriteCommand(0xC1); // POWER CONTROL, SAP[2:0]; BT[3:0]
    ILI9341_WriteData(0x10);

    ILI9341_WriteCommand(0xC5); // VCM CONTROL
    ILI9341_WriteData(0x3E);
    ILI9341_WriteData(0x28);

    ILI9341_WriteCommand(0xC7); // VCM CONTROL 2
    ILI9341_WriteData(0x86);

    ILI9341_WriteCommand(0x36); // MEMORY ACCESS CONTROL
    ILI9341_WriteData(0x48);

    ILI9341_WriteCommand(0x3A); // PIXEL FORMAT
    ILI9341_WriteData(0x55); // RGB565

    ILI9341_WriteCommand(0xB1); // FRAME RATIO CONTROL
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x18);

    ILI9341_WriteCommand(0xB6); // DISPLAY FUNCTION CONTROL
    ILI9341_WriteData(0x08);
    ILI9341_WriteData(0x82);
    ILI9341_WriteData(0x27);

    ILI9341_WriteCommand(0xF2); // 3GAMMA FUNCTION DISABLE
    ILI9341_WriteData(0x00);

    ILI9341_WriteCommand(0x26); // GAMMA CURVE SELECTED
    ILI9341_WriteData(0x01);

    ILI9341_WriteCommand(0xE0); // POSITIVE GAMMA CORRECTION
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0x2B);
    ILI9341_WriteData(0x0C);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x08);
    ILI9341_WriteData(0x4E);
    ILI9341_WriteData(0xF1);
    ILI9341_WriteData(0x37);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x10);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x09);
    ILI9341_WriteData(0x00);

    ILI9341_WriteCommand(0xE1); // NEGATIVE GAMMA CORRECTION
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x14);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x11);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0xC1);
    ILI9341_WriteData(0x48);
    ILI9341_WriteData(0x08);
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x0C);
    ILI9341_WriteData(0x31);
    ILI9341_WriteData(0x36);
    ILI9341_WriteData(0x0F);

    ILI9341_WriteCommand(0x11); // EXIT SLEEP
    Timer3_Delay_ms(100); // 100ms

    ILI9341_WriteCommand(0x29); // TURN ON DISPLAY

    ILI9341_SetRotation(SCREEN_VERTICAL_1);
}

void ILI9341_SetRotation(uint8_t rotation)
{
    ILI9341_WriteCommand(0x36);
    Timer3_Delay_ms(1); // 1ms

    switch (rotation)
    {
    case SCREEN_VERTICAL_1:
        ILI9341_WriteData(0x40 | 0x08);
        LCD_WIDTH = 240;
        LCD_HEIGHT = 320;
        break;
    case SCREEN_HORIZONTAL_1:
        ILI9341_WriteData(0x20 | 0x08);
        LCD_WIDTH = 320;
        LCD_HEIGHT = 240;
        break;
    case SCREEN_VERTICAL_2:
        ILI9341_WriteData(0x80 | 0x08);
        LCD_WIDTH = 240;
        LCD_HEIGHT = 320;
        break;
    case SCREEN_HORIZONTAL_2:
        ILI9341_WriteData(0x40 | 0x80 | 0x20 | 0x08);
        LCD_WIDTH = 320;
        LCD_HEIGHT = 240;
        break;
    default:
        break;
    }
}

void ILI9341_DrawColor(uint16_t color)
{
    uint8_t buffer[2] = {color >> 8, color};
    ILI9341_WriteBuffer(buffer, sizeof(buffer));
}

void ILI9341_DrawColorBurst(uint16_t color, uint32_t size)
{
    uint32_t BufferSize = 0;
    const uint32_t BURST_MAX_SIZE = 1024;

    if ((size * 2) < BURST_MAX_SIZE)
    {
        BufferSize = size * 2;
    }
    else
    {
        BufferSize = BURST_MAX_SIZE;
    }

    ILI9341_DC_HIGH();
    ILI9341_CS_LOW();

    uint8_t chifted = color >> 8;
    uint8_t BurstBuffer[BufferSize];

    for (uint32_t j = 0; j < BufferSize; j += 2)
    {
        BurstBuffer[j] = chifted;
        BurstBuffer[j + 1] = color;
    }

    uint32_t SendingSize = size * 2;
    uint32_t SendingInBlock = SendingSize / BufferSize;
    uint32_t RemainderFromBlock = SendingSize % BufferSize;

    if (SendingInBlock != 0)
    {
        for (uint32_t j = 0; j < SendingInBlock; j++)
        {
            ILI9341_SPI_TxBuffer(BurstBuffer, BufferSize);
        }
    }

    if (RemainderFromBlock > 0)
    {
        ILI9341_SPI_TxBuffer(BurstBuffer, RemainderFromBlock);
    }

    ILI9341_CS_HIGH();
}

void ILI9341_FillScreen(uint16_t color)
{
    ILI9341_SetAddress(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    ILI9341_DrawColorBurst(color, LCD_WIDTH * LCD_HEIGHT);
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
        return;

    uint8_t bufferX[4] = {x >> 8, x, (x + 1) >> 8, (x + 1)};
    uint8_t bufferY[4] = {y >> 8, y, (y + 1) >> 8, (y + 1)};
    uint8_t bufferC[2] = {color >> 8, color};

    ILI9341_WriteCommand(0x2A);
    ILI9341_WriteBuffer(bufferX, sizeof(bufferX));

    ILI9341_WriteCommand(0x2B);
    ILI9341_WriteBuffer(bufferY, sizeof(bufferY));

    ILI9341_WriteCommand(0x2C);
    ILI9341_WriteBuffer(bufferC, sizeof(bufferC));
}

void ILI9341_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
        return;

    if ((x + width - 1) >= LCD_WIDTH)
    {
        width = LCD_WIDTH - x;
    }

    if ((y + height - 1) >= LCD_HEIGHT)
    {
        height = LCD_HEIGHT - y;
    }

    ILI9341_SetAddress(x, y, x + width - 1, y + height - 1);
    ILI9341_DrawColorBurst(color, height * width);
}

void ILI9341_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color)
{
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
        return;

    if ((x + width - 1) >= LCD_WIDTH)
    {
        width = LCD_WIDTH - x;
    }

    ILI9341_SetAddress(x, y, x + width - 1, y);
    ILI9341_DrawColorBurst(color, width);
}

void ILI9341_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color)
{
    if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
        return;

    if ((y + height - 1) >= LCD_HEIGHT)
    {
        height = LCD_HEIGHT - y;
    }

    ILI9341_SetAddress(x, y, x, y + height - 1);
    ILI9341_DrawColorBurst(color, height);
}

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        ILI9341_DrawPixel(X + x, Y + y, color);
        ILI9341_DrawPixel(X + y, Y + x, color);
        ILI9341_DrawPixel(X - y, Y + x, color);
        ILI9341_DrawPixel(X - x, Y + y, color);
        ILI9341_DrawPixel(X - x, Y - y, color);
        ILI9341_DrawPixel(X - y, Y - x, color);
        ILI9341_DrawPixel(X + y, Y - x, color);
        ILI9341_DrawPixel(X + x, Y - y, color);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }
}

void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color)
{
    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = X - x; i <= X + x; i++)
        {
            ILI9341_DrawPixel(i, Y + y, color);
            ILI9341_DrawPixel(i, Y - y, color);
        }

        for (int i = X - y; i <= X + y; i++)
        {
            ILI9341_DrawPixel(i, Y + x, color);
            ILI9341_DrawPixel(i, Y - x, color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;

        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
    uint16_t xLen = 0;
    uint16_t yLen = 0;
    uint8_t negX = 0;
    uint8_t negY = 0;
    float negCalc = 0;

    negCalc = X1 - X0;
    if (negCalc < 0) negX = 1;
    negCalc = 0;

    negCalc = Y1 - Y0;
    if (negCalc < 0) negY = 1;

    if (!negX)
    {
        xLen = X1 - X0;
    }
    else
    {
        xLen = X0 - X1;
    }
    ILI9341_DrawHLine(X0, Y0, xLen, color);
    ILI9341_DrawHLine(X0, Y1, xLen, color);

    if (!negY)
    {
        yLen = Y1 - Y0;
    }
    else
    {
        yLen = Y0 - Y1;
    }

    ILI9341_DrawVLine(X0, Y0, yLen, color);
    ILI9341_DrawVLine(X1, Y0, yLen, color);

    if ((xLen > 0) || (yLen > 0))
    {
        ILI9341_DrawPixel(X1, Y1, color);
    }
}

void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color)
{
    uint16_t xLen = 0;
    uint16_t yLen = 0;
    uint8_t negX = 0;
    uint8_t negY = 0;
    int32_t negCalc = 0;
    uint16_t X0True = 0;
    uint16_t Y0True = 0;

    negCalc = X1 - X0;
    if (negCalc < 0) negX = 1;
    negCalc = 0;

    negCalc = Y1 - Y0;
    if (negCalc < 0) negY = 1;

    if (!negX)
    {
        xLen = X1 - X0;
        X0True = X0;
    }
    else
    {
        xLen = X0 - X1;
        X0True = X1;
    }

    if (!negY)
    {
        yLen = Y1 - Y0;
        Y0True = Y0;
    }
    else
    {
        yLen = Y0 - Y1;
        Y0True = Y1;
    }

    ILI9341_DrawRectangle(X0True, Y0True, xLen, yLen, color);
}

void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
    if ((ch < 31) || (ch > 127)) return;

    uint8_t fOffset, fWidth, fHeight, fBPL;
    uint8_t *tempChar;

    fOffset = font[0];
    fWidth = font[1];
    fHeight = font[2];
    fBPL = font[3];

    tempChar = (uint8_t*)&font[((ch - 0x20) * fOffset) + 4];

    ILI9341_DrawRectangle(X, Y, fWidth, fHeight, bgcolor);

    for (int j = 0; j < fHeight; j++)
    {
        for (int i = 0; i < fWidth; i++)
        {
            uint8_t z = tempChar[fBPL * i + ((j & 0xF8) >> 3) + 1];
            uint8_t b = 1 << (j & 0x07);
            if ((z & b) != 0x00)
            {
                ILI9341_DrawPixel(X + i, Y + j, color);
            }
        }
    }
}

void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
{
    uint8_t charWidth;
    uint8_t fOffset = font[0];
    uint8_t fWidth = font[1];

    while (*str)
    {
        ILI9341_DrawChar(*str, font, X, Y, color, bgcolor);

        uint8_t *tempChar = (uint8_t*)&font[((*str - 0x20) * fOffset) + 4];
        charWidth = tempChar[0];

        if (charWidth + 2 < fWidth)
        {
            X += (charWidth + 2);
        }
        else
        {
            X += fWidth;
        }

        str++;
    }
}

void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation)
{
    if (orientation == HORIZONTAL_IMAGE)
    {
        ILI9341_SetRotation(SCREEN_HORIZONTAL_1);
        ILI9341_SetAddress(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    }
    else if (orientation == VERTICAL_IMAGE)
    {
        ILI9341_SetRotation(SCREEN_VERTICAL_1);
        ILI9341_SetAddress(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    }

    uint32_t counter = 0;
    const uint32_t BURST_MAX_SIZE = 1024;
    uint32_t totalBytes = LCD_WIDTH * LCD_HEIGHT * 2;

    for (uint32_t i = 0; i < totalBytes / BURST_MAX_SIZE; i++)
    {
        ILI9341_WriteBuffer((uint8_t*)(image + counter), BURST_MAX_SIZE);
        counter += BURST_MAX_SIZE;
        Timer3_Delay_us(1); // 1us delay
    }

    uint32_t remainder = totalBytes % BURST_MAX_SIZE;
    if (remainder > 0)
    {
        ILI9341_WriteBuffer((uint8_t*)(image + counter), remainder);
    }
}