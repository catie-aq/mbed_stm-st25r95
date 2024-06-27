/*
 * Copyright (c) 2024, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "st25r95.h"
#include "mbed.h"

ST25R95::ST25R95(SPI *spi, DigitalOut *chip_select, DigitalOut *interrupt_input, int frequency)
{
    _spi_address = spi;
    _chip_select = chip_select;
    _interrupt_input = interrupt_input;
}

void ST25R95::initialize()
{
    _spi_address->format(8, 0);
    _spi_address->frequency(1000000);
    irqn_reset();
    _chip_select->write(0);
    _spi_address->write(0x01);
    ThisThread::sleep_for(1ms);
    _chip_select->write(1);
    irqn_reset();
    calibrate();
}

void ST25R95::irqn_reset()
{
    _interrupt_input->write(1);
    ThisThread::sleep_for(1ms);
    _interrupt_input->write(0);
    ThisThread::sleep_for(1ms);
    _interrupt_input->write(1);
    ThisThread::sleep_for(20ms);
}

void ST25R95::calibrate()
{
    _chip_select->write(0);
    _spi_address->write(0x07);
    _spi_address->write(0x0E);
    _spi_address->write(0x03);
    _spi_address->write(0xA1);
    _spi_address->write(0x00);
    _spi_address->write(0xF8);
    _spi_address->write(0x01);
    _spi_address->write(0x18);
    _spi_address->write(0x00);
    _spi_address->write(0x20);
    _spi_address->write(0x60);
    _spi_address->write(0x60);
    _spi_address->write(0x00);
    _spi_address->write(0x00);
    _spi_address->write(0x3F);
    _spi_address->write(0x00);
    _chip_select->write(1);
    _chip_select->write(0);
    _spi_address->write(0x02);
    _spi_address->write(0x00);
    _spi_address->write(0x00);
    _chip_select->write(1);
}

bool ST25R95::poll(bool default_value, uint8_t flags, uint8_t presc, uint8_t timer)
{
    uint8_t flag;
    if (default_value) {
        do {
            _chip_select->write(0);
            _spi_address->write(0x03);
            flag = _spi_address->write(0x00);
            ThisThread::sleep_for(15ms);
            _chip_select->write(1);
        } while ((flag & 0x08) == 0);
        return true;
    } else {
        do {
            _chip_select->write(0);
            _spi_address->write(0x03);
            _spi_address->write(0x03);
            _spi_address->write(flags);
            _spi_address->write(presc);
            _spi_address->write(timer);
            _spi_address->write(0x00);
            _spi_address->write(0x00);
            flag = _spi_address->write(0x00);
            ThisThread::sleep_for(15ms);
            _chip_select->write(1);
        } while ((flag & 0x01) == 0);
        return true;
    }
    return false;
}

uint8_t ST25R95::read_response()
{
    _chip_select->write(0);
    _spi_address->write(0x02);
    response_code = _spi_address->write(0x00);
    response_length = _spi_address->write(0x00);
    for (int i = 0; i < response_length; i++) {
        response[i] = _spi_address->write(0x00);
    }
    _chip_select->write(1);
    return response_length;
}

void ST25R95::get_idn_value()
{
    _chip_select->write(0);
    _spi_address->write(0x00);
    _spi_address->write(0x01);
    _spi_address->write(0x00);
    _chip_select->write(1);

    poll();
    read_response();
    for (size_t i = 0; i < response_length; i++) {
        printf("%02X ", response[i]);
    }
    printf("\n");
}