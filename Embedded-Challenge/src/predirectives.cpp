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

// Relevent LI3DH Registers
#ifndef CTRL_REG1
#define CTRL_REG1 (uint8_t) 0x20
#endif
#ifndef OUT_X_L
#define OUT_X_L (uint8_t) 0x28
#endif
#ifndef OUT_X_H
#define OUT_X_H (uint8_t) 0x29
#endif
#ifndef OUT_Y_L
#define OUT_Y_L (uint8_t) 0x2A
#endif
#ifndef OUT_Y_H
#define OUT_Y_H (uint8_t) 0x2B
#endif
#ifndef OUT_Z_L
#define OUT_Z_L (uint8_t) 0x2C
#endif
#ifndef OUT_Z_H
#define OUT_Z_H (uint8_t) 0x2D
#endif