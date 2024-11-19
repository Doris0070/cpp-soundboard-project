#include <Arduino.h>

const int outputPin = 8;  // Output pin for speaker/buzzer

// Define note frequencies in Hz for all required notes
#define NOTE_E5 659
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_G4 392
#define NOTE_E4 330
#define NOTE_A4 440
#define NOTE_F4 349
#define NOTE_F5 698
#define NOTE_D4 294
#define NOTE_A5 880  // Added definition for A5
#define NOTE_G5 784  // Added definition for G5

// Define the note duration (milliseconds) for tempo
const int tempo = 144;   // Adjust to speed up or slow down the song
int noteDuration;

// Melody array (frequency and duration)
int melody[] = {
  NOTE_E5, 8, NOTE_B4, 8, NOTE_C5, 8, NOTE_D5, 8, NOTE_C5, 8, NOTE_B4, 8,
  NOTE_A4, 8, NOTE_A4, 8, NOTE_C5, 8, NOTE_E5, 8, NOTE_D5, 8, NOTE_C5, 8,
  NOTE_B4, 8, NOTE_C5, 8, NOTE_D5, 8, NOTE_E5, 8, NOTE_C5, 8, NOTE_A4, 8,
  NOTE_A4, 8, NOTE_D5, 8, NOTE_F5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 8, NOTE_D5, 8, NOTE_C5, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_C5, 8,
  NOTE_D5, 8, NOTE_E5, 8, NOTE_C5, 8, NOTE_A4, 8, NOTE_A4, 8
};

void playTone(int frequency, int duration) {
  int period = 1000000 / frequency;        // Period of the tone in microseconds
  int pulse = period / 2;                  // Pulse width for the tone
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    digitalWrite(outputPin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(outputPin, LOW);
    delayMicroseconds(pulse);
  }
}

void setup() {
  pinMode(outputPin, OUTPUT);
}

void loop() {
  int melodyLength = sizeof(melody) / sizeof(melody[0]); // Get the melody length

  for (int i = 0; i < melodyLength; i += 2) {
    int note = melody[i];
    int duration = melody[i + 1];
    noteDuration = (1000 / tempo) * duration;

    playTone(note, noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }

  delay(2000);  // Delay between repetitions of the melody
}
