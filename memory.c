// memory.c
// Flash memory implementation for EEPROM emulation

#include "memory.h"
#include "hardware.h"
#include <string.h>

// ============================================================
// DEFAULT PARAMETERS (factory settings)
// ============================================================
static const DaliParams_t DefaultParams = {
    .magic = EEPROM_MAGIC,
    .crc = 0,
    .short_address = 1,
    .last_brightness = 0,
    .min_level = 0,
    .max_level = 254,
    .power_on_level = 254,
    .fade_time = 0,
    .reserved = {0}
};

// ============================================================
// PRIVATE FLASH PROGRAMMING FUNCTIONS
// ============================================================

// Unlock flash for programming (write unlock keys)
static void FLASH_Unlock(void)
{
    if(FLASH_CR & FLASH_CR_LOCK) {
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;
    }
}

// Lock flash (disable programming)
static void FLASH_Lock(void)
{
    FLASH_CR |= FLASH_CR_LOCK;
}

// Wait for flash operation to complete
// Returns: 1 if ready, 0 if error or timeout
static uint8_t FLASH_WaitForReady(uint32_t timeout_ms)
{
    for(uint32_t i = 0; i < timeout_ms; i++) {
        // Check if busy flag is cleared
        if(!(FLASH_SR & FLASH_SR_BSY)) {
            // Clear any error flags
            if(FLASH_SR & FLASH_SR_PGERR) {
                FLASH_SR |= FLASH_SR_PGERR;
                return 0;
            }
            if(FLASH_SR & FLASH_SR_WRPRTERR) {
                FLASH_SR |= FLASH_SR_WRPRTERR;
                return 0;
            }
            return 1;
        }
        // Small delay before checking again
        for(uint32_t j = 0; j < 10000; j++) {
            __asm("nop");
        }
    }
    return 0;  // Timeout
}

// Erase one page of flash (1KB)
// Returns: 1 if successful, 0 if error
static uint8_t FLASH_ErasePage(uint32_t address)
{
    FLASH_Unlock();
    
    // Wait for any ongoing operation
    if(!FLASH_WaitForReady(100)) return 0;
    
    // Enable page erase mode
    FLASH_CR |= FLASH_CR_PER;
    
    // Set the address to erase
    FLASH_AR = address;
    
    // Start erase operation
    FLASH_CR |= FLASH_CR_STRT;
    
    // Wait for completion
    uint8_t result = FLASH_WaitForReady(100);
    
    // Disable page erase mode
    FLASH_CR &= ~FLASH_CR_PER;
    
    FLASH_Lock();
    return result;
}

// Program a 32-bit word into flash
// Returns: 1 if successful, 0 if error
static uint8_t __attribute__((unused)) FLASH_ProgramWord(uint32_t address, uint32_t data)
{
    FLASH_Unlock();
    
    // Wait for any ongoing operation
    if(!FLASH_WaitForReady(100)) return 0;
    
    // Enable programming mode
    FLASH_CR |= FLASH_CR_PG;
    
    // Write the data
    *(volatile uint32_t*)address = data;
    
    // Wait for completion
    uint8_t result = FLASH_WaitForReady(100);
    
    // Disable programming mode
    FLASH_CR &= ~FLASH_CR_PG;
    
    FLASH_Lock();
    return result;
}

// Program a 16-bit half-word into flash
static uint8_t FLASH_ProgramHalfWord(uint32_t address, uint16_t data)
{
    FLASH_Unlock();
    
    if(!FLASH_WaitForReady(100)) return 0;
    
    FLASH_CR |= FLASH_CR_PG;
    *(volatile uint16_t*)address = data;
    
    uint8_t result = FLASH_WaitForReady(100);
    
    FLASH_CR &= ~FLASH_CR_PG;
    FLASH_Lock();
    return result;
}

// Read a 16-bit half-word from flash
static uint16_t FLASH_ReadHalfWord(uint32_t address)
{
    return *(volatile uint16_t*)address;
}

// ============================================================
// CRC CALCULATION (for data validation)
// ============================================================
// Simple CRC-16 (CCITT) calculation
static uint16_t CalculateCRC(DaliParams_t *params)
{
    uint16_t crc = 0xFFFF;
    uint8_t* data = (uint8_t*)params;
    uint16_t length = sizeof(DaliParams_t) - 4;  // Exclude magic and CRC fields
    
    for(uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++) {
            if(crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

// ============================================================
// PUBLIC FUNCTIONS
// ============================================================

// Initialize memory module
void MEMORY_Init(void)
{
    // Nothing to initialize - flash is already mapped
    // Just verify flash is accessible
    FLASH_ReadHalfWord(EEPROM_START_ADDR);
}

// Save parameters to flash (EEPROM emulation)
// Returns: 1 if successful, 0 if error
uint8_t MEMORY_Save(DaliParams_t *params)
{
    uint32_t address = EEPROM_START_ADDR;
    
    // Calculate CRC before saving
    params->magic = EEPROM_MAGIC;
    params->crc = CalculateCRC(params);
    
    // Erase the page first
    if(!FLASH_ErasePage(EEPROM_START_ADDR)) {
        return 0;
    }
    
    // Program each half-word (16 bits at a time)
    uint16_t* data = (uint16_t*)params;
    uint16_t count = sizeof(DaliParams_t) / 2;
    
    for(uint16_t i = 0; i < count; i++) {
        if(!FLASH_ProgramHalfWord(address + (i * 2), data[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Load parameters from flash
// Returns: 1 if valid data loaded, 0 if using defaults
uint8_t MEMORY_Load(DaliParams_t *params)
{
    DaliParams_t* flash_params = (DaliParams_t*)EEPROM_START_ADDR;
    
    // Check if magic number matches (valid data)
    if(flash_params->magic == EEPROM_MAGIC) {
        // Copy from flash
        memcpy(params, flash_params, sizeof(DaliParams_t));
        
        // Verify CRC
        uint16_t saved_crc = params->crc;
        params->crc = 0;
        uint16_t calculated_crc = CalculateCRC(params);
        params->crc = saved_crc;
        
        if(saved_crc == calculated_crc) {
            return 1;  // Valid data loaded
        }
    }
    
    // Invalid or uninitialized - use defaults
    MEMORY_ResetToDefaults(params);
    return 0;
}

// Check if flash contains valid parameters
// Returns: 1 if valid, 0 if invalid
uint8_t MEMORY_IsValid(void)
{
    DaliParams_t* flash_params = (DaliParams_t*)EEPROM_START_ADDR;
    
    if(flash_params->magic != EEPROM_MAGIC) {
        return 0;
    }
    
    // Verify CRC
    uint16_t saved_crc = flash_params->crc;
    DaliParams_t temp = *flash_params;
    temp.crc = 0;
    uint16_t calculated_crc = CalculateCRC(&temp);
    
    return (saved_crc == calculated_crc);
}

// Reset parameters to factory defaults
void MEMORY_ResetToDefaults(DaliParams_t *params)
{
    memcpy(params, &DefaultParams, sizeof(DaliParams_t));
    params->crc = CalculateCRC(params);
}