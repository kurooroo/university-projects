#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

byte buttonFlag = 0;              // button pressed flag
byte buttonState = 1;             // current state of button
volatile byte outputDataFlag = 0; // 1 = output data, 0 = no output data
volatile byte outputCounter = 0;  // counting output data bit
volatile byte outputArray[16] = {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1}; // mảng data output

int main(void)
{
    DDRD |= (1 << 1) | (1 << 2) | (1 << 3); // Output on pin 1, 2, 3 of port D
    DDRD &= ~(1 << 4);                      // Button input on pin 4 of port D
    //Timer
    TCCR1B |= (1 << WGM12);                     // Turn on CTC mode for Timer 1
    TCCR1B |= (1 << CS12) | (1 << CS10);        // Set up Timer 1 with prescale 1024
    TIMSK1 &= ~((1 << OCIE1A) | (1 << OCIE1B)); // Disable OCA and OCB match interrupt
    OCR1A = 31249;                              // Set Period = 2s
    OCR1B = 15625;                              // Set Duty = 50%
    sei();
    while (1)
    {
        // Read input button, detect falling edge
        if ((PIND & 0x10) == 0)
        {
            // nếu trạng thái trước đó của nút nhấn = 1 => có sườn xuống
            if (buttonState == 1)
            {
                buttonState = 0;
                buttonFlag = 1;
            }
        }
        else
        {
            buttonState = 1;
        }
        // If button is pressed
        if (buttonFlag)
        {
            TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B); // Enable OCA and OCB match interrupt
            TCNT1 = 0x0000; // Reset counting
            PORTD |= (1 << PD1);
            PORTD &= ~(1 << PD3);
            outputDataFlag = 1;
            outputCounter = 0;
            // Output data
            while (outputDataFlag)
            {
            }
            PORTD &= 0xF5;
            TIMSK1 &= ~((1 << OCIE1A) | (1 << OCIE1B)); // Disable OCA and OCB match interrupt
            buttonFlag = 0;
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    // sau khi xuất hết 16 bit dữ liệu xóa cờ outputDataFlag để kết thúc quá trình xuất
    if (outputCounter > 16) // After output 16 bit data, clear outputDataFlag
    {
        outputDataFlag = 0;
        return;
    }
    // bỏ qua chu kì đầu tiên
    // After 1 second after GPIO1 raise tp 1
    if (outputCounter == 0)
    {
        outputCounter++;
    }
    else
    {
        PORTD |= (1 << PD2); // Output 0.5Hz clock
        outputCounter++;
    }
}

ISR(TIMER1_COMPB_vect)
{
    if (outputCounter > 0)
    {
        if (outputArray[outputCounter - 1]) // Output data
        {
            PORTD |= (1 << PD3);
        }
        else
        {
            PORTD &= ~(1 << PD3);
        }
    }
    PORTD &= 0xFB; // Output 0.5Hz clock
}