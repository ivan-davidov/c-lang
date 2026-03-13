# Project Ideas — After Phase 3+

Electronics, signal processing, robotics, and sensing projects.
All build on embedded C skills from the roadmap.

---

## Signals & RF

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| RTL-SDR Scanner | RTL-SDR dongle | ~€10 | Easy | **Start here.** Listen to aircraft, ships, FM, weather. Zero hardware dev, opens RF world. `rtl_sdr` lib is C. |
| ADS-B Airplane Tracker | RTL-SDR + Raspberry Pi | ~€25 | Easy | Protocol decoding. `dump1090` is C. Live aircraft on a map. |
| LoRa Sensor Network | ESP32 + LoRa module | ~€20 | Medium | Extends smart home outdoors. 5-10km range. Interesting protocol. |
| Spectrum Analyzer | RTL-SDR | ~€10 | Medium | Visualize RF environment. Builds on SDR scanner. |
| WiFi Motion Detection | ESP32 x2 | ~€10 | Medium | Detect movement via signal variance. Research-y, interesting signal processing. |

**Skipped:** WiFi RSSI Positioning — RSSI is too noisy indoors, weeks of work for 3m accuracy.

## Audio & DSP

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| Audio Spectrum Analyzer | ESP32 + microphone | ~€10 | Easy | FFT on microcontroller, real-time visualization. Great DSP intro. |
| Directional Mic + Signal Processing | Mic array + DSP | ~€20 | Medium | Beamforming, noise cancellation. |

**Skipped:** Acoustic Levitation — cool physics demo but trivial programming (fixed frequency PWM).

## Mapping & Sensing

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| Ultrasonic Sonar Mapper | HC-SR04 + servo | ~€10 | Easy | 2D sound-based map. Good starter. |
| GPS Tracker | ESP32 + GPS module | ~€15 | Easy | UART + NMEA parsing. Skills already in place from Module 7. |
| Thermal Camera | MLX90640 + OLED | ~€25 | Easy | I2C sensor, impressive visual output for low cost. |
| 2D LiDAR SLAM | RPLIDAR A1 + Pi | ~€100 | Medium | Use existing SLAM library first. Algorithm is a deep rabbit hole — don't write your own initially. |
| DIY Spectrometer | Diffraction grating + camera | ~€15 | Medium | Analyze light composition. Physics + signal processing. |
| Photogrammetry 3D Scanner | Phone + Meshroom | Free | Easy | No hardware dev. 3D model from photos. |
| DIY 3D Laser Scanner | Laser + camera + platform | ~€30 | Medium | Accurate object scanning. Mechanical + CV. |

## Motors & Robotics

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| Self-Balancing Robot | Motors + MPU6050 IMU | ~€30 | Medium | **PID on real hardware.** IMU sensor fusion, real-time. Natural next step after STM32. |
| Robotic Arm | Servo motors | ~€40 | Medium | Inverse kinematics math is non-trivial. Budget extra time. |
| Drone from Scratch | Brushless motors + ESC | ~€100 | Hard | Real project, not a learning exercise. Do after self-balancing robot. Safety-critical real-time. Months. |
| Magnetic Levitation | Electromagnet + Hall sensor | ~€25 | Hard | PID at very high update rates. Extremely satisfying, extremely frustrating. |

**Skipped:** CNC Plotter (you'd use GRBL, not write firmware), POV Display (mechanical engineering, not firmware).

## Magnetic & Electric Fields

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| DIY Magnetometer | HMC5883L sensor | ~€5 | Easy | Map magnetic field. I2C, simple. |
| Metal Detector | Inductive coil circuit | ~€15 | Medium | Analog electronics + signal processing. |
| Faraday Cage Tester | RTL-SDR | ~€10 | Easy | Measure RF shielding. Pairs with SDR work. |

**Skipped:** Tesla Coil, Induction Heater — high voltage, real danger, limited programming content. EE projects, not embedded SW.

## Optics & Vision

| Project | Hardware | Cost | Difficulty | Notes |
|---------|----------|------|------------|-------|
| OpenCV Object Tracking | Pi + camera | ~€35 | Medium | Real-time CV. Python or C++. |
| Night Vision Camera | Pi NoIR + IR LEDs | ~€30 | Easy | Quick build, feeds into surveillance node. |
| Laser Projector | Galvo mirrors + laser | ~€50 | Hard | Precise timing, impressive output. |

---

## Recommended start order

1. **RTL-SDR Scanner** — €10, immediate payoff, opens RF world
2. **Audio Spectrum Analyzer** — FFT on ESP32, quick DSP win
3. **Self-Balancing Robot** — PID on hardware, everything comes together
4. **LoRa Sensor Network** — extends smart home outdoors
5. **RPLIDAR SLAM** — after robot, when comfortable with real-time control
