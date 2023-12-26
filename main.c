#include "USART.h"

int main(void)
{
  USART_Config_t USART_Config = {
    .Baudrate = 9600,
    .ClockPolarity = 0,
    .DeviceMode = USART_MODE_ASYNCH,
    .Direction = USART_DIRECTION_BOTH,
    .Size = USART_SIZE_8,
    .Stop = USART_STOP_1,
    .Parity = USART_PARITY_NONE,
    .EnableDoubleSpeed = false,
  };

  USART_Init(&USART_Config);

  USART_WriteLine("Hello, world!");

  while (1)
    ;
  return 0;
}