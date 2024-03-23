//!
//! \file       vga.c
//! \author     Abdelrahman Ali
//! \date       2024-01-31
//!
//! \brief      vga pio.
//!

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------

#include "vga.h"

//---------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------
uint16_t max_signal[DECIMATED_SIGNAL];

//---------------------------------------------------------------------------
// VGA DRAW CONTAINER FUNCTION
//---------------------------------------------------------------------------
void pio_vga_draw_container()
{
    pio_vga_draw_v_line(110, 20, 440, GREEN);
    pio_vga_draw_v_line(530, 20, 440, GREEN);
    pio_vga_draw_h_line(110, 20, 40, GREEN);
    pio_vga_set_text_color(GREEN);
    pio_vga_set_cursor(155, 20);
    pio_vga_set_text_size(1);
    pio_vga_write_string("ACQUISITION");
    pio_vga_draw_v_line(120, 460, 5, GREEN);
    pio_vga_set_cursor(120, 467);
    pio_vga_write_string("0");
    pio_vga_draw_v_line(220, 460, 5, GREEN);
    pio_vga_set_cursor(220, 467);
    pio_vga_write_string("100");
    pio_vga_draw_v_line(320, 460, 5, GREEN);
    pio_vga_set_cursor(320, 467);
    pio_vga_write_string("200");
    pio_vga_draw_v_line(420, 460, 5, GREEN);
    pio_vga_set_cursor(420, 467);
    pio_vga_write_string("300");
    pio_vga_draw_v_line(520, 460, 5, GREEN);
    pio_vga_set_cursor(520, 467);
    pio_vga_write_string("400");
    pio_vga_draw_h_line(215, 20, 315, GREEN);
    pio_vga_draw_h_line(110, 460, 420, GREEN);
}

//---------------------------------------------------------------------------
// VGA GET MAX SIGNAL AMPLITUDE FUNCTION
//---------------------------------------------------------------------------
uint16_t pio_vga_get_max_signal_amplitude(uint16_t max_signal_amplitude[DECIMATED_FACTOR])
{
    uint16_t max = 0;
    for (int i = 0; i < DECIMATED_FACTOR; i++)
    {
        uint16_t diff = abs(max_signal_amplitude[i] - 512);
        if (diff > max)
        {
            max = diff;
        }
    }
    return max;
}

//---------------------------------------------------------------------------
// VGA GET DECIMATED SIGNAL FUNCTION
//---------------------------------------------------------------------------
void pio_vga_get_decimated_signal(uint16_t decimate_signal[SIGNAL_COUNT])
{
    for (uint16_t i = 0; i < DECIMATED_SIGNAL; i++)
    {
        max_signal[i] = pio_vga_get_max_signal_amplitude((decimate_signal + (i * DECIMATED_FACTOR)));
    }
}

//---------------------------------------------------------------------------
// VGA DRAW FRAME FUNCTION
//---------------------------------------------------------------------------
void pio_vga_draw_frame()
{
    pio_vga_reset_vga();
    pio_vga_draw_container();
}

//---------------------------------------------------------------------------
// VGA MAIN FUNCTION
//---------------------------------------------------------------------------
void vga(uint16_t signal[SIGNAL_COUNT])
{
    pio_vga_draw_frame();
    pio_vga_get_decimated_signal(signal);
    uint32_t signal_height = 0;
    for (uint16_t i = 0; i < DECIMATED_SIGNAL; ++i)
    {
        signal_height = ((uint32_t)(max_signal[i] * 420) >> 9);
        pio_vga_draw_v_line((120 + i), (HEIGHT - (signal_height + 30)), signal_height, GREEN);
    }
}

//---------------------------------------------------------------------------
// END OF FILE
//---------------------------------------------------------------------------
