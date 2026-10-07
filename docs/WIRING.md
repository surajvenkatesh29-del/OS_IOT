# Wiring Guide

## GPIO mapping

| Road | IR input | Red LED | Yellow LED | Green LED |
|---|---:|---:|---:|---:|
| 1 | GPIO20 | GPIO4 | GPIO17 | GPIO27 |
| 2 | GPIO21 | GPIO22 | GPIO23 | GPIO24 |
| 3 | GPIO26 | GPIO25 | GPIO5 | GPIO6 |
| 4 | GPIO19 | GPIO12 | GPIO13 | GPIO16 |

All numbers are **BCM GPIO numbers**, not physical pin numbers.

## LED connection

For each LED:

`GPIO -> 220Ω resistor -> LED anode (+)`

`LED cathode (-) -> GND`

The program assumes an active-high LED output.

## IR sensor connection

For a 3.3V-compatible IR module:

- VCC -> 3.3V
- GND -> GND
- OUT -> assigned GPIO input

Many IR modules use an active-low output. This project assumes `LOW = vehicle detected`.

If your sensor behaves opposite, change `return value == 0;` in `src/traffic_signal.c` to `return value == 1;`.

## Physical-pin reference

- GPIO4 = pin 7
- GPIO17 = pin 11
- GPIO27 = pin 13
- GPIO22 = pin 15
- GPIO23 = pin 16
- GPIO24 = pin 18
- GPIO25 = pin 22
- GPIO5 = pin 29
- GPIO6 = pin 31
- GPIO12 = pin 32
- GPIO13 = pin 33
- GPIO16 = pin 36
- GPIO19 = pin 35
- GPIO20 = pin 38
- GPIO21 = pin 40
- GPIO26 = pin 37

Use any Raspberry Pi GND pin as common ground.
