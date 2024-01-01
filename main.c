/** 
 * --------------------------------------------------------------------------------------------+ 
 * @desc        Main example ILI9341 LCD driver
 * --------------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       12.12.2020
 * @update      13.12.2020
 * @file        main.c
 * @version     1.0
 * @tested      AVR Atmega328p
 *
 * @depend      ili9341.h
 * --------------------------------------------------------------------------------------------+
 * @interface   8080-I Series Parallel Interface
 * @pins        5V, 3.3V -> NC, GND, RST, CS, RS, WR, RD, D[7:0]
 *     
 */
#include <util/delay.h>
#include "lib/ili9341.h"

/**
 * @desc    Main function
 *
 * @param   Void
 *
 * @return  Void
 */
int main(void)
{
  // init lcd
  ILI9341_Init();

  // clear Screen
  ILI9341_ClearScreen(ILI9341_BLACK);

  // draw horizontal fast line
  ILI9341_DrawLineHorizontal(10, ILI9341_MAX_X - 10, 12, ILI9341_WHITE);
  // draw horizontal fast line
  ILI9341_DrawLineHorizontal(10, ILI9341_MAX_X - 10, 50, ILI9341_WHITE);

  // set position
  ILI9341_SetPosition(11, 25);  
  // draw string
  ILI9341_DrawString("ILI9341 LCD DRIVER", ILI9341_RED, X3);

  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}




