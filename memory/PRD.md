# ESP32 Guitar Tuner - PRD

## Original Problem Statement
Build a Guitar Tuner with display and rotary encoder for ESP32, specifically for a module with built-in 0.96" OLED display (SSD1306, 128x32, I2C) and EC11 rotary encoder.

Hardware reference: https://robu.in/product/0-96-inch-oled-display-screen-rotary-encoder/

## User Choices
- ESP32: Any generic ESP32 board
- Audio Input: Electret microphone
- Framework: Arduino IDE
- Features: Chromatic mode + Multiple guitar tunings + Bass + 7-string

## Architecture
- **Platform**: ESP32 (Arduino framework)
- **Display**: SSD1306 OLED 128x32 via I2C
- **Input**: EC11 Rotary encoder (GPIO interrupts)
- **Audio**: ADC sampling with timer interrupt
- **Algorithm**: Autocorrelation-based pitch detection

## Core Requirements
1. Real-time pitch detection (25Hz - 1200Hz range)
2. Visual cents deviation meter
3. Rotary encoder menu navigation
4. Multiple tuning presets

## What's Been Implemented
- [x] Main Arduino sketch with full tuner logic (Jan 2026)
- [x] Autocorrelation pitch detection algorithm
- [x] SSD1306 OLED display integration
- [x] EC11 rotary encoder with interrupt handling
- [x] Chromatic tuner mode
- [x] 6-string guitar tunings: Standard, Drop D, Half-Step Down, Drop C, Open G, DADGAD
- [x] 7-string guitar tunings: Standard, Drop A (Jan 2026)
- [x] Bass guitar tunings: 4-string, 5-string, Drop D (Jan 2026)
- [x] Wiring guide and documentation

## Tuning Modes (12 total)
| Mode | Strings |
|------|---------|
| Chromatic | All notes |
| Standard 6-str | E A D G B E |
| Drop D | D A D G B E |
| Half-Step Down | Eb Ab Db Gb Bb Eb |
| Drop C | C G C F A D |
| Open G | D G D G B D |
| DADGAD | D A D G A D |
| 7-String | B E A D G B E |
| 7-String Drop A | A E A D G B E |
| Bass 4-String | E A D G |
| Bass 5-String | B E A D G |
| Bass Drop D | D A D G |

## Files
- `/app/esp32_guitar_tuner/esp32_guitar_tuner.ino` - Main sketch
- `/app/esp32_guitar_tuner/pitchdetect.h` - Pitch detection header
- `/app/esp32_guitar_tuner/tunings.h` - Tuning definitions
- `/app/esp32_guitar_tuner/WIRING_GUIDE.h` - Wiring diagram
- `/app/esp32_guitar_tuner/README.md` - Documentation
- `/app/esp32_guitar_tuner.zip` - Downloadable package

## Backlog / Future Enhancements
- P1: Auto-detect string being played
- P1: Strobe tuner display mode
- P2: Ukulele tuning support (GCEA)
- P2: Custom tuning editor (save to EEPROM)
- P2: Calibration mode (adjust A4 reference)
- P3: Metronome feature
- P3: Battery level indicator (if battery powered)
