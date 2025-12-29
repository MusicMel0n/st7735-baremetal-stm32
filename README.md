# st7735-baremetal-stm32
Bare-metal SPI driver for ST7735 TFT displays on STM32

## Usage

### Required Includes
```c
#include "st7735.h"
```

### Initialization

Before using any display or drawing functions, the display must be initialized:

```c
tftInit();
```

This must be called once on startup

### Drawing Functions

```c
drawPixel(x, y, color);
```

Draws a single pixel at the specified screen coordinates

```c
fillScreen(color);
```

Fills the entire display with a single colour

```c
void drawFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
```

Draws a filled rectangle starting at ```(x, y)``` with the given width and height.

```c
drawRectangle(x, y, w, h, color, thickness);
```

Draws a rectangle outline with a specified border thickness.

## Compatibility

Tested On:
- STM32F303K8 (Nucleo-F303K8)
- ST7735S 128x160 SPI TFT

## Font Usage

This project uses a bitmap font stored in a C header file

The display driver ('st7735.c') expects a font header called 'font16.h'

### Font Compatibility

The script works best with **monospaced fonts.** Proportional fonts may appear overly spaced as variable glpyh widths are not currently handled

## Font Generator Script (font/convertFont.py)

A python script is provided to convert a '.bdf' bitmap font into a C header compatible with this project

You can convert any font to this format easily using tools such as fontForge

### Usage
```bash
python convertFont.py inputFont.bdf font16.h
```
