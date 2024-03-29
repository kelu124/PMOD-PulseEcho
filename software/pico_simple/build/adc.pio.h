// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// adc //
// --- //

#define adc_wrap_target 0
#define adc_wrap 1

static const uint16_t adc_program_instructions[] = {
            //     .wrap_target
    0x5000, //  0: in     pins, 32        side 1     
    0x8020, //  1: push   block           side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program adc_program = {
    .instructions = adc_program_instructions,
    .length = 2,
    .origin = -1,
};

static inline pio_sm_config adc_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + adc_wrap_target, offset + adc_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

    void adc_program_init(PIO pio, uint sm, uint offset, uint pin) 
    {
        pio_sm_config c = adc_program_get_default_config(offset);
        pio_gpio_init(pio, pin);
        pio_gpio_init(pio, pin + 1);
        pio_gpio_init(pio, pin + 2);
        pio_gpio_init(pio, pin + 3);
        pio_gpio_init(pio, pin + 4);
        pio_gpio_init(pio, pin + 5);
        pio_gpio_init(pio, pin + 6);
        pio_gpio_init(pio, pin + 7);
        pio_gpio_init(pio, pin + 8);
        pio_gpio_init(pio, pin + 9);
        pio_gpio_init(pio, pin + 10);
        sm_config_set_in_pins(&c, pin);
        sm_config_set_out_pins(&c, pin, 1);
        pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
        sm_config_set_sideset_pins(&c, pin);
        gpio_put(pin, false);
        sm_config_set_in_shift(&c, false, false, 32);
        sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
        float div = ((float)clock_get_hz(clk_sys) / 120000000.0);
        sm_config_set_clkdiv(&c, div);
        pio_sm_init(pio, sm, offset, &c);
        pio_sm_set_enabled(pio, sm, true);
    }

#endif

