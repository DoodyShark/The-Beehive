#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif

namespace SPI_Own {
  /// @brief Setup the SPI Connection as Master CPU
  void SPI_MasterInit()
  {
    /* Set MOSI, SCK, CS (SS as output, all others input */
    DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<CS)|(1<<0);
    /* 
      SPIE: 1 - SPI Interrupt enabled
      SPE: 1 - Enable SPI
      DORD: 0 - MSB transmitted first
      MSTR: 1 - Set it in Master mode
      CPOL: 1 - Clock Polarity high when idle, Trailing edge is rising edge
      CPHA: 1 - Sample at trailing edge (rising edge as specified by the accelerometer datasheet)
      (SPI2X_SPR1-0: 001) = Clock prescaling by 16 (clock rate = fosc/16 = 8 MHz / 16 = 0.5 MHz). Frequency can be at most 10 MHz
    */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPOL)|(1<<CPHA);
    // SREG |= (1<<7);
  }

  /// @brief Transmits the given data on the MOSI wire
  /// @param cData The data to be sent to the slave
  uint8_t SPI_Transfer(uint8_t cData)
  {
    /* Start transmission */
    SPDR = cData;
    asm volatile("nop"); // Might need this
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
    /* Read returned value */
    return SPDR;
  }

  uint16_t SPI_Transfer16(uint16_t cData)
  {
    
    uint8_t dataMSB, dataLSB;
    uint8_t outMSB, outLSB;

    dataLSB = (uint8_t) (cData & (uint16_t) 0x00FF);
    dataMSB = (uint8_t) ((cData & (uint16_t) 0xFF00) >> 8);
    // Serial.print("Sent dataMSB, dataLSB: ");
    // Serial.print(dataMSB);
    // Serial.print(" ");
    // Serial.println(dataLSB);

    /* Start transmission with MSB */
    SPDR = dataMSB;
    asm volatile("nop"); // Might need this
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
    /* Read returned value */
    outLSB = SPDR;
    /* Continue transmission with LSB */
    SPDR = dataLSB;
    asm volatile("nop"); // Might need this
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
    /* Read returned value */
    outMSB = SPDR;
    return ((uint16_t)outMSB) << 8 | (uint16_t) outLSB;
  }

  void SPI_BeginTransmission() {
    PORTB &= ~(1<<CS);
  }

  void SPI_EndTransmission() {
    PORTB |= (1<<CS);
  }

}