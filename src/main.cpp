#include <Arduino.h>
#include "driver/timer.h"  // Include the timer driver library

#define OUTPUT_PIN 25            // DAC output pin
#define NUM_BUTTONS 4            // Number of buttons
#define MAX_FREQS 3              // Max frequencies per button
#define SAMPLE_RATE 40000        // Sampling rate (40 kHz)
#define TABLE_SIZE 256           // Table size for square waves

// Button states (1 = pressed, 0 = not pressed)
volatile uint8_t buttonStates[NUM_BUTTONS] = {0};

// Frequency assignments for buttons
float buttonFreqs[NUM_BUTTONS][MAX_FREQS] = {
    {440, 554.37, 659.25},   // Button 1: A4, C#5, E5
    {330, 392, 494},         // Button 2: E4, G4, B4
    {220, 277.18, 349.23},   // Button 3: A3, C#4, F4
    {110, 146.83, 196}       // Button 4: A2, D3, G3
};

// Precomputed square wave lookup tables
uint8_t squareWave[TABLE_SIZE];      // Generic square wave
int tableIndices[NUM_BUTTONS][MAX_FREQS]; // Current indices for each button's frequencies
int stepSizes[NUM_BUTTONS][MAX_FREQS];    // Step sizes for each button's frequencies

// Timer interrupt for real-time mixing and DAC output
void IRAM_ATTR onTimer(void* arg) {
    uint16_t combinedSignal = 0; // To store the mixed signal

    // Mix square waves based on active button states
    for (int btn = 0; btn < NUM_BUTTONS; btn++) {
        if (buttonStates[btn] == 1) { // If the button is pressed
            for (int freq = 0; freq < MAX_FREQS; freq++) {
                // Add the current value of the square wave
                combinedSignal += squareWave[tableIndices[btn][freq]];
                // Increment and wrap the index
                tableIndices[btn][freq] = (tableIndices[btn][freq] + stepSizes[btn][freq]) % TABLE_SIZE;
            }
        }
    }

    // Normalize combined signal to fit in DAC range (0–255)
    combinedSignal = combinedSignal / (NUM_BUTTONS * MAX_FREQS);

    // Output to DAC
    analogWrite(OUTPUT_PIN, combinedSignal);
}

void setup() {
    // Configure DAC pin
    pinMode(OUTPUT_PIN, OUTPUT);
    analogWriteResolution(8);  // 8-bit resolution (0-255)
    analogWriteFrequency(SAMPLE_RATE);  // Set the frequency of the DAC output globally

    // Generate square wave lookup table
    for (int i = 0; i < TABLE_SIZE; i++) {
        squareWave[i] = (i < TABLE_SIZE / 2) ? 255 : 0; // Square wave: 50% duty cycle
    }

    // Calculate step sizes for each frequency
    for (int btn = 0; btn < NUM_BUTTONS; btn++) {
        for (int freq = 0; freq < MAX_FREQS; freq++) {
            stepSizes[btn][freq] = (buttonFreqs[btn][freq] * TABLE_SIZE) / SAMPLE_RATE;
            tableIndices[btn][freq] = 0; // Initialize indices
        }
    }

    // Set up hardware timer
    timer_config_t config;
    config.alarm_en = TIMER_ALARM_EN;
    config.counter_en = TIMER_PAUSE;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_dir = TIMER_COUNT_UP;
    config.auto_reload = TIMER_AUTORELOAD_EN; // Correct enum for auto-reload
    config.divider = 80; // 1 MHz clock (prescaler)

    // Initialize timer
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000 / SAMPLE_RATE); // Set interrupt frequency
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, onTimer, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

void loop() {
    // Example: Simulate button presses
    buttonStates[0] = 1; // Button 1 pressed
    buttonStates[1] = 0; // Button 2 not pressed
    buttonStates[2] = 1; // Button 3 pressed
    buttonStates[3] = 0; // Button 4 not pressed

    delay(100); // Adjust as needed for testing
}
