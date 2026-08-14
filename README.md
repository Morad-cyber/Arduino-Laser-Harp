# 🎵 Arduino Laser Harp

![Arduino Laser Harp](laser-harp-final.png)

## Mechatronics & Sensor Technology Project

An interdisciplinary engineering project focused on the development of an
eight-string, contactless Laser Harp.

The system combines optics, electronics, sensor technology, embedded
programming and MIDI communication. When a laser beam is interrupted,
the corresponding LDR sensor detects the change in light intensity.
The Arduino processes the sensor signal and generates the corresponding
MIDI note.

---

## 📌 Project Overview

The goal of the project was to design and build a functional Laser Harp
that converts physical interaction with laser beams into digital musical
signals.

The system consists of eight laser beams and eight light-dependent
resistors (LDRs). Each laser beam represents one virtual string of the
instrument.

The complete signal chain is:

**Laser interruption → LDR sensor → Arduino → Serial communication
→ MIDI → Virtual Instrument**

The MIDI signals can be routed to a DAW or VST host to generate the
corresponding sound.

---

## ⚙️ Key Features

- 8 laser beams / virtual strings
- 8 LDR light sensors
- Arduino Mega 2560
- Contactless note triggering
- Sensor calibration during system startup
- Three selectable musical scales
- MIDI note generation
- Signal hysteresis for stable detection
- Dedicated system and scale-status LEDs
- External power supply for stable operation

---

## 🔧 Hardware

Main components include:

- Arduino Mega 2560
- 8× KY-008 laser modules
- 8× GL5528 LDR sensors
- BC337 transistor
- 10 kΩ resistors
- 330 Ω base resistor
- 12 Ω laser resistors
- Push buttons for system and scale control
- Status LEDs
- 9 V / 1500 mA DC power supply

The electronic design, circuit diagrams, component selection and
measurements are documented in the project documentation.

---

## 💻 Software

The Arduino firmware is structured into separate areas for:

- Hardware I/O
- System state management
- Sensor calibration
- Scale selection
- Laser-string scanning
- MIDI note generation
- Note-off / panic handling

The software uses a central `harp_context_t` structure to manage the
system state.

A threshold-based detection method with hysteresis is used to prevent
unwanted note triggering when a hand is close to the edge of a laser beam.

---

## 🎹 MIDI Integration

The Arduino sends the detected musical events through serial
communication.

The project uses a software chain consisting of:

**Arduino → Hairless MIDI/Serial Bridge → loopMIDI → DAW/VST**

This allows the Laser Harp to control virtual instruments and produce
musical output.

---

## 🎥 Video Demonstration

➡️ **[Watch the Laser Harp Demonstration](https://youtu.be/LRuz3PJn_Mw)**

---

## 📄 Documentation

➡️ **[Read the Complete Project Documentation](Documentation_v1-5.pdf)**

The documentation contains the complete development process, including:

- Project planning
- Physical principles
- Hardware development
- Circuit design
- Software implementation
- MIDI integration
- Housing and construction
- Troubleshooting
- Measurements and testing
- Laser safety validation
- Final results and future improvements

---

## 💾 Source Code

➡️ **[View the Arduino Source Code](Code/sketch_dec1b.ino)**

The repository contains the complete Arduino firmware used for the
Laser Harp.

---

## 🚀 Future Development

Possible future improvements described in the project documentation
include:

- Adding ultrasonic sensing for expressive volume/modulation control
- Developing a standalone version using a Raspberry Pi
- Integrating sound generation directly into the instrument
- Reducing the dependence on an external computer

---

## 👨‍💻 Project Team

**Morad Lahrour**  
**Ahmad Berkel**  
**Majd Hagig**

Hochschule für Technik und Wirtschaft des Saarlandes (htw saar)

---

## 📚 Project Information

**Project:** Arduino Laser Harp  
**Project Number:** 2025-LH-001  
**Institution:** Hochschule für Technik und Wirtschaft des Saarlandes  
**Project Period:** November 2025 – February 2026


