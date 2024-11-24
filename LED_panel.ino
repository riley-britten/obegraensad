#include "./Panel.h"
#include "./Ripple.h"
#include "./TestImage.h"

#define USE_TIMER_1 true
#define TIMER_INTERVAL_MS 1
#include "TimerInterrupt.h"

#define LIFE_CUTOFF 1
#define RAIN_CUTOFF 6
#define MAX_RAIN_LENGTH 3
#define RAIN_MAX_RAND 250
#define RAIN_MAX_DELAY 10
#define SIM_FRAMES 256
#define NUM_PATTERNS 7
#define DEBOUNCE_CUTOFF 1000

// Delay between frames for each pattern
unsigned int animationDelays[NUM_PATTERNS] = {100, 50, 50, 400, 5000, 400, 400};

int BTN = 2;
unsigned long _previousInterrupt = 0;
unsigned long _previousMillis = 0;
int _curPattern = 4;
int _curFrame = 0;
Panel _panel;

// Track position, delay, and time since moved for each drop
unsigned char rainPos[16];
unsigned char rainDel[16];
unsigned char rainLast[16];

void handleButton() {
  const unsigned long now = millis();
  if (now - _previousInterrupt > DEBOUNCE_CUTOFF) {
    _previousInterrupt = now;
    _curPattern = (_curPattern + 1) % NUM_PATTERNS;
    _curFrame = 0;
  }
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), handleButton, FALLING);
  for (int i = 0; i < 16; i++) {
    rainPos[i] = 16;
  }
  ITimer1.init();
  ITimer1.attachInterruptInterval(15, drawDisplay);
}

void drawDisplay() {
  _panel.display();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - _previousMillis > animationDelays[_curPattern]) {
    switch (_curPattern) {
      case 0:
        scan();
        break;
      case 1:
        scroll();
        break;
      case 2:
        rain();
        break;
      case 3:
        ripple();
        break;
      case 4:
        showTestImage();
        break;
      case 5:
        gameOfLife();
        break;
      case 6:
        seed();
        break;
    }
    _previousMillis = currentMillis;
  }
}

void rain() {
  _panel.fillBuffer(0);
  for (int i = 0; i < 16; i++) {
    if (rainPos[i] < 16) {
      _panel.setPixel(rainPos[i], i, 1);
      for (int j = 0; j < MAX_RAIN_LENGTH; j++) {
        if (rainPos[i] - j >= 0) { _panel.setPixel(rainPos[i] - j, i, 1); }
      }
    }
    // If this drop has fallen of the end randomly decide whether to respawn
    if (rainPos[i] > 15 && random(0, RAIN_MAX_RAND) < RAIN_CUTOFF) {
      if (i > 0 && rainPos[i - 1] < 4) { continue; }
      if (i < 15 && rainPos[i + 1] < 4) { continue; }
      rainPos[i] = 0;
      rainDel[i] = random(0, RAIN_MAX_DELAY);
      rainLast[i] = 0;
    // If the drop is on screen update it
    } else if (rainPos[i] < 16) {
      rainLast[i]++;
      if (rainLast[i] > rainDel[i]) {
        rainPos[i]++;
        rainLast[i] = 0;
      }
    }
  }
}

void gameOfLife() {
  // Version of Conway's Game of Life
  if (_curFrame == 0) {
    initSim();
    _curFrame++;
    return;
  }
  unsigned char nextState[16][16];
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      unsigned char curState = _panel.getPixel(x, y);
      int adjacentLive = countAdjacentLive(x, y);
      if (!curState && adjacentLive == 3) {
        nextState[x][y] = 1;
      } else if (adjacentLive > 1 && adjacentLive < 4 && curState) {
        nextState[x][y] = 1;
      } else {
        nextState[x][y] = 0;
      }
    }
  }
  int different = 0;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      if (nextState[x][y] != _panel.getPixel(x, y)) {
        different++;
      }
      _panel.setPixel(x, y, nextState[x][y]);
    }
  }
  // Arbitrarily require at least LIFE_CUTOFF cells in different states.
  // Otherwise we'll declare this to be boring to watch and restart
  if (different < LIFE_CUTOFF) { _curFrame = 0; }
  else {_curFrame = (_curFrame + 1) % SIM_FRAMES; }
}

void seed() {
  // Version of Silverman's Seeds, similar to game of life
  if (_curFrame == 0) {
    initSim();
    _curFrame++;
    return;
  }
  unsigned char nextState[16][16];
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      int adjacentLive = countAdjacentLive(x, y);
      if (!_panel.getPixel(x, y) && adjacentLive == 2) {
        nextState[x][y] = 1;
      } else {
        nextState[x][y] = 0;
      }
    }
  }
  bool live = false;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      if (nextState[x][y]) { live = true; }
      _panel.setPixel(x, y, nextState[x][y]);
    }
  }
  if (!live) { _curFrame = 0; }
  else { _curFrame = (_curFrame + 1) % SIM_FRAMES; }
}

int countAdjacentLive(int x, int y) {
  int adjacentLive = 0;
  if (x > 0 && y > 0 && _panel.getPixel(x - 1, y - 1)) { adjacentLive++; }
  if (x > 0 && _panel.getPixel(x - 1, y) && _panel.getPixel(x - 1, y)) { adjacentLive++; }
  if (x > 0 && y < 15 && _panel.getPixel(x - 1, y + 1)) { adjacentLive++; }
  if (y < 15 && _panel.getPixel(x, y + 1)) { adjacentLive++; }
  if (x < 15 && y < 15 && _panel.getPixel(x + 1, y + 1)) { adjacentLive++; }
  if (x < 15 && _panel.getPixel(x + 1, y)) { adjacentLive++; }
  if (x < 15 && y > 0 && _panel.getPixel(x + 1, y - 1)) { adjacentLive++; }
  if (y > 0 && _panel.getPixel(x, y - 1)) { adjacentLive++; }
  return adjacentLive;
}

void initSim() {
  // Randomly assign starting states for game of life and seed. 
  int cutOff = random(0, 256);
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      int rand = random(0, 256);
      if (rand < cutOff) {
        _panel.setPixel(x, y, 0);
      } else {
        _panel.setPixel(x, y, 1);
      }
    }
  }
}

void showTestImage() {
  int onesSet = 0;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      _panel.setPixel(x, y, pgm_read_word_near(&testImage[x][y]));
    }
  }
}

void ripple() {
  _curFrame = (_curFrame + 1) % rippleFrameCount;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      _panel.setPixel(x, y, pgm_read_word_near(&rippleFrames[_curFrame][x][y]));
    }
  }
}

void scan() {
  _panel.fillBuffer(0);
  for (int i = 0; i < 16; i ++) {
    _panel.setPixel(i, _curFrame, 1);
  }
  _curFrame = (_curFrame + 1) % 16;
}

void scroll() {
  _curFrame = (_curFrame + 1) % 256;
  _panel.fillBuffer(0);
  _panel.setPixel(floor(_curFrame/16), _curFrame % 16, 1);
}