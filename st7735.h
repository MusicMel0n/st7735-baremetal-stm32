#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

#define TFT_RED   0xF800
#define TFT_GREEN 0x07E0
#define TFT_BLUE  0x001F
#define TFT_WHITE 0xFFFF
#define TFT_BLACK 0x0000

void tftInit(void);
void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void fillScreen(uint16_t color);
void drawFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t thickness);
void drawChar(uint16_t x, uint16_t y, char c, uint16_t color);
void drawString(uint16_t x, uint16_t y, const char str[], uint16_t color);

#endif