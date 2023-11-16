#ifndef PREDIRECTIVES
#include "predirectives.cpp"
#define PREDIRECTIVES 0
#endif


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

void WriteByte(uint8_t reg_addr, uint8_t data) {
  SPI_BeginTransmission();
  SPI_Transfer(reg_addr);
  SPI_Transfer(data);
  SPI_EndTransmission();
}

uint8_t ReadByte(uint8_t reg_addr) {
  SPI_BeginTransmission();
  SPI_Transfer((uint8_t)0b10000000 | reg_addr);
  uint8_t data = SPI_Transfer((uint8_t)0x00);
  SPI_EndTransmission();
  return data;
}

uint16_t ReadTwoBytes(uint8_t reg_addr) {
  SPI_BeginTransmission();
  SPI_Transfer((uint8_t)0b11000000 | reg_addr);
  uint16_t data = SPI_Transfer16((uint16_t) 0x0000);
  SPI_EndTransmission();
  return data;
}

void SetupAccelerometer() {
  uint8_t wanted_state = (uint8_t) 0b01000111;
  uint8_t state = (uint8_t) 0;
  do {
    // Accessing CTRL_REG1:
    WriteByte(CTRL_REG1, wanted_state);
    /*Check Change*/

    state = ReadByte(CTRL_REG1);
  } while (state != wanted_state);
}

uint16_t getXRaw() {
  uint16_t data = ReadTwoBytes(OUT_X_L);
  return data;
}

uint16_t getYRaw() {
  uint16_t data = ReadTwoBytes(OUT_Y_L);
  return data;
}

uint16_t getZRaw() {
  uint16_t data = ReadTwoBytes(OUT_Z_L);
  return data;
}

void ResetAccelerometer() {
  WriteByte((uint8_t) 0x1E, (uint8_t) 0b00010000); // CTRL_REG0
  WriteByte((uint8_t) 0x1F, (uint8_t) 0b00000000); // TEMP_CFG_REG
  WriteByte((uint8_t) 0x20, (uint8_t) 0b00000111); // CTRL_REG1
  WriteByte((uint8_t) 0x21, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x22, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x23, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x24, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x25, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x26, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x2E, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x30, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x32, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x33, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x34, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x36, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x37, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x38, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3A, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3B, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3C, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3D, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3E, (uint8_t) 0b00000000); // 
  WriteByte((uint8_t) 0x3F, (uint8_t) 0b00000000); //
}

void setup() {
  Serial.begin(9600);
  SPI_MasterInit();
  SetupAccelerometer();
}

void loop() {
  delay(20);
  uint16_t dataX = getXRaw();
  uint16_t dataY = getYRaw();
  uint16_t dataZ = getZRaw();
  
  Serial.print("At time ");
  Serial.print(millis());
  Serial.print(" (");
  Serial.print(dataX);
  Serial.print(", ");
  Serial.print(dataY);
  Serial.print(", ");
  Serial.print(dataZ);
  Serial.println(")");
}