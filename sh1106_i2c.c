//
// Created by sztuka on 17.06.2023.
//


#include "sh1106_i2c.h"

uint8_t pageBuffer[8][128];
const uint8_t bytes_per_char = 16 * (9 / 8 + ((9 % 8) ? 1 : 0));
#define FONT_HEIGHT 16
#define FONT_WIDTH 8

inline static void swap(uint8_t *a, uint8_t *b) {
    uint8_t *t=a;
    *a=*b;
    *b=*t;
};

int getOffset(char c){
// offset = (ascii_code(character) - ascii_code(' ')) * bytes_per_char
    return (int)(((int)c - 32) * bytes_per_char);
};

void SH1106_init(sh1106_t *sh1106, i2c_inst_t *i2c, uint8_t address, uint8_t width, uint8_t height) {
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

void SH1106_draw_hline(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t w, uint8_t color){
    if(x > sh1106->width || y > sh1106->height){
        return;
    }
    if((x + w) > sh1106->width){
        w = sh1106->width - x;
    }
    for(uint8_t i = 0; i < w; i++){
        SH1106_drawPixel(sh1106, x + i, y, color);
    }
};


void SH1106_clear(sh1106_t *sh1106){
    for(uint8_t i = 0; i < 8; i++){ //dark screen
        for(uint8_t j = 0; j < 128; j++){
            pageBuffer[i][j] = 0x00;
        }
    }
};


void SH1106_drawRectangle(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    for(uint8_t i=0; i<width; ++i)
        for(uint8_t j=0; j<height; ++j)
            SH1106_drawPixel(sh1106, x+i, y+j, color);

};

/*
void SH1106_drawChar(sh1106_t *sh1106, uint8_t x, uint8_t y, uint8_t scale, const uint8_t *font, char c, uint8_t color) {
    if(c<font[3]||c>font[4])
        return;

    uint32_t parts_per_line=(font[0]>>3)+((font[0]&7)>0);
    for(uint8_t w=0; w<font[1]; ++w) { // width
        uint32_t pp=(c-font[3])*font[1]*parts_per_line+w*parts_per_line+5;
        for(uint32_t lp=0; lp<parts_per_line; ++lp) {
            uint8_t line=font[pp];

            for(int8_t j=0; j<8; ++j, line>>=1) {
                if(line & 1)
                    SH1106_drawRectangle(sh1106, x+w*scale, y+((lp<<3)+j)*scale, scale, scale, color);
            }

            ++pp;
        }
    }
}

*/
void SH1106_drawChar(sh1106_t * sh1106, char c, uint8_t x, uint8_t y, uint8_t color, const uint8_t* font) {
    uint8_t i,j;
    uint32_t index;
    if (c == ' '){
        index = 0;
    }
    else if ( c < '!' || c > '~'){
        index = 0;
    }
    else{
        index = (c - 32) * 16; // -32 ascii {!} * 16 = bytes per char
    }
   for (i = 0; i < FONT_HEIGHT; i++){
       for(j = 0; j < FONT_WIDTH; j++){
           if(x+8-j > sh1106->width){
               return;
           }
           if(y+i > sh1106->height){
               return;
           }
           if (font[index+i] & (1 << j)){
               SH1106_drawPixel(sh1106, x+8-j, y + i, color);
           }
           else{
                SH1106_drawPixel(sh1106, x+8-j, y + i, !color);
           }
       }

   }
   if(color==0){
       SH1106_drawRectangle(sh1106, x, y, (FONT_WIDTH+1), 2, 0);
   }
}

void SH1106_drawString(sh1106_t *sh1106, char* str, uint8_t x, uint8_t y, uint8_t color, const uint8_t* font){
    uint8_t i = 0;
    while(str[i] != '\0'){
        if(x + i*(FONT_WIDTH+1) > sh1106->width){
            return;
        }
        SH1106_drawChar(sh1106, str[i], x + i*(FONT_WIDTH+1), y, color, font);

        if(color==0){ //fill black gaps between letters if text is inverted
            for(uint8_t j = 2; j < (FONT_HEIGHT); j++){
                SH1106_drawPixel(sh1106, x + i*9, y + j, 1);
            };
        }
        i++;
    }
}