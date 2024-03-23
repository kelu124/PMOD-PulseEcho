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

#include "adc.h"

#include "adc.pio.h"

//---------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------

PIO pio_adc;
uint sm;
uint offset;
uint dma_chan;
dma_channel_config dma_chan_cfg;
uint16_t buffer[SAMPLE_COUNT];
uint8_t trigger;

uint8_t pinPositions[] = {1, 2, 3, 4, 5, 6, 7, 8, 9,10};

uint8_t newPositions[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//---------------------------------------------------------------------------
// ADC INIT FUNCTION
//---------------------------------------------------------------------------

void pio_adc_init()
{
    pio_adc = pio0;
    sm = pio_claim_unused_sm(pio_adc, true);
    offset = pio_add_program(pio_adc, &adc_program);
    adc_program_init(pio_adc, sm, offset, PIN_BASE, ADC_CLK);
    dma_chan = dma_claim_unused_channel(true);
    dma_chan_cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&dma_chan_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_chan_cfg, false);
    channel_config_set_write_increment(&dma_chan_cfg, true);
    channel_config_set_dreq(&dma_chan_cfg, pio_get_dreq(pio_adc, sm, false));
}

//---------------------------------------------------------------------------
// ADC DMA FUNCTION
//---------------------------------------------------------------------------

void pio_adc_dma()
{
    dma_channel_configure(dma_chan, &dma_chan_cfg, buffer, &pio_adc->rxf[sm], SAMPLE_COUNT, true);

    dma_channel_wait_for_finish_blocking(dma_chan);
}

//---------------------------------------------------------------------------
// ADC CLEAN FIFO FUNCTION
//---------------------------------------------------------------------------

void pio_adc_clear_fifos()
{
    pio_sm_clear_fifos(pio_adc, sm);
}

//---------------------------------------------------------------------------
// ADC MAP PINS FUNCTION
//---------------------------------------------------------------------------

void pio_map_non_consecutive_pins(uint16_t *buffer_ptr)
{
    for (uint16_t y = 0; y < SAMPLE_COUNT; ++y)
    {
        uint16_t targetPinMapping = 0;

        for (uint16_t i = 0; i < 10; ++i)
        {

            uint16_t pinValue = (buffer_ptr[y] >> pinPositions[i]) & 1;

            targetPinMapping |= (pinValue << newPositions[i]);
        }

        buffer_ptr[y] = targetPinMapping;
    }
}

//---------------------------------------------------------------------------
// ADC MAIN FUNCTION
//---------------------------------------------------------------------------

uint16_t *adc()
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
    pio_adc_dma();
    pio_map_non_consecutive_pins(buffer);

    printf("----------Start of ACQ----------\n");

    for (uint16_t i = 0; i < SAMPLE_COUNT; ++i)
    {
        printf("%X,", buffer[i]);
    }

    printf("\n-----------End of ACQ-----------\n");

    return buffer;
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
