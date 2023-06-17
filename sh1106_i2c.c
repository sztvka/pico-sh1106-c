//
// Created by sztuka on 17.06.2023.
//


#include "sh1106_i2c.h"
uint8_t pageBuffer[8][128];

void SH1106_Init(sh1106_t *sh1106, i2c_inst_t *i2c, uint8_t address, uint8_t width, uint8_t height) {
    sh1106->address = address;
    sh1106->width = width;
    sh1106->height = height;
    sh1106->pages = height / 8;
    sh1106->i2c = i2c;
    sh1106->buffer = (uint8_t **) pageBuffer;
    SH1106_Write_CMD(sh1106, SET_DISP | 0x01);
    for(uint8_t i = 0; i < 8; i++){ //dark screen
        for(uint8_t j = 0; j < 128; j++){
            pageBuffer[i][j] = 0x00;
        }
    }
    SH1106_Write_CMD(sh1106, SET_SEG_REMAP | 0x01); //flip left-right
    SH1106_Write_CMD(sh1106, SET_SCAN_DIR | 0x08);  //flip top-bottom
}

void SH1106_Write_CMD(sh1106_t *sh1106, uint8_t command) {
    uint8_t buffer[2];
    buffer[0] = 0x80;
    buffer[1] = command;
    i2c_write_blocking(sh1106->i2c, sh1106->address, buffer, 2, false);
}
void SH1106_Write_Data(sh1106_t *sh1106, uint8_t* data) {
    size_t bufsize = sh1106->width+1;
    uint8_t broadCastBuffer[bufsize];
    broadCastBuffer[0] = 0x40;
    for (int i = 0; i < sh1106->width; i++) {
        broadCastBuffer[i+1] = data[i];
    }
    i2c_write_blocking(sh1106->i2c, sh1106->address, broadCastBuffer, bufsize, false);
}

void SH1106_draw(sh1106_t *sh1106){
    for(uint8_t page = 0; page < sh1106->pages; page++){
        SH1106_Write_CMD(sh1106, SET_PAGE_ADDR | page);
        SH1106_Write_CMD(sh1106, LOW_COL_ADDR | 0x02);
        SH1106_Write_CMD(sh1106, HIGH_COL_ADDR | 0x00);
        SH1106_Write_Data(sh1106, pageBuffer[page]);
    }
}

void SH1106_drawPixel(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t color){
    if(x > sh1106->width || y > sh1106->height){
        return;
    }
    if(color == 0){
        pageBuffer[y/8][x] &= ~(1 << (y % 8));
    }else{
        pageBuffer[y/8][x] |= (1 << (y % 8));
    }
}

void SH1106_clear(sh1106_t *sh1106){
    for(uint8_t i = 0; i < 8; i++){ //dark screen
        for(uint8_t j = 0; j < 128; j++){
            pageBuffer[i][j] = 0x00;
        }
    }
    SH1106_draw(sh1106);
}