/*
 * Copyright (c) 2024, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_ST25R95_H_
#define CATIE_SIXTRON_ST25R95_H_
#include "mbed.h"

#define EEmdSOFerror23 0X63 // SOF error in high part (duration 2 to 3 etu) in ISO/IEC 14443B
#define EEmdSOFerror10 0x65 // SOF error in low part (duration 10 to 11 etu) in ISO/IEC 14443B
#define EEmdEgt 0x66 // Error Extennded Guard Time error in ISO/IEC 14443B
#define ETr1_Too_Big 0x67 // Too long TR1 send by the card, reception stopped in ISO/IEC 14443BT
#define ETr1Too_small 0X68 // Too small TR1 send by the card in ISO/IEC 14443B
#define EinternalError 0X71 // Wong frame format decodes
#define EFrameRecvOK 0x80 // Frame correctly received (additionally see CRC/Parity information)
#define EInvalidCmdLen 0X82 // Invalid command length
#define EInvalidProto 0X83 // Invalid protocol
#define EUserStop 0X85 // Stopped by user (used only in Card mode)
#define ECommError 0X86 // Hardware communication error
#define EFrameWaitTOut 0X87 // Frame wait time out (no valid reception)
#define EInvalidSof 0X88 // Invalid SOF
#define EBufOverflow 0X89 // Too many bytes received and data still arriving
#define EFramingError 0X8A // if start bit = 1 or stop bit = 0
#define EEgtError 0X8B // EGT time out
#define EInvalidLen 0X8C // Valid for FeliCa™, if Length <3
#define ECrcError 0X8D // CRC error, Valid only for FeliCa™
#define ERecvLost 0X8E // When reception is lost without EOF received (or subcarrier was lost)
#define ENoField 0X8F // When Listen command detects the absence of external field
#define EUnintByte                                                                                 \
    0X90 // Residual bits in last byte. Useful for ACK/NAK reception of ISO/IEC 14443 Type A

class ST25R95 {
public:
    /*! Default Constructor
     * \param spi Pointer to the mbed SPI object
     * \param chip_select Pointer to the mbed DigitalOut object
     * \param interrupt_input Pointer to the mbed DigitalOut object
     */
    ST25R95(SPI *spi,
            DigitalOut *chip_select,
            DigitalOut *interrupt_input,
            int frequency = 1000000);

    /*! Initialize the ST25R95
     * \brief send all command st25r95 need to reset properly
     */
    void initialize();

    /*! Polling function for the ST25R95
     * \param default_value set this variable to false to change some parameter
     * \param flags set flags 01: Wait for RF field appearance 00: Wait for RF field disappearance
     * \param presc Timer prescaler (Optional)
     * \param timer Timer time-out (Optional)
     * \brief Flags, Presc and Timer parameters are optional. They must
     * \brief be specidfied if the application has to wait for RF field appearance
     * \brief or disappearance.
     * \brief The time to wait is (Presc+1)*(Timer+1)/13.56 μs.
     */
    bool poll(bool default_value = true, uint8_t flags = 0, uint8_t presc = 0, uint8_t timer = 0);

    /*! Read Response of the ST25R95
     *
     */
    uint8_t read_response();

    /*! get IDN value
     * \param tab Pointer to a char to fill it with IDN value
     */
    void get_idn_value(char *tab);

    /*! Select the protocole 14443-A
     * \param default_protocolset this variable to false to change some parameter
     * \param data_rate
     * \param pp 7:0 PP (0x00 - 0x0E, other values RFU) When there is PP, MM must also be provided.
     * \param mm 7:0 MM
     * \param dd 7:0 DD (0x00 - 0x7F, other values RFU)
     * \brief For Data Rate :
     * \brief [7:6] Transmission data rate 00: 106 Kbps 01: 212 Kbps 10: 424 Kbps 11: RFU
     * \brief [5:4] Reception data rate 00: 106 Kbps 01: 212 Kbps 10: 424 Kbps 11: RFU
     * \brief [3:0] RFU
     * \brief For PP MM DD :
     * \brief These 3 bytes are optional. The default
     * \brief PP:MM:DD value is 0 (corresponds to FDT 86/90μs). For other values,
     * \brief FDT = (2^PP)*(MM+1)*(DD+128) *32/13.56 μs
     */
    void select_14443_a_protocol(bool default_protocol = true,
            uint8_t data_rate = 0,
            uint8_t pp = 0,
            uint8_t mm = 0,
            uint8_t dd = 0);

    /*! Get Tag Value
     * \return Tag Value in uint32_t to simply compare two tag value
     */
    uint32_t get_tag_value();

    /*! fill char with response
     * \param tab Pointer to a char to fill it with IDN value
     */
    void fill_response(char *tab);

    /*! Tag is detected
     * \return true if tag is detected or false if no tag is detecteds
     */
    bool tag_is_detected();

    /*! get error value
     * \return error_value
     */
    uint8_t get_error_value();

    /*! detect if you present multiple tag
     * \return true if you have a single tag false if you have multiple tag present
     */
    bool no_multiple_tag();

    /*! Select protocol as you want
     *  \param protocol ID of protocol you want to use
     */
    uint8_t protocol(uint8_t protocol);

private:
    /*! SendRecv command to receive tag information
     *
     */
    void send_receive_command();

    /*! pulse Interrupt Input of st25r95
     *
     */
    void irqn_reset();

    /*! Send Calibrate command
     *
     */
    void calibrate();
    uint8_t response_code;
    SPI *_spi_address;
    DigitalOut *_chip_select;
    DigitalOut *_interrupt_input;
    uint8_t _protocol_select;
    uint32_t tag_value_32;
    uint8_t response[512];
    uint8_t response_length;
};

#endif // CATIE_SIXTRON_ST25R95_H_
