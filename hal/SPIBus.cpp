#include "SPIBus.h"

#if defined(MY_RF24_CS_PIN) && defined(MY_OTA_FLASH_SS) && \
    !defined(MY_OTA_FLASH_ON_SEPARATE_SPI_BUS)

static spi_device_t current_spi_dev = SPI_DEV_NONE;

void hwSpiInit(void)
{
    static bool initialized = false;
    if (initialized) return;

    hwPinMode(MY_RF24_CS_PIN, OUTPUT);
    hwPinMode(MY_OTA_FLASH_SS, OUTPUT);
    hwDigitalWrite(MY_RF24_CS_PIN, HIGH);
    hwDigitalWrite(MY_OTA_FLASH_SS, HIGH);
    current_spi_dev = SPI_DEV_NONE;
    initialized = true;
}

void hwSpiSelectDevice(const spi_device_t dev)
{
    if (current_spi_dev == dev) return;

    switch (dev) {
    case SPI_DEV_RF24:
        hwDigitalWrite(MY_OTA_FLASH_SS, HIGH);
        delayMicroseconds(SPI_BUS_SWITCH_DELAY_US);
        hwDigitalWrite(MY_RF24_CS_PIN, LOW);
        break;
    case SPI_DEV_FLASH:
        hwDigitalWrite(MY_RF24_CS_PIN, HIGH);
        delayMicroseconds(SPI_BUS_SWITCH_DELAY_US);
        hwDigitalWrite(MY_OTA_FLASH_SS, LOW);
        break;
    case SPI_DEV_NONE:
    default:
        hwDigitalWrite(MY_RF24_CS_PIN, HIGH);
        hwDigitalWrite(MY_OTA_FLASH_SS, HIGH);
        break;
    }
    current_spi_dev = dev;
}

#endif // MY_RF24_CS_PIN && MY_OTA_FLASH_SS && !MY_OTA_FLASH_ON_SEPARATE_SPI_BUS