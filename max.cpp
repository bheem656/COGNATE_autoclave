#include "max.h"
#include <SPI.h>

#include <Arduino.h>


//#include <SPI.h>

// void timer1_init(void)
// {
//   noInterrupts();  // disable all interrupts
//   TCCR1A = 0;
//   TCCR1B = 0;
//   TCNT1 = 0;

//   OCR1A = 31250;  // compare match register 16MHz/256/2Hz
//   TCCR1B |= (1 << WGM12);  // CTC mode

//   TCCR1B |= (1 << CS12);  // 256 prescaler
//   TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
//   interrupts();  // enable all interrupts

// }

void MAX7219_Clear(uint8_t device)
{
  char i;
  for (i = 1; i < 9; i++)
    maxTransfer(i, 0x00, device); // turn off all segments no decoding
}

void initialize_lcd()
{
  // Configure chip select pin as output.
  pinMode(LOAD_PIN, OUTPUT);

  // Wait a little to allow MAX7219 to see a correct logic level on CS pin.
  delay(100);

  // Configure SPI.
  // SPI.begin();
  // SPI.setBitOrder(MSBFIRST);
  // SPI.setDataMode(SPI_MODE0);
  // SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void Disp_board_config(void)
{
  // pinMode(LOAD_PIN, OUTPUT);
  //  SPI.begin();

  pinMode(LOAD_PIN, OUTPUT);
  SPI.begin();

  //  maxTransfer(OP_DISPLAYTEST, 0x01,1);
  //  maxTransfer(OP_DISPLAYTEST, 0x01,2);
  //  delay(1000);
  //  maxTransfer(OP_DISPLAYTEST, 0x00,1);
  //  maxTransfer(OP_DISPLAYTEST, 0x00,2);
  //  delay(1000);
  // Enable mode B
  maxTransfer(OP_DECODEMODE, 0x00, 1); // No decoding
  maxTransfer(OP_DECODEMODE, 0x00, 2); // No decoding
                                       // delay(1000);
  //  Use lowest intensity
  maxTransfer(OP_INTENSITY, 0x0F, 1);
  maxTransfer(OP_INTENSITY, 0x0F, 2);
  // delay(1000);
  //  Only scan one digit
  maxTransfer(OP_SCANLIMIT, 0xFF, 1);
  maxTransfer(OP_SCANLIMIT, 0xFF, 2);
  // delay(1000);
  //  Turn on chip
  maxTransfer(OP_SHUTDOWN, 0x01, 1); // enable first IC
  maxTransfer(OP_SHUTDOWN, 0x01, 2); // enable Second IC
                                     // delay(1000);
  MAX7219_Clear(1);
  MAX7219_Clear(2);
  //   delay(1000);
}

// void lcd1_temp(float temp)
//{
//   // uint8_t count = 0,pos = 1;
//   float num = 0;
//   if (temp > 0)
//   {
//     num = temp;
//   }
//
//   else
//   {
//     num = temp * (-1);
//   }
//
//   int intpart = (int)num;
//   float decpart = num - intpart;
//   int dec = (int)(decpart * 10);
//   // Serial.println(intpart);
//   // Serial.println(dec);
//
//    set_char(1, dec, 1, false);
//
//   int dig = intpart % 10;
//   intpart = intpart / 10;
//    set_char(2, dig, 1, true);
//
//   dig = intpart % 10;
//   intpart = intpart / 10;
//    set_char(3, dig, 1, false);
//
//   if (temp > 0)
//   {
//     dig = intpart % 10;
//     intpart = intpart / 10;
//      set_char(4, dig, 1, false);
//   }
//
//   else
//   {
//      set_char(4, '-', 1, false);
//   }
// }
//
// void lcd2_press(float press)
//{
//   // uint8_t count = 0,pos = 1;
//   float num = 0;
//   if (press > 0)
//   {
//     num = press;
//   }
//
//   else
//   {
//     num = press * (-1);
//   }
//
//   int intpart = (int)num;
//   float decpart = num - intpart;
//   int dec = (int)(decpart * 10);
//   // Serial.println(intpart);
//   // Serial.println(dec);
//
//    set_char(5, dec, 1, false);
//
//   int dig = intpart % 10;
//   intpart = intpart / 10;
//    set_char(6, dig, 1, true);
//
//   dig = intpart % 10;
//   intpart = intpart / 10;
//    set_char(7, dig, 1, false);
//
//   if (press > 0)
//   {
//     dig = intpart % 10;
//     intpart = intpart / 10;
//      set_char(8, dig, 1, false);
//   }
//
//   else
//   {
//      set_char(8, '-', 1, false);
//   }
// }

void lcd1_temp(float temp)
{
  uint8_t device = 1;

  //   uint8_t device = _dev;

  float num = 0;
  if (temp > 0)
  {
    num = temp;
  }

  else
  {
    num = temp * (-1);
  }

  int intpart = (int)num;
  float decpart = num - intpart;
  int dec = (int)(decpart * 10);
  // Serial.println(intpart);
  // Serial.println(dec);

  set_char(1, dec, device, false);

  int dig = intpart % 10;
  intpart = intpart / 10;
  set_char(2, dig, device, true);

  dig = intpart % 10;
  intpart = intpart / 10;
  set_char(3, dig, device, false);

  if (temp > 0)
  {
    dig = intpart % 10;
    intpart = intpart / 10;
    //     set_char(4, dig, 2, false);

    if (dig == 0)
      maxTransfer(4, 0x00, device);
    //       set_char(4, 0x00, 2, false);
    else
      set_char(4, dig, device, false);
  }

  else
  {
    set_char(4, '-', device, false);
  }
}

void lcd2_press(float press)
{
  // uint8_t count = 0,pos = 1;
  uint8_t pos = 4;

  float num = 0;
  if (press > 0)
  {
    num = press;
  }

  else
  {
    num = press * (-1);
  }

  int intpart = (int)num;
  float decpart = num - intpart;
  int dec = (int)(decpart * 10);
  // Serial.println(intpart);
  // Serial.println(dec);

  //   set_char(pos, dec, 1, false);

  int dig = intpart % 10;
  intpart = intpart / 10;
  set_char(pos + 1, dig, 1, true);

  dig = intpart % 10;
  intpart = intpart / 10;
  set_char(pos + 2, dig, 1, false);

  if (press > 0)
  {
    dig = intpart % 10;
    intpart = intpart / 10;
    if (dig == 0)
      set_char(pos + 3, 16, 1, false);
    else
      set_char(pos + 3, dig, 1, false);
  }

  else
  {
    set_char(pos + 3, '-', 1, false);
  }
}

// void lcd3_time_incr(uint8_t min, uint8_t sec)
// {

//   unsigned long time_now = 0, curr = 0;
//   uint16_t period = min * 60 + sec;

//   uint8_t mm = 0, ss = 1;
//   uint8_t mm4 = 0;
//   uint8_t mm3 = 0;
//   uint8_t ss2 = 0;
//   uint8_t ss1 = 1;

//   time_now = millis();
//   while (millis() < time_now + period)
//   {
//     // curr = millis();

//     while (millis() > time_now + 60 * ss)
//     {
//     }

//     ss++;
//     if (ss > 59)
//     {
//       ss = 0;
//       mm = 1;
//     }

//     ss1 = ss % 10;
//     ss2 = ss / 10;
//     mm3 = mm % 10;
//     mm4 = mm / 10;

//      set_char(1, mm4, 2, false);
//      set_char(2, mm3, 2, false);
//      set_char(3, ss2, 2, true);
//      set_char(4, ss1, 2, false);
//   }
// }

// void lcd3_time_decr(uint8_t min, uint8_t sec)
// {
// }

void set_char(uint8_t pos, uint8_t val, uint8_t device, bool dp)
{

  if (dp)
    val = characterArray[val] | 0x80;
  else
    val = characterArray[val];
  maxTransfer(pos, val, device);
}

void maxTransfer(uint8_t address, uint8_t value, uint8_t device)
{
  //
  if (device == 1)
  {
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE3));
    //    digitalWrite(LOAD_PIN, LOW);
    PORTB &= ~(1 << 0);
    SPI.transfer(address);
    //        SPI.transfer(0x00);
    SPI.transfer(value);
    //    digitalWrite(LOAD_PIN, HIGH);
    PORTB |= (1 << 0);
    SPI.endTransaction();
  }

  else if (device == 2)
  {
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE3));
    //    digitalWrite(LOAD_PIN, LOW);
    PORTL &= ~(1 << 5);
    // uint16_t temp1,temp2 ;

    SPI.transfer(address);

    SPI.transfer(value);
    //            SPI.transfer(0x00);
    //    SPI.transfer(0x00);

    //    digitalWrite(LOAD_PIN, HIGH);
    PORTL |= (1 << 5);
    SPI.endTransaction();
  }
}

void print_load()
{
  set_char(1, 68, 2, false); // 68
  set_char(2, 97, 2, false); // 65
  set_char(3, 79, 2, false); // 79
  set_char(4, 76, 2, false); // 76
}
void print_load_blink()
{
  set_char(1, 68, 2, false); // 68
  set_char(2, 97, 2, false); // 65
  set_char(3, 79, 2, false); // 79
  set_char(4, 76, 2, false); // 76
  delay(1000);
  MAX7219_Clear(2);
  delay(1000);
}
void print_he(void)
{
  set_char(3, 72, 2, false); // 72 h
  set_char(2, 69, 2, false); // 69 e
}
void print_ua(uint8_t number)
{
  set_char(3, 85, 2, false);     // 85 u
  set_char(2, 65, 2, false);     // 65 a
  set_char(1, number, 2, false); // 1
}
void print_pr(uint8_t number)
{
  set_char(3, 80, 2, false);     // 80 P
  set_char(2, 82, 2, false);     // 62 R
  set_char(1, number, 2, false); // 1
}
void print_st(void)
{
  set_char(3, 83, 2, false); // 83 s
  set_char(2, 84, 2, false); // 84 t
}
void print_re(uint8_t number)
{
  set_char(3, 82, 2, false);     // 62 R
  set_char(2, 69, 2, false);     // 69 e
  set_char(1, number, 2, false); // 1
}
void print_RE(void)
{
  set_char(3, 82, 2, false); // 62 R
  set_char(2, 69, 2, false); // 69 e
  // set_char(1, number, dev, false); // 1
}

void print_dr(void)
{
  set_char(2, 82, 2, false); // 62 R
  set_char(3, 68, 2, false); // 68 d

}

void show_time(uint8_t d4, uint8_t d3, uint8_t d2, uint8_t d1)
{

  set_char(4, d4, 2, false);
  set_char(3, d3, 2, true);
  set_char(2, d2, 2, false);
  set_char(1, d1, 2, false);
}

void print_code(uint8_t d2,uint8_t d1)
{
  set_char(4, 14, 2, false);
  set_char(3, 82, 2, true);
  set_char(2, d2, 2, false);
  set_char(1, d1, 2, false);
}

void print_pass(void)
{

  set_char(4, 80, 2, false); // 80
  set_char(3, 65, 2, false);  // 65
  set_char(2, 115, 2, false);  //115
  set_char(1, 115, 2, false);  //115

}