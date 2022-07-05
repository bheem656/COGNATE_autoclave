#include "BoardConfig.h"
#include <EEPROM.h>

int fullScale = 9630; // max pressure (span) adjust

// variables that required to convert voltage into resistance
float C = -268.55;    ////-319; //-326; //Constant of straight line (Y = mx + C)
float slope = 871.11; // 851//883.34; // Slope of straight line (Y = mx + C)
float R0 = 1000;
float alpha = 0.00385;
extern volatile uint8_t door_status;
extern uint8_t error_status;
extern int8_t process_status;
extern uint8_t prgrm_sw;
extern uint8_t test_sw;
void Timer1_init(void)
{
  // Timer Control - 2 registers:
  TCCR1A = 0x00;  // disable all pin outputs on compare
  TCCR1B = 0x02;  // clk/8   // 3.8 time for 1 sec
  TIMSK1 |= 0x01; // bit 0 - overflow interrupt enable
  TIFR1 |= 0x01;  // clear the overflow flag

  //    TCCR3A = 0x00;  // disable all pin outputs on compare
  //  TCCR3B = 0x03;  // clk/64   // 3.8 time for 1 sec
  //  TIMSK3 |= 0x01; // bit 0 - overflow interrupt enable
  //  TIFR3 |= 0x01;  // clear the overflow flag

  //  TCCR4A = 0x00;  // disable all pin outputs on compare
  //  TCCR4B = 0x02;  // clk/64   // 3.8 time for 1 sec
  //  TIMSK4 |= 0x01; // bit 0 - overflow interrupt enable
  //  TIFR4 |= 0x01;  // clear the overflow flag

  sei(); // enable global interrupts
}

void Beep(uint8_t _duration)
{
  PORTH |= (1 << 6);
  delay(_duration);
  PORTH &= ~(1 << 6);
}
void Beep_Toggle(uint8_t _count, uint16_t _duration)
{
  while (_count)
  {
    PORTH |= (1 << 6);
    delay(_duration); // on time
    PORTH &= ~(1 << 6);
    delay(_duration); // off time
    _count--;
  }
}
void board_init(void)
{

  DDRH |= _BV(DDH6);
  // DDRH |= (1<<6); // BUZZER
  // PORTB |= (1 << 5);
  //  TEMP SENSOR INPUT PORT

  DDRF &= (_BV(DDF4)); // PRESSUR
  DDRF &= (_BV(DDF5)); // TS1
  DDRF &= (_BV(DDF6)); // TS2
  DDRF &= (_BV(DDF7)); // TS3
  DDRK &= (_BV(DDK0)); // TS4

  // SENSOR SWITCHES (WATERx3, DOOR SWITCH)  INPUT PORT
  DDRE &= ~(1 << 4); // INT4
  DDRE &= ~(1 << 5); // SW3
  DDRE &= ~(1 << 6); // SW2
  DDRE &= ~(1 << 7); // SW1

  PORTK |= (1 << 4);
  PORTK |= (1 << 5);
  PORTK |= (1 << 6);
  PORTK |= (1 << 7);

  // INPUT BUTTONS
  // L4,5,6,7 , PK1
  DDRL &= ~(0xD0);  // 1101
  DDRL |= (1 << 5); // L5 AS A OUTPUT
  DDRK &= ~(1 << 1);

  // Enable the pull-up resistor on PL4 using the Port L Data Register (PORTL)
  PORTL |= _BV(PORTL4);
  // Enable the pull-up resistor on PL5 using the Port L Data Register (PORTL)
  //  PORTL |= _BV(PORTL5);
  // Enable the pull-up resistor on PL6 using the Port L Data Register (PORTL)
  PORTL |= _BV(PORTL6);
  PORTL |= _BV(PORTL7);
  PORTK |= _BV(PORTK1);

  // ALL VALVE  OUTPUT PORT
  DDRJ |= _BV(DDJ3);
  DDRJ |= _BV(DDJ4);
  DDRJ |= _BV(DDJ5);
  DDRJ |= _BV(DDJ6);
  // DDRB |= _BV(DDB6);

  // ALL AC OUTPUT PORT PWM
  // H3,4,5 , C4 ,5
  DDRH |= _BV(DDH3);
  DDRH |= _BV(DDH4);
  DDRH |= _BV(DDH5);
  DDRC |= _BV(DDC4);
  DDRC |= _BV(DDC5);

  // ALL LED
  DDRA |= 0xFF;
  DDRK |= 0xFC;
  DDRJ |= _BV(DDJ7);

  PORTA &= ~(0xFF);
  PORTK &= ~(0xFC);
  PORTJ &= ~(1 << 7);

  // PORTE |= _BV(PORTE4);
}

static uint8_t sample = 10;
// static uint8_t sample_time = 5;
static uint32_t curr_time;

float TS1(void)
{

  float V11 = 0;
  float ts11 = 0;
  float Rx11 = 0;
  float temp11 = 0;

  curr_time = millis();
  for (int i = 0; i < sample; i++)
  {
    ts11 += analogRead(rtd2);

    // while (millis() - curr_time <= sample_time)
    // {
    //   /* code */
    // }
    delay(10);
  }

  ts11 /= sample;
  V11 = (ts11 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
                               // // Serial1.print("VOLATAGE : ");
                               // // Serial1.println(V11);
                               //  Rx11 = 1000 * ((2.17 * V11) / (5 - V11)); // 2.18
  Rx11 = V11 * slope + C;      // y=mx+c

  // // Serial1.print("resistance : ");
  // // Serial1.println(Rx11);

  temp11 = ((Rx11 / R0) - 1) / alpha;

  // float C = 88;
  // float slope = 14.187;
  // float R0 = 100.0;
  // float alpha = 0.00385;

  // Rx11 = V11*slope+C; //y=mx+c
  //   // Resistance to Temperature
  //   temp11= (Rx11/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
  //         // Serial1.print("resistance : ");
  // // Serial1.println(Rx1);

  // Uncommect to convet celsius to fehrenheit
  // temp = temp*1.8+32;
  // Serial.println(temp);

  return temp11;
}

float TS2(void)
{

  float V12 = 0;
  float ts12 = 0;
  float Rx12 = 0;
  float temp12 = 0;

  curr_time = millis();
  for (int i = 0; i < sample; i++)
  {
    ts12 += analogRead(rtd3);
    // while (millis() - curr_time <= sample_time)
    // {
    //   /* code */
    // }
    delay(10);
  }

  ts12 /= sample;

  // Serial1.print("ts12 : ");
  // Serial1.println(V12);

  V12 = (ts12 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
  // Serial1.print("VOLATAGE : ");
  // Serial1.println(V12);

  //  Rx12 = 1000 * ((2.19 * V12) / (5 - V12));
  Rx12 = V12 * slope + C; // y=mx+c
  // Serial1.print("resistance : ");
  // Serial1.println(Rx12);

  temp12 = ((Rx12 / R0) - 1) / alpha;

  //     float C = 88;
  // float slope = 14.187;
  // float R0 = 100.0;
  // float alpha = 0.00385;

  // Rx12 = V12*slope+C; //y=mx+c
  //   // Resistance to Temperature
  //   temp12= (Rx12/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
  //         // Serial1.print("resistance : ");
  // // Serial1.println(Rx1);

  // Uncommect to convet celsius to fehrenheit
  // temp = temp*1.8+32;
  // Serial.println(temp);

  return temp12;
}

float TS3(void)
{

  float V1;
  float Rx1;
  float temp1; //, temp;
  float ts1 = 0;

  curr_time = millis();
  for (int i = 0; i < sample; i++)
  {
    ts1 += analogRead(rtd5);
    // while (millis() - curr_time <= sample_time)
    // {
    //   /* code */
    // }

    delay(10);
  }

  ts1 /= sample;
  V1 = (ts1 / 1023.0) * 5.0; // (bits/2^n-1)*Vmax
                             // // Serial1.print("VOLATAGE : ");
                             // // Serial1.println(V1);
                             //  Rx1 = 1000 * ((2.17 * V1) / (5 - V1));
  Rx1 = V1 * slope + C;      // y=mx+c

  // // Serial1.print("resistance : ");
  // // Serial1.println(Rx1);

  temp1 = ((Rx1 / R0) - 1) / alpha;
  // float C = 88;
  // float slope = 14.187;
  // float R0 = 100.0;
  // float alpha = 0.00385;

  // Rx1 = V1*slope+C; //y=mx+c
  //   // Resistance to Temperature
  //   temp= (Rx1/R0-1.0)/alpha; // from Rx = R0(1+alpha*X)
  //         // Serial1.print("resistance : ");
  // // Serial1.println(Rx1);

  // Uncommect to convet celsius to fehrenheit
  // temp = temp*1.8+32;
  // Serial.println(temp);

  return temp1;
}

float mpx(void)
{

  float offset = 127.5;
  curr_time = millis();

  int raw = 0;

  // for (int i = 0; i < sample; i++)
  // {
  //   raw += analogRead(A4);
  //   while (millis() - curr_time <= sample_time)
  //   {
  //     /* code */
  //   }

  //   // delay(5);
  // }

  raw = analogRead(A4); // Reads the MAP sensor raw value on analog port 0

  // raw /= sample;
  float voltage = raw * 0.004887586; // let the compiler determine how many digits it can handle
  float pressure = 0.0;

  //   // Serial1.print("voltage: \t");
  //  // Serial1.println(voltage,  3 );
  // MATH
  if (voltage < 4.6)
  {
    pressure = voltage * 700.0 / 4.6; // it raises 700KPA over 4.6 volt      faster =>   pressure =  voltage * 152.173913;
  }
  else
  {
    pressure = 700 + (voltage - 4.6) * 100 / 0.3; //   last piece raises  100KPa over 0.3 Volt.   // can be optimized to   p = c1 + v * c2
  }

  pressure = pressure - offset;

  //  uint32_t current_time = millis();
  //
  // int  rawValue = 0;
  // int offset = 220; //300; // zero pressure adjust
  //
  //  for (int x = 0; x < 10; x++)
  //  {
  //    // 2ms sampling rate or 500 hz
  //    while(millis() - current_time < 2)
  //    {
  //      }
  //    rawValue = rawValue + analogRead(A4);
  //    }
  //  rawValue = rawValue/10;
  //  volatile float prr = (rawValue - offset) * 700.0 / (fullScale - offset); // pressure conversion

  //
  //  // Serial1.print("Raw A/D is  ");
  //  // Serial1.print(rawValue);
  //  // Serial1.print("   Pressure is  ");
  //  // Serial1.print(prr, 1); // one decimal places
  //  // Serial1.println("  kPa");

  return pressure;
}
//
void status_led_glow()
{
  if (prgrm_sw)
  {

    if (prgrm_sw == 1)
    {
      PORTA |= _BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTA &= ~_BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);

      PORTJ |= _BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
    else if (prgrm_sw == 2)
    {
      PORTA &= ~_BV(unwrapped_led);
      PORTA |= _BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTA &= ~_BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);

      PORTJ |= _BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
    else if (prgrm_sw == 3)
    {
      PORTA &= ~_BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA |= _BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTA &= ~_BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);

      PORTJ |= _BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
    else if (prgrm_sw == 4)
    {
      PORTA &= ~_BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA |= _BV(porous_led);

      PORTA &= ~_BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);
      //
      PORTJ &= ~_BV(t134_led);
      PORTK |= _BV(t121_led);
    }
    else if (prgrm_sw == 5)
    {

      PORTA |= _BV(unwrapped_led);
      PORTA |= _BV(wrapped_led);
      PORTA |= _BV(prion_led);
      PORTA |= _BV(porous_led);

      PORTA &= ~_BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);

      PORTJ |= _BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
  }

  if (test_sw)
  {
    //  prgrm_sw = 0;

    if (test_sw == 1)
    {
      PORTA |= _BV(bnd_s__led);
      PORTK &= ~_BV(vacuum_s_led);

      PORTA &= ~_BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTJ |= _BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
    else if (test_sw == 2)
    {
      PORTA &= ~_BV(bnd_s__led);
      PORTK |= _BV(vacuum_s_led);

      PORTA &= ~_BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTJ &= ~_BV(t134_led);
      PORTK |= _BV(t121_led);
    }

    else if (test_sw == 3)
    {
      PORTA |= _BV(bnd_s__led);
      PORTK |= _BV(vacuum_s_led);

      PORTA &= ~_BV(unwrapped_led);
      PORTA &= ~_BV(wrapped_led);
      PORTA &= ~_BV(prion_led);
      PORTA &= ~_BV(porous_led);

      PORTJ &= ~_BV(t134_led);
      PORTK &= ~_BV(t121_led);
    }
  }
}

void start_process_led_glow()
{
  PORTA |= _BV(start_led);
  PORTK &= ~_BV(vacuum_led);
  PORTK &= ~_BV(serilize_led);
  PORTK &= ~_BV(dry_led);
  PORTK &= ~_BV(end_led);
}

void vaccume_process_led_glow()
{
  PORTA &= ~_BV(start_led);
  PORTK |= _BV(vacuum_led);
  PORTK &= ~_BV(serilize_led);
  PORTK &= ~_BV(dry_led);
  PORTK &= ~_BV(end_led);
}

void sterilize_process_led_glow()
{
  PORTA &= ~_BV(start_led);
  PORTK &= ~_BV(vacuum_led);
  PORTK |= _BV(serilize_led);
  PORTK &= ~_BV(dry_led);
  PORTK &= ~_BV(end_led);
}

void dry_process_led_glow()
{
  PORTA &= ~_BV(start_led);
  PORTK &= ~_BV(vacuum_led);
  PORTK &= ~_BV(serilize_led);
  PORTK |= _BV(dry_led);
  PORTK &= ~_BV(end_led);
}

void end_process_led_glow()
{

  PORTA &= ~_BV(start_led);
  PORTK &= ~_BV(vacuum_led);
  PORTK &= ~_BV(serilize_led);
  PORTK &= ~_BV(dry_led);
  PORTK |= _BV(end_led);
}

//
// void get_btn_status()
//{
//
//  if (!(PINL & (1 << 4)))
//  {
//
//    //  // Serial1.print("PRGM BTN PRESSED :  ");
//    //  // Serial1.println(prgrm_sw);
//    if (prgrm_sw >= 5)
//    {
//      prgrm_sw = 1;
//    }
//
//    else
//    {
//      prgrm_sw++;
//    }
//    test_sw = 0;
//    delay(200);
//  }
//
//  /********** Test program btn SWC *********************/
//
//  if (!(PINL & (1 << 6)))
//  {
//
//    prgrm_sw = 0;
//    if (test_sw >= 4)
//    {
//      test_sw = 1;
//    }
//
//    else
//    {
//      test_sw++;
//    }
//
//    delay(200);
//
//    Serial1.print("test PRGM BTN PRESSED :  ");
//    Serial1.println(test_sw);
//  }
//}

uint8_t error_list[20][5] = {
    "Er01",
    "Er02",
    "Er03",
    "Er04",
    "Er05",
    "Er06",
    "Er07",
    "Er08",
    "Er09",
    "Er10",
    "Er12",
    "Er14",
    "Er98",
    "Er99"

};

uint8_t error_details[20][100] = {
    "Steam Generator over temperature",             // STG TEMP > 220 ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er01
    "Heating Ring over temperature",                // 150 -- ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er02
    "Chamber over temperature",                     // 136-- ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er03
    "Fail to maintain temperature and pressure",    //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er04
    "Pressure not exhausted",                       //   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er05
    "Door open during cycle",                       //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er06
    "Working overtime",                             //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er07
    "Over Pressure",                                // 230   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er08
    "In-chamber sensors temp. too high or too low", // High=140, Low= 100   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er09
    "Temp. and Pressure doesn't match",             //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er10
    "Vacuum fail",                                  // (UA)vaccum cycle pressure not change  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er12
    "In-chamber sensors temp. differs too much",    //---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er14
    "Out of power during cycle",                    // power shut down during cycle  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er98
    "Forced exit"                                   // force fully cycle close  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er99
};

extern volatile uint8_t RS;
extern uint8_t current_cycle;
extern float steam_generator_temp;
extern float outer_body_temp;
extern float chamber_temp;
extern float pressure;

uint8_t over_steam_generator_temp = 250;
uint8_t over_outer_body_temp = 200;
uint8_t over_chamber_temp = 140;
uint8_t over_pressure = 230;

uint8_t InChamberTemp_High = 140;
uint8_t InChamberTemp_Low = 100;

extern volatile uint8_t door_status;

uint32_t total_time_cycle;
uint32_t overtime_total_time_cycle = 10000;

uint8_t power_failure = 0;

void Check_Error()
{
  door_status = digitalRead(48);

  // door_current_status_print();
  pressure = mpx();
  outer_body_temp = TS1();
  chamber_temp = TS2();
  steam_generator_temp = TS3();

  // Staem Generator over temperature
  if (steam_generator_temp > over_steam_generator_temp)
  {
    EEPROM.update(0, 0);
    error_status = 1;

    while (digitalRead(48))
    {
      print_code(0, 1);
      RS = 0;
    }
  }
  // Heating Ring over temperature
  if (outer_body_temp > over_outer_body_temp)
  {
    EEPROM.update(0, 0);
    error_status = 1;

    while (digitalRead(48))
    {
      print_code(0, 2);
      RS = 0;
    }
  }
  // Chamber over temperature
  if (chamber_temp > over_chamber_temp)
  {
    EEPROM.update(0, 0);
    error_status = 1;
    while (digitalRead(48))
    {
      print_code(0, 3);
      RS = 0;
    }
  }
  // Fail to maintain temperature and pressure
  //    if (steam_generator_temp > max_steam_generator_temp)
  //     {

  //         print_code( 0, 1);
  //         RS = 0;

  //     }
  // Pressure not exhausted
  //    if (steam_generator_temp > max_steam_generator_temp)
  //     {

  //         print_code( 0, 1);
  //         RS = 0;

  //     }
  //  Door open during cycle

  if (door_status == 0 && RS == 1)
  {
   EEPROM.update(0, 0);
    // TRUN OFF ALL AC
    PORTJ |= _BV(v2);
    PORTJ &= ~_BV(v3);
    PORTH &= ~_BV(vac);
    PORTH &= ~_BV(motor);
    PORTC &= ~_BV(steam);
    PORTC &= ~_BV(heat);
    Serial1.println("er06 all ac off");
    error_status = 1;
    while (!door_status)
    {
      beep_2();
      door_status = digitalRead(48);
      Serial1.println("waiting to close door");
      print_code(0, 6);
      // RS = 0;
    }
    // while (RS==0)
    // {
    //    RS = 0;
    // }
  }

  // Working overtime
  if (total_time_cycle > overtime_total_time_cycle)
  {
    EEPROM.update(0, 0);
    error_status = 1;

    while (digitalRead(48))
    {
      print_code(0, 7);
      RS = 0;
    }
  }
  // Over Pressure
  if (pressure > over_pressure)
  {
    EEPROM.update(0, 0);
    error_status = 1;
    while (digitalRead(48))
    {
      print_code(0, 8);
      RS = 0;
    }
  }
  // In-chamber sensors temp. too high or too low
  // if (chamber_temp > InChamberTemp_High || chamber_temp > InChamberTemp_Low)
  // {
  //   EEPROM.update(0, 0);
  //   error_status = 1;
  //   while (digitalRead(48))
  //   {
  //     print_code(0, 9);
  //     RS = 0;
  //   }
  // }

  // Temp. and Pressure doesn't match

  //    if (steam_generator_temp > max_steam_generator_temp)
  //     {

  //         print_code( 0, 1);
  //         RS = 0;

  //     }

  // Vacuum fail

  //    if (steam_generator_temp > max_steam_generator_temp)
  //     {

  //         print_code( 0, 1);
  //         RS = 0;

  //     }

  // In-chamber sensors temp. differs too much

  //    if (steam_generator_temp > max_steam_generator_temp)
  //     {

  //         print_code( 0, 1);
  //         RS = 0;

  //     }

  // Out of power during cycle

  if (steam_generator_temp > over_steam_generator_temp)
  {
    Serial1.print("steam_generator_temp");
    Serial1.println(steam_generator_temp);
    EEPROM.update(0, 0);
    error_status = 1;

    while (digitalRead(48))
    {
      print_code(0, 1);
      RS = 0;
    }
  }
  // Forced exit

  if ((RS == 0) && (process_status == 99))
  {
    EEPROM.update(0, 0);
    error_status = 1;
    print_code(9, 9);
    while (digitalRead(48))
    {
      print_code(0, 1);

      RS = 0;
    }
    process_status = 1;
  }
}

void door_current_status_print()
{
  door_status = digitalRead(48);

  if (door_status)
  {
    // beep_2();
    // Serial1.println("door close");
    // Serial1.println(door_status);
    print_load();
      
     
  }
  else
  {
    // Serial1.println("door open");
    print_load_blink();
    //  Beep_Toggle(1000);
  }
}