#include "rpi.h"

int main() {
  init_gpio();
  init_spi(0);
  init_uart(0);

  char msg1[] = "Hello world, this is iotest (" __TIME__ ")\r\nPress 'q' to reboot\r\n";
  uart_puts(0, 0, msg1, sizeof(msg1) - 1);
  char prompt[] = "PI> ";
  uart_puts(0, 0, prompt, sizeof(prompt) - 1);
  char c = ' ';
  while (c != 'q') {
    c = uart_getc(0, 0);
    if (c == '\r') {
      uart_puts(0, 0, "\r\n", 2);
      uart_puts(0, 0, prompt, sizeof(prompt) - 1);
    } else {
      uart_putc(0, 0, c);
    }
  }
  uart_puts(0, 0, "\r\n", 2);
  return 0;
}
