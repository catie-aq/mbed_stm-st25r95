/*
 * Copyright (c) 2024, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_ST25R95_H_
#define CATIE_SIXTRON_ST25R95_H_
#include "mbed.h"

class ST25R95 {
public:
    ST25R95(SPI *spi,
            DigitalOut *chip_select,
            DigitalOut *interrupt_input,
            int frequency = 1000000);
    void initialize();

    bool poll(bool default_value = true, uint8_t flags = 0, uint8_t presc = 0, uint8_t timer = 0);
    uint8_t read_response();
    void get_idn_value();

private:
    uint8_t response_code;
    SPI *_spi_address;
    DigitalOut *_chip_select;
    DigitalOut *_interrupt_input;
    uint8_t _protocol_select;
    uint32_t tag_value_32;
    void irqn_reset();
    void calibrate();
    uint8_t response[512];
    uint8_t response_length;
};

#endif // CATIE_SIXTRON_ST25R95_H_
