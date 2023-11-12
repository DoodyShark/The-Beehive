#include "predirectives.cpp"

/// @brief Setup the SPI Connection as Master CPU
void SPI_MasterInit()
{
  /* Set MOSI, SCK, CS (SS as output, all others input */
  DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<CS);
  /* 
    SPIE: 0 - SPI Interrupt disabled
    SPE: 1 - Enable SPI
    DORD: 0 - LSB transmitted first
    MSTR: 1 - Set it in Master mode
    CPOL: 1 - Clock Polarity high when idle, Trailing edge is rising edge
    CPHAL: 1 - Sample at trailing edge (rising edge as specified by the accelerometer datasheet)
    (SPI2X_SPR1-0: 001) = Clock prescaling by 16 (clock rate = fosc/16 = 8 MHz / 16 = 0.5 MHz). Frequency can be at most 10 MHz
  */
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPOL)|(1<<CPHA);
}

/// @brief Transmits the given data on the MOSI wire
/// @param cData The data to be sent to the slave
void SPI_MasterTransmit(uint8_t cData)
{
  /* Start transmission */
  SPDR = cData;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF))){
    Serial.println((PORTB >> DD_SCK) & 1);
  }
  ;
}

uint8_t SPI_BeginTransmission() {
  PORTB &= ~(1<<CS);
}

uint8_t SPI_EndTransmission() {
  PORTB |= (1<<CS);
}

uint8_t SPI_MasterRead()
{
  SPI_MasterTransmit((uint8_t) 0xFF);
  /* Wait for reception complete */
  while(!(SPSR & (1<<SPIF))) 
  ;
  /* Return Data Register */
  return SPDR;
}

void setup() {
  SPI_MasterInit();
  SPI_BeginTransmission();
  Serial.begin(9600);
}

void loop() {
  uint8_t val = SPI_MasterRead();
}