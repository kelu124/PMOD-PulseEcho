## Current experimet

* uart will print the sent character on both ports. Port-A:uart and Port-B:fifo
* please send one character/byte at a time on uart or fifo. If uartTX is busy with sending previous byte, then it will ignore current byte
