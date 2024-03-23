In rasp pi pico board it maps the pins consecutively, you give it a base pin and it counts incrementally if you give it a base PIN 10 then the base pin will be number 10 and counts incrementally from 10 and the last pin will be 9.
The Pico board has a map for side set pins you give it a pin as a side set and it maps it in the side set map as I did in the Pio code, in this code we use pin IO_0 as clk and pin 0 as a base pin.


```
sm_config_set_out_pins(&c, pin, 1);
pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
sm_config_set_sideset_pins(&c, pin);
```


but if you want to use another pin than IO_0 for example pin IO_10 with base pin 0 then the code will be as follows.



```
sm_config_set_out_pins(&c, pin + 10, 1);
pio_sm_set_consecutive_pindirs(pio, sm, pin + 10, 1, true);
sm_config_set_sideset_pins(&c, pin + 10);
```

