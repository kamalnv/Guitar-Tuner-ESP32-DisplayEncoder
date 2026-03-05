/*
 * Pitch Detection Header
 * Enhanced autocorrelation algorithm for guitar tuning
 */

#ifndef PITCHDETECT_H
#define PITCHDETECT_H

#include <Arduino.h>

// Configuration
#define PITCH_DETECT_THRESHOLD 0.5    // Minimum correlation threshold
#define PITCH_DETECT_MIN_FREQ 60.0    // Minimum detectable frequency (Hz)
#define PITCH_DETECT_MAX_FREQ 1200.0  // Maximum detectable frequency (Hz)
#define PITCH_DETECT_MIN_RMS 50       // Minimum signal level

// Pitch detection result structure
struct PitchResult {
  float frequency;
  float confidence;
  bool valid;
};

#endif // PITCHDETECT_H
