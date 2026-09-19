#include <pico/error.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "hardware/clocks.h"
#include "hardware/gpio.h"

//pinovi na LMT su 1 => 1 , 2 => 10, 3 =>100, 4=>1000
#define ENABLE_DEC 24
#define A0 25
#define A1 26
#define A2 27

int output_range = 1;
void init(){
gpio_init(ENABLE_DEC);
gpio_set_dir(ENABLE_DEC,GPIO_OUT);
gpio_put(ENABLE_DEC,1);
gpio_init(A0);
gpio_set_dir(A0,GPIO_OUT);
gpio_put(A0, 0);
gpio_init(A1);
gpio_set_dir(A1,GPIO_OUT);
gpio_put(A1, 0);
gpio_init(A2);
gpio_set_dir(A2,GPIO_OUT);
gpio_put(A1, 0);
}
void select_range(int index){
    gpio_put(A0, index&0x01);
    gpio_put(A1, (index&0x01) >> 1);
    gpio_put(A2, (index&0x01)>>2); //pomeranje bita u poziciju za dekoder
}
int main()
{
    stdio_init_all();sleep_ms(1000);
    while(!stdio_usb_connected()) sleep_ms(1000); //spavaj ako nisi povezan za manju potrosnju struje


    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));

    char buf[32];
    int idx=0;
    //pin setup 
    init();


    while (true) {
        //kod za prikupljanje podataka sa desktop racunara 
        int c = getchar_timeout_us(0);
        if(c != PICO_ERROR_TIMEOUT){
            if(c == '\n' || c == '\r'){
                buf[idx] = '\0';
                if(idx > 0){
                    if((strcmp(buf,"UP")==0) && (output_range<=100)){
                        output_range*=10; //mnozi sa 10 samo ako je range 1,10 ili 100
                    }else if((strcmp(buf,"DOWN")==0) && (output_range >=10)){
                        output_range/=10; //deli sa deset samo ako je range 10 100 ili 1000
                    }

                }
                idx=0;
            }else if(idx<sizeof(buf)+1){
                buf[idx++] =(char)c;
            }
        }
        //kod za kontrolisanje dekodera 
        //citamo output_range i biramo ga
        switch (output_range) {
            case 1: select_range(1); break;
            case 10: select_range(2); break;
            case 100: select_range(3); break;
            case 1000: select_range(4); break;
            default:select_range(0); break;
        }
  
    }
}
