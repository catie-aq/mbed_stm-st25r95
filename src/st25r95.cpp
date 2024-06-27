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
    _spi_address->format(8, 0);
    _spi_address->frequency(frequency);
}

void ST25R95::initialize()
{
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

void ST25R95::select_14443_A_protocol(
        bool default_protocol, uint8_t data_rate, uint8_t pp, uint8_t mm, uint8_t dd)
{
    if (default_protocol) {
        _chip_select->write(0);
        _spi_address->write(0x00); // Prepare to send command
        _spi_address->write(0x02); // Send command
        _spi_address->write(0x02); // Length of Data
        _spi_address->write(0x02); // Data (Protocol ID)
        _spi_address->write(0x00); // Data Rate
        _chip_select->write(1);
    } else {
        _chip_select->write(0);
        _spi_address->write(0x00); // Prepare to send command
        _spi_address->write(0x02); // Send command
        _spi_address->write(0x05); // Length of Data
        _spi_address->write(0x02); // Data (Protocol ID)
        _spi_address->write(data_rate); // Data Rate
        // PP MM DD Value are useful to calculate FDT Frame delay time ref 5.4 p15 of st25r95
        // datasheet
        _spi_address->write(pp); // PP
        _spi_address->write(mm); // MM
        _spi_address->write(dd); // DD
        _chip_select->write(1);
    }

    _protocol_select = 0x02;
}

char *ST25R95::get_tag_value()
{
    send_receive_command();
    if (read_response()) {
        tag_value_32 = ((uint32_t)response[0] << 24) | ((uint32_t)response[1] << 16)
                | ((uint32_t)response[2] << 8) | ((uint32_t)response[3] << 0);
        char tag_value[response_length];
        for (size_t i = 0; i < response_length; i++) {
            tag_value[i] = response[i];
            printf("%02X ",tag_value[i]);
        }
        printf("\n");
        return tag_value;
    } else {
        return 0;
    }
}

void ST25R95::send_receive_command()
{
    switch (_protocol_select) {
        case 0x02:
            _chip_select->write(0);
            _spi_address->write(0x00);
            _spi_address->write(0x04); // Command
            _spi_address->write(0x02); // Length
            _spi_address->write(0x26); // Data
            _spi_address->write(0x07); // Data
            _chip_select->write(1);
            poll();
            read_response();
            _chip_select->write(0);
            _spi_address->write(0x00);
            _spi_address->write(0x04); // Command
            _spi_address->write(0x03); // Length
            _spi_address->write(0x93); // Data
            _spi_address->write(0x20); // Data
            _spi_address->write(0x08); // Data
            _chip_select->write(1);
            poll();
            break;

        default:
            break;
    }
}