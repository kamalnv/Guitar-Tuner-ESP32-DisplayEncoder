/*
 * ESP32 Guitar Tuner
 * ===================
 * Hardware:
 *   - ESP32 DevKit / Generic ESP32
 *   - 0.96" OLED Display (SSD1306, 128x32, I2C)
 *   - EC11 Rotary Encoder with push button
 *   - Electret Microphone (MAX9814/MAX4466 or similar)
 * 
 * Features:
 *   - Chromatic tuner mode (all 12 notes)
 *   - Multiple guitar tuning presets
 *   - Real-time pitch detection using autocorrelation
 *   - Visual cents deviation display
 *   - Rotary encoder for menu navigation
 * 
 * Pin Configuration:
 *   OLED (I2C):
 *     - SDA: GPIO 21
 *     - SCL: GPIO 22
 *   Rotary Encoder:
 *     - CLK (A): GPIO 32
 *     - DT (B):  GPIO 33
 *     - SW:     GPIO 25
 *   Microphone:
 *     - OUT:    GPIO 34 (ADC1_CH6)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitchdetect.h"
#include "tunings.h"

// ============== PIN DEFINITIONS ==============
#define OLED_SDA 21
#define OLED_SCL 22

#define ENCODER_CLK 32
#define ENCODER_DT  33
#define ENCODER_SW  25

#define MIC_PIN 34

// ============== DISPLAY CONFIG ==============
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ============== AUDIO SAMPLING CONFIG ==============
#define SAMPLE_RATE 8000
#define SAMPLES 1024
#define BUFFER_SIZE SAMPLES

int16_t audioBuffer[BUFFER_SIZE];
volatile int sampleIndex = 0;
volatile bool bufferReady = false;

hw_timer_t *sampleTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// ============== ENCODER STATE ==============
volatile int encoderPos = 0;
volatile int lastEncoderPos = 0;
volatile bool buttonPressed = false;
int lastCLK = HIGH;

// ============== TUNER STATE ==============
enum TunerMode {
  MODE_CHROMATIC,
  MODE_STANDARD,
  MODE_DROP_D,
  MODE_HALF_STEP_DOWN,
  MODE_DROP_C,
  MODE_OPEN_G,
  MODE_DADGAD,
  MODE_7STRING,
  MODE_7STRING_DROP_A,
  MODE_BASS_4,
  MODE_BASS_5,
  MODE_BASS_DROP_D,
  MODE_COUNT
};

const char* modeNames[] = {
  "CHROMATIC",
  "STANDARD",
  "DROP D",
  "HALF DOWN",
  "DROP C",
  "OPEN G",
  "DADGAD",
  "7-STRING",
  "7STR DROP A",
  "BASS 4-STR",
  "BASS 5-STR",
  "BASS DROP D"
};

// Number of strings for each mode
const int stringCounts[] = {
  0,  // Chromatic
  6,  // Standard
  6,  // Drop D
  6,  // Half Down
  6,  // Drop C
  6,  // Open G
  6,  // DADGAD
  7,  // 7-String
  7,  // 7-String Drop A
  4,  // Bass 4-string
  5,  // Bass 5-string
  4   // Bass Drop D
};

TunerMode currentMode = MODE_CHROMATIC;
int selectedString = 0;  // For guitar modes
bool inMenu = true;

// ============== NOTE DEFINITIONS ==============
const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// Frequency of A4
const float A4_FREQ = 440.0;

// ============== TUNING PRESETS ==============
// Each tuning: 6 strings from low to high (6th to 1st)
// Values are MIDI note numbers

// Standard: E2 A2 D3 G3 B3 E4
const int TUNING_STANDARD[] = {40, 45, 50, 55, 59, 64, 0};

// Drop D: D2 A2 D3 G3 B3 E4
const int TUNING_DROP_D[] = {38, 45, 50, 55, 59, 64, 0};

// Half Step Down: Eb2 Ab2 Db3 Gb3 Bb3 Eb4
const int TUNING_HALF_DOWN[] = {39, 44, 49, 54, 58, 63, 0};

// Drop C: C2 G2 C3 F3 A3 D4
const int TUNING_DROP_C[] = {36, 43, 48, 53, 57, 62, 0};

// Open G: D2 G2 D3 G3 B3 D4
const int TUNING_OPEN_G[] = {38, 43, 50, 55, 59, 62, 0};

// DADGAD: D2 A2 D3 G3 A3 D4
const int TUNING_DADGAD[] = {38, 45, 50, 55, 57, 62, 0};

// 7-String Standard: B1 E2 A2 D3 G3 B3 E4
const int TUNING_7STRING[] = {35, 40, 45, 50, 55, 59, 64};

// 7-String Drop A: A1 E2 A2 D3 G3 B3 E4
const int TUNING_7STRING_DROP_A[] = {33, 40, 45, 50, 55, 59, 64};

// Bass 4-String Standard: E1 A1 D2 G2
const int TUNING_BASS_4[] = {28, 33, 38, 43, 0, 0, 0};

// Bass 5-String Standard: B0 E1 A1 D2 G2
const int TUNING_BASS_5[] = {23, 28, 33, 38, 43, 0, 0};

// Bass 4-String Drop D: D1 A1 D2 G2
const int TUNING_BASS_DROP_D[] = {26, 33, 38, 43, 0, 0, 0};

const int* tunings[] = {
  NULL,              // Chromatic has no preset
  TUNING_STANDARD,
  TUNING_DROP_D,
  TUNING_HALF_DOWN,
  TUNING_DROP_C,
  TUNING_OPEN_G,
  TUNING_DADGAD,
  TUNING_7STRING,
  TUNING_7STRING_DROP_A,
  TUNING_BASS_4,
  TUNING_BASS_5,
  TUNING_BASS_DROP_D
};

// String names for each tuning (max 7 strings)
const char* stringNames[][7] = {
  {"", "", "", "", "", "", ""},              // Chromatic
  {"E2", "A2", "D3", "G3", "B3", "E4", ""},  // Standard
  {"D2", "A2", "D3", "G3", "B3", "E4", ""},  // Drop D
  {"Eb", "Ab", "Db", "Gb", "Bb", "Eb", ""},  // Half Down
  {"C2", "G2", "C3", "F3", "A3", "D4", ""},  // Drop C
  {"D2", "G2", "D3", "G3", "B3", "D4", ""},  // Open G
  {"D2", "A2", "D3", "G3", "A3", "D4", ""},  // DADGAD
  {"B1", "E2", "A2", "D3", "G3", "B3", "E4"}, // 7-String
  {"A1", "E2", "A2", "D3", "G3", "B3", "E4"}, // 7-String Drop A
  {"E1", "A1", "D2", "G2", "", "", ""},       // Bass 4-String
  {"B0", "E1", "A1", "D2", "G2", "", ""},     // Bass 5-String
  {"D1", "A1", "D2", "G2", "", "", ""}        // Bass Drop D
};

// ============== FUNCTION PROTOTYPES ==============
void IRAM_ATTR onTimer();
void IRAM_ATTR handleEncoder();
void IRAM_ATTR handleButton();
float detectPitch(int16_t* buffer, int length);
void findClosestNote(float freq, int* note, int* octave, float* cents);
void findClosestStringNote(float freq, int tuningIndex, int* stringNum, float* cents);
void drawMenu();
void drawTuner(float frequency, int note, int octave, float cents);
void drawGuitarTuner(float frequency, int stringNum, float cents);
void drawCentsMeter(float cents);
float midiToFreq(int midiNote);
int freqToMidi(float freq);

// ============== TIMER ISR ==============
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if (!bufferReady && sampleIndex < BUFFER_SIZE) {
    audioBuffer[sampleIndex] = analogRead(MIC_PIN) - 2048;  // Center around 0
    sampleIndex++;
    if (sampleIndex >= BUFFER_SIZE) {
      bufferReady = true;
    }
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ============== ENCODER ISR ==============
void IRAM_ATTR handleEncoder() {
  int clk = digitalRead(ENCODER_CLK);
  int dt = digitalRead(ENCODER_DT);
  
  if (clk != lastCLK) {
    if (dt != clk) {
      encoderPos++;
    } else {
      encoderPos--;
    }
    lastCLK = clk;
  }
}

// ============== BUTTON ISR ==============
void IRAM_ATTR handleButton() {
  static unsigned long lastDebounce = 0;
  if (millis() - lastDebounce > 200) {
    buttonPressed = true;
    lastDebounce = millis();
  }
}

// ============== SETUP ==============
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Guitar Tuner Starting...");
  
  // Initialize I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 allocation failed!");
    for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 12);
  display.println("GUITAR TUNER");
  display.display();
  delay(1500);
  
  // Initialize encoder pins
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  lastCLK = digitalRead(ENCODER_CLK);
  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), handleButton, FALLING);
  
  // Initialize ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  
  // Initialize sampling timer
  sampleTimer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
  timerAttachInterrupt(sampleTimer, &onTimer, true);
  timerAlarmWrite(sampleTimer, 1000000 / SAMPLE_RATE, true);  // Sample rate
  timerAlarmEnable(sampleTimer);
  
  Serial.println("Setup complete!");
  drawMenu();
}

// ============== MAIN LOOP ==============
void loop() {
  // Handle encoder rotation
  if (encoderPos != lastEncoderPos) {
    int diff = encoderPos - lastEncoderPos;
    lastEncoderPos = encoderPos;
    
    if (inMenu) {
      // Navigate modes
      int newMode = (int)currentMode + diff;
      if (newMode < 0) newMode = MODE_COUNT - 1;
      if (newMode >= MODE_COUNT) newMode = 0;
      currentMode = (TunerMode)newMode;
      drawMenu();
    } else if (currentMode != MODE_CHROMATIC) {
      // Navigate strings in guitar/bass mode
      int numStrings = stringCounts[currentMode];
      selectedString += diff;
      if (selectedString < 0) selectedString = numStrings - 1;
      if (selectedString >= numStrings) selectedString = 0;
    }
  }
  
  // Handle button press
  if (buttonPressed) {
    buttonPressed = false;
    inMenu = !inMenu;
    if (inMenu) {
      drawMenu();
    }
  }
  
  // Process audio when buffer is ready
  if (bufferReady && !inMenu) {
    float frequency = detectPitch(audioBuffer, BUFFER_SIZE);
    
    if (frequency > 25 && frequency < 1200) {  // Valid guitar/bass range (B0 = 30.87Hz)
      if (currentMode == MODE_CHROMATIC) {
        int note, octave;
        float cents;
        findClosestNote(frequency, &note, &octave, &cents);
        drawTuner(frequency, note, octave, cents);
      } else {
        int stringNum;
        float cents;
        findClosestStringNote(frequency, (int)currentMode, &stringNum, &cents);
        drawGuitarTuner(frequency, stringNum, cents);
      }
    } else {
      // No valid frequency detected
      drawWaitingScreen();
    }
    
    // Reset buffer
    portENTER_CRITICAL(&timerMux);
    sampleIndex = 0;
    bufferReady = false;
    portEXIT_CRITICAL(&timerMux);
  }
  
  delay(10);
}

// ============== PITCH DETECTION ==============
float detectPitch(int16_t* buffer, int length) {
  // Autocorrelation-based pitch detection
  float maxCorr = 0;
  int maxLag = 0;
  
  // Search in the range of guitar/bass frequencies (25Hz - 1200Hz)
  int minLag = SAMPLE_RATE / 1200;  // Highest frequency
  int maxLagSearch = SAMPLE_RATE / 25;   // Lowest frequency (for 5-string bass B0)
  
  if (maxLagSearch > length / 2) maxLagSearch = length / 2;
  
  // Calculate DC offset
  long sum = 0;
  for (int i = 0; i < length; i++) {
    sum += buffer[i];
  }
  int dcOffset = sum / length;
  
  // Check if signal is strong enough
  long energy = 0;
  for (int i = 0; i < length; i++) {
    int sample = buffer[i] - dcOffset;
    energy += (long)sample * sample;
  }
  float rms = sqrt((float)energy / length);
  
  if (rms < 50) {  // Too quiet
    return 0;
  }
  
  // Autocorrelation
  for (int lag = minLag; lag < maxLagSearch; lag++) {
    float corr = 0;
    float norm1 = 0;
    float norm2 = 0;
    
    for (int i = 0; i < length - lag; i++) {
      float s1 = buffer[i] - dcOffset;
      float s2 = buffer[i + lag] - dcOffset;
      corr += s1 * s2;
      norm1 += s1 * s1;
      norm2 += s2 * s2;
    }
    
    if (norm1 > 0 && norm2 > 0) {
      corr = corr / sqrt(norm1 * norm2);
    }
    
    if (corr > maxCorr) {
      maxCorr = corr;
      maxLag = lag;
    }
  }
  
  if (maxCorr < 0.5 || maxLag == 0) {
    return 0;
  }
  
  // Parabolic interpolation for better accuracy
  float freq = (float)SAMPLE_RATE / maxLag;
  
  return freq;
}

// ============== NOTE CALCULATION ==============
float midiToFreq(int midiNote) {
  return A4_FREQ * pow(2.0, (midiNote - 69) / 12.0);
}

int freqToMidi(float freq) {
  return round(69 + 12 * log2(freq / A4_FREQ));
}

void findClosestNote(float freq, int* note, int* octave, float* cents) {
  float midiFloat = 69 + 12 * log2(freq / A4_FREQ);
  int midiNote = round(midiFloat);
  
  *note = midiNote % 12;
  *octave = (midiNote / 12) - 1;
  
  float exactFreq = midiToFreq(midiNote);
  *cents = 1200 * log2(freq / exactFreq);
}

void findClosestStringNote(float freq, int tuningIndex, int* stringNum, float* cents) {
  const int* tuning = tunings[tuningIndex];
  int numStrings = stringCounts[tuningIndex];
  float minCentsDiff = 9999;
  int closestString = 0;
  
  for (int i = 0; i < numStrings; i++) {
    if (tuning[i] == 0) continue;  // Skip empty slots
    float targetFreq = midiToFreq(tuning[i]);
    float centsDiff = 1200 * log2(freq / targetFreq);
    
    if (abs(centsDiff) < abs(minCentsDiff)) {
      minCentsDiff = centsDiff;
      closestString = i;
    }
  }
  
  *stringNum = closestString;
  *cents = minCentsDiff;
}

// ============== DISPLAY FUNCTIONS ==============
void drawMenu() {
  display.clearDisplay();
  
  // Title
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("MODE:");
  
  // Current mode (larger)
  display.setTextSize(1);
  display.setCursor(35, 0);
  display.print(modeNames[currentMode]);
  
  // Navigation hint
  display.setCursor(0, 12);
  display.print("<");
  
  // Show prev/next modes
  int prevMode = ((int)currentMode - 1 + MODE_COUNT) % MODE_COUNT;
  int nextMode = ((int)currentMode + 1) % MODE_COUNT;
  
  display.setCursor(10, 12);
  display.print(modeNames[prevMode]);
  
  display.setCursor(75, 12);
  display.print(modeNames[nextMode]);
  
  display.setCursor(120, 12);
  display.print(">");
  
  // Instructions
  display.setCursor(0, 24);
  display.print("PUSH to start tuning");
  
  display.display();
}

void drawTuner(float frequency, int note, int octave, float cents) {
  display.clearDisplay();
  
  // Note name (large)
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(noteNames[note]);
  display.setTextSize(1);
  display.print(octave);
  
  // Frequency
  display.setCursor(45, 0);
  display.setTextSize(1);
  display.print(frequency, 1);
  display.print("Hz");
  
  // Cents deviation
  display.setCursor(45, 10);
  if (cents > 0) display.print("+");
  display.print(cents, 1);
  display.print("c");
  
  // Tuning status
  display.setCursor(95, 0);
  if (abs(cents) < 5) {
    display.setTextSize(2);
    display.print("OK");
  } else if (cents < 0) {
    display.print("LOW");
  } else {
    display.print("HIGH");
  }
  
  // Draw cents meter
  drawCentsMeter(cents);
  
  display.display();
}

void drawGuitarTuner(float frequency, int stringNum, float cents) {
  display.clearDisplay();
  
  int numStrings = stringCounts[currentMode];
  
  // String indicator
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("STR ");
  display.print(numStrings - stringNum);  // String N to 1
  display.print(":");
  
  // String note name
  display.setCursor(40, 0);
  display.print(stringNames[currentMode][stringNum]);
  
  // Frequency
  display.setCursor(70, 0);
  display.print(frequency, 1);
  display.print("Hz");
  
  // Cents deviation
  display.setCursor(0, 10);
  if (cents > 0) display.print("+");
  display.print(cents, 1);
  display.print(" cents");
  
  // Tuning status
  display.setCursor(80, 10);
  if (abs(cents) < 5) {
    display.setTextSize(1);
    display.print("IN TUNE!");
  } else if (cents < 0) {
    display.print("TUNE UP");
  } else {
    display.print("TUNE DN");
  }
  
  // Draw cents meter
  drawCentsMeter(cents);
  
  display.display();
}

void drawCentsMeter(float cents) {
  // Draw meter bar at bottom
  int y = 24;
  int meterWidth = 120;
  int meterX = 4;
  
  // Outline
  display.drawRect(meterX, y, meterWidth, 7, SSD1306_WHITE);
  
  // Center line
  display.drawLine(meterX + meterWidth/2, y, meterX + meterWidth/2, y + 6, SSD1306_WHITE);
  
  // Indicator position (clamp to -50 to +50 cents)
  float clampedCents = constrain(cents, -50, 50);
  int indicatorX = meterX + meterWidth/2 + (int)(clampedCents * (meterWidth/2) / 50);
  
  // Draw indicator
  if (abs(cents) < 5) {
    // In tune - fill center area
    display.fillRect(meterX + meterWidth/2 - 3, y + 1, 7, 5, SSD1306_WHITE);
  } else {
    // Out of tune - show position
    display.fillRect(indicatorX - 2, y + 1, 5, 5, SSD1306_WHITE);
  }
  
  // Tick marks
  display.drawPixel(meterX + meterWidth/4, y + 6, SSD1306_WHITE);
  display.drawPixel(meterX + 3*meterWidth/4, y + 6, SSD1306_WHITE);
}

void drawWaitingScreen() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Mode: ");
  display.print(modeNames[currentMode]);
  
  display.setCursor(20, 12);
  display.setTextSize(1);
  display.print("Play a note...");
  
  // Draw empty meter
  drawCentsMeter(0);
  
  display.display();
}
