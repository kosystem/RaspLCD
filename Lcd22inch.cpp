#include <stdio.h>
#include <string.h>
#include "Lcd22inch.h"

Lcd22inch::Lcd22inch()
{
  this->width = 240;
  this->height = 320;
}

Lcd22inch::~Lcd22inch()
{
  printf("Tarminated LCD 2.2 inch\n");
  bcm2835_gpio_fsel(LCD_RESET, BCM2835_GPIO_FSEL_INPT);
  bcm2835_spi_end();
  bcm2835_close();
}

void Lcd22inch::init(unsigned int rotate)
{
  setupIO();
  setupLcd(rotate);
  if (rotate >= LCD_TO_RIGHT)
  {
    this->width = 320;
    this->height = 240;
  }
}

unsigned short Lcd22inch::rgb24to16(unsigned char r, unsigned char g, unsigned char b)
{
  unsigned short rgb=0;
  rgb = (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));
  
  return rgb;
}

// Low lever API -----------------------------------------------------------------
void Lcd22inch::setReset()
{
  bcm2835_gpio_write(LCD_RESET, LOW);
}

void Lcd22inch::clrReset()
{
  bcm2835_gpio_write(LCD_RESET, HIGH);
}

void Lcd22inch::sendCommand(unsigned char command)
{
  bcm2835_gpio_write(LCD_DATA_COMMAND, LOW);
  bcm2835_spi_transfer(command);
}

void Lcd22inch::sendData(unsigned char data)
{
  bcm2835_gpio_write(LCD_DATA_COMMAND, HIGH);
  bcm2835_spi_transfer(data);
}

// API --------------------------------------------------------------------------
void Lcd22inch::setDrawPosition(unsigned int x, unsigned int y)
{
  sendCommand(0x2a);
  sendData(x >> 8);
  sendData(x & 0xff);
  sendCommand(0x2b);
  sendData(y >> 8);
  sendData(y & 0xff);
  sendCommand(0x2c);
}

void Lcd22inch::setDrawArea(unsigned int x, unsigned int y, unsigned int xe, unsigned ye)
{
  sendCommand(0x2a);
  sendData(x >> 8);
  sendData(x & 0xff);
  sendData(xe >> 8);
  sendData(xe & 0xff);
  sendCommand(0x2b);
  sendData(y >> 8);
  sendData(y & 0xff);
  sendData(ye >> 8);
  sendData(ye & 0xff);
  sendCommand(0x2c);
}

void Lcd22inch::drawPixel(unsigned short rgb16)
{
  sendData(rgb16 >> 8);
  sendData(rgb16 & 0xff);
}

void Lcd22inch::drawChar(unsigned char c, unsigned int x, unsigned int y)
{
  if (c < 0x20) return;
  
  setDrawArea(x, y, x+5, y+7);
  c -= 0x20;
  for(int i=0; i<8; i++)
    {
    for(int j=0; j<6; j++)
      {
      if((Font[c][j] >> i) & 0x01 && j < 5 )
        drawPixel(rgb24to16(255, 255, 255));
      else
        drawPixel(rgb24to16(0, 0, 0));
      }
    }
}

void Lcd22inch::drawString(const char *str, unsigned int x, unsigned int y)
{
  for(int i=0; i<strlen(str); i++)
    {
    this->drawChar(*(str+i), x, y);
    x += 6;
    }
}

void Lcd22inch::drawTestChart()
{
  unsigned short colors[7];
  
  this->setDrawArea(0, 0, width, height);

  // Test patern 1
  colors[0] = rgb24to16(192, 192, 192);
  colors[1] = rgb24to16(192, 192,   0);
  colors[2] = rgb24to16(  0, 192, 192);
  colors[3] = rgb24to16(  0, 192,   0);
  colors[4] = rgb24to16(192,   0, 192);
  colors[5] = rgb24to16(192,   0,   0);
  colors[6] = rgb24to16(  0,   0, 192);

  int limit = height * 2 / 3;
  int band = width / 7;
  
  for(int i=0; i<limit; i++)
    {
    for(int j=0; j<width; j++)
      {
      int index = j / band;
      if (index > 6) index = 6;
      drawPixel(colors[index]);
      }
    }

  // Test patern 2
  colors[0] = rgb24to16(  0,   0,   0);
  colors[1] = rgb24to16( 42,  42,  42);
  colors[2] = rgb24to16( 84,  84,  84);
  colors[3] = rgb24to16(126, 126, 126);
  colors[4] = rgb24to16(168, 168, 168);
  colors[5] = rgb24to16(210, 210, 210);
  colors[6] = rgb24to16(252, 252, 252);
  limit = height / 6;
  for(int i=0; i<limit; i++)
    {
    for(int j=0; j<width; j++)
      {
      int index = j / band;
      if (index > 6) index = 6;
      drawPixel(colors[index]);
      }
    }

  // Test patern 3
  colors[0] = rgb24to16(255,   0,   0);
  colors[1] = rgb24to16( 64,   0,   0);
  colors[2] = rgb24to16(  0, 255,   0);
  colors[3] = rgb24to16(  0,  64,   0);
  colors[4] = rgb24to16(  0,   0, 255);
  colors[5] = rgb24to16(  0,   0,  64);
  band = width / 6;
  limit = height / 6;
  for(int i=0; i<limit; i++)
    {
    for(int j=0; j<width; j++)
      {
      int index = j / band;
      if (index > 5) index = 5;
      drawPixel(colors[index]);
      }
    }
}

// Set up ------------------------------------------------------------------------
void Lcd22inch::setupIO()
{
  if (!bcm2835_init()) return;
  // init GPIO
  bcm2835_gpio_fsel(LCD_RESET, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(LCD_DATA_COMMAND, BCM2835_GPIO_FSEL_OUTP);
  
  // init SPI
  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
}

void Lcd22inch::setupLcd(unsigned int rotate)
{
  setReset();
  bcm2835_delay(1);
  clrReset();
  bcm2835_delay(120);

  sendCommand(0xCB);  
  sendData(0x39); 
  sendData(0x2C); 
  sendData(0x00); 
  sendData(0x34); 
  sendData(0x02); 

  sendCommand(0xCF);  
  sendData(0x00); 
  sendData(0XC1); 
  sendData(0X30); 

  sendCommand(0xE8);  
  sendData(0x85); 
  sendData(0x00); 
  sendData(0x78); 

  sendCommand(0xEA);  
  sendData(0x00); 
  sendData(0x00); 

  sendCommand(0xED);  
  sendData(0x64); 
  sendData(0x03); 
  sendData(0X12); 
  sendData(0X81); 

  sendCommand(0xF7);  
  sendData(0x20); 

  sendCommand(0xC0);    //Power control 
  sendData(0x23);   //VRH[5:0] 

  sendCommand(0xC1);    //Power control 
  sendData(0x10);   //SAP[2:0];BT[3:0] 

  sendCommand(0xC5);    //VCM control 
  sendData(0x3e); //
  sendData(0x28); 

  sendCommand(0xC7);    //VCM control2 
  sendData(0x86);  //--

  sendCommand(0x36);    // Memory Access Control 
  //0x48 0x68
  //0x28 0xE8
  if(rotate == 0)  sendData(0x48);
  else if(rotate == 1) sendData(0x88);
  else if(rotate == 2) sendData(0x28);
  else if(rotate == 3) sendData(0xE8);
    
  sendCommand(0x3A);    
  sendData(0x55); 

  sendCommand(0xB1);    
  sendData(0x00);  
  sendData(0x18); 

  sendCommand(0xB6);    // Display Function Control 
  sendData(0x08); 
  sendData(0x82);
  sendData(0x27);  

  sendCommand(0xF2);    // 3Gamma Function Disable 
  sendData(0x00); 

  sendCommand(0x26);    //Gamma curve selected 
  sendData(0x01); 

  sendCommand(0xE0);    //Set Gamma 
  sendData(0x0F); 
  sendData(0x31); 
  sendData(0x2B); 
  sendData(0x0C); 
  sendData(0x0E); 
  sendData(0x08); 
  sendData(0x4E); 
  sendData(0xF1); 
  sendData(0x37); 
  sendData(0x07); 
  sendData(0x10); 
  sendData(0x03); 
  sendData(0x0E); 
  sendData(0x09); 
  sendData(0x00); 

  sendCommand(0XE1);    //Set Gamma 
  sendData(0x00); 
  sendData(0x0E); 
  sendData(0x14); 
  sendData(0x03); 
  sendData(0x11); 
  sendData(0x07); 
  sendData(0x31); 
  sendData(0xC1); 
  sendData(0x48); 
  sendData(0x08); 
  sendData(0x0F); 
  sendData(0x0C); 
  sendData(0x31); 
  sendData(0x36); 
  sendData(0x0F); 

  sendCommand(0x11);    //Exit Sleep 

  bcm2835_delay(120); 

  sendCommand(0x29);    //Display on 
  sendCommand(0x2c); 
}







