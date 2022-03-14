#include "main.h"


void setup()
{
    UART2.begin(9600, SERIAL_8N1, UART_RX2, UART_TX2);
    DBG.begin(9600);
    DBG.println("A9NonBlock0");
    tmr1_soft_cur = millis();
    tmr1_soft_pre = tmr1_soft_cur;
}

void loop()
{
    sys_soft_irq();
    sys_soft_timers();
}
