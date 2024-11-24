#include "Arduino.h"
#include "Panel.h"

unsigned char _frameCount = 0;

Panel::Panel() {
  pinMode(_CLA, OUTPUT);
  pinMode(_CLK, OUTPUT);
  pinMode(_IN, OUTPUT);
  pinMode(_EN, OUTPUT);
}

void Panel::display() {
  // Pull write pin high
  digitalWrite(_EN, HIGH);
  // Index of each 16 bit value
  uint8_t w2 = 0;

  for (int i = 0; i < _bufferSize; i ++) {
    // Write data to data pin
    if (_panelBuffer[i]) {
      digitalWrite(_IN, HIGH);
    } else {
      digitalWrite(_IN, LOW);
    }
    // Create a clock pulse
    digitalWrite(_CLK, HIGH);
    digitalWrite(_CLK, LOW);
    // Increment counter tracking sent bits
    w2++;

    if (w2 > 15) {
      // Every 16 bits pulse latch pin and reset counter
      w2 = 0;
      digitalWrite(_CLA, HIGH);
      digitalWrite(_CLA, LOW);
    }
  }
  // Pull write pin low
  digitalWrite(_EN, LOW);
  _frameCount = (_frameCount + 1) % 256;
}

void Panel::fillBuffer(unsigned char val) {
  for (int i = 0; i < _bufferSize; i++) {
    _panelBuffer[i] = val;
  }
}

void Panel::setPixel(int x, int y, unsigned char val) {
  _panelBuffer[_addrTable[15 - x][y]] = val;
}

void Panel::setPixelAtIndex(int i, unsigned char val) {
  _panelBuffer[i] = val;
}

unsigned char Panel::getPixel(int x, int y) {
  return _panelBuffer[_addrTable[15 - x][y]];
}