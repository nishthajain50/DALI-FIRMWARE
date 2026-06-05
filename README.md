# DALI LED Dimmer Firmware using LKS32MC033H6P8B

## Overview

This project implements a DALI (Digital Addressable Lighting Interface) LED dimmer firmware using the LKS32MC033H6P8B microcontroller.

The firmware receives DALI commands, decodes the incoming frames, and controls LED brightness using PWM output.

The project was initially developed and verified in Keil uVision simulation and is being extended for real hardware testing.

---

## Features

- DALI frame reception
- DALI command decoding
- LED brightness control through PWM
- Dimming level adjustment
- Modular firmware architecture
- Memory management for DALI parameters
- Status LED indication

---

## Hardware

### Microcontroller
- LKS32MC033H6P8B

### Interfaces
- DALI Bus RX
- DALI Bus TX
- PWM Output for LED Driver
- Status LED

---

## Project Structure

├── main.c
├── hardware.h
├── dali_machine.c
├── dali_machine.h
├── dali_protocol.c
├── dali_protocol.h
├── led_dimmer.c
├── led_dimmer.h
├── memory.c
├── memory.h
└── startup_lks32mc033x.s

### Module Description

- hardware.c/h → GPIO and peripheral definitions
- dali_protocol.c/h → DALI frame handling
- dali_machine.c/h → DALI state machine
- led_dimmer.c/h → PWM brightness control
- memory.c/h → Device memory management
- main.c → Application entry point

---

## Working Principle

1. Receive DALI command from bus.
2. Decode received frame.
3. Validate command.
4. Update brightness level.
5. Generate PWM output.
6. Drive LED load.

---

## Development Environment

- Keil uVision
- ARM Compiler
- LKS32MC033 SDK

---

## Current Status

✅ Firmware architecture completed

✅ DALI command processing implemented

✅ PWM dimming implemented

✅ Simulation verified in Keil

🔄 Real hardware testing in progress

---

## Future Improvements

- Full DALI commissioning support
- Device addressing
- Group addressing
- Scene storage
- Error diagnostics
- DALI-2 compatibility enhancements

---

## Author

Nishtha Jain

Embedded Systems & Firmware Development

GitHub: https://github.com/nishthajain50
