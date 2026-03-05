# ESP32 Guitar Tuner

A full-featured guitar tuner running on ESP32 with OLED display and rotary encoder.

## Hardware Requirements

### Components
1. **ESP32 DevKit** (or any generic ESP32 board)
2. **0.96" OLED Display Module with EC11 Rotary Encoder** 
   - Display: SSD1306, 128x32 pixels, I2C interface
   - Rotary Encoder: EC11 with push button
   - [Product Link](https://robu.in/product/0-96-inch-oled-display-screen-rotary-encoder/)
3. **Direct Guitar Input Circuit** (see below)

### Guitar Input Preamp Components (with Buffered Output)
| Qty | Component | Value | Notes |
|-----|-----------|-------|-------|
| 1 | TL072 Dual Op-Amp | - | Both op-amps used |
| 1 | 1/4" Mono Audio Jack | - | Guitar INPUT |
| 1 | 1/4" Mono Audio Jack | - | Amp OUTPUT |
| 1 | Resistor | 1MΩ | Input impedance |
| 3 | Resistor | 100kΩ | Bias divider + feedback |
| 1 | Resistor | 10kΩ | Gain feedback |
| 1 | Resistor | 47kΩ | ADC protection |
| 3 | Electrolytic Capacitor | 10µF | Coupling caps |
| 1 | Ceramic Capacitor | 100nF | Power decoupling |

### Wiring Diagram

```
ESP32          OLED+Encoder Module (9-pin header)
------         ---------------------------------
3.3V    -----> VCC
GND     -----> GND
GPIO21  -----> SDA (I2C Data)
GPIO22  -----> SCL (I2C Clock)
GPIO32  -----> CLK (Encoder A)
GPIO33  -----> DT  (Encoder B)
GPIO25  -----> SW  (Encoder Button)

ESP32          Guitar Preamp Circuit Output
------         ----------------------------
3.3V    -----> Op-amp VCC (pin 8)
GND     -----> Op-amp GND (pin 4)
GPIO34  -----> Preamp output (via 47kΩ)
```

### Guitar Input Preamp Schematic (with Buffered Output to Amp)
```
   SIGNAL FLOW:
   
   GUITAR ──> [INPUT JACK] ──┬──> [OP-AMP A: Gain=11] ──> ESP32 (tuning)
              (1/4" jack)    │
                             └──> [OP-AMP B: Buffer]  ──> [OUTPUT JACK] ──> AMP
                                                          (1/4" jack)

   OP-AMP A (Tuner - pins 1,2,3):
   - Amplifies guitar signal 11x for ESP32 ADC
   - Adds 1.65V DC bias

   OP-AMP B (Buffer - pins 5,6,7):
   - Unity gain (1x) buffer
   - Low impedance output to amp
   - No tone loss through long cables
```

```
                        +3.3V
                          │
                     ┌────┴────┐
                     │  100kΩ  │
                     └────┬────┘
                          │ VREF (1.65V)
                     ┌────┴────┐
                     │  100kΩ  │
                     └────┬────┘
                          │
                         GND

  GUITAR     1MΩ      10µF     ┌─────────────────┐
  INPUT ────[====]────┤├───┬───┤3    TL072     1├────┤├────[47kΩ]───> GPIO34
  TIP                       │   │    OP-AMP A    │   10µF
                      VREF──┴───┤2               │
                                │           ┌────┤
                              10kΩ        100kΩ  │ (feedback)
                                │           │    │
                               GND──────────┴────┘
                                
                           10µF ┌─────────────────┐         10µF
  (from junction) ─────────┤├───┤5    TL072     7├─────────┤├────> OUTPUT
                                │    OP-AMP B    │                  TO AMP
                            ┌───┤6               │
                            │   └────────────────┘
                            └────────┘ (unity gain: pin 6 to pin 7)
                            
  GUITAR INPUT SLEEVE ──────────────────────────────────────────> GND
  AMP OUTPUT SLEEVE ────────────────────────────────────────────> GND
```

See `GUITAR_INPUT_CIRCUIT.h` for detailed schematic and breadboard layout.

### Pin Summary
| Function | ESP32 Pin |
|----------|-----------|
| I2C SDA  | GPIO 21   |
| I2C SCL  | GPIO 22   |
| Encoder CLK | GPIO 32 |
| Encoder DT  | GPIO 33 |
| Encoder SW  | GPIO 25 |
| Guitar IN | GPIO 34  |

## Software Setup

### Arduino IDE Setup

1. **Install ESP32 Board Support**
   - Open Arduino IDE
   - Go to `File > Preferences`
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`
   - Search "ESP32" and install "esp32 by Espressif Systems"

2. **Install Required Libraries**
   - Go to `Sketch > Include Library > Manage Libraries`
   - Install:
     - `Adafruit GFX Library`
     - `Adafruit SSD1306`

3. **Select Board**
   - Go to `Tools > Board > ESP32 Arduino`
   - Select "ESP32 Dev Module" or your specific board

4. **Upload Settings**
   - Upload Speed: 921600
   - CPU Frequency: 240MHz
   - Flash Frequency: 80MHz
   - Flash Mode: QIO
   - Flash Size: 4MB
   - Partition Scheme: Default 4MB with spiffs

5. **Upload**
   - Connect ESP32 via USB
   - Select correct COM port in `Tools > Port`
   - Click Upload

## Usage

### Menu Navigation
- **Rotate encoder**: Switch between tuning modes
- **Push encoder**: Enter/exit tuning mode

### Tuning Modes

| Mode | Description | Strings |
|------|-------------|---------|
| **CHROMATIC** | All 12 notes | Any note |
| **STANDARD** | Standard guitar | E A D G B E |
| **DROP D** | Drop D guitar | D A D G B E |
| **HALF DOWN** | Half step down | Eb Ab Db Gb Bb Eb |
| **DROP C** | Drop C guitar | C G C F A D |
| **OPEN G** | Open G guitar | D G D G B D |
| **DADGAD** | Celtic tuning | D A D G A D |
| **7-STRING** | 7-string standard | B E A D G B E |
| **7STR DROP A** | 7-string Drop A | A E A D G B E |
| **BASS 4-STR** | 4-string bass | E A D G |
| **BASS 5-STR** | 5-string bass | B E A D G |
| **BASS DROP D** | Bass Drop D | D A D G |

### Display Information

#### Chromatic Mode
```
┌──────────────────────────┐
│ E4  329.6Hz         OK   │
│     +2.5c                │
│ ◄═══════════╪═══════════►│
└──────────────────────────┘
```
- Note name and octave
- Detected frequency
- Cents deviation (+/- from target)
- Visual tuning meter

#### Guitar Mode
```
┌──────────────────────────┐
│ STR 6: E2    82.4Hz      │
│ +3.2 cents     TUNE DN   │
│ ◄═══════════╪═══════════►│
└──────────────────────────┘
```
- String number and target note
- Detected frequency
- Cents deviation
- Tuning direction indicator
- Visual tuning meter

### Tuning Tips

1. **In Tune**: Meter indicator in center, "OK" or "IN TUNE!" displayed
2. **Tune Up**: Note is flat (low), increase string tension
3. **Tune Down**: Note is sharp (high), decrease string tension
4. **Target**: Keep cents deviation within ±5 cents

## Troubleshooting

### No Display
- Check I2C connections (SDA, SCL)
- Verify OLED address is 0x3C (can try 0x3D)
- Check power supply (3.3V)

### Encoder Not Responding
- Verify CLK, DT, SW connections
- Check for loose connections
- Ensure pull-up resistors are enabled (internal pull-ups used)

### No Pitch Detection
- Check guitar cable connection
- Verify preamp circuit is powered (3.3V to op-amp)
- Check 10µF capacitors are correct polarity
- Ensure op-amp output reaches GPIO34
- Try a different guitar/cable

### Inaccurate Readings
- Adjust preamp gain if signal clips (reduce 100kΩ feedback resistor)
- Check for loose connections in preamp circuit
- Ensure good solder joints
- Use shielded cable from preamp to ESP32

### Signal Too Weak
- Increase gain: use 220kΩ instead of 100kΩ for feedback
- Check guitar volume knob is up
- Verify 1MΩ input resistor value

### Signal Clipping (Distorted)
- Decrease gain: use 47kΩ instead of 100kΩ for feedback
- For hot humbuckers, gain of 5-6 may be sufficient

## Technical Details

### Pitch Detection Algorithm
- Uses **autocorrelation** for fundamental frequency detection
- Sample rate: 8000 Hz
- Buffer size: 1024 samples
- Frequency range: 60 Hz - 1200 Hz (covers all guitar strings)

### Display
- Resolution: 128 x 32 pixels
- Driver: SSD1306
- Interface: I2C @ 400kHz

### Accuracy
- Typical accuracy: ±1-2 cents under good conditions
- Refresh rate: ~10 Hz

## License

Open source - free to use and modify.

## Credits

Built for the ESP32 platform using:
- Adafruit GFX Library
- Adafruit SSD1306 Library
- ESP32 Arduino Core
