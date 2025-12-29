# st7735-baremetal-stm32
Bare-metal SPI driver for ST7735 TFT displays on STM32

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
