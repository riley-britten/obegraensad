/*
  Panel.h - Library for controlling Ikea Obegraensad.
  Created by Caoimhe Riley, November, 2024.
*/

#ifndef Panel_h
#define Panel_h

#include "Arduino.h"

class Panel {
  public:
    Panel();
    void display();
    void fillBuffer(unsigned char val);
    void setPixel(int x, int y, unsigned char val);
    void setPixelAtIndex(int i, unsigned char val);
    unsigned char getPixel(int x, int y);
    void setBuffer(int val[]);
  private:
    const int _bufferSize = 256;
    bool _panelBuffer[256];
    const int _CLA = 10;
    const int _CLK = 11;
    const int _IN = A1;
    const int _EN = A0;
    const int _timing = 10;
    const unsigned int _addrTable[16][16] = {
      {232, 233, 234, 235, 236, 237, 238, 239, 248, 249, 250, 251, 252, 253, 254, 255},
      {231, 230, 229, 228, 227, 226, 225, 224, 247, 246, 245, 244, 243, 242, 241, 240},
      {216, 217, 218, 219, 220, 221, 222, 223, 200, 201, 202, 203, 204, 205, 206, 207},
      {215, 214, 213, 212, 211, 210, 209, 208, 199, 198, 197, 196, 195, 194, 193, 192},
      {168, 169, 170, 171, 172, 173, 174, 175, 184, 185, 186, 187, 188, 189, 190, 191},
      {167, 166, 165, 164, 163, 162, 161, 160, 183, 182, 181, 180, 179, 178, 177, 176},
      {152, 153, 154, 155, 156, 157, 158, 159, 136, 137, 138, 139, 140, 141, 142, 143},
      {151, 150, 149, 148, 147, 146, 145, 144, 135, 134, 133, 132, 131, 130, 129, 128},
      {104, 105, 106, 107, 108, 109, 110, 111, 120, 121, 122, 123, 124, 125, 126, 127},
      {103, 102, 101, 100, 99 , 98 , 97 , 96 , 119, 118, 117, 116, 115, 114, 113, 112},
      {88 , 89 , 90 , 91 , 92 , 93 , 94 , 95 , 72 , 73 , 74 , 75 , 76 , 77 , 78 , 79 },
      {87 , 86 , 85 , 84 , 83 , 82 , 81 , 80 , 71 , 70 , 69 , 68 , 67 , 66 , 65 , 64 },
      {40 , 41 , 42 , 43 , 44 , 45 , 46 , 47 , 56 , 57 , 58 , 59 , 60 , 61 , 62 , 63 },
      {39 , 38 , 37 , 36 , 35 , 34 , 33 , 32 , 55 , 54 , 53 , 52 , 51 , 50 , 49 , 48 },
      {24 , 25 , 26 , 27 , 28 , 29 , 30 , 31 , 8  , 9  , 10 , 11 , 12 , 13 , 14 , 15 },
      {23 , 22 , 21 , 20 , 19 , 18 , 17 , 16 , 7  , 6  , 5  , 4  , 3  , 2  , 1  , 0  }
    };
};
#endif