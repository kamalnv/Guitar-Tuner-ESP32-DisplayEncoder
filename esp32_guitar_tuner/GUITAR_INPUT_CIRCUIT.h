/*
 * ESP32 Guitar Tuner - Guitar Input Circuit
 * ==========================================
 * 
 * GUITAR INPUT PREAMP CIRCUIT WITH BUFFERED OUTPUT
 * =================================================
 * 
 * This circuit:
 * 1. Takes guitar signal input
 * 2. Buffers and amplifies for ESP32 ADC
 * 3. Provides clean buffered output to amplifier
 * 
 * You can leave the tuner inline - guitar -> tuner -> amp
 * 
 * 
 * COMPONENTS NEEDED:
 * ==================
 * - 1x 1/4" (6.35mm) Mono Audio Jack (guitar INPUT)
 * - 1x 1/4" (6.35mm) Mono Audio Jack (amp OUTPUT)
 * - 1x TL072 Dual Op-Amp (both op-amps used)
 * - 3x 100kΩ resistors
 * - 1x 1MΩ resistor (input impedance)
 * - 1x 10kΩ resistor (gain feedback)
 * - 3x 10µF electrolytic capacitors
 * - 1x 100nF ceramic capacitor (power decoupling)
 * - 1x 47kΩ resistor (ADC protection)
 * 
 * 
 * FULL SCHEMATIC WITH INPUT AND OUTPUT:
 * =====================================
 * 
 *                                         +3.3V
 *                                           │
 *                         ┌─────────────────┼─────────────────────────┐
 *                         │                 │                         │
 *                         │            ┌────┴────┐                    │
 *                         │            │  100kΩ  │                    │
 *                         │            └────┬────┘                    │
 *                         │                 │ VREF (1.65V)            │
 *                         │            ┌────┴────┐                    │
 *                         │            │  100kΩ  │                    │
 *                         │            └────┬────┘                    │
 *                         │                 │                         │
 *                         │                GND                        │
 *                         │                                           │
 *     ┌───────────────────┼───────────────────────────────────────────┤
 *     │                   │                                           │
 *     │   1MΩ      10µF   │        ┌───────────────────┐              │
 *     │  ┌───┐    ┌───┐   │        │     TL072         │              │
 *  ───┴──┤   ├──┬─┤ + ├───┼────────┤3              1├──┬──┤├──[47kΩ]──┼──> GPIO34
 *  GUITAR└───┘  │ └───┘   │        │   OP-AMP A       │  │ 10µF       │     (ADC)
 *  INPUT        │         │    ┌───┤2                 │  │            │
 *  (TIP)        │         │    │   │                  │  │  100kΩ     │
 *               │         │  10kΩ  │    ┌─────────────┤  │ (feedback) │
 *               │         │    │   │    │             │  └─────┬──────┘
 *               │         │    │   │    │  100kΩ      │        │
 *               │         │   GND  │    │ (feedback)  │        │
 *               │         │        │    │             │        │
 *               │         │        └────┼─────────────┘        │
 *               │         │             │                      │
 *               │  10µF   │        ┌────┴────────────┐         │
 *               │ ┌───┐   │        │                 │         │
 *               └─┤ + ├───┼────────┤5   OP-AMP B   7├─────────┐│
 *                 └───┘   │        │                 │         ││
 *                         │    ┌───┤6                │         ││
 *                         │    │   │                 │         ││
 *                         │    │   │            8├───┼─(+3.3V) ││
 *                         │    │   │            4├───┼─(GND)   ││
 *                         │    │   └─────────────────┘         ││
 *                         │    │         │                     ││
 *                         │    └─────────┘ (unity gain buffer) ││
 *                         │                                    ││
 *                         │                              10µF  ││
 *                         │                             ┌───┐  ││
 *                         │                             │ + ├──┘│
 *                         │                             └─┬─┘   │
 *                         │                               │     │
 *                         │                               │     │
 *  ───────────────────────┼───────────────────────────────┴─────┘
 *  GUITAR                 │                               │
 *  OUTPUT                 │                               │
 *  TO AMP ────────────────┼───────────────────────────────┘
 *  (TIP)                  │
 *                         │
 *  SLEEVE (GND) ──────────┴──────────────────────────────────────> GND
 * 
 * 
 * SIMPLIFIED BLOCK DIAGRAM:
 * =========================
 * 
 *                    ┌─────────────────────────────────────────────┐
 *                    │              ESP32 GUITAR TUNER             │
 *                    │                                             │
 *   GUITAR     ┌─────┴─────┐      ┌──────────┐      ┌─────────┐   │
 *   IN    ────>│  INPUT    │─────>│  OP-AMP  │─────>│  ESP32  │   │
 *   (1/4")     │  JACK     │  │   │    A     │      │  ADC    │   │
 *              └───────────┘  │   │ (Gain=11)│      │ GPIO34  │   │
 *                             │   └──────────┘      └─────────┘   │
 *                             │                                   │
 *                             │   ┌──────────┐                    │
 *                             └──>│  OP-AMP  │                    │
 *                                 │    B     │                    │
 *   AMP        ┌───────────┐     │ (Buffer) │                    │
 *   OUT   <────│  OUTPUT   │<────│ (Gain=1) │                    │
 *   (1/4")     │  JACK     │     └──────────┘                    │
 *              └─────┬─────┘                                      │
 *                    │                                            │
 *                    └────────────────────────────────────────────┘
 * 
 * 
 * TL072 DUAL OP-AMP PINOUT:
 * =========================
 * 
 *            ┌────────────┐
 *            │   TL072    │
 *            │  ┌──────┐  │
 *            │  │1    8│  │   Pin 1: Output A  ──> To ESP32 (via cap + resistor)
 *            │  │2    7│  │   Pin 2: Inv A     ──> Feedback network  
 *            │  │3    6│  │   Pin 3: Non-Inv A ──> Guitar input (via cap)
 *            │  │4    5│  │   Pin 4: V-        ──> GND
 *            │  └──────┘  │   Pin 5: Non-Inv B ──> Guitar input (via cap)
 *            └────────────┘   Pin 6: Inv B     ──> Connected to output (buffer)
 *                             Pin 7: Output B  ──> To amp output jack
 *                             Pin 8: V+        ──> +3.3V
 * 
 * 
 * COMPLETE WIRING TABLE:
 * ======================
 * 
 * ┌─────────────────────┬─────────────────┬─────────────────────────────┐
 * │ Component           │ Connect To      │ Notes                       │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ Guitar INPUT TIP    │ 1MΩ resistor    │ High input impedance        │
 * │ Guitar INPUT SLEEVE │ GND             │ Ground                      │
 * │ 1MΩ other end       │ Junction point  │ Splits to both op-amps      │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ Junction point      │ 10µF cap A (+)  │ To op-amp A (tuner)         │
 * │ Junction point      │ 10µF cap B (+)  │ To op-amp B (buffer)        │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ 10µF cap A (-)      │ TL072 Pin 3     │ Tuner amplifier input       │
 * │ TL072 Pin 3         │ VREF (1.65V)    │ Bias point                  │
 * │ TL072 Pin 2         │ 10kΩ to GND     │ Sets gain with 100kΩ        │
 * │ TL072 Pin 2         │ 100kΩ to Pin 1  │ Gain = 1 + 100k/10k = 11    │
 * │ TL072 Pin 1         │ 10µF cap (+)    │ Output coupling             │
 * │ 10µF cap (-)        │ 47kΩ to GPIO34  │ To ESP32 ADC                │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ 10µF cap B (-)      │ TL072 Pin 5     │ Buffer input                │
 * │ TL072 Pin 6         │ TL072 Pin 7     │ Unity gain (100% feedback)  │
 * │ TL072 Pin 7         │ 10µF cap (+)    │ Output coupling             │
 * │ 10µF cap (-)        │ OUTPUT jack TIP │ To amplifier                │
 * ├─────────────────────┼─────────────────┼─────────────────────────────┤
 * │ TL072 Pin 4         │ GND             │ Negative supply             │
 * │ TL072 Pin 8         │ 3.3V            │ Positive supply             │
 * │ 100nF cap           │ Pin 8 to GND    │ Power decoupling            │
 * │ Guitar OUTPUT SLEEVE│ GND             │ Ground                      │
 * └─────────────────────┴─────────────────┴─────────────────────────────┘
 * 
 * 
 * BREADBOARD LAYOUT:
 * ==================
 * 
 *     +3.3V Rail ══════════════════════════════════════════════════════
 *     
 *     Row 1:  [100kΩ]─●─[100kΩ]   Voltage divider: VREF = 1.65V
 *                     │(to 3.3V)  │(to GND)
 *                     │           │
 *     Row 2:     VREF─●───────────┘
 *                     │
 *     Row 3:          │    ┌─────[100nF]─────┐(to GND, decoupling)
 *                     │    │                 │
 *     Row 4:          │    │   TL072 IC      │
 *                     │    │   ┌─────────┐   │
 *     Row 5:          │    │   │ 1  ○  8 │───●───(+3.3V)
 *                     │    │   │ 2     7 │───●───[10µF+]───> OUTPUT JACK TIP
 *     Row 6:          │    │   │ 3     6 │───┘   (buffer out)
 *                     │    └───│ 4     5 │───●───[10µF+]───┐
 *     Row 7:          │   GND──┘         │                 │
 *                     │                   └─────────────────┤
 *     Row 8:          │                                     │
 *                     └──●─────────────────────────────────│
 *                        │(VREF to pin 3)                   │
 *     Row 9:             │                                  │
 *                   [10µF+]                                 │
 *     Row 10:            │                                  │
 *              [1MΩ]─────●──────────────────────────────────┘
 *                │       │                          (signal junction)
 *     Row 11:    │       │
 *           INPUT JACK   │
 *           TIP──────────┘
 *     
 *     Row 12: Pin 2 ──[10kΩ]──GND
 *                 │
 *     Row 13:     └──[100kΩ]──Pin 1
 *     
 *     Row 14: Pin 1 ──[10µF+]──[47kΩ]──> GPIO34
 *     
 *     GND Rail ════════════════════════════════════════════════════════
 * 
 * 
 * SIGNAL FLOW:
 * ============
 * 
 * 1. Guitar signal enters through INPUT jack
 * 2. 1MΩ resistor provides high input impedance (doesn't load pickups)
 * 3. Signal splits to two paths:
 * 
 *    PATH A (Tuner):
 *    - 10µF cap blocks DC, passes audio
 *    - Op-amp A amplifies signal 11x
 *    - 1.65V bias centers signal for ADC
 *    - 47kΩ protects ESP32 input
 *    - ESP32 reads pitch via GPIO34
 * 
 *    PATH B (Output Buffer):
 *    - 10µF cap blocks DC, passes audio
 *    - Op-amp B configured as unity gain buffer
 *    - Provides low-impedance output
 *    - 10µF output cap blocks DC to amp
 *    - Clean signal to OUTPUT jack -> Amplifier
 * 
 * 
 * WHY USE A BUFFER FOR OUTPUT?
 * ============================
 * 
 * - Guitar pickups are high impedance (~10kΩ passive, higher for piezo)
 * - Long cables can cause signal loss and high frequency roll-off
 * - Buffer provides LOW output impedance (~100Ω)
 * - Drives long cables without tone loss
 * - Isolates guitar from amp loading effects
 * - No signal degradation from the tuner circuit
 * 
 * 
 * PASSIVE ALTERNATIVE (No Output Buffer):
 * =======================================
 * 
 * If you want simpler circuit without buffered output,
 * you can wire the output jack directly in parallel:
 * 
 *   GUITAR INPUT TIP ────┬────> To preamp circuit (as before)
 *                        │
 *                        └────> OUTPUT JACK TIP (direct pass-through)
 *   
 *   GUITAR INPUT SLEEVE ─┬────> GND
 *                        │
 *   OUTPUT JACK SLEEVE ──┘
 * 
 * Note: This passive approach may slightly load the guitar signal
 * when both tuner and amp are connected. The buffered version is better.
 * 
 * 
 * PARTS LIST:
 * ===========
 * 
 * │ Qty │ Component              │ Value    │ Notes                     │
 * │─────│────────────────────────│──────────│───────────────────────────│
 * │  1  │ TL072 Dual Op-Amp      │ -        │ Both op-amps used         │
 * │  1  │ 1/4" Mono Audio Jack   │ -        │ Guitar INPUT              │
 * │  1  │ 1/4" Mono Audio Jack   │ -        │ Amp OUTPUT                │
 * │  1  │ Resistor               │ 1MΩ      │ Input impedance           │
 * │  3  │ Resistor               │ 100kΩ    │ Bias + feedback           │
 * │  1  │ Resistor               │ 10kΩ     │ Gain feedback             │
 * │  1  │ Resistor               │ 47kΩ     │ ADC protection            │
 * │  3  │ Electrolytic Capacitor │ 10µF     │ Coupling (watch polarity!)│
 * │  1  │ Ceramic Capacitor      │ 100nF    │ Power decoupling          │
 * │  1  │ 8-pin DIP socket       │ -        │ Optional, for op-amp      │
 * 
 */
