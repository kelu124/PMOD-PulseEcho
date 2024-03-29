// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// vga //
// --- //

#define vga_wrap_target 2
#define vga_wrap 8

static const uint16_t vga_program_instructions[] = {
    0x80a0, //  0: pull   block                      
    0xa047, //  1: mov    y, osr                     
            //     .wrap_target
    0xe000, //  2: set    pins, 0                    
    0xa022, //  3: mov    x, y                       
    0x23c1, //  4: wait   1 irq, 1               [3] 
    0x80a0, //  5: pull   block                      
    0x6403, //  6: out    pins, 3                [4] 
    0x6203, //  7: out    pins, 3                [2] 
    0x0045, //  8: jmp    x--, 5                     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program vga_program = {
    .instructions = vga_program_instructions,
    .length = 9,
    .origin = -1,
};

static inline pio_sm_config vga_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + vga_wrap_target, offset + vga_wrap);
    return c;
}

static inline void vga_program_init(PIO pio, uint sm, uint offset, uint pin) 
{
    pio_sm_config c = vga_program_get_default_config(offset);
    sm_config_set_set_pins(&c, pin, 3);
    sm_config_set_out_pins(&c, pin, 3);
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);
    pio_gpio_init(pio, pin+2);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 3, true);
    pio_sm_init(pio, sm, offset, &c);
}

#endif

