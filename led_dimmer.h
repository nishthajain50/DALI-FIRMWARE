// led_dimmer.h
// PWM control for LED dimming

#ifndef LED_DIMMER_H
#define LED_DIMMER_H

#include <stdint.h>

// Function prototypes
void LED_Init(void);
void LED_SetBrightness(uint8_t brightness);
void LED_Update(void);
uint8_t LED_GetBrightness(void);

#endif  // LED_DIMMER_H