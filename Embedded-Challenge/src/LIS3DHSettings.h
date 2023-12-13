// Import relevant code

#ifndef INT_TYPES
#define INT_TYPES 0
#include <inttypes.h>
#endif
#ifndef LIS3DH_PREDIRECTIVES
#include "LIS3DH_predirectives.h"
#define LIS3DH_PREDIRECTIVES 0
#endif

// Available Power Modes
enum PM {
    L = 0,
    N = 1,
    H = 2
};

// Enabled modes
enum EN {
    DISABLED, ENABLED
};

/// @brief Stores and handles the settings relevant to LIS3DH accelerometer
class LIS3DHSettings{

    private:
    uint8_t max_accel;
    uint16_t frequency;
    PM power_mode;
    bool xen;
    bool yen;
    bool zen;

    public:
    
    /// @brief Default constructor
    LIS3DHSettings() {
        this->max_accel = 16;
        this->frequency = 1;
        this->power_mode = (PM) L;
        this->xen = true;
        this->yen = true;
        this->zen = true;
    }

    /// @brief Non-default constructor
    /// @param max_a the maximum absolute acceleration
    /// @param freq the data collection frequency
    /// @param p_m the power mode
    /// @param x whether x is enabled
    /// @param y whether y is enabled
    /// @param z whether z is enabled
    LIS3DHSettings(uint8_t max_a, uint16_t freq, PM p_m, EN x, EN y, EN z) {
        this->max_accel = max_a;
        this->frequency = freq;
        this->power_mode = p_m;
        this->xen = x;
        this->yen = y;
        this->zen = z;
    }

    // Getters

    uint8_t get_freq() {
        return this->frequency;
    }

    uint16_t get_max_accel() {
        return this->max_accel;
    }

    PM get_power_mode() {
        return this->power_mode;
    }

    bool get_xen() {
        return this->xen;
    }

    bool get_yen() {
        return this->yen;
    }

    bool get_zen() {
        return this->zen;
    }

    /// @brief Converts the frequency setting to the corresponding byte
    /// @param curr_val takes the current value of the relevant register
    /// @return the byte after converting the frequency as described in the datasheet
    uint8_t Freq_to_Byte(uint8_t curr_val) {
        switch (this->frequency) {
        case 0:
            curr_val &= ~((uint8_t)0b1 << ORD0);
            curr_val &= ~((uint8_t)0b1 << ORD1);
            curr_val &= ~((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 1:
            curr_val |= ((uint8_t)0b1 << ORD0);
            curr_val &= ~((uint8_t)0b1 << ORD1);
            curr_val &= ~((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 10:
            curr_val &= ~((uint8_t)0b1 << ORD0);
            curr_val |= ((uint8_t)0b1 << ORD1);
            curr_val &= ~((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 25:
            curr_val |= ((uint8_t)0b1 << ORD0);
            curr_val |= ((uint8_t)0b1 << ORD1);
            curr_val &= ~((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 50:
            curr_val &= ~((uint8_t)0b1 << ORD0);
            curr_val &= ~((uint8_t)0b1 << ORD1);
            curr_val = ((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 100:
            curr_val = ((uint8_t)0b1 << ORD0);
            curr_val &= ~((uint8_t)0b1 << ORD1);
            curr_val = ((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 200:
            curr_val &= ~((uint8_t)0b1 << ORD0);
            curr_val = ((uint8_t)0b1 << ORD1);
            curr_val = ((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        case 400:
            curr_val = ((uint8_t)0b1 << ORD0);
            curr_val = ((uint8_t)0b1 << ORD1);
            curr_val = ((uint8_t)0b1 << ORD2);
            curr_val &= ~((uint8_t)0b1 << ORD3);
            break;
        }
        return curr_val;
    }

    /// @brief Converts the max absolute acceleration setting to the corresponding byte
    /// @param curr_val takes the current value of the relevant register
    /// @return the byte after converting the max absolute acceleration as described in the datasheet
    uint8_t Max_Accel_to_Byte(uint8_t curr_val) {
        switch (this->max_accel) {
        case 2:
            curr_val &= ~((uint8_t)0b1 << FS0);
            curr_val &= ~((uint8_t)0b1 << FS1);
            break;
        case 4:
            curr_val |= ((uint8_t)0b1 << FS0);
            curr_val &= ~((uint8_t)0b1 << FS1);
            break;
        case 8:
            curr_val &= ~((uint8_t)0b1 << FS0);
            curr_val |= ((uint8_t)0b1 << FS1);
            break;
        case 16:
            curr_val |= ((uint8_t)0b1 << FS0);
            curr_val |= ((uint8_t)0b1 << FS1);
            break;
        }
        return curr_val;
    }

    /// @brief Converts the power mode setting to the corresponding byte
    /// @param curr_val1 takes the current value of the first relevant register
    /// @param curr_val4 takes the current value of the second relevant register
    /// @return the byte after converting the power mode as described in the datasheet
    uint16_t Power_Mode_to_Byte(uint8_t curr_val1, uint8_t curr_val4) {
        switch (this->power_mode) {
            case (PM) L:
                curr_val1 |= ((uint8_t)1 << LPEN);
                curr_val4 &= ~((uint8_t)1 << HR);
            break;
            case (PM) N:
                curr_val1 &= ~((uint8_t)1 << LPEN);
                curr_val4 &= ~((uint8_t)1 << HR);
            break;
            case (PM) H:
                curr_val1 &= ~((uint8_t)1 << LPEN);
                curr_val4 |= ((uint8_t)1 << HR);
            break;
        }
        return ((uint16_t) curr_val4) | ((uint16_t) curr_val1 ) << 8;
    }

    /*
        Calculates the factor to convert from raw acceleration to acceleration in g
    */
    float Calc_Div_Factor() {
        if (this->power_mode == 'l') {
            switch (this->max_accel) {
                case 2:
                    return 64;
                case 4:
                    return 32;
                case 8:
                    return 16;
                case 16:
                    return 8;
            }
        }
        else if (this->power_mode == 'n') {
            switch (this->max_accel) {
                case 2:
                    return 256;
                case 4:
                    return 128;
                case 8:
                    return 64;
                case 16:
                    return 32;
            }
        }
        else {
            switch (this->max_accel) {
                case 2:
                    return 1028;
                case 4:
                    return 512;
                case 8:
                    return 256;
                case 16:
                    return 128;
            }
        }
        return 1;
    }

};