// memory.h
// Flash memory management for EEPROM emulation

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// ============================================================
// DALI PARAMETERS STRUCTURE (saved in flash)
// ============================================================
typedef struct {
    uint16_t magic;             // Validation magic number (0x5A3C)
    uint16_t crc;               // Checksum for validation
    uint8_t short_address;      // DALI address (1-63)
    uint8_t last_brightness;    // Last brightness level (0-254)
    uint8_t min_level;          // Minimum brightness
    uint8_t max_level;          // Maximum brightness
    uint8_t power_on_level;     // Level when power returns
    uint8_t fade_time;          // Fade time (0-15)
    uint8_t reserved[8];        // For future use
} DaliParams_t;

// ============================================================
// FUNCTION PROTOTYPES
// ============================================================
void MEMORY_Init(void);
uint8_t MEMORY_Save(DaliParams_t *params);
uint8_t MEMORY_Load(DaliParams_t *params);
uint8_t MEMORY_IsValid(void);
void MEMORY_ResetToDefaults(DaliParams_t *params);

#endif  // MEMORY_H