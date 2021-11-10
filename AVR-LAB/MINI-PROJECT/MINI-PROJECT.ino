#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SET_TIME 1 
#define DIGIT 2    
#define VALUE 3    
#define DISPLAY 4
#define ALARM PB5

byte displaySelect = 1; //0 = Show hour, 1 = show minute
byte buttonState[4] = {1, 1, 1, 1};
volatile unsigned long second = 0; // biến lưu thời gian hiện tại
unsigned long alarmOffSecond = 180; // 3 minute alarm in second 3*60
unsigned long initTime = 0; //set time in second
unsigned long alarmTime = 50; // change alarm time here, 5 = 5 second
byte numberTable[10] = {0xFE, 0xB0, 0xED, 0xF9, 0xB3, 0xDB, 0xDF, 0xF0, 0xFF, 0xFB}; // lookupTable for 7 segment display
volatile char alarmOff = 0; // alarm flag
volatile char freqDivider = 0; // 

char ReadFallingEdgeButton(char);
void ShowTime(char, unsigned long);
void SetTime();

int main(void)
{
  DDRD = 0xFF;
  DDRB |= (1 << 0) | (1 << 5);
  DDRB &= ~((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4));
  // Timer
  // Timer 1 for keeping time
  TCCR1B |= (1 << WGM12);              // Turn on CTC mode for Timer 1
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set up Timer 1 with prescale 1024
  TIMSK1 = (1 << OCIE1A);              // Enable OCA  match interrupt
  OCR1A = 15625;                       // Set Period = 1s
  // Timer 0 for ALARM alarm
  TCCR0B |= (1 << WGM02);              // Turn on CTC mode for Timer 1
  TCCR0B |= (1 << CS02) | (1 << CS00); // Set up Timer 1 with prescale 1024
  TIMSK0 = (1 << OCIE0A);              // Enable OCA  match interrupt
  OCR0A = 194;                         // Set frequency = 80Hz
  sei();                               // Enable interrupt
  PORTB &= ~(1 << ALARM);              //Turn off alarm
  SetTime();// đưa đồng hồ vào chế độ set time sau khi khởi động
  while (1)
  {
    // lựa chọn hiển thị giờ hoặc phút
    if (displaySelect == 0)
    {
      ShowTime(0, second);
    }
    else if (displaySelect == 1)
    {
      ShowTime(1, second);
    }
    // kiểm tra điều kiện alarm
    if ((second - initTime >= alarmOffSecond) && (second - initTime < (alarmOffSecond + alarmTime)))
    {
      alarmOff = 1;
    }
    else
    {
      alarmOff = 0;
      PORTB &= ~(1 << ALARM); //Turn off alarm
    }
    // kiểm tra nút nhấn đặt giờ
    if (ReadFallingEdgeButton(SET_TIME))
    {
      SetTime();
    }
    // kiểm tra nút nhấn lựa chọn hiển thị giờ hoặc phút
    if (ReadFallingEdgeButton(DISPLAY))
    {
      if (displaySelect == 1)
      {
        displaySelect = 0;
      }
      else
      {
        displaySelect = 1;
      }
    }
  }
}

// hàm ngắt cho timer 1, xuất hiện ngắt mỗi 1s để theo dõi thời gian
ISR(TIMER1_COMPA_vect)
{
  second += 10; // change here to increase simulation speed, 1 unit = 1 second
  // nếu thời gian qua 24h thì reset lại về 0 để đếm tiếp
  if (second >= 86400)
  {
    second = 0;
  }
}

// hàm ngắt timer 0 8bit, ngắt ở tần số 20Hz. Toggle alarm nên sẽ ra tín hiệu có tần số 10Hz
ISR(TIMER0_COMPA_vect)
{
  if (alarmOff == 1)
  {
    freqDivider++;
    // Divide 80Hz to get 20Hz freqency toggle => 10Hz freq output
    // 4 lần ngắt ở 80Hz sẽ xuất tín hiệu 1 lần => 20Hz
    if (freqDivider == 3)
    {
      PORTB = PINB ^ 0x20; //Togle alarm
      freqDivider = 0;
    }
  }
}

// Mulitiplex 2 7segment leg
void Display(int number)
{
  char temp = 0;
  // Tắt cả 2 led 7 thanh
  PORTD = 0x80;
  PORTB |= (1 << PB0);
  // hiển thị số hàng đơn vị
  temp = (char)(number % 10);
  PORTD = numberTable[temp];
  PORTD &= ~(1 << PD7);
  PORTB |= (1 << PB0);
  _delay_ms(10);
  PORTD = 0x80;
  // hiển thị số hàng chục
  temp = (char)((number / 10) % 10);
  PORTD = numberTable[temp];
  PORTD |= (1 << PD7);
  PORTB &= ~(1 << PB0);
  _delay_ms(10);
  PORTB |= (1 << PB0);
  PORTD = 0x80;
}

// hàm đọc sườn xuống các nút nhấn, nếu tín hiệu ở mức thấp mà trạng thái trước của tín hiệu (check buttonState) ở mức cao thì là có nhấn nút
char ReadFallingEdgeButton(char buttonIndex)
{
  // Read input button, detect falling edge
  if ((PINB & (1 << buttonIndex)) == 0)
  {
    // _delay_ms(100); //Debounce button if needed
    if ((PINB & (1 << buttonIndex)) == 0)
    {
      if (buttonState[buttonIndex - 1] == 1)
      {
        buttonState[buttonIndex - 1] = 0;
        return 1;
      }
    }
  }
  else
  {
    buttonState[buttonIndex - 1] = 1;
  }
  return 0;
}

// hàm settime đọc giờ và phút từ người dùng, chuyển thành số giây
void SetTime()
{
  char digitIndex = 0; // hh:mm = digitIndex3 digitIndex 2:digit Index 1 digitIndex 0
  unsigned long hourInit = 0;
  unsigned long minuteInit = 0;
  while (1)
  {
    // nếu nhấn nút set time thì chuyển giờ phút thành số giây để đồng hồ chạy
    if (ReadFallingEdgeButton(SET_TIME))
    {
      initTime = (hourInit * 3600) + (minuteInit * 60);
      second = initTime;
      break;
    }
    // nếu nhấn nút digit thì thay đổi vị trí con trỏ theo thú tự 0 1 2 3 0
    if (ReadFallingEdgeButton(DIGIT))
    {
      digitIndex++;
      if (digitIndex > 3)
      {
        digitIndex = 0;
      }
    }
    // nhấn nút value thì sẽ xử lí theo vị trí của digit để đưa ra số giờ và phút tương ứng
    if (ReadFallingEdgeButton(VALUE))
    {
      switch (digitIndex)
      {
      case 0:
        minuteInit++;
        if ((minuteInit % 10) == 0)
        {
          minuteInit -= 10;
        }
        break;
      case 1:
        minuteInit += 10;
        if (minuteInit > 59)
        {
          minuteInit = minuteInit - 60;
        }
        break;
      case 2:
        hourInit++;
        if ((hourInit % 10) == 0)
        {
          hourInit -= 10;
        }
        break;
      case 3:
        hourInit += 10;
        if (hourInit > 23)//03 13 23 03
        {
          hourInit = hourInit - 30;
        }
        break;
      default:
        break;
      }
    }
    if (digitIndex > 1)
    {
      Display(hourInit);
    }
    else
    {
      Display(minuteInit);
    }
  }
}

// hiển thị thời gian
// select = 1 = hiển thị phút, = 0 = hiển thị giờ
void ShowTime(char select, unsigned long currentSecond)
{
  unsigned long hour = 0;
  unsigned long minute = 0;
  // tính số giờ từ số giây
  hour = currentSecond / 3600;
  if (select == 0)
  {
    Display(hour);
  }
  else if (select == 1)
  {
    // tính phút từ giây
    minute = (unsigned long)((currentSecond - (hour * 3600)) / 60);
    Display(minute);
  }
}