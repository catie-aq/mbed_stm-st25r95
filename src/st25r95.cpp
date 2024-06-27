/*
 * Copyright (c) 2024, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "st25r95.h"
#include "mbed.h"

ST25R95::ST25R95(SPI *spi, DigitalOut *chip_select, DigitalOut *interrupt_input)
{
    _spi_address = spi;
    _chip_select = chip_select;
    _interrupt_input = interrupt_input;
}
