//!
//! \file       main.c
//! \author     Abdelrahman Ali
//! \date       2024-01-20
//!
//! \brief      adc dac vga main entry.
//!

//---------------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------------
#include "dac/dac.h"
#include "adc/adc.h"
#include "vga/vga.h"

#define PonP 11
#define PonN 12
#define Pdamp 16
#define OE 17
#define PINA 21
#define PINB 28

//---------------------------------------------------------------------------
// EXTRA PIN INIT
//---------------------------------------------------------------------------

void extra_pin_init()
{
    gpio_init(PonP);
    gpio_init(PonN);
    gpio_init(Pdamp);
    gpio_init(OE);
    gpio_init(PINA);
    gpio_init(PINB);
    gpio_set_dir(PonP, GPIO_OUT);
    gpio_set_dir(PonN, GPIO_OUT);
    gpio_set_dir(Pdamp, GPIO_OUT);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_set_dir(PINA, GPIO_OUT);
    gpio_set_dir(PINB, GPIO_OUT);
    gpio_put(PonP, 0);
    gpio_put(PonN, 0);
    gpio_put(Pdamp, 0);
    gpio_put(OE, 1);
    gpio_put(PINA, 0);
    gpio_put(PINB, 0);
}

//---------------------------------------------------------------------------
// PIN C THREAD ENTRY
//---------------------------------------------------------------------------

void thread_entry_point()
{
    while (1)
    {
        gpio_put(PINA, 1);
        gpio_put(PINB, 1);
        sleep_us(1000);
        gpio_put(PINA, 0);
        gpio_put(PINB, 0);
        sleep_us(1000);
    }
}

//---------------------------------------------------------------------------
// EXTRA PIN OUTPUT
//---------------------------------------------------------------------------

void extra_pin_output()
{
    gpio_put(PonP, 1);
    sleep_us(1); //pulse
    gpio_put(PonP, 0);
    gpio_put(Pdamp, 1);
    sleep_us(5); // Damp
    gpio_put(Pdamp, 0);
}


//---------------------------------------------------------------------------
// MAIN FUNCTION
//---------------------------------------------------------------------------

int main()
{
    stdio_init_all();
    dac_init();
    pio_adc_init();
    pio_vga_init();
    extra_pin_init();
    
    multicore_launch_core1(thread_entry_point);
    while (true)
    {
        dac();
        sleep_us(10);
        vga(adc());
        sleep_ms(10);
    }
}
