//#include <stdio.h>
//#include <stdlib.h>
#include <time.h>
#include "Lcd22inch.h"

int main(int argc, char **argv)
{
  Lcd22inch *lcd = new Lcd22inch();
  lcd->init(LCD_TO_TOP);
  lcd->drawTestChart();
  
  lcd->drawString("Hellow LCD 2.2 inch !!", 0, 0);

  time_t timer;
  
  for (int i=0; i<4; i++)
  {
    time(&timer);
    lcd->drawString(ctime(&timer), 0, 8); 
    bcm2835_delay(500);
  }

  delete lcd;

  return 0;
}

