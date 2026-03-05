/*
 * ESP32 Guitar Tuner - Compact PCB Layout for 125B Enclosure
 * ===========================================================
 * 
 * 125B ENCLOSURE DIMENSIONS:
 * ==========================
 * External: 122mm x 66mm x 39mm (4.8" x 2.6" x 1.5")
 * Internal usable: ~116mm x 60mm x 35mm
 * 
 * PCB SIZE: 100mm x 50mm (fits with room for wiring)
 * 
 * 
 * ENCLOSURE DRILL TEMPLATE (Top View):
 * ====================================
 * 
 *     ←─────────────── 122mm ───────────────→
 *     ┌─────────────────────────────────────┐ ↑
 *     │                                     │ │
 *     │      ┌───────────────────┐          │ │
 *     │      │   OLED DISPLAY    │          │ │
 *     │      │   (28mm x 12mm    │          │ │
 *     │      │    cutout)        │          │ │
 *     │      └───────────────────┘          │ │
 *     │              ↑                      │ │
 *     │           30mm from top             │ 66mm
 *     │              ↓                      │ │
 *     │         ○ ENCODER                   │ │
 *     │        (7mm hole)                   │ │
 *     │                                     │ │
 *     │                                     │ │
 *     │              ○                      │ │
 *     │         FOOTSWITCH                  │ │
 *     │         (12mm hole)                 │ ↓
 *     └─────────────────────────────────────┘
 *     
 *     Left side:    ○ INPUT JACK (10mm hole)
 *     Right side:   ○ OUTPUT JACK (10mm hole)
 *     Top/Back:     ○ DC POWER (8mm hole) - optional
 * 
 * 
 * PCB LAYOUT (Component Side - Top View):
 * =======================================
 * 
 * PCB Size: 100mm x 50mm
 * All measurements from bottom-left corner
 * 
 *     ←────────────────── 100mm ──────────────────→
 *     
 *     ┌──────────────────────────────────────────────┐  ↑
 *  50 │  ○           [OLED HEADER]           ○      │  │
 *     │  M3     ┌─────────────────────┐      M3     │  │
 *  45 │         │ 1  2  3  4  5  6  7 │             │  │
 *     │         │ GND VCC SCL SDA ... │             │  │
 *  40 │         └─────────────────────┘             │  │
 *     │                                             │  │
 *  35 │    [ENCODER]        ┌─────────────────┐     │  │
 *     │    ○ ○ ○ ○ ○        │                 │     │  │
 *  30 │    C D S + -        │  ESP-WROOM-32   │     │  │
 *     │                     │    MODULE       │     │  │
 *  25 │    ┌─────┐          │   (25x18mm)     │     │  │
 *     │    │TL072│          │                 │     │  50mm
 *  20 │    │     │          │    ○    ○       │     │  │
 *     │    │1   8│          └─────────────────┘     │  │
 *  15 │    │2   7│                                  │  │
 *     │    │3   6│     R1  R2  R3  R4  R5  R6      │  │
 *  10 │    │4   5│     ░░  ░░  ░░  ░░  ░░  ░░      │  │
 *     │    └─────┘                                  │  │
 *   5 │                 C1   C2   C3   C4          │  │
 *     │    IN  OUT      ██   ██   ██   ░░          │  │
 *   0 │  ○  ●    ●  ○                          ○   │  │
 *     │  M3          M3                        M3  │  ↓
 *     └──────────────────────────────────────────────┘
 *       0    10   20   30   40   50   60   70   80   90  100
 * 
 * 
 * COMPONENT PLACEMENT (Coordinates in mm):
 * ========================================
 * 
 * │ Component        │ X     │ Y     │ Notes                    │
 * │──────────────────│───────│───────│──────────────────────────│
 * │ TL072 (U1)       │ 15    │ 17    │ 8-pin DIP, pin 1 at SW   │
 * │ ESP-WROOM-32     │ 60    │ 25    │ Module center point      │
 * │ OLED Header      │ 50    │ 43    │ 7-pin header for module  │
 * │ Encoder Header   │ 15    │ 33    │ 5-pin (CLK,DT,SW,+,-)    │
 * │──────────────────│───────│───────│──────────────────────────│
 * │ R1 (1MΩ)         │ 35    │ 12    │ Input impedance          │
 * │ R2 (100kΩ)       │ 42    │ 12    │ Bias divider upper       │
 * │ R3 (100kΩ)       │ 49    │ 12    │ Bias divider lower       │
 * │ R4 (10kΩ)        │ 56    │ 12    │ Gain feedback            │
 * │ R5 (100kΩ)       │ 63    │ 12    │ Gain feedback            │
 * │ R6 (47kΩ)        │ 70    │ 12    │ ADC protection           │
 * │──────────────────│───────│───────│──────────────────────────│
 * │ C1 (10µF)        │ 35    │ 5     │ Input coupling           │
 * │ C2 (10µF)        │ 45    │ 5     │ Buffer input coupling    │
 * │ C3 (10µF)        │ 55    │ 5     │ Output coupling          │
 * │ C4 (100nF)       │ 65    │ 5     │ Power decoupling         │
 * │──────────────────│───────│───────│──────────────────────────│
 * │ INPUT pad        │ 10    │ 2     │ Wire to 1/4" jack        │
 * │ OUTPUT pad       │ 25    │ 2     │ Wire to 1/4" jack        │
 * │ M3 holes         │corners│       │ 5,5 / 5,45 / 95,5 / 95,45│
 * 
 * 
 * SCHEMATIC TO PCB MAPPING:
 * =========================
 * 
 *                      +3.3V (from ESP32 3V3 pin)
 *                         │
 *                    ┌────┴────┐
 *                    │ R2 100k │
 *                    └────┬────┘
 *                         ├──────────────────────────┐
 *                    ┌────┴────┐                     │
 *                    │ R3 100k │                     │
 *                    └────┬────┘                     │
 *                         │                         │
 *                        GND                        │(VREF 1.65V)
 *                                                   │
 *   IN ──[R1 1M]──[C1 +]──┬─────────────────────────┼───> TL072 Pin 3
 *   PAD              10µF │                         │
 *                         │                         │
 *                         ├──[C2 +]─────────────────┼───> TL072 Pin 5
 *                         │   10µF                  │
 *                         │                         │
 *                         │              TL072 Pin 2├──┬──[R4 10k]──GND
 *                         │                         │  │
 *                         │                         │  └──[R5 100k]──Pin 1
 *                         │                         │
 *                         │              TL072 Pin 1├──[C3 +]──[R6 47k]──> GPIO34
 *                         │                              10µF
 *                         │
 *                         │              TL072 Pin 6├──────┐
 *                         │                         │      │
 *                         │              TL072 Pin 7├──────┼──[C4 +]──> OUT PAD
 *                         │                                     10µF
 *                         │
 *                         │              TL072 Pin 8├──[C4 100n]──GND
 *                         │                         │      │
 *                         │                         └──────┴──> +3.3V
 *                         │
 *                         │              TL072 Pin 4├──> GND
 * 
 * 
 * PCB TRACES (Simplified - Single Layer):
 * =======================================
 * 
 * Use 0.5mm traces for signals, 1mm for power/ground
 * 
 *     ┌──────────────────────────────────────────────┐
 *     │                                              │
 *     │  ════════════════ +3.3V RAIL ═══════════════│
 *     │                                              │
 *     │         OLED: SDA──────────────GPIO21       │
 *     │                SCL──────────────GPIO22       │
 *     │                                              │
 *     │    ENC: CLK────────────────────GPIO32       │
 *     │         DT─────────────────────GPIO33       │
 *     │         SW─────────────────────GPIO25       │
 *     │                                              │
 *     │    TL072                                     │
 *     │    Pin1 ──C3──R6──────────────GPIO34        │
 *     │    Pin7 ──C4──────────────────OUT PAD       │
 *     │                                              │
 *     │    IN PAD──R1──C1─────────────Pin3,Pin5     │
 *     │                                              │
 *     │  ════════════════ GND RAIL ═════════════════│
 *     │                                              │
 *     └──────────────────────────────────────────────┘
 * 
 * 
 * FOOTSWITCH WIRING (True Bypass - 3PDT):
 * =======================================
 * 
 * The footswitch is wired OFF-BOARD for easier assembly.
 * Use a 3PDT (3-pole double-throw) latching switch.
 * 
 *                    3PDT FOOTSWITCH
 *                    ┌───────────────┐
 *                    │ 1   2   3     │  ← Row 1 (top)
 *                    │ ○   ○   ○     │
 *                    │               │
 *                    │ 4   5   6     │  ← Row 2 (middle/common)
 *                    │ ○   ○   ○     │
 *                    │               │
 *                    │ 7   8   9     │  ← Row 3 (bottom)
 *                    │ ○   ○   ○     │
 *                    └───────────────┘
 * 
 * BYPASS MODE (LED off):
 *   - Guitar signal goes directly to amp (true bypass)
 *   - Tuner circuit not in signal path
 * 
 * TUNER MODE (LED on):
 *   - Guitar signal goes through tuner circuit
 *   - Buffered output to amp
 * 
 * WIRING:
 * 
 *   Pin 1 ──> INPUT JACK TIP
 *   Pin 2 ──> PCB INPUT PAD (to tuner preamp)
 *   Pin 3 ──> (not used, or link to pin 1 for always-on tuner input)
 *   
 *   Pin 4 ──> OUTPUT JACK TIP  
 *   Pin 5 ──> PCB OUTPUT PAD (buffered output from tuner)
 *   Pin 6 ──> Link to Pin 1 (direct signal for bypass)
 *   
 *   Pin 7 ──> LED Cathode (-)
 *   Pin 8 ──> GND
 *   Pin 9 ──> +3.3V via 1kΩ resistor (LED anode +)
 * 
 * 
 *   BYPASS MODE (switch up):        TUNER MODE (switch down):
 *   
 *   IN───●───────────●───OUT        IN───●               ●───OUT
 *        │    4-6    │                   │  2      5     │
 *        1     ↓     4                   1    ↓      ↓   4
 *             [  ]                           [  ]  [  ]
 *        2     ↑     5                   2    ↑      ↑   5
 *        │           │                   │    │      │   │
 *        └──TUNER────┘                   └──TUNER IN │   │
 *          (disconn)                                 └───┘
 *                                                  TUNER OUT
 * 
 * 
 * ALTERNATIVE: ACTIVE BYPASS (No 3PDT needed):
 * ============================================
 * 
 * If you want to skip the 3PDT switch, you can use a simple
 * momentary switch + relay, or just leave the tuner always inline
 * (the buffer actually improves your tone on long cable runs).
 * 
 * For always-inline wiring:
 *   - INPUT JACK TIP ──> PCB INPUT PAD
 *   - PCB OUTPUT PAD ──> OUTPUT JACK TIP
 *   - INPUT JACK SLEEVE ──> GND
 *   - OUTPUT JACK SLEEVE ──> GND
 *   - Simple SPST footswitch can mute output or control LED
 * 
 * 
 * 125B ENCLOSURE ASSEMBLY:
 * ========================
 * 
 *     TOP VIEW (lid removed):
 *     
 *     ┌─────────────────────────────────────────┐
 *     │ ○                                     ○ │ ← Lid screw holes
 *     │   ┌─────────────────────────────────┐   │
 *     │   │                                 │   │
 *     │   │     PCB (component side up)     │   │
 *     │   │         100mm x 50mm            │   │
 *     │   │                                 │   │
 *     │   │  [TL072]    [ESP32]    [OLED]   │   │
 *     │   │                        header   │   │
 *     │   │  [resistors]  [caps]            │   │
 *     │   │                                 │   │
 *     │   └──────────────┬──────────────────┘   │
 *     │                  │                      │
 *     │    ┌─────────────┴─────────────┐        │
 *     │    │       WIRES TO:           │        │
 *     │    │  - Input/Output jacks     │        │
 *     │    │  - Footswitch             │        │
 *     │    │  - DC power jack          │        │
 *     │    └───────────────────────────┘        │
 *     │                                         │
 *     │              [3PDT SWITCH]              │
 *     │                  ○○○                    │
 *     │                  ○○○                    │
 *     │                  ○○○                    │
 *     │                                         │
 *     │ ○                                     ○ │
 *     └─────────────────────────────────────────┘
 *     
 *     SIDE VIEW:
 *     
 *     ┌────────────────────┐
 *     │░░░░░░░░░░░░░░░░░░░░│ ← Lid with OLED window + encoder hole
 *     │                    │
 *     │ ══PCB══            │ ← PCB mounted on standoffs
 *     │    │               │
 *     │ ┌──┴──┐            │
 *  ●──│─┤JACK │            │ ← Input jack on side
 *     │ └─────┘            │
 *     │         ┌──────┐   │
 *     │         │SWITCH│   │ ← Footswitch from bottom
 *     └─────────┴──────┴───┘
 * 
 * 
 * OLED + ENCODER MODULE MOUNTING:
 * ===============================
 * 
 * The OLED+Encoder module mounts to the ENCLOSURE LID, not the PCB.
 * Use wires to connect from PCB header to module.
 * 
 *     ENCLOSURE LID (inside view):
 *     
 *     ┌─────────────────────────────────┐
 *     │                                 │
 *     │    ┌─────────────────────┐      │
 *     │    │▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│      │  ← OLED display visible
 *     │    │▓▓▓ OLED WINDOW ▓▓▓▓│      │    through rectangular cutout
 *     │    └─────────────────────┘      │
 *     │              │                  │
 *     │              ○ ← Encoder shaft  │  ← 7mm hole for encoder
 *     │              │    through hole  │
 *     │    ┌─────────┴─────────┐        │
 *     │    │  OLED+ENC MODULE  │        │  ← Module mounted with
 *     │    │   (mounted with   │        │    hot glue or bracket
 *     │    │    standoffs)     │        │
 *     │    │  [wire harness]   │        │
 *     │    └────────┬──────────┘        │
 *     │             │                   │
 *     │         to PCB header           │
 *     │                                 │
 *     └─────────────────────────────────┘
 * 
 * 
 * WIRE HARNESS (7-wire ribbon recommended):
 * =========================================
 * 
 *     OLED+Encoder Module    PCB Header
 *     ──────────────────     ──────────
 *     VCC  ─────────────────  +3.3V
 *     GND  ─────────────────  GND
 *     SCL  ─────────────────  GPIO22
 *     SDA  ─────────────────  GPIO21
 *     CLK  ─────────────────  GPIO32
 *     DT   ─────────────────  GPIO33
 *     SW   ─────────────────  GPIO25
 * 
 * 
 * POWER OPTIONS:
 * ==============
 * 
 * Option 1: USB Power (simplest)
 *   - Use USB-C or micro-USB breakout board
 *   - Mount USB jack on enclosure side
 *   - Connect 5V to ESP32 VIN pin
 * 
 * Option 2: 9V DC Jack (standard pedal power)
 *   - Use 2.1mm DC jack (center negative, standard for pedals)
 *   - Add 7805 or AMS1117-3.3 voltage regulator
 *   - Or use LM2596 DC-DC buck converter module
 * 
 * Option 3: 9V Battery
 *   - Add battery clip inside enclosure
 *   - Use voltage regulator as above
 *   - Add power switch or use input jack switching
 * 
 * 
 * GERBER FILE NOTES:
 * ==================
 * 
 * For professional PCB fabrication, create Gerber files with:
 * - 2-layer board (top + bottom copper)
 * - 1.6mm thickness
 * - HASL or ENIG finish
 * - Green solder mask (or your preference)
 * - White silkscreen
 * 
 * Minimum specs:
 * - Trace width: 0.25mm (10 mil)
 * - Trace spacing: 0.25mm (10 mil)
 * - Via size: 0.8mm drill, 1.2mm pad
 * - Hole sizes: M3 = 3.2mm, component = as needed
 * 
 * 
 * BILL OF MATERIALS (BOM):
 * ========================
 * 
 * │ Qty │ Component              │ Value/Type    │ Package     │
 * │─────│────────────────────────│───────────────│─────────────│
 * │  1  │ ESP-WROOM-32 module    │ -             │ Module      │
 * │  1  │ TL072 op-amp           │ -             │ DIP-8       │
 * │  1  │ 8-pin DIP socket       │ -             │ DIP-8       │
 * │  1  │ OLED+Encoder module    │ 0.96" SSD1306 │ Module      │
 * │─────│────────────────────────│───────────────│─────────────│
 * │  1  │ Resistor               │ 1MΩ           │ 1/4W axial  │
 * │  3  │ Resistor               │ 100kΩ         │ 1/4W axial  │
 * │  1  │ Resistor               │ 10kΩ          │ 1/4W axial  │
 * │  1  │ Resistor               │ 47kΩ          │ 1/4W axial  │
 * │  1  │ Resistor               │ 1kΩ           │ 1/4W (LED)  │
 * │─────│────────────────────────│───────────────│─────────────│
 * │  3  │ Electrolytic capacitor │ 10µF/25V      │ Radial 5mm  │
 * │  1  │ Ceramic capacitor      │ 100nF         │ Disc/MLCC   │
 * │─────│────────────────────────│───────────────│─────────────│
 * │  2  │ 1/4" Mono jack         │ Switchcraft   │ Panel mount │
 * │  1  │ 3PDT footswitch        │ Latching      │ Panel mount │
 * │  1  │ LED 3mm                │ Any color     │ 3mm         │
 * │  1  │ DC power jack          │ 2.1mm         │ Panel mount │
 * │  1  │ 125B enclosure         │ Aluminum      │ -           │
 * │─────│────────────────────────│───────────────│─────────────│
 * │  4  │ M3 standoffs           │ 10mm          │ M3          │
 * │  8  │ M3 screws              │ 6mm           │ M3          │
 * │  1  │ 7-pin header           │ 2.54mm pitch  │ Through-hole│
 * │  1  │ Hookup wire            │ 22-24 AWG     │ Stranded    │
 * 
 */
