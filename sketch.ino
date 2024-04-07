/*

Created: 2024-03-30
Author: Johnathan Jackson

Sources: Arturo Guadalupi Arduino 7 Button MIDI Tutorial
http://www.arduino.cc/en/Tutorial/MidiDevice

*/


// 1. Libraries
#include "MIDIUSB.h"
//#include "PitchToNote.h"
#define NUM_BUTTONS  12

// 2. Initial Variables

// Map buttons to pin number:
// Each button variable is a byte (unsigned integer of length 8)

const uint8_t button1 = 2; // Button 1 | Note C | Pin 2
const uint8_t button2 = 9; // Button 2 | Note C# | Pin 9
const uint8_t button3 = 3; // Button 3 | Note D | Pin 3
const uint8_t button4 = 10; // Button 4 | Note D# | Pin 10
const uint8_t button5 = 4; // Button 5 | Note E | Pin 4
const uint8_t button6 = 5; // Button 6 | Note F | Pin 5
const uint8_t button7 = 11; // Button 7 | Note F# | Pin 11
const uint8_t button8 = 6; // Button 8 | Note G | Pin 6
const uint8_t button9 = 14; // Button 9 | Note G# | Pin 12
const uint8_t button10 = 7; // Button 10 | Note A | Pin 7
const uint8_t button11 = 15; // Button 11 | Note A# | Pin 13
const uint8_t button12 = 8; // Button 12 | Note B | Pin 8
  
// Potentiometers
const int octavePot = A8; // A0

// MIDI Arrays
const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};
//const byte notePitchesName[NUM_BUTTONS] = {C1, D1b, D1, E1b, E1, F1, G1b, G1, A1b, A1, B1b, B1};
const byte notePitchesNum[NUM_BUTTONS] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

// Button States
uint8_t notesTime[NUM_BUTTONS];
uint16_t pressedButtons = 0x00;
uint16_t previousButtons = 0x00;
uint8_t intensity = 60;
uint8_t octave;

// 3. Setup Function - Activate Pullup resistors for button pins
void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);
  	Serial.begin(31250);
}

// 4. Main Program
void loop() {
  readButtons();
  readOctave();
  playNotes();
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).


void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


void readButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void readOctave()
{
  int val = analogRead(octavePot);
  octave = (uint8_t) (map(val, 0, 1023, 0, 6));
  
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitchesNum[i] + (octave * 12), intensity);
        //Serial.println("Pressed Buttons");
        //Serial.println(pressedButtons);
        //Serial.println("Octave");
        //Serial.println(octave);
        //Serial.println("Note On");
        //Serial.println(notePitchesNum[i] + (octave * 12));
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitchesNum[i] + (octave * 12), 0);
        //Serial.println("Note Off");
        //Serial.println(notePitchesNum[i] + (octave * 12));
        MidiUSB.flush();
      }
    }
  }
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}


  
