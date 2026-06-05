// dali_machine.h
#ifndef DALI_MACHINE_H
#define DALI_MACHINE_H

#include <stdint.h>

// Public function prototypes
void DALI_Machine_Init(void);
void DALI_SendBit(uint8_t bit);
void DALI_SendByte(uint8_t data);
uint8_t DALI_ReceiveBit(void);
uint8_t DALI_ReceiveByte(uint32_t timeout_ms);
uint8_t DALI_IsDataAvailable(void);

#endif // DALI_MACHINE_H