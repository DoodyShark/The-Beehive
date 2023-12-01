// Include <Arduino.h> if it's available
#ifdef __has_include
    #if __has_include(<Arduino.h>)
        #include <Arduino.h>
    #endif
#endif

// Relevant SPI Registers & Values
#ifndef DDRB
#define DDRB (*(volatile uint8_t *)((0x04) + 0x20))
#endif
#ifndef DDR_SPI
#define DDR_SPI (*(volatile uint8_t *)((0x04) + 0x20))
#endif
#ifndef SPCR 
#define SPCR (*(volatile uint8_t *)((0x2C) + 0x20))
#endif
#ifndef SPE 
#define SPE 6
#endif
#ifndef MSTR 
#define MSTR 4
#endif
#ifndef SPR0 
#define SPR0 0
#endif
#ifndef CPOL 
#define CPOL 3
#endif
#ifndef CPHA 
#define CPHA 2
#endif
#ifndef DD_MOSI
#define DD_MOSI 2
#endif
#ifndef DD_SCK
#define DD_SCK 1
#endif
#ifndef CS
#define CS 4
#endif  
#ifndef PORTB
#define PORTB (*(volatile uint8_t *)((0x05) + 0x20))
#endif
#ifndef SPDR
#define SPDR (*(volatile uint8_t *)((0x2E) + 0x20))
#endif
#ifndef SPSR
#define SPSR (*(volatile uint8_t *)((0x2D) + 0x20))
#endif
#ifndef SPIF
#define SPIF 7
#endif
#ifndef abs
#define abs(x) (x>0)?x:(-x)
#endif
#ifndef DDRC
#define DDRC (*(volatile uint8_t *)((0x07) + 0x20))
#endif
