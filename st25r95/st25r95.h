/*
 * Copyright (c) 2024, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_ST25R95_H_
#define CATIE_SIXTRON_ST25R95_H_
#include "mbed.h"

class ST25R95 {
public:
    ST25R95(SPI *spi, DigitalOut *chip_select, DigitalOut *interrupt_input);
    // ST25R95(SPI *spi, DigitalOut *chip_select, DigitalOut *interrupt_input);
private:
    SPI *_spi_address;
    DigitalOut *_chip_select;
    DigitalOut *_interrupt_input;
};

#endif // CATIE_SIXTRON_ST25R95_H_
