# Learning Roadmap — C + Electronics + Embedded Systems

> **Goal:** Deploy real home automation. Build DSP, robotics, and RF projects. Get close to hardware and science.
>
> **Approach:** Three parallel tracks with sync points. No calendar deadlines — progress gated by milestones.
>
> **Daily split:** ~1 hr electronics (reading + breadboard), ~1.5–2 hr C / embedded.

---

## Three Tracks

| Track | Starting point | Resources |
|-------|---------------|-----------|
| **A — C Language** | King Ch. 18 (nearly done reading) | King Ch. 18–20, Effective C, Beej's Guide, CS:APP labs |
| **B — Electronics** | Zero | AllAboutCircuits textbooks, Ben Eater, Afrotechmods, Phil's Lab |
| **C — Embedded Hardware** | Have Pico + STM32 + ESP32 | Pico SDK, STM32 bare metal, ESP-IDF |

Tracks A and B run **in parallel** and are mostly independent. Track C depends on both — it cannot advance faster than A or B will support.

## Stage Flow

```
Stage 1          Stage 2              Stage 3               Stage 4
Foundation  ───→ Core Building   ───→ STM32 Core       ───→ Sensors &
                 Blocks               Peripherals           Actuators
  A: King done     A: Ring buffer       A: UART cmd parser    A: TCP client
  A: State mach.   B: Caps, diodes,     B: MOSFETs,           A: Custom malloc
  B: DC basics        transistors           op-amps, digital   B: Datasheets,
  C: Pico blink    C: STM32 HAL →       C: Timers, IRQ,          ADC/DAC,
                      registers              UART                  motor theory
                          │                                    C: I2C, SPI,
                     BUY: scope,                                  PWM, ADC
                     bench supply,
                     components

     Stage 5                Stage 6              Stage 7
───→ ESP32 Home        ───→ RTOS &          ───→ Advanced
     Automation             Systems               Projects
  A: Producer-consumer   CS:APP + labs          IDEAS.md
  A: Mini shell          OSTEP                  RTL-SDR, DSP,
  B: Power design        FreeRTOS on STM32      robotics, RF
  C: WiFi, MQTT,         Home hub (RPi)
     Projects 1–4
     Deploy in home
```

Each stage requires the **previous stage's milestone** to be met before starting.

---

## Stage 1 — Foundation

> Finish C reading. Start electronics from zero. Get a quick hardware win.

### A — C Language

- [ ] **Read:** King Ch. 18 — Declarations
- [ ] **Read:** King Ch. 19 — Program design
- [ ] **Read:** King Ch. 20 — Low-level programming
- [ ] **Read:** Effective C Ch. 1–3 (undefined behavior, object types, defensive coding)
- [ ] **Review:** Walk through all your notes — identify anything you read but couldn't explain to someone
- [ ] **Project: State machine framework**
  - [ ] Define states as `enum`, transitions as function pointer table
  - [ ] Build demo: traffic light FSM
  - [ ] Build demo: button-debounce FSM
  - [ ] Multi-file build: `main.c`, `fsm.h`, `fsm.c`
  - Exercises: enums, structs, function pointers, headers, multi-file compilation
  - Directly useful — every embedded system is a state machine

### B — Electronics

- [ ] **Read:** AllAboutCircuits DC (Vol. I) Ch. 1 — Basic concepts: voltage, current, resistance
- [ ] **Read:** AAC DC Ch. 2 — Ohm's law: V = IR, power (P = IV), calculating circuit values
- [ ] **Read:** AAC DC Ch. 3 — Electrical safety (skim — good to internalize early)
- [ ] **Read:** AAC DC Ch. 4 — Series DC circuits: current same everywhere, voltages add
- [ ] **Read:** AAC DC Ch. 5 — Parallel DC circuits: voltage same everywhere, currents add
- [ ] **Read:** AAC DC Ch. 6 — Divider circuits, Kirchhoff's voltage and current laws
- [ ] **YouTube:** Afrotechmods — "What is a resistor?", "What is current?", basic component videos
- [ ] **Lab:** Use multimeter to measure battery voltage, resistance of resistors from your kit
- [ ] **Lab:** Build LED + resistor circuit, measure current, verify Ohm's law
- [ ] **Lab:** Build a voltage divider, predict its output voltage on paper, measure it
- [ ] **Lab:** AllAboutCircuits Experiments (Vol. VI) — labs matching Ch. 1–6

### C — Hardware

- [ ] **Raspberry Pi Pico 2:** Install SDK, build blink example, flash via USB
- [ ] Wire an external LED + resistor on breadboard → blink it
- [ ] Understand: a GPIO pin outputs 3.3V (HIGH) or 0V (LOW)
- [ ] Add button input → read pin state, observe contact bounce (don't fix yet — feel the bug)
- [ ] Write `void blink(int times, int delay_ms)` function

### Milestone

- [ ] State machine compiles, runs, and demonstrates two different FSMs
- [ ] Can build a circuit on a breadboard, predict voltage across any component using Ohm's law, verify with multimeter
- [ ] Pico blinks external LED and reads a button

---

## Stage 2 — Core Building Blocks

> Essential embedded data structures. Electronic components that make circuits do things. First STM32.

### A — C Language

- [ ] **Project: Ring buffer library**
  - [ ] Implement: `rb_create()`, `rb_push()`, `rb_pop()`, `rb_full()`, `rb_empty()`, `rb_destroy()`
  - [ ] Circular array with read/write indices, modular wrap-around
  - [ ] Test edge cases: push when full, pop when empty, wrap-around, single element
  - [ ] Run under Valgrind — no leaks, no invalid reads
  - THE embedded data structure — used in every UART driver, DMA buffer, event queue

### B — Electronics

- [ ] **Read:** AAC DC (Vol. I) Ch. 7 — Series-parallel combination circuits
- [ ] **Read:** AAC DC Ch. 13 — Capacitors: charge/discharge, decoupling (why every IC has a 100nF cap next to it), filtering
- [ ] **Read:** AAC DC Ch. 14 — Magnetism and electromagnetism (background for motors and inductors)
- [ ] **Read:** AAC DC Ch. 15 — Inductors: energy storage in magnetic fields, relationship to motors
- [ ] **Read:** AAC Semiconductors (Vol. III) Ch. 3 — Diodes: one-way current, protection diodes, rectification
- [ ] **Read:** AAC Semiconductors Ch. 4 — LEDs, Zener diodes (voltage regulation)
- [ ] **Read:** AAC Semiconductors Ch. 5 — Bipolar junction transistors (BJT): NPN/PNP as switches — THIS is how microcontrollers control motors, relays, and high-power loads
- [ ] **YouTube:** Afrotechmods — "Transistors", "How a transistor works"
- [ ] **YouTube:** Ben Eater — logic gates videos
- [ ] **Lab:** Transistor switching an LED from a Pico GPIO pin
- [ ] **Lab:** Capacitor charge/discharge — time the curve with multimeter or scope
- [ ] **Lab:** RC low-pass filter on breadboard
- [ ] **Lab:** Flyback diode across motor (drive motor with transistor, observe what happens without the diode vs with it)
- [ ] **Lab:** AAC Experiments (Vol. VI) — labs matching chapters above
- [ ] **BUY: Oscilloscope + bench power supply** — you need the scope to see capacitor charge curves, noisy signals, and to debug everything from Stage 3 onward

### C — Hardware

- [ ] **Read:** Beginning STM32 Ch. 1–3, or RM0401 reference manual GPIO section
- [ ] **STM32 Nucleo F401RE:** Install STM32CubeIDE, create project, blink with HAL
- [ ] Rewrite blink with direct register access: `RCC->AHB1ENR`, `GPIOA->MODER`, `GPIOA->ODR`
  - NOW you understand what "set bit 5 in the mode register" means because you understand binary, voltage levels, and digital logic from Track B
- [ ] Measure the GPIO pin toggling on oscilloscope — see the square wave, measure its frequency

### Milestone

- [ ] Ring buffer passes all edge case tests, clean under Valgrind
- [ ] Can explain what a transistor does and why you need a flyback diode on a motor
- [ ] STM32 blinks via direct register writes
- [ ] Oscilloscope shows a clean square wave on the GPIO pin

---

## Stage 3 — STM32 Core Peripherals

> Timers, interrupts, and talking to the PC. The MCU becomes useful.

### A — C Language

- [ ] **Project: UART command parser**
  - [ ] Parse structured commands from a string: `"SET LED 3 ON"`, `"GET TEMP"`, `"STATUS"`
  - [ ] Tokenizer splits input into command + arguments
  - [ ] Dispatch table maps command strings to handler functions (function pointers)
  - [ ] Handle malformed input gracefully (error codes, not crashes)
  - [ ] Develop and test on PC, then deploy the same parser on STM32 over UART
  - Directly used in every embedded debug console and command interface

### B — Electronics

- [ ] **Read:** AAC Semiconductors (Vol. III) Ch. 7 — MOSFETs: modern switching transistors, motor drivers, power circuits
- [ ] **Read:** AAC Semiconductors Ch. 9 — Operational amplifiers: signal conditioning, amplification (needed for analog sensors + ADC)
- [ ] **Read:** AAC Digital (Vol. IV) Ch. 1 — Number systems and codes
- [ ] **Read:** AAC Digital Ch. 2 — Logic gates
- [ ] **Read:** AAC Digital Ch. 3 — Boolean algebra
  - These map directly to understanding GPIO registers, bit manipulation, and digital protocols
- [ ] **Concept study:** Serial communication fundamentals
  - [ ] UART: start bit, data bits, stop bit, baud rate
  - [ ] Why TX/RX are crossed between devices
  - [ ] Pull-up resistors for I2C (you know what pull-ups are now from transistor work)
- [ ] **YouTube:** Phil's Lab — STM32 beginner videos, oscilloscope usage
- [ ] **YouTube:** Ben Eater — serial communication, how RS-232 works
- [ ] **Lab:** Use oscilloscope to look at UART signals from STM32
- [ ] **Lab:** Use logic analyzer to decode UART signals
- [ ] **Lab:** Build an op-amp voltage follower (buffer) circuit on breadboard

### C — Hardware

- [ ] **Read:** Beginning STM32 Ch. 4–5 (timers, interrupts) or RM0401 TIM + NVIC sections
- [ ] **SysTick timer:** Implement `millis()` — millisecond counter via SysTick interrupt
- [ ] **Hardware timers:** Timer-based LED toggle (no busy-wait loops anywhere)
- [ ] **EXTI interrupt:** Button triggers interrupt → toggles LED
  - [ ] Debounce in software (compare with RC filtering you learned in Track B)
- [ ] **UART to PC:**
  - [ ] Configure UART2 (connected to ST-Link virtual COM port)
  - [ ] Implement `printf` redirection over UART
  - [ ] **Use your ring buffer** as the TX buffer — first real use of your own C library in embedded
  - [ ] Print button press counts, mock sensor data
- [ ] **Deploy UART command parser on STM32** — type commands in PC terminal, STM32 responds

### Milestone

- [ ] No `HAL_Delay()` anywhere — all timing via SysTick/timers
- [ ] Button interrupt works reliably with debounce
- [ ] UART prints to PC terminal using ring buffer
- [ ] Command parser works both on PC and on STM32 over UART
- [ ] Can use oscilloscope and logic analyzer to see and decode UART signals

---

## Stage 4 — Sensors & Actuators

> Connect real sensors, display data, control motors. The STM32 interacts with the physical world.

### A — C Language

- [ ] **Project: TCP client** (Beej's Guide to Network Programming, Ch. 5–6)
  - [ ] Create a socket, connect to a server, send an HTTP GET request, print the response
  - [ ] Handle connection errors, timeouts, partial reads
  - [ ] ~100–200 lines — small project, big payoff
  - Directly relevant: ESP32 networking in Stage 5 uses the same socket API
- [ ] **Project: Custom malloc**
  - [ ] Manage a large `char[]` as your heap
  - [ ] Implement `my_malloc()` and `my_free()` using a free-list with headers
  - [ ] Track allocations with `__FILE__` / `__LINE__`
  - [ ] Detect: leaks (allocated but never freed), double-free, buffer overrun (canary values)
  - [ ] ~300–400 lines
  - Understanding what malloc does internally matters when memory is constrained (STM32 has 96KB RAM)

### B — Electronics

- [ ] **Datasheet reading:** Learn to navigate a real datasheet
  - [ ] BME280: find the I2C address, register map, measurement sequence
  - [ ] SSD1306: find the command set, initialization sequence
  - [ ] Skills: pinouts, absolute maximum ratings, timing diagrams, register descriptions
- [ ] **Read:** AAC Digital (Vol. IV) Ch. 13 — ADC/DAC: how analog-to-digital conversion works (resolution, reference voltage, successive approximation)
- [ ] **Motor driving:** H-bridge theory — how L298N drives a motor in both directions, PWM duty cycle controls speed, flyback diodes (you already built one in Stage 2)
- [ ] **YouTube:** Phil's Lab — I2C explained, SPI explained, ADC fundamentals

### C — Hardware

- [ ] **Read:** Beginning STM32 Ch. 7 (SPI), Ch. 8 (I2C), Ch. 9 (ADC/PWM)
- [ ] **I2C:** Wire BME280 to Nucleo, read temperature/humidity/pressure, print to UART
  - [ ] Debug with logic analyzer — see the address byte, ACK/NACK, data bytes
- [ ] **SPI:** Wire SSD1306 OLED, display sensor readings
  - [ ] Faster protocol, more wires — understand the tradeoff vs I2C
- [ ] **ADC:** Read potentiometer voltage — map to a value, display on OLED
- [ ] **PWM:** Control LED brightness, then servo position (SG90)
- [ ] **Combine:** Read BME280 → display on OLED → servo needle shows temperature range
- [ ] **Motor:** Drive DC motor with L298N, control speed via PWM, direction via GPIO

### Milestone

- [ ] TCP client fetches a web page and prints it
- [ ] Custom malloc detects a deliberately planted leak and double-free
- [ ] Multi-sensor system on STM32: BME280 readings on OLED, potentiometer adjusts something, servo and motor respond to inputs
- [ ] Can read a datasheet and find what you need without tutorials

---

## Stage 5 — ESP32 Home Automation

> WiFi, MQTT, and real projects deployed in your home.

### A — C Language

- [ ] **Read:** OSTEP Ch. 1–4 (processes, threads, concurrency)
- [ ] **Project: Producer-consumer with pthreads**
  - [ ] Shared ring buffer (reuse yours!), producer and consumer threads
  - [ ] Synchronize with mutex + condition variable
  - [ ] Intentionally remove mutex → observe corruption
  - [ ] Stress test: high-frequency produce/consume, verify no data loss
  - This is exactly how FreeRTOS tasks communicate — learn the pattern on PC first
- [ ] **Project: Mini shell** (~200 lines, Stephen Brennan's tutorial or from scratch)
  - [ ] Read a command line, parse it, `fork()` + `exec()` the program
  - [ ] Implement pipes: `ls | grep .c | wc -l`
  - [ ] Handle `cd` (built-in), `exit`, background processes (`&`)
  - [ ] Directly teaches the Unix process model — essential for RPi/Linux work in Stage 6
  - Pairs naturally with OSTEP reading

### B — Electronics

- [ ] **Power design:** How to power standalone ESP32 nodes
  - [ ] USB power vs battery vs wall adapter
  - [ ] Voltage regulators: when you need 3.3V from 5V (AMS1117-3.3)
  - [ ] Current budgeting: ESP32 WiFi TX draws ~200mA, plan accordingly
- [ ] **Soldering practice:** Solder headers, wires, connections on perfboard
- [ ] **Enclosures and deployment:** Making circuits permanent, strain relief

### C — Hardware

- [ ] **ESP32-S3 setup:** Install ESP-IDF, build hello_world, flash via USB
  - Your boards are ESP32-S3-DevKitM-1-N8 — use ESP-IDF (not Arduino) for proper learning
- [ ] **WiFi:** Station mode, connect to home network
- [ ] **MQTT:** Set up Mosquitto broker on your PC (or a Raspberry Pi)
- [ ] **Project 1: Garage Door Sensor**
  - [ ] Reed switch → GPIO interrupt → "door opened" MQTT message → Telegram alert
  - [ ] "Still open" warning after 10 min (timer-based)
  - Simple, one sensor, one output — first end-to-end IoT project
- [ ] **Project 2: Air Quality Sensor Node**
  - [ ] BME280 (I2C) + SDS011 (UART) + SSD1306 OLED
  - [ ] Read every 60s, publish to MQTT, display on OLED
  - [ ] Alert on high PM2.5 via Telegram
  - Skills: multi-protocol (I2C + UART), data formatting, MQTT publishing
- [ ] **Project 3: Air Purifier Controller**
  - [ ] Subscribe to air quality MQTT topic from Project 2
  - [ ] Control HEPA fan speed via PWM through L298N
  - [ ] Manual override via MQTT (phone control)
  - Skills: MQTT subscribe, actuator control, state machine (auto/manual modes)
- [ ] **Project 4: Roller Blind Controller**
  - [ ] BH1750 light sensor (I2C) + DC motor + L298N
  - [ ] Auto up/down by lux threshold, schedule override, MQTT control
  - Skills: I2C + motor + scheduling + MQTT — combines everything

### Milestone

- [ ] Producer-consumer passes stress test with and without synchronization (can demonstrate the corruption)
- [ ] Mini shell runs commands, handles pipes, and doesn't leak child processes
- [ ] Four ESP32 nodes deployed in your home, communicating over MQTT, controllable from phone

---

## Stage 6 — RTOS & Systems

> Real-time operating system. Computer systems foundations. Full home automation hub.

### Reading

- [ ] **CS:APP** (Bryant & O'Hallaron) — memory hierarchy, assembly, linking, processes, virtual memory, networking
  - [ ] Bomb lab
  - [ ] Attack lab
  - [ ] Shell lab
  - [ ] Malloc lab (you already built a custom malloc — this deepens it)
  - [ ] Proxy lab (you already built a TCP client — this extends it)
- [ ] **OSTEP** (Arpaci-Dusseau) — scheduling, virtual memory, concurrency, persistence (continue from Stage 5)

### Hardware

- [ ] **FreeRTOS on STM32:**
  - [ ] Port BME280 code to FreeRTOS — sensor task pushes readings to a queue, display task reads from queue
  - [ ] Add UART task with mutex (shared peripheral)
  - This is the producer-consumer pattern from Stage 5, now on real hardware with real constraints
- [ ] **Buy Raspberry Pi 4** (for the hub)
- [ ] **Project 5: Surveillance Node** (optional)
  - [ ] RPi 4 + camera + Frigate NVR
  - [ ] ESP32 PIR motion trigger via MQTT
  - [ ] Local recording, Telegram snapshots
- [ ] **Project 6: Home Automation Hub**
  - [ ] RPi 4: Mosquitto MQTT broker + Home Assistant + Grafana/InfluxDB
  - [ ] All ESP32 nodes connect here
  - [ ] Automations: fire alarm (MQ-2), motion alerts (PIR), temperature notifications, roller blind scheduling
  - One dashboard to rule them all

### Milestone

- [ ] FreeRTOS multi-task system running stable on STM32
- [ ] At least 3 CS:APP labs completed
- [ ] Home hub operational with all nodes connected, automations running, data visualized in Grafana

---

## Stage 7 — Advanced Projects

> Pick your direction. All build on everything before.

### Recommended start order (from IDEAS.md)

- [ ] **RTL-SDR Scanner** — €10 dongle, immediate payoff, opens RF world. `rtl_sdr` library is C.
- [ ] **Audio Spectrum Analyzer** — FFT on ESP32, real-time visualization. Great DSP intro.
- [ ] **Self-Balancing Robot** — PID control on real hardware, IMU sensor fusion. Everything comes together.
- [ ] **LoRa Sensor Network** — Extends home automation outdoors, 5–10 km range.
- [ ] **RPLIDAR SLAM** — After robot, when comfortable with real-time control.

### Additional directions

- [ ] **Robotics firmware** — Read: Making Embedded Systems (Elecia White)
- [ ] **Linux kernel / device drivers** — Write a character device module. Read: CS:APP Ch. 8–9, OSTEP fully.
- [ ] **Deeper C systems** — HTTP server with epoll. Read: Beej's Guide to Network Programming.
- [ ] **PCB design** — Phil's Lab KiCad series. Design and order your own boards.

---

## C Projects Summary

| Stage | Project | Lines | What it exercises | Directly used in |
|-------|---------|-------|-------------------|-----------------|
| 1 | State machine framework | ~200 | Enums, structs, function pointers, multi-file | Every embedded system is an FSM |
| 2 | Ring buffer | ~150 | Circular arrays, pointer arithmetic, modular indexing | UART drivers, DMA, event queues |
| 3 | UART command parser | ~200 | Tokenizing, dispatch tables, function pointers | Every embedded debug console |
| 4 | TCP client | ~150 | Sockets, connect, send, recv, error handling | ESP32 HTTP/MQTT networking |
| 4 | Custom malloc | ~350 | Free-list, metadata headers, leak/corruption detection | Memory-constrained MCU programming |
| 5 | Producer-consumer | ~150 | pthreads, mutex, condition variables, race conditions | FreeRTOS task communication |
| 5 | Mini shell | ~200 | fork, exec, pipe, waitpid, process model | Linux/RPi work, CS:APP prep |

Each project is 150–350 lines. Builds directly on the previous ones. Every one feeds into embedded, systems, or networking work that follows.

---

## Resources

### Books

| Book | Role | When |
|------|------|------|
| **King** — C Programming: A Modern Approach | Primary C textbook | Stage 1 (finishing) |
| **Effective C** — Seacord | UB, defensive C, safety | Stage 1 |
| **Beginning STM32** — Gay | Bridge from C to STM32 | Stages 2–4 |
| **Beej's Guide to Network Programming** (free) | Sockets, TCP/UDP, HTTP | Stage 4 |
| **CS:APP** — Bryant & O'Hallaron | Memory, assembly, processes, networking | Stage 6 |
| **OSTEP** — Arpaci-Dusseau (free) | OS internals: threads, scheduling, VM | Stages 5–6 |
| **Crafting Interpreters** Part III — Nystrom | Bytecode VM in C | Stage 7+ |
| **Making Embedded Systems** — White | Embedded architecture, patterns | Stage 7+ |

### Online

| Resource | What | When |
|----------|------|------|
| **AllAboutCircuits** — DC (Vol. I) | Voltage, current, resistance through capacitors/inductors | Stages 1–2 |
| **AllAboutCircuits** — Semiconductors (Vol. III) | Diodes, transistors, MOSFETs, op-amps | Stages 2–3 |
| **AllAboutCircuits** — Digital (Vol. IV) | Logic gates, Boolean algebra, ADC/DAC | Stages 3–4 |
| **AllAboutCircuits** — Experiments (Vol. VI) | Hands-on labs matching theory chapters | Throughout |
| **Stephen Brennan's shell tutorial** | Reference for mini shell project | Stage 5 |

### YouTube

| Channel | Best for | When |
|---------|----------|------|
| **Afrotechmods** | Component basics: what is a resistor/capacitor/transistor, practical demos | Stages 1–2 |
| **Ben Eater** | Digital electronics, serial communication, logic from first principles | Stages 2–3 |
| **Phil's Lab** | STM32 tutorials, oscilloscope usage, PCB design, DSP on MCUs | Stages 3+ |

---

## Equipment

### Already Ordered (elektromodul.rs)

| # | Item | Qty |
|---|------|-----|
| 1 | STM32 Nucleo-64 F401RE | 1 |
| 2 | Raspberry Pi Pico 2 | 1 |
| 3 | Jumper wires M-F 40pin 30cm | 1 |
| 4 | Jumper wires M-M 40pin 30cm | 1 |
| 5 | Resistor set 600pcs 1/4W 1% metal film | 1 |
| 6 | ESP32-S3-DevKitM-1-N8 | 3 |
| 7 | BME280 temperature/humidity/pressure | 3 |
| 8 | BH1750 light sensor | 2 |
| 9 | PIR motion sensor | 3 |
| 10 | Reed switch MC31 | 5 |
| 11 | MQ-2 smoke sensor | 1 |
| 12 | SSD1306 OLED 0.96" | 1 |
| 13 | Servo SG90 | 1 |
| 14 | L298N motor driver | 1 |
| 15 | DC motor 3-6VDC | 1 |
| 16 | SDS011 particle sensor | 1 |
| 17 | Logic analyzer 8ch Saleae clone | 1 |

### Buy Now — Local (Sprintel / Tehnotronik, Novi Sad)

- [ ] Breadboard 830 points x3 — circuits accumulate, one is never enough
- [ ] Tactile buttons 6mm x10 — input for Pico and STM32 projects
- [ ] LEDs 5mm assorted (red, green, yellow, blue) x20 — not on your invoice, need these
- [ ] Multimeter (decent, not cheapest) — first and most-used tool
- [ ] Soldering iron (TS101 or Pinecil V2) — temperature controlled, USB-C powered
- [ ] Solder wire 0.8mm (60/40 or lead-free)
- [ ] Wire strippers + flush cutters
- [ ] Helping hands / PCB holder — third hand for soldering

### Buy Now — Online (for electronics learning)

These are the components you need that are NOT on your invoice.

| Item | Qty | Approx. | Why |
|------|-----|---------|-----|
| **Oscilloscope** — Rigol DS1054Z or Siglent SDS1104X-E | 1 | €300–400 | THE most important instrument. See signals, debug analog, measure timing. Non-negotiable. |
| **Bench power supply** — adjustable 0–30V, 0–5A | 1 | €50–80 | Safe, controlled, current-limited power for circuits |
| Capacitor assortment (ceramic 100pF–1µF + electrolytic 1µF–1000µF) | 1 kit | €8–15 | Decoupling, filtering, timing — capacitors are everywhere |
| Diode kit: 1N4148 (signal) + 1N4007 (rectifier) + 1N5819 (Schottky) | 20 each | €3–5 | Protection, rectification, flyback |
| Transistor kit: 2N2222 or 2N3904 (NPN) + 2N3906 (PNP) | 20 + 10 | €3–5 | Switching loads from MCU pins |
| MOSFET: IRLZ44N (logic-level N-channel) | 5 | €3 | Higher-power switching (motors, LED strips) |
| Op-amp IC: LM358 (dual) | 5 | €2 | Signal conditioning for analog sensors |
| 555 timer IC | 5 | €2 | Classic timing/oscillator circuits for learning |
| Voltage regulator: LM7805 (5V) + AMS1117-3.3 (3.3V) | 5 each | €3 | Power supply circuits |
| IC sockets: 8-pin DIP + 14-pin DIP | 10 each | €2 | Don't solder ICs directly |
| Potentiometers 10kΩ (extras) | 3 | €2 | You have 1 — get more for parallel circuits |
| Prototype PCB / perfboard | 5 | €3 | Making circuits permanent |
| Heat shrink tubing assortment | 1 | €3 | Clean wire connections |
| **Subtotal** | | **~€400–530** | |

### Buy Later

| Item | When | Approx. |
|------|------|---------|
| Raspberry Pi 4 + power supply + SD card | Stage 6 | €70–100 |
| Raspberry Pi camera module | Stage 6 (if doing surveillance) | €25 |
| RTL-SDR dongle (RTL2832U) | Stage 7 | €10–15 |
| MPU6050 or MPU9250 IMU | Stage 7 (self-balancing robot) | €5 |
| INMP441 digital microphone | Stage 7 (audio DSP) | €3 |
| LoRa modules SX1276 x2 | Stage 7 (LoRa network) | €15 |
| RPLIDAR A1 | Stage 7 (SLAM) | €100 |

---

## Rules

- Read and write code the same day — no pure-reading days
- Commit everything, even if broken
- 80% building, 20% reading — every week
- Each electronics chapter: read it, then build something on a breadboard that proves you understood it
- Don't move to the next stage until the milestone is met
- If stuck for more than a session on one track, switch to the other — momentum matters
- No time pressure — stages take as long as they take
