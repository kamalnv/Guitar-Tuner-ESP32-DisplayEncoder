/*
 * Guitar Tuning Definitions
 * Contains MIDI note numbers and frequency calculations
 */

#ifndef TUNINGS_H
#define TUNINGS_H

// Reference frequency for A4
#define A4_FREQUENCY 440.0

// MIDI note numbers for reference
// C0 = 12, C4 (middle C) = 60, A4 = 69

// Standard guitar string MIDI notes (low to high):
// E2=40, A2=45, D3=50, G3=55, B3=59, E4=64

// Common alternate tunings frequency table
// Note: These are theoretical frequencies
// String order: 6th(low) to 1st(high)

/*
 * Tuning Reference Table:
 * 
 * STANDARD (E A D G B E):
 *   E2: 82.41 Hz  (MIDI 40)
 *   A2: 110.0 Hz  (MIDI 45)
 *   D3: 146.8 Hz  (MIDI 50)
 *   G3: 196.0 Hz  (MIDI 55)
 *   B3: 246.9 Hz  (MIDI 59)
 *   E4: 329.6 Hz  (MIDI 64)
 * 
 * DROP D (D A D G B E):
 *   D2: 73.42 Hz  (MIDI 38)
 *   A2: 110.0 Hz  (MIDI 45)
 *   D3: 146.8 Hz  (MIDI 50)
 *   G3: 196.0 Hz  (MIDI 55)
 *   B3: 246.9 Hz  (MIDI 59)
 *   E4: 329.6 Hz  (MIDI 64)
 * 
 * HALF STEP DOWN (Eb Ab Db Gb Bb Eb):
 *   Eb2: 77.78 Hz  (MIDI 39)
 *   Ab2: 103.8 Hz  (MIDI 44)
 *   Db3: 138.6 Hz  (MIDI 49)
 *   Gb3: 185.0 Hz  (MIDI 54)
 *   Bb3: 233.1 Hz  (MIDI 58)
 *   Eb4: 311.1 Hz  (MIDI 63)
 * 
 * DROP C (C G C F A D):
 *   C2: 65.41 Hz  (MIDI 36)
 *   G2: 98.00 Hz  (MIDI 43)
 *   C3: 130.8 Hz  (MIDI 48)
 *   F3: 174.6 Hz  (MIDI 53)
 *   A3: 220.0 Hz  (MIDI 57)
 *   D4: 293.7 Hz  (MIDI 62)
 * 
 * OPEN G (D G D G B D):
 *   D2: 73.42 Hz  (MIDI 38)
 *   G2: 98.00 Hz  (MIDI 43)
 *   D3: 146.8 Hz  (MIDI 50)
 *   G3: 196.0 Hz  (MIDI 55)
 *   B3: 246.9 Hz  (MIDI 59)
 *   D4: 293.7 Hz  (MIDI 62)
 * 
 * DADGAD (D A D G A D):
 *   D2: 73.42 Hz  (MIDI 38)
 *   A2: 110.0 Hz  (MIDI 45)
 *   D3: 146.8 Hz  (MIDI 50)
 *   G3: 196.0 Hz  (MIDI 55)
 *   A3: 220.0 Hz  (MIDI 57)
 *   D4: 293.7 Hz  (MIDI 62)
 */

// Chromatic scale note names
const char CHROMATIC_NOTES[12][3] = {
  "C", "C#", "D", "D#", "E", "F", 
  "F#", "G", "G#", "A", "A#", "B"
};

// Calculate frequency from MIDI note number
inline float midiNoteToFrequency(int midiNote) {
  return A4_FREQUENCY * pow(2.0, (midiNote - 69) / 12.0);
}

// Calculate MIDI note from frequency
inline int frequencyToMidiNote(float frequency) {
  return round(69 + 12 * log2(frequency / A4_FREQUENCY));
}

// Calculate cents deviation from target frequency
inline float calculateCents(float measuredFreq, float targetFreq) {
  return 1200.0 * log2(measuredFreq / targetFreq);
}

#endif // TUNINGS_H
