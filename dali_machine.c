
// dali_machine.c
// Manchester encoding/decoding implementation for LKS32MC033H6P8B

#include "dali_machine.h"
#include "hardware.h"
#include "lks32mc03x.h"

// ============================================================
// PIN DEFINITIONS FOR LKS32MC033H6P8B
// ============================================================
// Adjust these based on your actual hardware connections


// DALI timing constants (bit period = 1.667ms at 1200 baud)
// 1.667ms = 1667 microseconds


// ============================================================
// PRIVATE GPIO FUNCTIONS (static = only visible in this file)
// ============================================================

// Initialize DALI pins (RX = input with pull-up, TX = output)
void SystemInit(void)
{
    // Called by startup code before main()
    __disable_irq();
    __enable_irq();
}

static void GPIO_Init_DALI(void)
{
    // ----- Configure DALI RX pin (P0_DALI_RX_PIN) as INPUT with PULL-UP -----
    // Disable output (set as input)
    GPIO0_POE &= ~(1 << DALI_RX_PIN);    // POE = Output Enable (0 = input)
    
    // Enable input
    GPIO0_PIE |= (1 << DALI_RX_PIN);     // PIE = Input Enable
    
    // Enable pull-up
    GPIO0_PUE |= (1 << DALI_RX_PIN);     // PUE = Pull-Up Enable
    
    // Disable open-drain (use push-pull for input)
    GPIO0_PODE &= ~(1 << DALI_RX_PIN);   // PODE = Open-Drain Enable
    
    // ----- Configure DALI TX pin (P0_DALI_TX_PIN) as OUTPUT push-pull -----
    // Enable output
    GPIO0_POE |= (1 << DALI_TX_PIN);     // POE = Output Enable (1 = output)
    
    // Enable input (optional, but good practice)
    GPIO0_PIE |= (1 << DALI_TX_PIN);     // PIE = Input Enable
    
    // Disable pull-up/pull-down for output
    GPIO0_PUE &= ~(1 << DALI_TX_PIN);    // PUE = Pull-Up Enable (disabled)
    
    // Disable open-drain (use push-pull)
    GPIO0_PODE &= ~(1 << DALI_TX_PIN);   // PODE = Open-Drain Enable (0 = push-pull)
    
    // Set initial HIGH (DALI idle state is HIGH)
    GPIO0_BSRR = (1 << DALI_TX_PIN);     // BSRR = Bit Set Register
}

// Write a value to the DALI TX pin
static void GPIO_Write_TX(uint8_t value)
{
    if(value) {
        GPIO0_BSRR = (1 << DALI_TX_PIN);    // Set HIGH using BSRR
    } else {
        GPIO0_BRR = (1 << DALI_TX_PIN);     // Set LOW using BRR
    }
}

// Read the value from the DALI RX pin
static uint8_t GPIO_Read_RX(void)
{
    return (GPIO0_PDI >> DALI_RX_PIN) & 0x01;
}

// ============================================================
// SIMPLE DELAY FUNCTIONS (using CPU cycles)
// ============================================================

// Microsecond delay (approximate for 48MHz clock)
static void delay_us(uint32_t us)
{
    // For 48MHz, 1us ˜ 48 cycles
    // Adjust the loop count based on your actual system clock
    for(uint32_t i = 0; i < us; i++) {
        for(uint8_t j = 0; j < 48; j++) {  // 48 cycles per us at 48MHz
            __asm("nop");
        }
    }
}

// Millisecond delay
static void __attribute__((unused)) delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++) {
        delay_us(1000);  // Use delay_us for better accuracy
    }
}

// ============================================================
// PUBLIC MANCHESTER FUNCTIONS
// ============================================================

// Initialize the DALI Manchester module
void DALI_Machine_Init(void)
{
    GPIO_Init_DALI();
}

// Send one Manchester-encoded bit
// bit = 0 or 1
void DALI_SendBit(uint8_t bit)
{
    if(bit == 1) {
        // Logic '1': Rising edge at middle of bit period (LOW -> HIGH)
        GPIO_Write_TX(0);               // Start LOW
        delay_us(HALF_BIT_US);          // Wait half bit period
        GPIO_Write_TX(1);               // Transition to HIGH
        delay_us(HALF_BIT_US);          // Wait remaining half bit
    } else {
        // Logic '0': Falling edge at middle of bit period (HIGH -> LOW)
        GPIO_Write_TX(1);               // Start HIGH
        delay_us(HALF_BIT_US);          // Wait half bit period
        GPIO_Write_TX(0);               // Transition to LOW
        delay_us(HALF_BIT_US);          // Wait remaining half bit
    }
}

// Send one complete byte with Manchester encoding
void DALI_SendByte(uint8_t data)
{
    // Send START bit (always logic '1' for DALI)
    DALI_SendBit(1);
    
    // Send 8 data bits (Most Significant Bit first)
    for(int i = 7; i >= 0; i--) {
        DALI_SendBit((data >> i) & 0x01);
    }
    
    // STOP condition: idle HIGH for 2 bit periods
    GPIO_Write_TX(1);
    delay_us(FULL_BIT_US * 2);
}

// Receive one Manchester-encoded bit
// Returns: 0 or 1 for valid bit, 2 for error
uint8_t DALI_ReceiveBit(void)
{
    uint8_t first_level, second_level;
    
    // Sample at start of bit period
    first_level = GPIO_Read_RX();
    delay_us(HALF_BIT_US);
    
    // Sample at middle of bit period (where transition occurs)
    second_level = GPIO_Read_RX();
    delay_us(HALF_BIT_US);
    
    // Detect transition direction
    if(first_level == 0 && second_level == 1) {
        return 1;   // Rising edge = Logic '1'
    } else if(first_level == 1 && second_level == 0) {
        return 0;   // Falling edge = Logic '0'
    } else {
        return 2;   // Error - no valid transition detected
    }
}

// Receive one complete byte with timeout
// Returns: received byte or 0xFF if timeout/error
uint8_t DALI_ReceiveByte(uint32_t timeout_ms)
{
    uint8_t data = 0;
    uint32_t timeout = 0;
    
    // Wait for falling edge (start of DALI frame)
    while(GPIO_Read_RX() == 1) {
        timeout++;
        if(timeout > timeout_ms * 1000) {
            return 0xFF;    // Timeout - no data
        }
        delay_us(1);
    }
    
    // Verify START bit (should be logic '1')
    delay_us(HALF_BIT_US);
    if(GPIO_Read_RX() == 0) {
        return 0xFF;        // Invalid start bit
    }
    delay_us(HALF_BIT_US);
    
    // Receive 8 data bits (MSB first)
    for(int i = 7; i >= 0; i--) {
        uint8_t bit = DALI_ReceiveBit();
        if(bit == 2) {
            return 0xFF;    // Manchester encoding error
        }
        data |= (bit << i);
    }
    
    // Wait for STOP bits (2 bit periods of idle HIGH)
    delay_us(FULL_BIT_US * 2);
    
    return data;
}

// Check if DALI data is available (non-blocking)
// Returns: 1 if data is available, 0 if not
uint8_t DALI_IsDataAvailable(void)
{
    // DALI frame starts with a falling edge (HIGH -> LOW)
    // Check if RX pin is LOW
    if(GPIO_Read_RX() == 0) {
        return 1;
    }
    return 0;
}