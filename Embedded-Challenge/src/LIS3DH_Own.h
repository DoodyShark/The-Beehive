#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif
#ifndef SPI_OWN
#include "SPI_Own.h"
#define SPI_OWN
#endif
#ifndef LIS3DH_SETTINGS
#include "LIS3DHSettings.h"
#define LIS3DH_SETTINGS 0
#endif

using namespace SPI_Own;

class LIS3DH {
    private:
    LIS3DHSettings settings;

    public:
    LIS3DH(LIS3DHSettings settings) {
        this->settings = settings;
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

    void setFreq() {
        uint8_t curr_val = ReadByte(CTRL_REG1);
        curr_val = this->settings.Freq_to_Byte(curr_val);
        WriteByte(CTRL_REG1, curr_val);
    }

    void setPowerMode() {
        uint8_t curr_val1 = ReadByte(CTRL_REG1);
        uint8_t curr_val4 = ReadByte(CTRL_REG4);
        uint16_t new_val = this->settings.Power_Mode_to_Byte(curr_val1, curr_val4);
        curr_val1 = (uint8_t) (new_val >> 8);
        curr_val4 = (uint8_t) new_val;
        WriteByte(CTRL_REG1, curr_val1);
        WriteByte(CTRL_REG4, curr_val4);
    }

    void setMaxAccel() {
        uint8_t curr_val = ReadByte(CTRL_REG4);
        curr_val = this->settings.Max_Accel_to_Byte(curr_val);
        WriteByte(CTRL_REG4, curr_val);
    }

    void ResetAccelerometer() {
        WriteByte((uint8_t) 0x1E, (uint8_t) 0b00010000); // CTRL_REG0
        WriteByte((uint8_t) 0x1F, (uint8_t) 0b00000000); // TEMP_CFG_REG
        WriteByte((uint8_t) 0x20, (uint8_t) 0b00000111); // CTRL_REG1
        WriteByte((uint8_t) 0x21, (uint8_t) 0b00000000); // 
        WriteByte((uint8_t) 0x22, (uint8_t) 0b00000000); // 
        WriteByte((uint8_t) 0x23, (uint8_t) 0b00000000); // 
        WriteByte((uint8_t) 0x24, (uint8_t) 0b10000000); // 
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

    void SetupAccelerometer() {
        ResetAccelerometer();
        setFreq();
        setPowerMode();
        setMaxAccel();
    }

    int16_t getXRaw() {
        uint16_t data = ReadTwoBytes(OUT_X_L);
        return (int16_t) data;
    }

    float getXFloat() {
        float div_factor = this->settings.Calc_Div_Factor();
        return (float) getXRaw() / div_factor;
        return 0.0;
    }

    int16_t getYRaw() {
        uint16_t data = ReadTwoBytes(OUT_Y_L);
        return (int16_t) data;
    }

    float getYFloat() {
        float div_factor = this->settings.Calc_Div_Factor();
        return (float) getYRaw() / div_factor;
        return 0.0;
    }

    int16_t getZRaw() {
        uint16_t data = ReadTwoBytes(OUT_Z_L);
        return (int16_t) data;
    }

    float getZFloat() {
        float div_factor = this->settings.Calc_Div_Factor();
        return (float) getZRaw() / div_factor;
        return 0.0;
    }

};