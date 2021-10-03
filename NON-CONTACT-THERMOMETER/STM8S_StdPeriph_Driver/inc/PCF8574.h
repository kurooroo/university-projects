#include "stm8s.h"                              
#define PCF8574_address 0x4E

unsigned char PCF8574_read(void);
void PCF8574_write(unsigned char data_byte);