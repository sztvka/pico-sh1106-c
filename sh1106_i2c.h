//
// Created by sztuka on 17.06.2023.
//

#ifndef PI_PICO_SH1106_SH1106_I2C_H
#define PI_PICO_SH1106_SH1106_I2C_H


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/malloc.h"
#include <malloc.h>
#include "hardware/i2c.h"


#define SET_DISP 0xAE
#define SET_SCAN_DIR 0xC0
#define SET_SEG_REMAP 0xA0
#define LOW_COL_ADDR 0x00
#define HIGH_COL_ADDR 0x10
#define SET_PAGE_ADDR 0xB0

typedef struct sh1106 {
    uint8_t address;
    uint8_t width;
    uint8_t height;
    uint8_t pages;
    uint8_t **buffer;
    i2c_inst_t *i2c;
} sh1106_t;
void SH1106_Write_Data(sh1106_t *sh1106, uint8_t* data);
void SH1106_Write_CMD(sh1106_t *sh1106, uint8_t command);
void SH1106_init(sh1106_t *sh1106, i2c_inst_t *i2c, uint8_t address, uint8_t width, uint8_t height);
void SH1106_draw(sh1106_t *sh1106);
void SH1106_drawPixel(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t color);
void SH1106_draw_hline(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t w, uint8_t color);
void SH1106_clear(sh1106_t *sh1106);
void SH1106_drawRectangle(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void SH1106_drawChar(sh1106_t * sh1106, char c, uint8_t x, uint8_t y, uint8_t color, const uint8_t* font);
void SH1106_drawString(sh1106_t *sh1106, char* str, uint8_t x, uint8_t y, uint8_t color, const uint8_t* font);
#endif //PI_PICO_SH1106_SH1106_I2C_H
