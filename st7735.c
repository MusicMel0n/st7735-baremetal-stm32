#include <stm32f303x8.h>
#include "st7735.h"
#include "gpio.h"
#include "font16.h"

#define X_OFFSET 2
#define Y_OFFSET 1

void displayReset(void);
void sendByte(uint8_t data, uint8_t mode);
void sendRawByte(uint8_t data);
void displayReset(void);
void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void tftInit(void){
    RCC->AHBENR |= ((1 << 17) | (1 << 18));
    RCC -> APB2ENR |= (1 << 12);

    //Sets PA1 as output for RST
    pinMode(GPIOA, 1, 1);

    //Sets PA5 and PA7 to alernate function mode (SPI CLOCK (Tells screen when to read a bit) and SPI MOSI (Carries the data for the screen))
    pinMode(GPIOA, 5, 2); 
    pinMode(GPIOA, 7, 2);

    GPIOA->AFR[0] &= ~((0xF << 20) | (0xF << 28)); //Clears Alternate Function Register for PA5 and PA7
    GPIOA->AFR[0] |= ((5 << 20) | (5 << 28)); //Sets Alternate Function Register to 0 (Mode for SPI1) for PA5 and PA7

    pinMode(GPIOA, 3, 1); //DC (Data/Command - Low = Command | High = data) Output
    pinMode(GPIOA, 4, 1); //CS (Chip Select - When Low Listen SPI, When High Ignore SPI) Output

    GPIOA->ODR |= (1 << 4); //Set CS to High on start to ignore data incase there is noise/random data

    SPI1->CR1 = 0; //Clears config for SPI
    SPI1->CR1 |= (1 << 2); //Sets the board as the Master
    SPI1->CR1 |= (1 << 9); //Ignore NSS Pin so we can manually control CS
    SPI1->CR1 |= (1 << 8); //Internal NSS is treated as high so we avoid faults
    SPI1->CR1 |= (0 << 3); //Sets the baud rate
    SPI1->CR1 |= (1 << 6); //Turns SPI on

    displayReset();

    sendByte(0x11, 0);   // Sleep out
    delay(120000);

    /* Pixel format */
    sendByte(0x3A, 0);
    sendByte(0x05, 1);

    /* Memory access control */
    sendByte(0x36, 0);
    sendByte(0xC0, 1);  // IMPORTANT for this panel

    /* Display on */
    sendByte(0x29, 0);
    delay(20000);
}

void sendByte(uint8_t data, uint8_t mode){
    if(mode == 0){ //Command
        GPIOA->ODR &= ~(1 << 3);
    }
    else if(mode == 1){ //Data
        GPIOA->ODR |= (1 << 3);
    }

    GPIOA->ODR &= ~(1 << 4); //Listen to SPI

    while((SPI1->SR & (1 << 1)) == 0); //Wait until TXE is 1 meaning the transmit buffer is empty and we can send a new byte
    *(volatile uint8_t*)&SPI1->DR = data; //Writes the byte to the Data Register but treats it as a pointer to an 8bit location as DR is 16bits
    while((SPI1->SR & (1 << 7)) != 0); //Wait until SPI is not busy

    GPIOA->ODR |= (1 << 4); //Stop Listening to SPI
}

void sendRawByte(uint8_t data){
    while((SPI1->SR & (1 << 1)) == 0); //Wait until TXE is 1 meaning the transmit buffer is empty and we can send a new byte
    *(volatile uint8_t*)&SPI1->DR = data; //Writes the byte to the Data Register but treats it as a pointer to an 8bit location as DR is 16bits
}

void displayReset(void){
    GPIOA->ODR &= ~(1 << 1); // RES = 0
    delay(50000);

    GPIOA->ODR |= (1 << 1);  // RES = 1
    delay(200000);
}

//This function tells the screen to only draw in this rectangle and when we send colours
//to draw them here left-to-right, top-to-bottom
void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    x0 += X_OFFSET;
    x1 += X_OFFSET;
    y0 += Y_OFFSET;
    y1 += Y_OFFSET;

    sendByte(0x2A, 0); //CASET - Column Address Set
    sendByte(0x00, 1); //Send 0 before coordinate as expecting 2 bytes
    sendByte(x0, 1);
    sendByte(0x00, 1);
    sendByte(x1, 1);

    sendByte(0x2B, 0); //RASET - Row Address Set
    sendByte(0x00, 1); //Send 0 before coordinate as expecting 2 bytes
    sendByte(y0, 1);
    sendByte(0x00, 1);
    sendByte(y1, 1);

    sendByte(0x2C, 0); // RAMWR - Write to display RAM | now expecting colour data to be sent
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color){
    setAddressWindow(x, y, x, y);

    sendByte(color >> 8, 1);
    sendByte(color & 0xFF, 1);
}

void fillScreen(uint16_t color)
{
    setAddressWindow(0, 0, 127, 159);

    GPIOA->ODR |= (1 << 3); //Data
    GPIOA->ODR &= ~(1 << 4); //Listen to SPI

    uint8_t hi = color >> 8; //First 8 bits of colour
    uint8_t low = color & 0xFF; //Last 8 bits of color

    for(uint32_t i = 0; i < (128 * 160); i++)
    {
        sendRawByte(hi);
        sendRawByte(low);
    }

    while((SPI1->SR & (1 << 7)) != 0); //Wait until SPI is not busy
    GPIOA->ODR |= (1 << 4); //Stop Listening to SPI
}

void drawFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    setAddressWindow(x, y, x + w - 1, y + h - 1);

    GPIOA->ODR |= (1 << 3); //Data
    GPIOA->ODR &= ~(1 << 4); //Listen to SPI

    uint8_t hi = color >> 8; //First 8 bits of colour
    uint8_t low = color & 0xFF; //Last 8 bits of color

    for(uint32_t i = 0; i < (w * h); i++){
        sendRawByte(hi);
        sendRawByte(low);
    }

    while((SPI1->SR & (1 << 7)) != 0); //Wait until SPI is not busy
    GPIOA->ODR |= (1 << 4); //Stop Listening to SPI
}

void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t thickness){
    //top edge
    drawFillRectangle(x, y, w, thickness, color);

    //left edge
    drawFillRectangle(x, y, thickness, h, color);

    //right edge
    drawFillRectangle(x + w - thickness, y, thickness, h, color);

    //bottom edge
    drawFillRectangle(x, y + h - thickness, w, thickness, color);
}

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color){
    if (c > 127 || c < 32){ //As the font only ranges from ASCII 32 ' ' to ASCII 127 DEL
        return;
    }

    const uint16_t *glyph = font[c - 32]; //Holds the address to the row containing the hex values for the character

    //The CPU stores each hex value as binary so we can iterate through each of the hex values and draw a pixel where there is a 1
    for(int row = 0; row < FONT_HEIGHT; row++){ //Iterates through each value for the glyph
        uint16_t bits = glyph[row]; //Binary bitmask
        for(int col = 0; col < FONT_WIDTH; col++){ //Iterates through each bit
            if(bits & (1 << (FONT_WIDTH - 1 - col))){ //Check if the bit is on | (FONT_WIDTH - 1 - col) so we start on bit 10 from the right (Always ignore first 5) | these values depend on the font
                drawPixel(x + col, y + row, color); //Draw pixel and shift it right depending on which col we are on and down by the row
            }
        }
    }
}

void drawString(uint16_t x, uint16_t y, const char str[], uint16_t color){
    uint8_t index = 0;

    while(str[index] != '\0'){
        drawChar(x, y, str[index], color);
        x += FONT_WIDTH;
        index++;
    }
}
