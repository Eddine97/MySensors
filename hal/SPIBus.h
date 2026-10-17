#ifndef SPIBus_h
#define SPIBus_h

#include <stdint.h>
#include "MyConfig.h"

// ----------------------------------------------------------------------------
// Arbitrage CS entre RF24 et FLASH
// ----------------------------------------------------------------------------
// Cet arbitrage n'a de sens QUE si les deux devices partagent le même bus SPI.
// Sur une carte où le RF24 et la flash OTA sont sur DEUX bus distincts (ex.
// Pink Board : RF24 sur SPI1, W25Q sur SPI2), l'arbitrage est inutile et peut
// même être nuisible (il pilote le CS d'un device pendant qu'on parle à un
// autre sur un bus différent).
//
// Pour désactiver l'arbitrage, définir dans le sketch :
//     #define MY_OTA_FLASH_ON_SEPARATE_SPI_BUS
// ----------------------------------------------------------------------------

#if defined(MY_RF24_CS_PIN) && defined(MY_OTA_FLASH_SS) && \
    !defined(MY_OTA_FLASH_ON_SEPARATE_SPI_BUS)

typedef enum {
    SPI_DEV_NONE = 0,
    SPI_DEV_RF24,
    SPI_DEV_FLASH
} spi_device_t;

#ifndef SPI_BUS_SWITCH_DELAY_US
#define SPI_BUS_SWITCH_DELAY_US 2
#endif

void hwSpiInit(void);
void hwSpiSelectDevice(const spi_device_t dev);

#else

// Bus séparés ou fonctionnalité désactivée : no-op
#define hwSpiInit()                        do { } while (0)
#define hwSpiSelectDevice(dev)             do { } while (0)

#endif

#endif