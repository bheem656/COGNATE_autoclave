#include <Sodaq_PcInt.h>
#include "BoardConfig.h"
#include "max.h"
#include <EEPROM.h>
// #include "ErrorList.h"

/************** Global Variable  ********************/
uint8_t int_status = 0;
uint8_t dev = 2;
// uint8_t current_cycle = 2;
uint8_t bypass_temp_SG = 150;
uint8_t bypass_temp_RING = 100;

int8_t process_status;
uint32_t cuurent_time;
uint32_t last_time;
volatile uint8_t RS;
uint8_t error_status;
volatile uint8_t door_status = 0;
volatile uint8_t fresh = 0, drain = 0;

uint32_t clock_running;
float pres;
float tmp4;
float tmp2;
float tmp3;
uint8_t test_sw = 0;
uint8_t prgrm_sw = 0;
bool start_sw = 0;
uint8_t temp = 0;

volatile uint8_t int_flag = 1;
volatile int count;

uint8_t err_time;

void handleA0()
{

  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  //   noInterrupts();
  if ((interrupt_time - last_interrupt_time) > 2000)
  {
    // Serial1.println("............A000000000000000000000000000000000 .............");

    RS = !RS;
  }
  // Serial1.print("Intrupt generated ###########################3");
  last_interrupt_time = interrupt_time;
}

/************** Timer1 ISR  ********************/
volatile float _temp, _pres;
ISR(TIMER1_OVF_vect)
{

  count++;
  if (count >= 120)
  {
    count = 0;
    _pres = mpx();
    _temp = TS2() + 0.5; // +0.5
    lcd1_temp(_temp);
    lcd2_press(_pres);
    TIFR1 |= 0x01;
  }
  TIFR1 |= 0x01; // clear the interrupt flag
}

volatile uint8_t mm = 0, ss = 1;
volatile uint8_t mm4 = 0;
volatile uint8_t mm3 = 0;
volatile uint8_t ss2 = 0;
volatile uint8_t ss1 = 1;
// unsigned long time_now ;

volatile uint8_t up_clock = 2; // 1: up clock   , 2 : down clock , 0 : disable

volatile uint8_t timeout_sec = 80;
volatile uint16_t _ss = 0;

ISR(TIMER4_OVF_vect)
{

  count++;

  if (count >= 61 && (up_clock > 0))
  {
    static uint16_t tim;
    tim++;

    if (tim >= timeout_sec)
    {
      up_clock = 0;
      MAX7219_Clear(1);
    }

    if (up_clock == 1)
    {

      //      if( tim > = timeout_sec )
      //      {
      //        up_clock = 0;
      //        }
      mm = tim / 60;
      ss = tim % 60;

      mm4 = mm / 10;
      mm3 = mm % 10;
      ss2 = ss / 10;
      ss1 = ss % 10;

      set_char(4, mm4, 2, false);
      set_char(3, mm3, 2, true);
      set_char(2, ss2, 2, false);
      set_char(1, ss1, 2, false);
    }

    else if (up_clock == 2)
    {

      _ss = timeout_sec - tim;

      mm = _ss / 60;
      ss = _ss % 60;

      mm4 = mm / 10;
      mm3 = mm % 10;
      ss2 = ss / 10;
      ss1 = ss % 10;

      set_char(4, mm4, 2, false);
      set_char(3, mm3, 2, true);
      set_char(2, ss2, 2, false);
      set_char(1, ss1, 2, false);
    }

    count = 0;

    TIFR4 |= 0x01;
  }

  TIFR4 |= 0x01; // clear the interrupt flag
}

// void ISR_door()
// {
//   //  PORTJ |= _BV(t134_led);

//   if (!(PINE & (1 << 4)))
//   {
//     door_status = 1;
//     Serial1.println("............Door Closed .............");
//   }

//   else
//   {
//     door_status = 0;
//     Serial1.println("..........Please close the Door first  and check water status...........");
//   }
// }

void ISR_water_fresh()
{
  if (PINE & (1 << 5))
  {
    //  // Serial1.println("lower sensor WATER empty");
    PORTA |= _BV(water_status_led);
    fresh = 1;
  }
  else
  {
    PORTA &= ~_BV(water_status_led);
    fresh = 0;
  }
}

uint8_t readFlag = 0;
unsigned long timer;
unsigned long duration;
int analogVal0 = 0;
int analogVal1 = 0;
int analogVal8 = 0;

ISR(ADC_vect)
{
  //  duration = micros() - timer;
  //  timer = micros();

  // analogVal0 = ADCL | (ADCH << 8);//alway read Low byte first!!

  switch (ADMUX)
  {
  case 0x40:
    analogVal0 = ADCL | (ADCH << 8); // alway read Low byte first!!
    if (analogVal0 >= 250)
      ADMUX = 0x41;
    break;

  case 0x41:
    analogVal1 = ADCL | (ADCH << 8); // alway read Low byte first!!
    ADMUX = 0x48;
    break;

  case 0x48:
    analogVal8 = ADCL | (ADCH << 8); // alway read Low byte first!!
    ADMUX = 0x40;
    break;

  default:
    break;
  }

  ADCSRA |= (1 << ADSC);
  readFlag = 1;
  //  Serial.println(analogVal);
}

void adc_init(void)
{
  ADMUX |= (1 << REFS0);                                // VCC AREF
  ADCSRA |= (1 << ADIF);                                // ADC Interrupt Flag
  ADCSRA |= (1 << ADIE);                                // ADC Interrupt Enable
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS1); // 111 - 16MHZ /128
  ADCSRA |= (1 << ADEN);                                // ADC Enable
  ADCSRA |= (1 << ADSC);
  sei();
}

float pr;
uint8_t eep_data_0 = 0;

//*******************************  buzzer  Program ***********************************************************//
void buzzer_beep()
{

  PORTH |= (1 << 6);
  delay(50);
  PORTH &= ~(1 << 6);
  delay(60);
  PORTH |= (1 << 6);
  delay(50);
  PORTH &= ~(1 << 6);
  delay(60);
  PORTH |= (1 << 6);
  delay(50);
  PORTH &= ~(1 << 6);
  delay(60);
}

void beep_2()
{
  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);

  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);
}

void beep_4()
{
  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);

  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);

  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);

  PORTH |= (1 << 6);
  delay(200);
  PORTH &= ~(1 << 6);
  delay(100);
}

//*******************************  check water tank   staus ***********************************************************//
void check_water_tank()
{
  //..............................................................................
  if (PINE & (1 << 5))
  {
    //  Distilled water
    PORTA |= _BV(water_status_led);
    fresh = 1;
    beep_4();
  }
  else
  {
    PORTA &= ~_BV(water_status_led);
    fresh = 0;
  }

  /****************   Drain Status ********************/
  if (!(PINE & (1 << 6)))
  {
    PORTA |= _BV(drain_status_led);
    drain = 1;
    beep_4();
  }

  else
  {
    PORTA &= ~_BV(drain_status_led);
    drain = 0;
  }
}

/**
 * @brief setting Rtc time
 *
 */

/*********************************** RTC Time Set ********************************************/
// set time rtc

void print_s3_rtc(uint8_t num)
{
  // Serial1.print("print s3 : ");
  // Serial1.println(num);
  set_char(4, 16, 2, false); // blank
  set_char(3, 16, 2, false); // blank

  set_char(1, (num % 10), 2, false);
  set_char(2, (num / 10), 2, false);
}

void print_s2_rtc(uint8_t num)
{
  Serial1.print("print s2 : ");
  Serial1.println(num);
  set_char(8, 16, 1, false); // blank
  set_char(7, 45, 1, false); // blank
  set_char(6, num, 1, false);
  set_char(5, 45, 1, false);
}

void print_s1_rtc(void)
{
  Serial1.print("print s1 : ");
  // Serial1.println(num);
  set_char(4, 116, 1, true); // t
  set_char(3, 83, 1, true);  // S
  set_char(2, 14, 1, false);
  set_char(1, 116, 1, false);
}

uint32_t _time = 0;

uint8_t change_data(uint8_t _data_)
{

  // if ((millis() - _time) > 20)
  // {
  // delay(700);
  // if (!digitalRead(45)) // prgm
  if (!(PINL & (1 << 6)))
  {
    // delay(20);
    Serial1.println("test button press");
    delay(2000);
    _data_--;
    // delay(300);
    // return _data_;
  }

  // if (!digitalRead(43)) // test  /// 63 -pk1 start
  else if (!(PINK & (1 << 1)))
  {
    Serial1.println("start button press");
    delay(2000);
    _data_++;
    //  delay(300);
    // return _data_;
  }
  // else
  // {
  //   // return _data_;
  // }
  // Serial1.print("......................._data_.....................");

  // Serial1.println(_data_);

  // _time = millis();
  return _data_;
  // }
  // _time = millis();
}
#define june 6
#define november 11
#define april 4
#define september 9
#define february 2

uint8_t get_data_rtc_fields(uint8_t pos)
{

  static uint8_t d1 = 22, d2 = 6, d3 = 7, d4 = 15, d5 = 10, d6 = 5;
  delay(3000);
  switch (pos)
  {
  case 1:
    Serial1.println("pos 1: ");
    d1 = change_data(d1);
    return d1;
    break;

  case 2:
    d2 = change_data(d2);
    if (d2 >= 12)
    {
      d2 = 12;
    }
    else if (d2 <= 1)
    {

      d2 = 1;
    }
    return d2;
    break;

  case 3:

    d3 = change_data(d3);
    if ((d2 == june) || (d2 == november) || (d2 == april) || (d2 == september))
    {
      if (d3 >= 30)
      {
        d3 = 30;
      }
    }

    else if (d2 == february)
    {
      if ((d1 % 4) == 4)
      {

        if (d3 >= 29)
        {
          d3 = 29;
        }
        else if (d3 <= 1)
        {
          d3 = 1;
        }
      }
      else
      {
        if (d3 >= 28)
        {
          d3 = 28;
        }
        else if (d3 <= 1)
        {
          d3 = 1;
        }
      }
    }

    else
    {

      if (d3 >= 31)
      {
        d3 = 31;
      }
      else if (d3 <= 1)
      {
        d3 = 1;
      }
    }
    return d3;
    break;

  case 4:

    d4 = change_data(d4);
    if (d4 >= 24)
    {
      d4 = 24;
    }
    else
    {
      if (d4 <= 1)
      {
        d4 = 1;
      }
    }
    return d4;
    break;

  case 5:

    d5 = change_data(d5);
    if (d5 >= 59)
    {
      d5 = 59;
    }
    else
    {
      if (d5 <= 1)
      {
        d5 = 1;
      }
    }
    return d5;
    break;

  case 6:

    d6 = change_data(d6);
    if (d6 >= 59)
    {
      d6 = 59;
    }
    else
    {
      if (d6 <= 1)
      {
        d6 = 1;
      }
    }
    return d6;
    break;

  default:
    break;
  }
}

void set_time_rtc(void)
{

  uint32_t lat_time;

  uint8_t status_rtc_update = ((!(PINL & (1 << 4))) && (!(PINL & (1 << 6))));
  if ((status_rtc_update))
  {
    Serial1.println("rtc time mode call");
    lat_time = millis();

    while ((millis() - lat_time) < 4000)
    {
      status_rtc_update = ((!(PINL & (1 << 4))) && (!(PINL & (1 << 6))));
      if (!status_rtc_update)
      {
        // delay(1000);
        Serial1.println("exit rtc");
        break;
      }

    }
    delay(1000);
    Serial1.println("set rct time");
    // pl4 39
    // pl6 41
    // set_char(uint8_t pos, uint8_t val, uint8_t device, bool dp);
    uint8_t cnt = 1;
    uint8_t s3_data = 1;

    print_s1_rtc();  // print s.set
    print_s2_rtc(1); // print default year -1-
    uint8_t cnt_new = 1;
    uint8_t cnt_flag = 0;
    uint8_t stop = 0;
    while (!stop) // exit out -7- start press
    {
      cli();
      Serial1.print("Waiting for exit : ");
      // Serial1.println(num);
      // EEPROM.read(1);
      if (cnt == 7)
      {
        delay(200);
        if (!digitalRead(63))
        {
          stop = 1;
          Serial1.print("program exit : ");
          break;
        }
        else
        {
          stop = 0;
        }
      }

      if (!(PINL & (1 << 4))) //
      {
        delay(100);
        if (cnt >= 7)
        {
          cnt = 1;
          // cnt_flag = 0;
        }

        else
        {
          cnt++;
          cnt_flag = 1;
        }
        print_s2_rtc(cnt);

        Serial1.print("PRGM BTN PRESSED :  ");
        Serial1.println(cnt);
      }
      else
      {
        cnt_flag = 0;
      }

      switch (cnt)
      {
      case 1:
        /* display & save year */
        Serial1.print("case 1 : ");
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;
      case 2:
        /* display & save month */
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;

      case 3:
        /* display & save date */
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;

      case 4:
        /* display & save hh */
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;

      case 5:
        /* display & save mm */
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;

      case 6:
        /* display & save ss */
        s3_data = get_data_rtc_fields(cnt);
        print_s3_rtc(s3_data);
        break;

      case 7:
        /* display out */

        set_char(4, 16, 2, false);  // blank
        set_char(3, 111, 2, false); // blank
        set_char(2, 117, 2, false);
        set_char(1, 116, 2, false);

        if (!(PINK & (1 << 1)))
        {
          delay(300);
          stop = 1;
          sei();

          int_status = 1;
        }

        break;

      default:
        break;
      }
      // }
    }
  }
}

/********************************** end rtc function ***************************/

//*******************************  check selected   Program ***********************************************************//

void check_selected_program()
{
  /****************   program button input switch  ********************/
  if (!(PINL & (1 << 4))) //
  {

    Beep(100);
    test_sw = 0;

    if (prgrm_sw >= 5)
    {
      prgrm_sw = 1;
    }

    else
    {
      prgrm_sw++;
    }

    status_led_glow();

    Serial1.print("PRGM BTN PRESSED :  ");
    Serial1.println(prgrm_sw);

    delay(100);
  }

  //   /********** Test program button switch *********************/

  if (!(PINL & (1 << 6)))
  {

    Beep(100);
    prgrm_sw = 0;

    if (test_sw >= 3)
    {
      test_sw = 1;
    }

    else
    {
      test_sw++;
    }

    status_led_glow();

    Serial1.print("test PRGM BTN PRESSED :  ");
    Serial1.println(test_sw);

    delay(100);
  }
}

//*******************************  check error  Program ***********************************************************//
/**
 * @brief condition for check ERROR 98 -->  AUTOMATOC POWER SHUT DOWN DURING RUNNING CYCLE
 *
 */

void vaccume_98()
{
  uint32_t mm_;
  uint32_t ss_;

  uint8_t _m4;
  uint8_t _m3;
  uint8_t _s2;
  uint8_t _s1;

  uint32_t _curr_new_time = millis();
  uint32_t _last_time = millis();
  uint32_t _last_time_new = millis();
  PORTJ &= ~_BV(v2);
  PORTJ &= ~_BV(v3);
  PORTJ |= _BV(v4);
  PORTH |= _BV(vac);
  PORTH &= ~_BV(motor);
  PORTH &= ~_BV(steam);
  PORTH &= ~_BV(heat);

  _last_time = millis() - _curr_new_time;
  Serial1.print("call vaccum 98 function : ");
  while (_last_time < 240000)
  {
    PORTH |= _BV(vac); // vaccume pump on
    PORTJ |= _BV(v4);  // 4th valve on
    _last_time = millis() - _curr_new_time;
    _last_time_new = 240000 - _last_time;
    mm_ = _last_time_new / 60000; // Total minutes
    ss_ = _last_time_new % 60000; // Total seconds
    ss_ = ss_ / 1000;

    _m4 = mm_ / 10; // 4th digit
    _m3 = mm_ % 10; // 3rd digit
    _s2 = ss_ / 10; // 2nddigit
    _s1 = ss_ % 10; // 1st digit
    Serial1.println("....................... ");
    Serial1.print("Time : ");
    Serial1.print(mm_);
    Serial1.print(" : "); // minutes
    Serial1.println(ss_);

    show_time(_m4, _m3, _s2, _s1);
  }
  // RS = 0;
  // print_load_blink();
  PORTJ |= _BV(v2);
  PORTJ |= _BV(v3);
  PORTJ &= ~_BV(v4);
  PORTH &= ~_BV(vac);
  PORTH &= ~_BV(motor);
  PORTC &= ~_BV(steam);
  PORTC &= ~_BV(heat);
  check_selected_program();
}

void find_error()
{

  // // eep_data_0 = EEPROM.read(0);
  // if (EEPROM.read(0))
  // {

  //   EEPROM.update(0, 0);
  //   PORTJ |= _BV(v2);
  //   PORTJ &= ~_BV(v3);
  //   PORTH &= ~_BV(vac);
  //   PORTH &= ~_BV(motor);
  //   PORTC &= ~_BV(steam);
  //   PORTC &= ~_BV(heat);

  //   while (!RS)
  //   {
  //     Serial1.print("############error 98 display : ");
  //     print_code(9, 8);
  //   }

  //   vaccume_98();
  // }
  // else
  // {

  //   if (error_status == 0)
  //   {
  //     PORTJ |= _BV(v2);
  //     door_current_status_print();
  //   }

  //   else
  //   {
  //     PORTJ |= _BV(v2);
  //     PORTJ &= ~_BV(v3);
  //     PORTH &= ~_BV(vac);
  //     PORTH &= ~_BV(motor);
  //     PORTC &= ~_BV(steam);
  //     PORTC &= ~_BV(heat);

  //     Check_Error();
  //     error_status = 0;
  //      print_load();
  //   }
  // }

  if (error_status == 0)
  {
    PORTJ |= _BV(v2);
    door_current_status_print();
    Serial1.println("error status 0");
  }

  else
  {
    Serial1.println("error status 1");
    PORTJ |= _BV(v2);
    PORTJ &= ~_BV(v3);
    PORTH &= ~_BV(vac);
    PORTH &= ~_BV(motor);
    PORTC &= ~_BV(steam);
    PORTC &= ~_BV(heat);

    Check_Error();
    error_status = 0;
    print_load();
  }
}

// void check_door_status()
// {

//   if (digitalRead(48))
//   {
//     door_status = 1;
//     Check_Error();
//     RS = 0;
//   }
//   else
//   {

//   }
// }

//*******************************  Setup  Program ***********************************************************//
uint32_t _timeout = 0;
void setup()
{

  Serial1.begin(9600);
  PcInt::attachInterrupt(63, handleA0);

  // attachInterrupt(digitalPinToInterrupt(2), ISR_door, CHANGE);        // door sensor
  attachInterrupt(digitalPinToInterrupt(3), ISR_water_fresh, CHANGE); // water sensor
  board_init();
  Disp_board_config();
  Timer1_init();
  // EEPROM.write(0, 0);
  // EEPROM.update(0, 0);
  delay(10000);
  /******************************** check er98 **********************/
  if (EEPROM.read(0) == 1)
  {
    EEPROM.update(0, 0);
    PORTJ |= _BV(v2);
    PORTJ &= ~_BV(v3);
    PORTH &= ~_BV(vac);
    PORTH &= ~_BV(motor);
    PORTC &= ~_BV(steam);
    PORTC &= ~_BV(heat);

    while (!RS)
    {
      eep_data_0 = EEPROM.read(0);
      Serial1.print("eprom data  ");
      Serial1.println(eep_data_0);
      // Serial1.print("....error 98 display : ");
      print_code(9, 8);
    }
    vaccume_98();
  }
  //
  Serial1.print("checking error 98 ");
  eep_data_0 = EEPROM.read(0);
  Serial1.print("eprom data  ");
  Serial1.println(eep_data_0);
  RS = 0;
  // find_error();
  // EEPROM.write(0, 1);
  RS = 0;
  PORTH &= ~_BV(vac);

  if (eep_data_0 != 0)
  {
    print_code(9, 8);
    Serial1.print("eprom data  ");
    Serial1.println(eep_data_0);
  }
  print_load();

  PORTH |= _BV(fan);
  PORTJ |= _BV(v2);
  delay(25000);

  PORTH &= ~_BV(vac);
  PORTJ &= ~_BV(v2);
  PORTH &= ~_BV(vac);
  PORTJ &= ~_BV(v3);
  PORTJ &= ~_BV(v4);

  pinMode(48, INPUT);
}

//*******************************  Main  Program ***********************************************************//
void loop()
{
  // eep_data_0 = EEPROM.read(0);
  // Serial1.print("eprom data  ");
  // Serial1.println(eep_data_0);
  // RS = 0;
  if (int_status)
  {
    sei();
    int_status = 0;
  }
  set_time_rtc();
  //   // Serial1.print("  door_status : ");
  //   // Serial1.println(door_status);

  door_status = digitalRead(48);
  while (!door_status)
  {
    if (RS)
    {
      door_status = digitalRead(48);
      print_code(0, 6);
      RS = 0;
      beep_4();
    }
    else
    {
      door_current_status_print();
      beep_2();
    }
  }
  find_error();
  check_selected_program();
  check_water_tank();

  // Serial1.println("...........Please start  program..............");

  if (RS)
  {

    delay(300);

    // IF CONDITION activate only if drain water over flow or distel water empty or door open

    eep_data_0 = EEPROM.read(0);
    door_status = digitalRead(48);
    if ((drain == 1) || (fresh == 1) || (door_status == 0)) //|| (door_status == 1)
    {

      beep_4();
      if (!door_status)
        Serial1.println("PLEASE CLOSE THE DOOR");
      if (fresh)
        Serial1.println("distilled water empty");
      if (drain)
        Serial1.println("drain water tank full");
      RS = 0;
    }
    else
    {
      Serial1.print("eprom data  ");
      Serial1.println(eep_data_0);

      Serial1.print("Door Status ");
      Serial1.println(door_status);

      Serial1.print("Drain Water tank status ");
      Serial1.println(drain);

      Serial1.print("distilled water tank status ");
      Serial1.println(fresh);

      EEPROM.write(0, 1); // eprom zero adderess 1 start cycle running
    }
    /*************
     * Get Temp & Pressure to  skip pre heating cycle *******
     * ********/

    tmp4 = TS1(); // OUTER BODY
    tmp3 = TS3(); // STEAM G

    if (tmp3 > bypass_temp_SG || tmp4 > bypass_temp_RING)
    {
      process_status = 2;
    }

    else
    {

      process_status = 1;
    }

    /************************  IF RS = 1 start running cycle ******************************/
    while (RS)
    {

      // Program going to start from here
      PORTJ &= ~_BV(v2);
      beep_2();

      /*********** Run Main Program Cycle *****************************/
      if (prgrm_sw)
      {

        switch (prgrm_sw)
        {

        case 1:
          status_led_glow();
          Serial1.println("unwrapped_cycle");
          unwrapped_cycle();
          break;

        case 2:
          status_led_glow();
          Serial1.println("wrapped_cycle");
          wrapped_cycle();
          break;

        case 3:
          status_led_glow();
          Serial1.println("prion_cycle");
          prion_cycle();
          break;

        case 4:
          status_led_glow();
          Serial1.println("porous_cycle");
          porous_cycle();
          break;

        case 5:
          status_led_glow();
          Serial1.println("all_prgm_cycle");
          all_prgm_cycle();
          break;

        default:
          Serial1.println("Done prgm................");

          break;
        }
      }

      /*********** Run Test  Program Cycle *****************************/
      else if (test_sw)
      {

        switch (test_sw)
        {
        case 1:
          status_led_glow();
          Serial1.println("bnd_test_cycle");
          bnd_test_cycle();
          break;

        case 2:
          status_led_glow();
          Serial1.println("vaccume_test_cycle");
          vaccume_test_cycle();
          break;

        case 3:
          status_led_glow();
          Serial1.println("all_test_prgm_cycle");
          all_test_prgm_cycle();
          break;

        default:
          Serial1.println("Done test prgm................");
          break;
        }
      }
    }
  }

  delay(300);
}

//*******************************  END Program ***********************************************************//
