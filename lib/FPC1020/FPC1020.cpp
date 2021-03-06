#include <Arduino.h>
#include <SPI.h>

#define FPC_IRQ D1
#define FPC_RST D0

#include "FPC1020.h"

void FPC1020::init()
{
    // Chip select
    pinMode(PIN_SPI_SS, OUTPUT);
    digitalWrite(PIN_SPI_SS, HIGH);

    // IRQ / data ready
    pinMode(FPC_IRQ, INPUT);
    digitalWrite(FPC_IRQ, LOW);

    // RST
    pinMode(FPC_RST, OUTPUT);

    SPI.begin();
}

void FPC1020::reset()
{
    digitalWrite(FPC_RST, LOW);
    delay(10);
    digitalWrite(FPC_RST, HIGH);
}

void FPC1020::setup()
{
    setup_rev3();
}

void FPC1020::setup_rev3()
{
    Serial.printf("0x%016llX\n", transmit64(FPC102X_REG_SAMPLE_PX_DLY, 0x1717171723232323ULL));

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_PXL_RST_DLY, 0x0f));

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_FINGER_DRIVE_DLY, 0x18));

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_FINGER_DRIVE_CONF, 0x02));

    Serial.printf("0x%04X\n", transmit16(FPC102X_REG_ADC_SHIFT_GAIN, 0x1002));

    Serial.printf("0x%04X\n", transmit16(FPC102X_REG_PXL_CTRL, 0x000a | 0x0F00));

    Serial.printf("0x%02X\n", transmit8(FPC102X_REG_IMAGE_SETUP, 0x03 | 0x08));

    Serial.printf("0x%02X\n", transmit8(FPC1020_REG_FNGR_DET_THRES, 0x50));

    Serial.printf("0x%04X\n", transmit16(FPC1020_REG_FNGR_DET_CNTR, 0x00FF));
}

uint8_t FPC1020::interrupt(bool clear)
{
    return transmit8(clear ? FPC102X_REG_READ_INTERRUPT_WITH_CLEAR : FPC102X_REG_READ_INTERRUPT, 0);
}

uint8_t FPC1020::error()
{
    return transmit8(FPC102X_REG_READ_ERROR_WITH_CLEAR, 0);
}

uint16_t FPC1020::hardware_id()
{
    return transmit16(FPC102X_REG_HWID, 0);
}

uint16_t FPC1020::finger_present_status()
{
    return transmit16(FPC102X_REG_FINGER_PRESENT_STATUS, 0);
}

void FPC1020::command(fpc1020_reg reg)
{
    digitalWrite(PIN_SPI_SS, LOW);
    SPI.write(reg);
    digitalWrite(PIN_SPI_SS, HIGH);
}

uint8_t FPC1020::transmit8(fpc1020_reg reg, uint8_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);
    SPI.write(reg);
    uint8_t ret = SPI.transfer(val);
    digitalWrite(PIN_SPI_SS, HIGH);
    return ret;
}

uint16_t FPC1020::transmit16(fpc1020_reg reg, uint16_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);
    SPI.write(reg);
    uint16_t ret = SPI.transfer16(val);
    digitalWrite(PIN_SPI_SS, HIGH);
    return ret;
}

uint32_t FPC1020::transmit32(fpc1020_reg reg, uint32_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);

    SPI.write(reg);

    uint32_t out = 0;
    uint8_t *pout = (uint8_t *)&out;
    uint8_t *pin = (uint8_t *)&val;

    for (unsigned int i = 0; i < sizeof(uint32_t); i++)
    {
        pout[sizeof(uint32_t) - i - 1] = SPI.transfer(pin[sizeof(uint32_t) - i - 1]);
        // pout[i] = SPI.transfer(pin[i]);
    }

    digitalWrite(PIN_SPI_SS, HIGH);

    return out;
}

uint64_t FPC1020::transmit64(fpc1020_reg reg, uint64_t val)
{
    digitalWrite(PIN_SPI_SS, LOW);

    SPI.write(reg);

    uint64_t out = 0;
    uint8_t *pout = (uint8_t *)&out;
    uint8_t *pin = (uint8_t *)&val;

    for (unsigned int i = 0; i < sizeof(uint64_t); i++)
    {
        pout[sizeof(uint64_t) - i - 1] = SPI.transfer(pin[sizeof(uint64_t) - i - 1]);
        // pout[i] = SPI.transfer(pin[i]);
    }

    digitalWrite(PIN_SPI_SS, HIGH);

    return out;
}
