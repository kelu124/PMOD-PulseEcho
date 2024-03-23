//!
//! \file       adc.c
//! \author     Abdelrahman Ali
//! \date       2024-01-20
//!
//! \brief      adc dac pio.
//!

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

#include "adc.pio.h"

//---------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------

#define PIN_BASE 0
#define SAMPLE_COUNT 16000
#define DMA_CHANNEL 0

#define PIN_MOSI 15
#define PIN_CS 13
#define PIN_SCLK 14

#define PIN_A 16 // Damp
#define OE 17 // OE

#define PIN_C 18 // OE

//---------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------

PIO pio;
uint sm;
uint offset;
dma_channel_config dma_chan_cfg;
uint32_t buffer[SAMPLE_COUNT];
uint32_t input;
uint32_t data;
uint32_t trigger;

uint32_t pinPositions[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

uint32_t newPositions[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//---------------------------------------------------------------------------
// EXTRA PIN INIT
//---------------------------------------------------------------------------

void extra_pin_init()
{
    gpio_init(PIN_A); 
    gpio_init(PIN_C);
    gpio_init(OE);
    gpio_set_dir(PIN_A, GPIO_OUT); 
    gpio_set_dir(PIN_C, GPIO_OUT);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_put(PIN_A, 0); 
    gpio_put(PIN_C, 0);
    gpio_put(OE, 1);
}

//---------------------------------------------------------------------------
// PIN C THREAD ENTRY
//---------------------------------------------------------------------------

void thread_entry_point()
{
    while (1)
    {
        gpio_put(PIN_C, 1);
        sleep_us(500000);
        gpio_put(PIN_C, 0);
        sleep_us(500000);
    }
}

//---------------------------------------------------------------------------
// EXTRA PIN OUTPUT
//---------------------------------------------------------------------------

void extra_pin_output()
{
    gpio_put(PIN_A, 1);
    sleep_us(10);
    gpio_put(PIN_A, 0);
}

//---------------------------------------------------------------------------
// DAC INIT FUNCTION
//---------------------------------------------------------------------------

void dac_init()
{
    gpio_init(PIN_MOSI);
    gpio_init(PIN_CS);
    gpio_init(PIN_SCLK);
    gpio_set_dir(PIN_MOSI, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_SCLK, GPIO_OUT);
    gpio_put(PIN_MOSI, 0);
    gpio_put(PIN_CS, 1);
    gpio_put(PIN_SCLK, 0);
}

//---------------------------------------------------------------------------
// DAC DATA PRESENTAGE CALCULATION
//---------------------------------------------------------------------------

void dac_data_calculation(uint32_t *data, uint32_t input, uint32_t config_bits)
{
    *data = 0;
    if (input == 0)
    {
        *data = ((*data << 2) | config_bits);
    }
    else if (input == 1)
    {
        *data = (uint32_t)(38);
        *data = ((*data << 2) | config_bits);
    }
    else if (input == 2)
    {
        *data = (uint32_t)(76);
        *data = ((*data << 2) | config_bits);
    }
    else if (input == 3)
    {
        *data = (uint32_t)(115);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 4)
    {
        *data = (uint32_t)(153);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 5)
    {
        *data = (uint32_t)(191);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 6)
    {
        *data = (uint32_t)(230);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 7)
    {
        *data = (uint32_t)(268);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 8)
    {
        *data = (uint32_t)(306);

        *data = ((*data << 2) | config_bits);
    }
    else if (input == 9)
    {
        *data = (uint32_t)(358);

        *data = ((*data << 2) | config_bits);
    }
}

//---------------------------------------------------------------------------
// DAC SPI WRITE FUNCTION
//---------------------------------------------------------------------------

void dac_spi_write(uint32_t data)
{
    for (int32_t bit = 15; bit >= 0; --bit)
    {
        gpio_put(PIN_MOSI, (data >> bit) & 1);
        sleep_us(1);
        gpio_put(PIN_SCLK, 1);
        sleep_us(1);
        gpio_put(PIN_SCLK, 0);
        sleep_us(1);
    }
}

//---------------------------------------------------------------------------
// DAC WRITE FUNCTION
//---------------------------------------------------------------------------

void dac_write(uint32_t data)
{
    gpio_put(PIN_CS, 0);
    dac_spi_write(data);
    gpio_put(PIN_CS, 1);
}

//---------------------------------------------------------------------------
// DAC MAIN FUNCTION
//---------------------------------------------------------------------------

void dac()
{
    while (true)
    {
        printf("Enter a number between 0 and 9: ");
        input = getchar();
        printf("%d\n", input - '0');
        if (input >= '0' && input <= '9')
        {
            input = input - '0';
            dac_data_calculation(&data, input, 0x3000);
            break;
        }
    }

    dac_write(data);
}

//---------------------------------------------------------------------------
// ADC INIT FUNCTION
//---------------------------------------------------------------------------

void pio_adc_init()
{
    pio = pio0;
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &adc_program);
    adc_program_init(pio, sm, offset, PIN_BASE);
    dma_chan_cfg = dma_channel_get_default_config(DMA_CHANNEL);
    channel_config_set_read_increment(&dma_chan_cfg, false);
    channel_config_set_write_increment(&dma_chan_cfg, true);
    channel_config_set_dreq(&dma_chan_cfg, pio_get_dreq(pio, sm, false));
}

//---------------------------------------------------------------------------
// ADC DMA FUNCTION
//---------------------------------------------------------------------------

void pio_adc_dma()
{
    dma_channel_configure(DMA_CHANNEL, &dma_chan_cfg, buffer, &pio->rxf[sm], SAMPLE_COUNT, true);

    dma_channel_wait_for_finish_blocking(DMA_CHANNEL);
}

//---------------------------------------------------------------------------
// ADC CLEAN FIFO FUNCTION
//---------------------------------------------------------------------------

void pio_adc_clear_fifos()
{
    pio_sm_clear_fifos(pio, sm);
}

//---------------------------------------------------------------------------
// ADC MAP PINS FUNCTION
//---------------------------------------------------------------------------

void pio_map_non_consecutive_pins(uint32_t *buffer)
{
    for (uint32_t y = 0; y < SAMPLE_COUNT; ++y)
    {
        uint32_t targetPinMapping = 0;

        for (uint32_t i = 0; i < 10; ++i)
        {

            uint32_t pinValue = (buffer[y] >> pinPositions[i]) & 1;

            targetPinMapping |= (pinValue << newPositions[i]);
        }

        buffer[y] = targetPinMapping;
    }
}

//---------------------------------------------------------------------------
// ADC MAIN FUNCTION
//---------------------------------------------------------------------------

void adc()
{
    while (true)
    {
        printf("Enter a triggering number: ");
        trigger = getchar();
        printf("%d\n", trigger - '0');
        if (trigger == '1')
        {
            break;
        }
    }

    pio_adc_clear_fifos();
    extra_pin_output();
    pio_adc_dma();
    pio_map_non_consecutive_pins(buffer);

    // printf("----------Start of ACQ----------\n");

    for (uint32_t i = 0; i < SAMPLE_COUNT; ++i)
    {
        printf("%X,", buffer[i]);
    }

    // printf("\n-----------End of ACQ-----------\n");
}

//---------------------------------------------------------------------------
// MAIN FUNCTION
//---------------------------------------------------------------------------

void main()
{
    stdio_init_all();
    dac_init();
    pio_adc_init();
    extra_pin_init();

    multicore_launch_core1(thread_entry_point);
    while (true)
    {
        dac();
        sleep_us(10);
        adc();
    }
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
