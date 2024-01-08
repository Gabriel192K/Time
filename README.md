# Terminal
- Time library for timekeeping

## Key features
- Compatible with `Microchip Studio IDE`
- ```begin()``` and ```end()``` functions to easily enable or disable the `Time` implementation
- Finetuned for `16MHz`, yet to be generalized for any frequency
- Delays within microseconds written in `AVR inline asembly`
- `Timer Overflow Interrupt` driven with a resolution precalculated at compile time, eg. for `16 MHz` and `prescaler of 8` a `128 uS` resolution is available, `higer frequencies have better resolutions`, and `lower have worse ones`

## Tested on
- `Microchip Studio IDE` with `ATmega328PB`