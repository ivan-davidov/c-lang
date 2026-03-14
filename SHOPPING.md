# Shopping List

## TODO — Check When Package Arrives

- [ ] **SSD1306 OLED — count the pins.** If 4 pins (VCC, GND, SCL, SDA) → I2C only. If 7 pins (VCC, GND, SCK, MOSI, RES, DC, CS) → SPI capable. The roadmap Stage 4 says "SPI: Wire SSD1306 OLED" — if yours is I2C-only, either buy a 7-pin SPI version or adjust the roadmap to do both BME280 and OLED over I2C.
- [ ] **Buy a second SSD1306 OLED** if you want STM32 demo (Stage 4) and ESP32 air quality node (Stage 5 Project 2) running simultaneously. ~900 RSD.
- [ ] **USB cables — verify you have all three types:**
  - Mini USB (Type B) → for STM32 Nucleo F401RE (least common, check this one)
  - Micro USB → for Raspberry Pi Pico 2
  - USB-C → for ESP32-S3-DevKitM
- [ ] **MQ-2 smoke sensor (Stage 6):** outputs analog 0–5V but ESP32-S3 ADC is 3.3V max. You'll need a voltage divider (2 resistors from your kit) when you wire it up. No purchase needed, just a note.
- [ ] **L298N + DC motor:** only 1 of each — fine for learning (Stage 4), but you'll need extras when deploying Stage 5 projects (purifier, roller blind) simultaneously. Buy additional when you get there.

---

## Ordered (elektromodul.rs, 12.03.2026 — 30,440 RSD)

| | Item | Qty | Stage |
|---|------|-----|-------|
| [x] | STM32 Nucleo-64 F401RE | 1 | 2–4 |
| [x] | Raspberry Pi Pico 2 | 1 | 1 |
| [x] | Jumper wires M-F 40pin 30cm | 1 set | 1+ |
| [x] | Jumper wires M-M 40pin 30cm | 1 set | 1+ |
| [x] | Resistor set 600pcs 1/4W 1% metal film | 1 | 1+ |
| [x] | ESP32-S3-DevKitM-1-N8 | 3 | 5 |
| [x] | BME280 temp/humidity/pressure sensor | 3 | 4–5 |
| [x] | BH1750 light sensor | 2 | 5 |
| [x] | PIR motion sensor | 3 | 5–6 |
| [x] | Reed switch MC31 | 5 | 5 |
| [x] | MQ-2 smoke sensor | 1 | 6 |
| [x] | SSD1306 OLED 0.96" | 1 | 4–5 |
| [x] | Servo SG90 | 1 | 4 |
| [x] | L298N motor driver | 1 | 4–5 |
| [x] | DC motor 3-6VDC | 1 | 4–5 |
| [x] | SDS011 particle sensor | 1 | 5 |
| [x] | Logic analyzer 8ch Saleae clone | 1 | 3+ |

## To Buy — elektromodul.rs

Sent request for offer via email. Items needed before Stage 2.

| | Item | Qty | Stage | Notes |
|---|------|-----|-------|-------|
| | **Kits** | | | |
| [ ] | Starter kit sa 830 breadboard (A4231) | 1 | 1 | Includes: 1 breadboard, 10 buttons, 50 LEDs, power supply module, buzzer, potentiometer, 74HC595, capacitors (partial) |
| [ ] | Breadboard 830 rupa | 2 | 1 | 3 total with starter kit |
| | **Capacitors** | | | |
| [ ] | Ceramic capacitor set (450pcs, 15 values, 10pF–100nF) | 1 | 2 | Starter kit has only 22pF + 100nF |
| [ ] | Electrolytic capacitor set (200pcs, 15 values, 0.1µF–220µF) | 1 | 2 | Starter kit has only 10µF + 100µF |
| | **Semiconductors** | | | |
| [ ] | Diode set (1N4148, 1N4007, 1N5819 + others) | 1 | 2 | 7-type set they carry |
| [ ] | Transistor set 200pcs (2N2222, 2N3904, 2N3906 + others) | 1 | 2 | 10 types, 20 each |
| [ ] | MOSFET IRLZ44N logic-level N-channel | 5 | 3 | NOT IRFZ44N — must be logic-level (3.3V gate drive) |
| [ ] | Op-amp LM358 DIP-8 | 5 | 3 | Need through-hole, not SMD |
| [ ] | 555 timer NE555P DIP-8 | 5 | 2 | |
| | **Voltage regulators** | | | |
| [ ] | LM7805 5V regulator (TO-220 bare IC or DIY kit) | 5 | 5 | |
| [ ] | AMS1117-3.3V regulator | 5 | 5 | |
| | **Sockets & passives** | | | |
| [ ] | IC socket 8-pin DIP | 10 | 2 | |
| [ ] | IC socket 14-pin DIP | 10 | 2 | |
| [ ] | Potentiometer 10kΩ | 3 | 2 | Extra — have 1 from order + 1 from starter kit |
| | **Prototyping** | | | |
| [ ] | Perfboard/PCB set (4 sizes) | 1 | 5 | |
| [ ] | Heat shrink tubing set (127pcs) | 1 | 2 | |
| | **Tools** | | | |
| [ ] | Multimeter (decent, with continuity buzzer) | 1 | 1 | |
| [ ] | Soldering iron (temp controlled — TS101, Pinecil V2, or similar) | 1 | 2 | |
| [ ] | Solder wire 0.8mm | 1 | 2 | |
| [ ] | Wire strippers | 1 | 1 | |
| [ ] | Flush cutters | 1 | 1 | |
| [ ] | Helping hands / PCB holder | 1 | 2 | |
| | **Equipment** | | | |
| [ ] | Oscilloscope 4ch 50–100MHz (Rigol DS1054Z / Siglent SDS1104X-E) | 1 | 2 | Ask for recommendation if they don't carry |
| [ ] | Bench power supply 0–30V 0–5A (AC input, not DC-DC module) | 1 | 2 | Ask for recommendation if they don't carry |

## Buy Later

| | Item | Qty | Stage |
|---|------|-----|-------|
| [ ] | Raspberry Pi 4 + power supply + SD card | 1 | 6 |
| [ ] | Raspberry Pi camera module | 1 | 6 |
| [ ] | RTL-SDR dongle (RTL2832U) | 1 | 7 |
| [ ] | MPU6050 / MPU9250 IMU | 1 | 7 |
| [ ] | INMP441 digital microphone | 1 | 7 |
| [ ] | LoRa modules SX1276 | 2 | 7 |
| [ ] | RPLIDAR A1 | 1 | 7 |
