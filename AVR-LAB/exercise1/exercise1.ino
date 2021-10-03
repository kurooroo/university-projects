#include <avr/io.h>
#include <avr/interrupt.h>

int mode = 1; // Set initial mode

int main(void)
{
  DDRB |= (1 << 1);
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 = (1 << OCIE1A);
  OCR1A = 46875;
  sei();
  while (1)
  {
  }
}

ISR(TIMER1_COMPA_vect)
{
  TIFR1 = (1<<OCF1A);
  TIMSK1 = (0 << OCIE1A);
  PORTB = PINB^0x02;
  switch(mode)
  {
    case 0: //3s
      OCR1A = 46875;
      mode++;
      break;
    case 1: //3s
      OCR1A = 46875;
      mode++;
      break;
    case 2: //3s
      OCR1A = 46875;
      mode++;
      break;
    case 3: //2s
      OCR1A = 31250;
      mode++;
      break; 
    case 4: //1s
      OCR1A = 15625;
      mode++;
      break; 
    case 5: //1s
      OCR1A = 15625;
      mode++;
      break; 
    case 6: //1s
      OCR1A = 15625;
      mode++;
      break; 
    case 7: //4s
      OCR1A = 62500;
      mode = 0;
      break;
  }
  TIMSK1 = (1 << OCIE1A);
}

