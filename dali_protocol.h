// dali_protocol.h
// DALI command handling and device management

#ifndef DALI_PROTOCOL_H
#define DALI_PROTOCOL_H

#include <stdint.h>

// ============================================================
// DALI DEVICE STRUCTURE
// ============================================================
// Stores all settings for this DALI device
typedef struct {
    uint8_t short_address;      // DALI address (1-63)
    uint8_t brightness;         // Current brightness (0-254)
    uint8_t target_brightness;  // Target during fade
    uint8_t power_on;           // 0=off, 1=on
    uint8_t min_level;          // Minimum brightness (0-254)
    uint8_t max_level;          // Maximum brightness (0-254)
    uint8_t fade_time;          // Fade time (0-15)
} DaliDevice_t;

// ============================================================
// FUNCTION PROTOTYPES
// ============================================================
void DALI_Protocol_Init(uint8_t address);
void DALI_ProcessCommand(uint8_t address, uint8_t command);
uint8_t DALI_IsMyAddress(uint8_t address);
void DALI_SetBrightness(uint8_t brightness);
uint8_t DALI_GetBrightness(void);
void DALI_SaveState(void);

// Make device available to other files
extern DaliDevice_t DaliDevice;

#endif  // DALI_PROTOCOL_H