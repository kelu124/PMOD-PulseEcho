# PMOD-PulseEcho

* Objective: test two designs for pulser and acquisition boards.
* Might work well with the [FT600 test board](https://github.com/kelu124/ft600) (OSHWA certified)

## Software

### FT600 

* First tries with [FT600, on Kilsyth](software/kilsyth/), some issues. 
* Current tests on ACQ, FIFO seems OK.
* Today: on hold.

### [icebreaker](/software/icebreaker/)

* UP5K board
* Today: tests ongoing. FIFO tests.

### [pico / rp2040](/software/pico/)

* Tests ongoing
* Today: Acq seems OK. Issue with some pins (D6 D7 seem buggy). Migth try with lower frequency.

## Hardware

* First designs done, some mistakes.

* Pulser TODO
  * PMOD pin reversed, to correct
  * Spacing of top pins to respect a 2.54" grid.
  
* Acq TODO
  * Right spacing of PMOD headers
  * Right labelling of pins
