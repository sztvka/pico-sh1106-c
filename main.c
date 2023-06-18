#include "sh1106_i2c.h"
#include "font_inconsolata.h"



void setup_i2c(){
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
}


int main() {

    stdio_init_all();
    setup_i2c();
    sh1106_t mydisp;
    SH1106_init(&mydisp, i2c1, 0x3C, 128, 64);
    SH1106_drawString(&mydisp, "T: 20.0 ms", 0, 0, 1, inconsolata);
    SH1106_drawString(&mydisp, "Ton: 10.0 ms", 0, 16, 1, inconsolata);
    SH1106_drawString(&mydisp, "OFF", 0, 32, 1, inconsolata);
    SH1106_drawString(&mydisp, "Analog", 0, 48, 0, inconsolata);
    SH1106_draw(&mydisp);


    return 0;
}
