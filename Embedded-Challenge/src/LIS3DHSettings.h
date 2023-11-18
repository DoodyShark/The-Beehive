#ifndef INT_TYPES
#define INT_TYPES 0
#include <inttypes.h>
#endif
#ifndef LIS3DH_PREDIRECTIVES
#include "LIS3DH_predirectives.h"
#define LIS3DH_PREDIRECTIVES 0
#endif

enum PM {
    L = 0,
    N = 1,
    H = 2
};

enum EN {
    DISABLED, ENABLED
};

class LIS3DHSettings{

    private:
    int max_accel;
    int frequency;
    PM power_mode;
    bool xen;
    bool yen;
    bool zen;

    public:

    LIS3DHSettings() {
        this->max_accel = 16;
        this->frequency = 1;
        this->power_mode = (PM) L;
        this->xen = true;
        this->yen = true;
        this->zen = true;
    }

    LIS3DHSettings(int max_a, int freq, PM p_m, EN x, EN y, EN z) {
        this->max_accel = max_a;
        this->frequency = freq;
        this->power_mode = p_m;
        this->xen = x;
        this->yen = y;
        this->zen = z;
    }

    int get_freq() {
        return this->frequency;
    }

    int get_max_accel() {
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