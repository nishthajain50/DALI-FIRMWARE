// dali_protocol.c
// DALI command handling implementation

#include "dali_protocol.h"
#include "led_dimmer.h"
#include "hardware.h"

// ============================================================
// GLOBAL VARIABLES
// ============================================================
DaliDevice_t DaliDevice;

// ============================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================

// Simple delay in milliseconds (CPU cycle based)
static void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) {
        for(uint32_t j = 0; j < 8000; j++) {
            __asm("nop");
        }
    }
}

// Status LED control (PO_1 on Pin 7)
static void StatusLED_Init(void)
{
    // Enable clock for GPIO Port 0
    RCC_AHBENR |= RCC_GPIOA_EN;
    // Configure as output
    GPIOA_MODER &= ~(0x03 << (STATUS_LED_PIN * 2));
    GPIOA_MODER |= (0x01 << (STATUS_LED_PIN * 2));
    GPIOA_ODR &= ~(1 << STATUS_LED_PIN);  // Start OFF
}

static void StatusLED_On(void)
{
    GPIOA_ODR |= (1 << STATUS_LED_PIN);
}

static void StatusLED_Off(void)
{
    GPIOA_ODR &= ~(1 << STATUS_LED_PIN);
}

// ============================================================
// PUBLIC FUNCTIONS
// ============================================================

// Initialize the DALI protocol handler
void DALI_Protocol_Init(uint8_t address)
{
    // Initialize status LED
    StatusLED_Init();
    
    // Initialize device structure
    DaliDevice.short_address = address;
    DaliDevice.brightness = 0;
    DaliDevice.target_brightness = 0;
    DaliDevice.power_on = 0;
    DaliDevice.min_level = 0;
    DaliDevice.max_level = 254;
    DaliDevice.fade_time = 0;
}

// Check if a DALI address belongs to this device
uint8_t DALI_IsMyAddress(uint8_t address)
{
    // Check individual short address
    if(address == DaliDevice.short_address) {
        return 1;
    }
    // Check broadcast address
    if(address == BROADCAST_ADDRESS) {
        return 1;
    }
    return 0;
}

// Process a DALI command
void DALI_ProcessCommand(uint8_t address, uint8_t command)
{
    // Check if command is for this device
    if(!DALI_IsMyAddress(address)) {
        return;
    }
    
    // Show activity on status LED
    StatusLED_On();
    
    switch(command) {
        case DALI_CMD_OFF:
            // Turn off
            DaliDevice.target_brightness = 0;
            DaliDevice.power_on = 0;
            LED_SetBrightness(0);
            break;
        case DALI_CMD_UP:
            // Increase brightness by 1 step
            if(DaliDevice.target_brightness < DaliDevice.max_level) {
                DaliDevice.target_brightness++;
                DaliDevice.power_on = 1;
                LED_SetBrightness(DaliDevice.target_brightness);
            }
            break;
            
        case DALI_CMD_DOWN:
            // Decrease brightness by 1 step
            if(DaliDevice.target_brightness > DaliDevice.min_level) {
                DaliDevice.target_brightness--;
                if(DaliDevice.target_brightness == 0) {
                    DaliDevice.power_on = 0;
                }
                LED_SetBrightness(DaliDevice.target_brightness);
            }
            break;
            
        case DALI_CMD_RECALL_MAX:
            // Go to maximum brightness
            DaliDevice.target_brightness = DaliDevice.max_level;
            DaliDevice.power_on = 1;
            LED_SetBrightness(DaliDevice.target_brightness);
            break;
            
        case DALI_CMD_RECALL_MIN:
            // Go to minimum brightness
            DaliDevice.target_brightness = DaliDevice.min_level;
            DaliDevice.power_on = (DaliDevice.min_level > 0);
            LED_SetBrightness(DaliDevice.target_brightness);
            break;
            
        default:
            // Unknown command - ignore
            break;
    }
    
    // Update current brightness
    DaliDevice.brightness = DaliDevice.target_brightness;
    
    // Keep LED on briefly to show activity
    delay_ms(50);
    StatusLED_Off();
}

// Set brightness directly (0-254)
void DALI_SetBrightness(uint8_t brightness)
{
    if(brightness > DaliDevice.max_level) {
        brightness = DaliDevice.max_level;
    }
    DaliDevice.target_brightness = brightness;
    DaliDevice.power_on = (brightness > 0);
    LED_SetBrightness(brightness);
    DaliDevice.brightness = brightness;
}

// Get current brightness
uint8_t DALI_GetBrightness(void)
{
    return DaliDevice.brightness;
}

// Save current state to EEPROM (will be implemented in memory.c)
void DALI_SaveState(void)
{
    // This function is called from main.c
    // The actual saving is done in memory.c
}