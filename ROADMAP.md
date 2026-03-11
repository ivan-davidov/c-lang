# Embedded Systems — Learning Roadmap

> Goal: Smart Home + Robotics Firmware
> Pattern: read chapter → C project on PC → apply concept on hardware

---

## Phase 1 — C Language + First Hardware (Modules 1–5)

Comfortable enough with C to not be blocked by syntax on bare metal.

### Module 1 — C Basics + First Blink

**Read:** King Ch. 1–3 (program structure, variables, types, printf)

**C Project:** `mycat` + `mywc` — clone Unix `cat` and `wc`. File I/O, argc/argv. Keep each under 100 lines.

**Hardware:** Raspberry Pi Pico — install SDK, build blink example, flash it.

### Module 2 — Control Flow, Loops, Functions

**Read:** King Ch. 4–6 (if/else, loops, functions)

**C Project:** Infix calculator — reads `"3 + 4"` from stdin, prints 7. Separate function per operation.

**Hardware:** Rewrite blink with `void blink(int times, int delay_ms)`. Add button input. Don't debounce yet — feel the bug.

### Module 3a — Pointers: The Concept

**Read:** King Ch. 11 (pointers). Modern C Ch. 6 as backup.

**C Project:** Pointer exercises — double value in place, swap via pointers, print addresses with `%p`, function pointers, `const char*` vs `char*`. Each exercise its own `.c` file.

### Module 3b — Pointers on Real Data: Arrays + Strings

**Read:** King Ch. 12–13 (arrays/pointers, strings)

**C Project:** CSV parser — read file line by line, split into fields with pointer walking, store as `struct Row`. Handle empty fields, quoted strings, malformed lines.

**Hardware:** Array of LED blink patterns as structs. Cycle through on button press using pointer arithmetic.

### Module 4 — Structs, Headers, Secure C

**Read:** King Ch. 15–16 (structs). Effective C Ch. 1–3 (UB, types, safety).

**C Project:** State machine framework — states as enum, transitions as function pointer table. Demo: traffic light FSM and button FSM.

**Hardware:** `typedef struct { int pin; int delay_ms; bool state; } LED;` — functions take `LED*`. First multi-file project: `main.c` + `led.h` + `led.c`.

---

## Phase 2 — Bare Metal STM32 (Modules 5–9)

Switch to Nucleo F411RE. C projects continue in parallel on PC.

**Toolchain:** STM32CubeIDE — new project → blink → flash via USB.

### Module 5 — GPIO: Registers, Not HAL

**Read:** Beginning STM32 Ch. 1–3. RM0383 GPIO section.

**C Project:** Ring buffer — `rb_push()`, `rb_pop()`, `rb_full()`, `rb_empty()`. Test edge cases. Core embedded data structure used in UART drivers.

**Hardware:** Blink with HAL first, then rewrite with direct register writes (`RCC->AHB1ENR`, `GPIOA->MODER`, `GPIOA->ODR`).

### Module 6 — Timers + Interrupts

**Read:** Beginning STM32 Ch. 4–5. RM0383 TIM section.

**C Project:** RPN calculator — `"3 4 + 2 *"` → 14. Stack-based evaluation models how interrupt queues work.

**Hardware:** Remove all `HAL_Delay()`. Implement `millis()` via SysTick, timer-based LED toggle, button via EXTI interrupt.

### Module 7 — UART: Talk to Your PC

**Read:** Beginning STM32 Ch. 6. RM0383 USART section.

**C Project:** Harden CSV parser — bounds checking, pointer validation, error codes, run under Valgrind.

**Hardware:** `printf` to PC via UART2. Use ring buffer as TX buffer. Print sensor data and button press counts.

### Module 8 — I2C: First Real Sensor

**Read:** Beginning STM32 Ch. 8. BME280 datasheet registers section.

**C Project:** Producer-consumer with pthreads — shared buffer, mutex, semaphore. Intentionally remove mutex to see corruption.

**Hardware:** Wire BME280 to Nucleo (I2C). Read temperature, print to UART.

### Module 9 — SPI + PWM + ADC

**Read:** Beginning STM32 Ch. 7 (SPI), Ch. 9 (ADC + PWM).

**C Project:** Memory debugger — wrap malloc/free with `__FILE__`/`__LINE__` tracking. Detect leaks and double-frees.

**Hardware:** SSD1306 OLED via SPI, potentiometer via ADC, LED brightness via PWM, servo control.

---

## Phase 3 — Real Projects on ESP32 (Months 3–4)

Switch to ESP32 for WiFi. Everything from Nucleo transfers.

### Project 1: Garage Door Sensor

Reed switch → GPIO interrupt → Telegram alert. "Still open" warning after 10 min.

### Project 2: Air Quality Sensor Node

BME280 (I2C) + PMS5003 (UART) + SSD1306 OLED. Reads every 60s, publishes to MQTT, alerts on high PM2.5.

### Project 3: Air Purifier Controller

Subscribes to air quality MQTT topic. Controls HEPA fan speed via PWM through L298N driver. Manual override via phone.

### Project 4: Roller Blind Controller

BH1750 light sensor (I2C) + DC motor/stepper + L298N. Auto up/down by lux level. Schedule override. MQTT control.

---

## Phase 4 — RTOS + Full System (Months 5–6)

### FreeRTOS on Nucleo

Port BME280 code to FreeRTOS — sensor task pushes to queue, UART task prints from queue. Add mutex for shared UART.

**Read:** OSTEP Ch. 1–4 (processes, threads, concurrency).

### Project 5: Surveillance Node

Raspberry Pi 4 + camera + Frigate NVR. ESP32 PIR motion trigger via MQTT. Local video recording, Telegram snapshots.

### Project 6: Home Automation Hub

Raspberry Pi as always-on hub. Mosquitto MQTT broker + Home Assistant + Grafana/InfluxDB. All ESP32 nodes connect here. Automations: fire alarm, motion alerts, temperature notifications, roller blind scheduling.

---

## Phase 5 — Go Deeper (Month 7+)

Pick your direction:

- **Robotics firmware** — 2-wheel robot, PID speed control in bare-metal C. Read: Making Embedded Systems (Elecia White).
- **Linux kernel / device drivers** — write a character device kernel module. Read: CS:APP Ch. 8, 9, 12. OSTEP fully.
- **Deeper C systems** — build a shell (`fork`/`exec`/pipes), custom `malloc`, HTTP server with `epoll`. Read: Beej's Guide to Network Programming.

---

## Hardware Shopping List

### Buy now (Phase 1–2)

| Item | Approx. price |
|------|---------------|
| STM32 Nucleo F411RE | ~$15 |
| Raspberry Pi Pico 2 | ~$7 |
| Breadboard | ~$3 |
| Jumper wires (M-M + M-F) | ~$4 |
| LEDs 5mm x10 | ~$1 |
| Resistors 220Ω, 330Ω x20 each | ~$1 |
| Resistors 10kΩ x10 | ~$1 |
| Push buttons 6mm x5 | ~$1 |
| Multimeter (basic) | ~$15 |

### Buy at Phase 3 (not before)

| Item | Approx. price |
|------|---------------|
| ESP32 DevKit x3 | ~$8 each |
| BME280 x3 | ~$3 each |
| PMS5003 or SDS011 | ~$15 |
| BH1750 x2 | ~$1 each |
| PIR motion sensor x3 | ~$2 each |
| Reed switches x5 | ~$1 each |
| MQ-2 smoke sensor | ~$2 |
| SSD1306 OLED 0.96" | ~$3 |
| Potentiometer 10kΩ | ~$1 |
| Servo SG90 | ~$2 |
| DC motor + L298N driver | ~$4 |
| Logic analyzer (clone) | ~$8 |

---

## C Projects Summary

| Module | Project | Core skill |
|--------|---------|-----------|
| 1 | `mycat` + `mywc` | File I/O, argc/argv |
| 2 | Infix calculator | Functions, loops, input handling |
| 3a | Pointer exercises | Pointers as a concept |
| 3b | CSV parser | Pointer arithmetic on structured data |
| 4 | State machine | Function pointers, reusable FSM |
| 5 | Ring buffer | Core embedded data structure |
| 6 | RPN calculator | Stack logic, models event queues |
| 7 | Harden CSV parser | Defensive C, Valgrind |
| 8 | Producer-consumer | Queues + mutexes before FreeRTOS |
| 9 | Memory debugger | Leak and double-free detection |

---

## Rules

- Read → write code the same day
- Never finish a chapter without a project to show for it
- Commit everything to GitHub, even if broken
- 80% coding/hardware, 20% reading — every week
- Modules are not calendar weeks — progress tied to outcome, not date
