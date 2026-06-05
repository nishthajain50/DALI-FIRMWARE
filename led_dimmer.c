// led_dimmer.c
// Optimized software PWM for LED dimming

#include "led_dimmer.h"
#include "lks32mc03x.h"

// ============================================================
// PRIVATE VARIABLES
// ============================================================

// PWM duty (0-49)
static volatile uint16_t pwm_duty = 0;

// PWM counter
static volatile uint16_t pwm_counter = 0;

// Brightness value
static uint8_t current_brightness = 0;

// ============================================================
// GPIO INIT
// ============================================================

static void PWM_GPIO_Init(void)
{
    // Enable GPIO clock
    SYS_CLK_FEN |= (1 << MCLK_RCG_B_GPIO);

    // Unlock GPIO config
    SYS_WR_PROTECT = 0x7a83;

    // Configure pins as GPIO
    GPIO0_F7654 = 0x00000000;

    // Lock protection
    SYS_WR_PROTECT = 0;

    // Enable output on pin
    GPIO0_POE |= (1 << 10);

    // Start LOW
    GPIO0_PDO &= ~(1 << 10);
}

// ============================================================
// PWM WRITE
// ============================================================

static void PWM_Write(uint8_t value)
{
    if(value)
    {
        GPIO0_PDO |= (1 << 10);
    }
    else
    {
        GPIO0_PDO &= ~(1 << 10);
    }
}

// ============================================================
// PUBLIC FUNCTIONS
// ============================================================

// ------------------------------------------------------------
// Initialize dimmer
// ------------------------------------------------------------
void LED_Init(void)
{
    PWM_GPIO_Init();

    pwm_duty = 0;
    pwm_counter = 0;
    current_brightness = 0;
}

// ------------------------------------------------------------
// Set brightness
// 0 ? 254
// ------------------------------------------------------------
void LED_SetBrightness(uint8_t brightness)
{
    if(brightness > 254)
    {
        brightness = 254;
    }

    current_brightness = brightness;

    // Convert brightness to PWM duty
    // 0-254 ? 0-49
    pwm_duty = (brightness * 49) / 254;

    // Immediate OFF
    if(brightness == 0)
    {
        PWM_Write(0);
    }
}

// ------------------------------------------------------------
// Generate software PWM
// Called from SysTick interrupt
// ------------------------------------------------------------
void LED_Update(void)
{
    // Increment counter
    pwm_counter++;

    // 50 PWM steps
    if(pwm_counter >= 50)
    {
        pwm_counter = 0;
    }

    // PWM generation
    if(pwm_counter < pwm_duty)
    {
        PWM_Write(1);
    }
    else
    {
        PWM_Write(0);
    }
}

// ------------------------------------------------------------
// Get brightness
// ------------------------------------------------------------
uint8_t LED_GetBrightness(void)
{
    return current_brightness;
}












// led_dimmer.c
// Software PWM LED dimmer for LKS32MC03x

/*#include "led_dimmer.h"
#include "lks32mc03x.h"

// ============================================================
// PRIVATE VARIABLES
// ============================================================

// PWM duty cycle (0-99)
static volatile uint16_t pwm_duty = 0;

// PWM counter (0-99)
static volatile uint16_t pwm_counter = 0;

// Current brightness (0-254)
static uint8_t current_brightness = 0;

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

// ------------------------------------------------------------
// Initialize PWM GPIO
// ------------------------------------------------------------
static void PWM_GPIO_Init(void)
{
    // Enable GPIO clock
    SYS_CLK_FEN |= (1 << 7);

    // Unlock GPIO configuration
    SYS_WR_PROTECT = 0x7a83;

    // Configure pins as GPIO
    GPIO0_F7654 = 0x00000000;

    // Lock protection
    SYS_WR_PROTECT = 0;

    // Enable Pin 6 output
    GPIO0_POE |= (1 << 10);

    // Start LOW
    GPIO0_PDO &= ~(1 << 10);
}

// ------------------------------------------------------------
// Write PWM output
// ------------------------------------------------------------
static void PWM_Write(uint8_t value)
{
    if(value)
    {
        GPIO0_PDO |= (1 << 10);
    }
    else
    {
        GPIO0_PDO &= ~(1 << 10);
    }
}

// ============================================================
// PUBLIC FUNCTIONS
// ============================================================

// ------------------------------------------------------------
// Initialize LED dimmer
// ------------------------------------------------------------
void LED_Init(void)
{
    PWM_GPIO_Init();

    pwm_duty = 0;
    pwm_counter = 0;
    current_brightness = 0;
}

// ------------------------------------------------------------
// Set brightness
// 0 = OFF
// 254 = FULL BRIGHTNESS
// ------------------------------------------------------------
void LED_SetBrightness(uint8_t brightness)
{
    // Limit brightness
    if(brightness > 254)
    {
        brightness = 254;
    }

    current_brightness = brightness;

    // Convert brightness to PWM duty
    // 0-254 ? 0-99
    pwm_duty = (brightness * 19) / 254;

    // Immediate OFF optimization
    if(brightness == 0)
    {
        PWM_Write(0);
    }
}

// ------------------------------------------------------------
// Generate software PWM
// Call repeatedly
// ------------------------------------------------------------
void LED_Update(void)
{
    // Increment PWM counter
    pwm_counter++;

    // Reset after 100 counts
    if(pwm_counter >= 20)
    {
        pwm_counter = 0;
    }

    // PWM generation
    if(pwm_counter < pwm_duty)
    {
        PWM_Write(1);
    }
    else
    {
        PWM_Write(0);
    }
}

// ------------------------------------------------------------
// Get current brightness
// ------------------------------------------------------------
uint8_t LED_GetBrightness(void)
{
    return current_brightness;
}
*/









// led_dimmer.c
// Software PWM implementation for LED dimming
/*
#include "led_dimmer.h"
#include "hardware.h"

// ============================================================
// PRIVATE VARIABLES
// ============================================================
static volatile uint16_t pwm_duty = 0;      // Current duty cycle (0-1000)
static volatile uint16_t pwm_counter = 0;   // Current position in PWM cycle
static uint8_t current_brightness = 0;      // Current brightness (0-254)

// ============================================================
// PRIVATE FUNCTIONS
// ============================================================

// Initialize the PWM pin (P1_8)
static void PWM_GPIO_Init(void)
{
    // Enable clock for GPIO Port 1
    RCC_AHBENR |= RCC_GPIOB_EN;
    
    // Configure P1_8 as output push-pull
    GPIOB_MODER &= ~(0x03 << (PWM_OUT_PIN * 2));
    GPIOB_MODER |= (0x01 << (PWM_OUT_PIN * 2));   // Output mode
    GPIOB_OTYPER &= ~(1 << PWM_OUT_PIN);           // Push-pull (0)
    GPIOB_ODR &= ~(1 << PWM_OUT_PIN);              // Start LOW
}

// Write to the PWM pin (HIGH or LOW)
static void PWM_Write(uint8_t value)
{
    if(value) {
        GPIOB_ODR |= (1 << PWM_OUT_PIN);     // Set HIGH
    } else {
        GPIOB_ODR &= ~(1 << PWM_OUT_PIN);    // Set LOW
    }
}

// ============================================================
// PUBLIC FUNCTIONS
// ============================================================

// Initialize LED dimmer module
void LED_Init(void)
{
    PWM_GPIO_Init();
    pwm_duty = 0;
    pwm_counter = 0;
    current_brightness = 0;
}

// Set brightness level (0-254)
// 0 = off, 254 = maximum brightness
void LED_SetBrightness(uint8_t brightness)
{
    if(brightness > 254) {
        brightness = 254;
    }
    current_brightness = brightness;
    
    // Convert 0-254 to 0-1000 duty cycle
    // (brightness * 1000) / 254
    pwm_duty = (brightness * 99) / 254;
    
    // If brightness is 0, turn off completely to save power
    if(brightness == 0) {
        PWM_Write(0);
    }
}

// Update PWM output - call this function regularly (every ~100us)
// This creates the PWM waveform by turning the pin ON/OFF
void LED_Update(void)
{
    // Increment cycle counter
    pwm_counter++;
    
    // Reset at end of cycle (1000 counts = 100% period)
    if(pwm_counter >= 100) {
        pwm_counter = 0;
    }
    
    // Turn ON for the duty cycle portion, OFF for the rest
    if(pwm_counter < pwm_duty) {
        PWM_Write(1);    // ON
    } else {
        PWM_Write(0);    // OFF
    }
}

// Get current brightness value
uint8_t LED_GetBrightness(void)
{
    return current_brightness;
}
*/