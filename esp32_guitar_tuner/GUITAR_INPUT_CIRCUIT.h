/*
 * ESP32 Guitar Tuner - Guitar Input Circuit
 * ==========================================
 * 
 * GUITAR INPUT PREAMP CIRCUIT
 * ===========================
 * 
 * Guitar pickups output a very weak signal (~100mV peak).
 * ESP32 ADC needs 0-3.3V range centered at ~1.65V.
 * This circuit amplifies and biases the guitar signal.
 * 
 * 
 * COMPONENTS NEEDED:
 * ==================
 * - 1x 1/4" (6.35mm) Mono Audio Jack (guitar input)
 * - 1x TL072 or LM358 Op-Amp (dual op-amp IC)
 * - 2x 100kΩ resistors (voltage divider for bias)
 * - 1x 1MΩ resistor (input impedance)
 * - 1x 10kΩ resistor (feedback)
 * - 1x 100kΩ resistor (feedback - gain control)
 * - 2x 10µF electrolytic capacitors (coupling)
 * - 1x 100nF ceramic capacitor (power decoupling)
 * - 1x 47kΩ resistor (output protection)
 * 
 * 
 * SCHEMATIC:
 * ==========
 * 
 *                                    +3.3V
 *                                      │
 *                                      │
 *     ┌──────────────────────────────┬─┴─┬──────────────────────┐
 *     │                              │   │                      │
 *     │                         100kΩ│   │100nF                 │
 *     │                              │   │ (decoupling)         │
 *     │                              │   │                      │
 *     │    1MΩ         10µF     ┌────┴───┴────┐                  │
 *     │   ┌───┐       ┌───┐     │             │                  │
 *  ┌──┴───┤   ├───┬───┤ + ├─────┤2-  TL072  3 ├──────┬───────────┤
 *  │      └───┘   │   └───┘     │      A      │      │           │
 *  │              │             │    ┌───┐    │      │           │
 * ─┴─             │             │  1 │   │ 8  │      │  10µF     │
 * GUITAR          │             │ OUT│   │VCC+│    ┌─┴─┐   47kΩ  │
 * INPUT           │             └──┬─┴───┴──┬─┘    │ + │  ┌───┐  │
 * JACK            │                │        │      └─┬─┘  │   │  │
 * (TIP)           │           100kΩ│        │        │    └─┬─┘  │
 *                 │                │        │        └──────┴────┼──> GPIO34
 *  ─┬─            │                │        │                    │    (ADC)
 *  │ SLEEVE       │             ┌──┴──┐     │                    │
 *  │ (GND)        │             │     │     │                    │
 *  │              │         100kΩ     │     │                    │
 *  │              │             │     │     │                    │
 *  │              │             │   GND   GND                    │
 *  │              │             │   (pin4)                       │
 *  │              │             │                                │
 *  │              │    10kΩ     │                                │
 *  │              │   ┌───┐     │                                │
 *  │              └───┤   ├─────┘                                │
 *  │                  └───┘                                      │
 *  │                    │                                        │
 *  │                    │  100kΩ                                 │
 *  │                    │ ┌───┐                                  │
 *  │                    └─┤   ├──────────────────────────────────┘
 *  │                      └───┘                                  
 *  │                        │                                    
 *  └────────────────────────┴─────────────────────> GND
 * 
 * 
 * SIMPLIFIED CIRCUIT EXPLANATION:
 * ===============================
 * 
 *                    +3.3V
 *                      │
 *                    [100kΩ]──┬──[100nF]──GND
 *                             │
 *                         VREF (1.65V bias)
 *                             │
 *   GUITAR ──[1MΩ]──[10µF]──┬─┤
 *   TIP                     │ │
 *                        [10kΩ]
 *                           │
 *                    ┌──────┴──────┐
 *                    │   TL072     │
 *                    │   Op-Amp    │──[10µF]──[47kΩ]──> GPIO34
 *                    │   (Gain~11)│
 *                    └──────┬──────┘
 *                           │
 *                        [100kΩ]
 *                           │
 *                          GND
 * 
 * 
 * PIN CONNECTIONS:
 * ================
 * 
 * TL072/LM358 Op-Amp Pinout:
 * ┌────────────┐
 * │  TL072     │
 * │ ┌──────┐   │
 * │ │1    8│   │  Pin 1: Output A      -> To coupling capacitor
 * │ │2    7│   │  Pin 2: Inverting A   -> Feedback network
 * │ │3    6│   │  Pin 3: Non-Inv A     -> Guitar signal input
 * │ │4    5│   │  Pin 4: V-            -> GND
 * │ └──────┘   │  Pin 8: V+            -> 3.3V
 * └────────────┘  (Pins 5,6,7 unused - second op-amp)
 * 
 * 
 * COMPLETE WIRING TABLE:
 * ======================
 * 
 * ┌─────────────────────┬─────────────────┬─────────────────────────────┐
 * │ Component           │ Connect To      │ Notes                       │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ Guitar Jack TIP     │ 1MΩ resistor    │ Input impedance             │
 * │ Guitar Jack SLEEVE  │ GND             │ Ground reference            │
 * │ 1MΩ other end       │ 10µF cap (+)    │ DC blocking                 │
 * │ 10µF cap (-)        │ TL072 Pin 3     │ Signal to op-amp            │
 * │ TL072 Pin 3         │ Bias point      │ 1.65V from voltage divider  │
 * │ TL072 Pin 2         │ 10kΩ to GND     │ Feedback resistor           │
 * │ TL072 Pin 2         │ 100kΩ to Pin 1  │ Gain = 1 + 100k/10k = 11    │
 * │ TL072 Pin 1         │ 10µF cap (+)    │ Output coupling             │
 * │ 10µF cap (-)        │ 47kΩ resistor   │ Output protection           │
 * │ 47kΩ other end      │ ESP32 GPIO34    │ ADC input                   │
 * │ TL072 Pin 4         │ GND             │ Negative supply             │
 * │ TL072 Pin 8         │ 3.3V            │ Positive supply             │
 * │ 100nF cap           │ Pin 8 to GND    │ Power decoupling            │
 * │ 100kΩ + 100kΩ       │ 3.3V-mid-GND    │ Voltage divider (1.65V)     │
 * └─────────────────────┴─────────────────┴─────────────────────────────┘
 * 
 * 
 * BREADBOARD LAYOUT:
 * ==================
 * 
 *     Power Rails: +3.3V (red), GND (blue)
 *     
 *     +3.3V ═══════════════════════════════════════════════════════
 *     
 *     Row 1:  [100kΩ to +3.3V]──●──[100kΩ to GND]  (bias = 1.65V)
 *                               │
 *     Row 2:  ●─────────────────┴──[100nF]──●(GND)
 *             │(bias point)
 *     Row 3:  │    [TL072 IC placed here, pins 1-4 on left]
 *             │     ┌─────────┐
 *     Row 4:  │     │ 1  ○  8 │──●──(+3.3V)
 *             │     │ 2     7 │
 *     Row 5:  └─────│ 3     6 │
 *                   │ 4     5 │──●──(GND)
 *                   └─────────┘
 *     
 *     Row 6:  [Guitar Jack TIP]──[1MΩ]──[10µF +]──●(to pin 3)
 *     
 *     Row 7:  [Pin 2]──[10kΩ]──●(GND)
 *                 │
 *     Row 8:      └──[100kΩ]──●(to Pin 1)
 *     
 *     Row 9:  [Pin 1]──[10µF +]──[47kΩ]──●──> GPIO34
 *     
 *     GND ═══════════════════════════════════════════════════════
 * 
 * 
 * GAIN CALCULATION:
 * =================
 * 
 * Non-inverting amplifier gain: G = 1 + (Rf/Ri)
 * With Rf = 100kΩ and Ri = 10kΩ:
 * G = 1 + (100000/10000) = 1 + 10 = 11
 * 
 * Guitar signal: ~100mV peak typical
 * Amplified signal: ~1.1V peak
 * With 1.65V bias: Signal swings from ~0.55V to ~2.75V
 * This is within ESP32 ADC range (0-3.3V) ✓
 * 
 * For hotter pickups (humbuckers), you may want to reduce gain:
 * - Use 47kΩ instead of 100kΩ for Rf → Gain = 5.7
 * - Or add a 100kΩ potentiometer for adjustable gain
 * 
 * 
 * ALTERNATIVE: SIMPLER PASSIVE CIRCUIT (Lower Quality)
 * ====================================================
 * 
 * If you don't have an op-amp, this passive circuit works
 * but with lower signal quality:
 * 
 *   GUITAR ──[10µF]──┬──[100kΩ]──(+3.3V)
 *   TIP              │
 *                    ├──[100kΩ]──(GND)
 *                    │
 *                    └──[47kΩ]──> GPIO34
 *   
 *   SLEEVE ──────────────────────> GND
 * 
 * Note: Passive circuit has no amplification, 
 * works best with high-output pickups.
 * 
 * 
 * PARTS LIST (for active preamp):
 * ===============================
 * 
 * │ Qty │ Component              │ Value    │ Notes                    │
 * │─────│────────────────────────│──────────│──────────────────────────│
 * │  1  │ TL072 / LM358 Op-Amp   │ -        │ Dual op-amp IC           │
 * │  1  │ 1/4" Mono Audio Jack   │ -        │ Guitar input             │
 * │  1  │ Resistor               │ 1MΩ      │ Input impedance          │
 * │  2  │ Resistor               │ 100kΩ    │ Bias divider + feedback  │
 * │  1  │ Resistor               │ 10kΩ     │ Feedback                 │
 * │  1  │ Resistor               │ 47kΩ     │ Output protection        │
 * │  2  │ Electrolytic Capacitor │ 10µF     │ Coupling (observe pol!)  │
 * │  1  │ Ceramic Capacitor      │ 100nF    │ Power decoupling         │
 * │  1  │ 8-pin DIP socket       │ -        │ Optional, for op-amp     │
 * 
 */
