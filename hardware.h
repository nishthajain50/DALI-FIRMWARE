// hardware.h
// Pin definitions, register addresses, and constants for LKS32MC03x

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// ============================================================
// PIN DEFINITIONS (Based on your pin diagram)
// ============================================================
// Pin 5  - PO_0  -> DALI RX (Receive)
// Pin 6  - PO_2  -> DALI TX (Transmit)
// Pin 2  - P1_8  -> PWM Output (LED Dimming)
// Pin 7  - PO_1  -> Status LED (optional)

#define DALI_RX_PIN         3       // PO_0 on Pin 5
#define DALI_TX_PIN         19    // PO_2 on Pin 6
#define STATUS_LED_PIN      4      // PO_1 on Pin 7
#define PWM_OUT_PIN         10     // P1_8 on Pin 2

// ============================================================
// GPIO PORT 0 REGISTERS (for PO_0, PO_1, PO_2)
// ============================================================
#define GPIOA_BASE          0x40020000
#define GPIOA_MODER         (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_OTYPER        (*(volatile uint32_t*)(GPIOA_BASE + 0x04))
#define GPIOA_PUPDR         (*(volatile uint32_t*)(GPIOA_BASE + 0x0C))
#define GPIOA_IDR           (*(volatile uint32_t*)(GPIOA_BASE + 0x10))
#define GPIOA_ODR           (*(volatile uint32_t*)(GPIOA_BASE + 0x14))

// ============================================================
// GPIO PORT 1 REGISTERS (for P1_8)
// ============================================================
#define GPIOB_BASE          0x40020040
#define GPIOB_MODER         (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER        (*(volatile uint32_t*)(GPIOB_BASE + 0x04))
#define GPIOB_ODR           (*(volatile uint32_t*)(GPIOB_BASE + 0x14))

// ============================================================
// RCC (Reset and Clock Control) REGISTERS
// ============================================================
#define RCC_BASE            0x40021000
#define RCC_AHBENR          (*(volatile uint32_t*)(RCC_BASE + 0x14))

#define RCC_GPIOA_EN        (1 << 17)   // Enable clock for GPIO Port 0
#define RCC_GPIOB_EN        (1 << 18)   // Enable clock for GPIO Port 1

// ============================================================
// DALI TIMING CONSTANTS
// ============================================================
// DALI runs at 1200 baud
// 1 bit = 1/1200 = 833 microseconds
#define HALF_BIT_US         416         // Half bit period
#define FULL_BIT_US         833         // Full bit period

// ============================================================
// DALI COMMAND DEFINITIONS (from IEC 62386 standard)
// ============================================================
#define DALI_CMD_OFF        0x00        // Turn off
#define DALI_CMD_UP         0x01        // Increase brightness
#define DALI_CMD_DOWN       0x02        // Decrease brightness
#define DALI_CMD_STEP_UP    0x03        // Step up (fast)
#define DALI_CMD_STEP_DOWN  0x04        // Step down (fast)
#define DALI_CMD_RECALL_MAX 0x05        // Maximum brightness
#define DALI_CMD_RECALL_MIN 0x06        // Minimum brightness
#define DALI_CMD_SET_LEVEL  0x11        // Set specific level

// ============================================================
// DALI ADDRESS TYPES
// ============================================================
#define BROADCAST_ADDRESS   255         // Command to all devices
#define SHORT_ADDRESS_MIN   0           // Minimum individual address
#define SHORT_ADDRESS_MAX   63          // Maximum individual address

// ============================================================
// FLASH MEMORY ADDRESSES
// ============================================================
#define FLASH_BASE          0x00000000      // Flash start address
#define FLASH_INFO_BASE     0x00008000      // Info block (Unique ID)
#define FLASH_SIZE_KB       32              // Total flash size: 32KB
#define FLASH_PAGE_SIZE     1024            // Each page: 1KB
#define EEPROM_START_ADDR   0x00007C00      // Last page (for EEPROM emulation)
#define EEPROM_MAGIC        0x5A3C          // Magic number for validation

// ============================================================
// FLASH CONTROL REGISTERS
// ============================================================
#define FLASH_CTRL_BASE     0x40022000
#define FLASH_KEYR          (*(volatile uint32_t*)(FLASH_CTRL_BASE + 0x00))
#define FLASH_SR            (*(volatile uint32_t*)(FLASH_CTRL_BASE + 0x04))
#define FLASH_CR            (*(volatile uint32_t*)(FLASH_CTRL_BASE + 0x08))
#define FLASH_AR            (*(volatile uint32_t*)(FLASH_CTRL_BASE + 0x0C))

// Flash Control Register bits
#define FLASH_CR_PG         (1 << 0)        // Programming enable
#define FLASH_CR_PER        (1 << 1)        // Page erase enable
#define FLASH_CR_MER        (1 << 2)        // Mass erase enable
#define FLASH_CR_STRT       (1 << 16)       // Start operation
#define FLASH_CR_LOCK       (1 << 31)       // Lock flash

// Flash Status Register bits
#define FLASH_SR_BSY        (1 << 0)        // Busy
#define FLASH_SR_PGERR      (1 << 2)        // Programming error
#define FLASH_SR_WRPRTERR   (1 << 4)        // Write protection error

// Flash unlock keys (from LKS32 datasheet)
#define FLASH_KEY1          0x45670123
#define FLASH_KEY2          0xCDEF89AB

#endif  // HARDWARE_H 