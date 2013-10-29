#ifndef __LCD22INCH_H
#define __LCD22INCH_H

#include <bcm2835.h>
#include "font.h"

#define LCD_RESET RPI_V2_GPIO_P1_22
#define LCD_DATA_COMMAND RPI_V2_GPIO_P1_18


enum {
  LCD_TO_TOP,
  LCD_TO_BOTTOM,
  LCD_TO_RIGHT,
  LCD_TO_LEFT
};

class Lcd22inch
{
public:
  Lcd22inch();
  ~Lcd22inch();

public:
  void init(unsigned int rotate=0);
  static unsigned short rgb24to16(unsigned char r, unsigned char g, unsigned char b);

  void drawPixel(unsigned short rgb16);
  void drawChar(unsigned char c, unsigned int x, unsigned int y);
  void drawString(const char *str, unsigned int x, unsigned int y);
  void drawTestChart();

protected:
  void setupIO();
  void setupLcd(unsigned int rotate);

  void setReset();
  void clrReset();

  void sendCommand(unsigned char command);
  void sendData(unsigned char data);

  void setDrawPosition(unsigned int x, unsigned int y);
  void setDrawArea(unsigned int x, unsigned int y, unsigned int xe, unsigned ye);

  int width;
  int height;
};

#endif //__LCD22INCH_H
