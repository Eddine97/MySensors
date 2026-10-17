/*
 * MyOTAConfig.h
 *
 * Types, constants and macros shared by the OTA modules.
 *
 * This file is designed to be included by:
 *   - MyOTAFirmwareUpdate.h/.cpp
 *   - MyOTAInternalFlash.h/.cpp
 *   - MyOTAExternalFlash.h/.cpp
 *   - MyOTAMetrics.h
 *
 * It does not depend on any other OTA header, which breaks the dependency
 * cycle and allows free inclusion order in MySensors.h.
 *
 * Copyright (C) 2013-2026 Sensnology AB
 */

#ifndef MyOTAConfig_h
#define MyOTAConfig_h

#include <stdint.h>
#include <string.h>

// ============================================================================
// LOCAL MACRO (shared by all OTA modules)
// ============================================================================
#ifndef LOCAL
  #define LOCAL static
#endif

// ============================================================================
// FIRMWARE BLOCK SIZE
// ============================================================================
#if MAX_PAYLOAD_SIZE >= 22
  #define FIRMWARE_BLOCK_SIZE (16u)
#else
  #define FIRMWARE_BLOCK_SIZE (8u)
  #ifndef FIRMWARE_PROTOCOL_31
    #define FIRMWARE_PROTOCOL_31
  #endif
#endif

// ============================================================================
// RETRY CONFIGURATION
// ============================================================================
#ifndef MY_OTA_RETRY
  #define MY_OTA_RETRY (15u)
#endif
#ifndef MY_OTA_RETRY_DELAY
  #define MY_OTA_RETRY_DELAY (400u)
#endif

// ============================================================================
// FIRMWARE OFFSET (defined after the FLXIMG/FLXIMH macros)
// ============================================================================
// NOTE: FIRMWARE_START_OFFSET is defined below, after
//       FIRMWARE_START_OFFSET_FLXIMG and FIRMWARE_START_OFFSET_FLXIMH.


// ============================================================================
// BOOTLOADER VERSION
// ============================================================================
#define MY_OTA_BOOTLOADER_MAJOR_VERSION (3u)
#ifdef FIRMWARE_PROTOCOL_31
  #define MY_OTA_BOOTLOADER_MINOR_VERSION (1u)
#else
  #define MY_OTA_BOOTLOADER_MINOR_VERSION (0u)
#endif
#define MY_OTA_BOOTLOADER_VERSION \
    (MY_OTA_BOOTLOADER_MINOR_VERSION * 256 + MY_OTA_BOOTLOADER_MAJOR_VERSION)

// ============================================================================
// OTA DEBUG
// ============================================================================
#if defined(MY_DEBUG_VERBOSE_OTA_UPDATE)
  #define OTA_DEBUG(x,...) DEBUG_OUTPUT(x, ##__VA_ARGS__)
#else
  #define OTA_DEBUG(x,...)
#endif


// ============================================================================
// OTA HEADER STRUCTURES
// ============================================================================

/**
 * @brief FLXIMG header — legacy 10-byte format (DualOptiboot compatible)
 *
 * Wire format:
 *   Offset 0-5  : "FLXIMG"
 *   Offset 6    : ':'
 *   Offset 7-8  : image_size (uint16 BE)
 *   Offset 9    : ':'
 *   Offset 10+  : firmware
 */
typedef struct {
    char     signature[6];    // "FLXIMG"
    uint8_t  colon1;          // ':'
    uint16_t image_size;      // big-endian uint16
    uint8_t  colon2;          // ':'
} __attribute__((packed)) firmware_header_t;

/**
 * @brief FLXIMH header — modern 16-byte format (with CRC16)
 *
 * Wire format:
 *   Offset 0-5  : "FLXIMH"
 *   Offset 6    : ':'
 *   Offset 7-10 : image_size (uint32 BE)
 *   Offset 11   : ':'
 *   Offset 12-13: crc16 (uint16 BE)
 *   Offset 14-15: padding (0x00 0x00)
 *   Offset 16+  : firmware
 */
typedef struct {
    char     signature[6];    // "FLXIMH"
    uint8_t  colon1;          // ':'
    uint32_t image_size;      // big-endian uint32
    uint8_t  colon2;          // ':'
    uint16_t crc16;           // big-endian uint16
    uint8_t  reserved[2];     // alignment
} __attribute__((packed)) firmware_header_flximh_t;

// Compile-time size checks
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(firmware_header_t) == 10, "FLXIMG header must be 10 bytes");
_Static_assert(sizeof(firmware_header_flximh_t) == 16, "FLXIMH header must be 16 bytes");
#endif

// ============================================================================
// FIRMWARE OFFSETS
// ============================================================================
#define FIRMWARE_START_OFFSET_FLXIMG  10u
#define FIRMWARE_START_OFFSET_FLXIMH  16u

// Default offset (backward compatibility)
#ifndef MCUBOOT_PRESENT
  #if defined(MY_OTA_INTERNAL_FLASH)
    #define FIRMWARE_START_OFFSET (0u)
  #else
    #define FIRMWARE_START_OFFSET (FIRMWARE_START_OFFSET_FLXIMH)  // or _FLXIMG depending on default
  #endif
#else
  #define FIRMWARE_START_OFFSET (FLASH_AREA_IMAGE_1_OFFSET_0)
#endif



// ============================================================================
// PROTOCOL 3.1 STRUCTURES
// ============================================================================

#ifdef FIRMWARE_PROTOCOL_31
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t block;
    uint16_t blocks;
    uint32_t crc;
    uint16_t BLVersion;
    uint16_t blockSize;
    uint8_t img_commited;
    uint16_t img_revision;
    uint32_t img_build_num;
} __attribute__((packed)) requestFirmwareBlock31_t;
#endif


// ============================================================================
// PROTOCOL STRUCTURES (wire format — do not modify without reason)
// ============================================================================

/**
 * @brief Firmware configuration stored in EEPROM
 */
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t blocks;
    uint16_t crc;
} __attribute__((packed)) nodeFirmwareConfig_t;

/**
 * @brief Firmware configuration request (node -> gateway)
 */
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t blocks;
    uint16_t crc;
    uint16_t BLVersion;
#ifdef FIRMWARE_PROTOCOL_31
    uint8_t  blockSize;
    uint8_t  img_commited;
    uint16_t img_revision;
    uint32_t img_build_num;
#endif
} __attribute__((packed)) requestFirmwareConfig_t;

/**
 * @brief Firmware block request (node -> gateway)
 */
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t block;
} __attribute__((packed)) requestFirmwareBlock_t;

/**
 * @brief Firmware block reply (gateway -> node)
 */
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t block;
    uint8_t data[FIRMWARE_BLOCK_SIZE];
} __attribute__((packed)) replyFirmwareBlock_t;

#ifdef FIRMWARE_PROTOCOL_31
/**
 * @brief Firmware block reply, RLE-encoded (gateway -> node)
 */
typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t block;
    uint16_t number_of_blocks;
    uint8_t data;
} __attribute__((packed)) replyFirmwareBlockRLE_t;
#endif

#endif // MyOTAConfig_h