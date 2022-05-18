#include <Sodaq_PcInt.h>
#include "BoardConfig.h"
#include "max.h"

/************** Global Variable  ********************/

uint8_t dev = 2;

int8_t process_status;
uint32_t cuurent_time;
uint32_t last_time;
volatile uint8_t RS;

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
  if ((interrupt_time - last_interrupt_time) > 700)
  {
    Serial1.println("............A000000000000000000000000000000000 .............");

    RS = !RS;
  }
  last_interrupt_time = interrupt_time;
}

/************** Timer1 ISR  ********************/
volatile float _temp, _pres;
ISR(TIMER1_OVF_vect)
{

  count++;
  if (count >= 40)
  {
    count = 0;
    _pres = mpx();
    _temp = TS2();
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

void setup()
{
  Serial1.begin(9600);
  PcInt::attachInterrupt(63, handleA0);

  // attachInterrupt(digitalPinToInterrupt(2), ISR_door, CHANGE);        // door sensor
  attachInterrupt(digitalPinToInterrupt(3), ISR_water_fresh, CHANGE); // water sensor
  board_init();
  Disp_board_config();
  Timer1_init();
  //  MAX7219_Clear(1);
  //  MAX7219_Clear(2);
   print_load();

  PORTH |= _BV(fan);

  // PORTJ |= _BV(v2);

}

void loop()
{

// delay(1000);
// if (!(PINE & (1 << 4)))
// {

// door_status = 1;
// Serial1.print("..................."); // get from intrupt
//   // Serial1.println(door_status);


// }

// else
// {
// door_status = 1;

// }


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


  Serial1.println("............................................................"); // get from intrupt

  Serial1.print("Door staus   --->  1 - open :: 2 - close ::::"); // get from intrupt
  Serial1.println(door_status);

  Serial1.print("Drain staus  --->  1 - full :: 0 - empty :::::"); // get from polling
  Serial1.println(drain);

  Serial1.print("Fresh staus  --->  0 - full  :: 1 - empty :::::"); // get from intrupt
  Serial1.println(fresh);

  Serial1.println("--------------------------------------");

  /****************   Drain Status ********************/
  if (!(PINE & (1 << 6)))
  {
    PORTA |= _BV(drain_status_led);
    drain = 1;
    Serial1.print("Drain Status :  ");
    Serial1.println(drain);
  }

  else
  {
    PORTA &= ~_BV(drain_status_led);
    drain = 0;
    Serial1.print("Drain Status :");
    Serial1.println(drain);
  }

  /****************   program button input switch  ********************/
  if (!(PINL & (1 << 4))) // if (! digitalRead(45)) //  if (!(PINL & (1 << 4)))
  {

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

  /********** Switches current status *********************/

  Serial1.print("RS Status : ");
  Serial1.println(RS);

  Serial1.print("cycle Program  switch : ");
  Serial1.println(prgrm_sw);

  Serial1.print("test Program  Status : ");
  Serial1.println(test_sw);

  Serial1.print("start_sw Status : ");
  Serial1.println(start_sw);

  // RS =1 ;
  if (RS)
  {

    delay(300);

    if ((drain == 0) && (fresh == 0)) // && (door_status == 1)
    {
      RS = 1;
      Serial1.println("TRIGGER ...................");
    }
    else
    {
      RS = 0;
      Serial1.println("PLEASE CLOSE THE DOOR");
    }
    /*************
     * Get Temp & Pressure to  skip pre heating cycle *******
     * ********/

    tmp4 = TS1();
    tmp3 = TS3();

    if (tmp3 > 150 && tmp4 > 80)
    {
      process_status = 2;
    }

    else
    {

      process_status = 1;
    }

    /************************  IF RS = 1 start running cycle ******************************/
    // process_status = 1;
    // prgrm_sw = 1;
    RS = 1;
    while (RS)
    {

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
          // prion_cycle();
          break;

        case 4:
        status_led_glow();
          Serial1.println("porous_cycle");
          // porous_cycle();
          break;

        case 5:
        status_led_glow();
          Serial1.println("all_prgm_cycle");
          // all_prgm_cycle();
          break;

        default:
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
          // vaccume_test_cycle();
          break;

        case 3:
        status_led_glow();
          Serial1.println("all_test_prgm_cycle");
          // all_test_prgm_cycle();
          break;

        default:
          break;
        }
      }
    }
  }

  delay(300);
}

/* check start stop */
// if (!(PINL & (1 << 5)))
// {
//   RS = 0;
//   delay(300);
//   break;
// }
/* check pressure and temp */
// get_sensor_data(); // print Data

/* pre heating   -- HEAT , STEAM */

/* ##################################### PRE HEATING #########################################################*/
/* ##################################### HE #########################################################*/
/********  HEAT , STEAM START WITH 00 END WITH 6:40 **********/
//   unwrapped_cycle();

///********** Switches current status end*********************/
//
///********** Switches current status end*********************/
//
// Serial1.print("RS Status : ");
// Serial1.println(RS);
//
//
// // Serial1.print("cycle Program  Status : ");
// // Serial1.println(prgrm_sw);
//
// // Serial1.print("program running Status : ");
// // Serial1.println(start_sw);
//
// // Serial1.println("...................................");
// RS = 1;

//  DDRE &= ~_BV(DDE4);
//  DDRE &= ~_BV(DDE5);
//
//  PORTE |= _BV(PORTE4);
//  PORTE |= _BV(PORTE5);
//
////  EICRA |= _BV(ISC40); //  0 1 Any logical change on INT0 generates an interrupt request.
////  EICRA |= _BV(ISC50); //  0 1 Any logical change on INT1 generates an interrupt request.
//
//  EIMSK |= _BV(INT4); // INT0 Enable
//  EIMSK |= _BV(INT5); // INT1 Enable

// /********** cycle program status / start button *********************/
// if (!(PINK & (1 << 1)))
// {
//   //  if(runningStatus)
//   //    runningStatus = 0;
//   //  else

//   if (!(PINE & (1 << 7)) && (fresh == 0) && (drain == 0))
//   {

//     RS = 1;
//     Serial1.println("............Door Closed .............");
//   }

//   else
//   {

//     Serial1.println("..........Please close the Door first  and check water status...........");
//   }

//   Serial1.print("Program Status : ");
//   Serial1.println(RS);
//   delay(200);
// }

// ISR(INT5_vect)
//{
//   PORTA |= _BV(water_status_led);
////  static unsigned long last_interrupt_time = 0;
////  unsigned long interrupt_time = millis();
////  // noInterrupts();
////  // If interrupts come faster than 200ms, assume it's a bounce and ignore
////  if ((interrupt_time - last_interrupt_time) > timeout) // && (int0_flag == 0
////  {
////    t0 = millis();
////  }
////  last_interrupt_time = interrupt_time;
//}

// ISR(INT4_vect)
//{
//
//  PORTJ |= _BV(t134_led);
////  static unsigned long last_interrupt_time = 0;
////  unsigned long interrupt_time = millis();
////  // noInterrupts();
////  // If interrupts come faster than 200ms, assume it's a bounce and ignore
////  if ((interrupt_time - last_interrupt_time) > timeout) // && (int0_flag == 0
////  {
////    t0 = millis();
////  }
////  last_interrupt_time = interrupt_time;
//}

/********** Glow LED *********************/
// if (prgrm_sw)
// {

//   //  test_sw = 0;
//   //    status_led_glow();

//   if (prgrm_sw == 1)
//   {
//     PORTA |= _BV(unwrapped_led);
//     PORTA &= ~_BV(wrapped_led);
//     PORTA &= ~_BV(prion_led);
//     PORTA &= ~_BV(porous_led);

//     PORTA &= ~_BV(bnd_s__led);
//     PORTK &= ~_BV(vacuum_s_led);

//     PORTJ |= _BV(t134_led);
//     PORTK &= ~_BV(t121_led);
//   }
//   else if (prgrm_sw == 2)
//   {
//     PORTA &= ~_BV(unwrapped_led);
//     PORTA |= _BV(wrapped_led);
//     PORTA &= ~_BV(prion_led);
//     PORTA &= ~_BV(porous_led);

//     PORTA &= ~_BV(bnd_s__led);
//     PORTK &= ~_BV(vacuum_s_led);

//     PORTJ |= _BV(t134_led);
//     PORTK &= ~_BV(t121_led);
//   }
//   else if (prgrm_sw == 3)
//   {
//     PORTA &= ~_BV(unwrapped_led);
//     PORTA &= ~_BV(wrapped_led);
//     PORTA |= _BV(prion_led);
//     PORTA &= ~_BV(porous_led);

//     PORTA &= ~_BV(bnd_s__led);
//     PORTK &= ~_BV(vacuum_s_led);

//     PORTJ |= _BV(t134_led);
//     PORTK &= ~_BV(t121_led);
//   }
//   else if (prgrm_sw == 4)
//   {
//     PORTA &= ~_BV(unwrapped_led);
//     PORTA &= ~_BV(wrapped_led);
//     PORTA &= ~_BV(prion_led);
//     PORTA |= _BV(porous_led);

//     PORTA &= ~_BV(bnd_s__led);
//     PORTK &= ~_BV(vacuum_s_led);
//     //
//     PORTJ &= ~_BV(t134_led);
//     PORTK |= _BV(t121_led);
//   }
//   else if (prgrm_sw == 5)
//   {

//     PORTA |= _BV(unwrapped_led);
//     PORTA |= _BV(wrapped_led);
//     PORTA |= _BV(prion_led);
//     PORTA |= _BV(porous_led);

//     PORTA &= ~_BV(bnd_s__led);
//     PORTK &= ~_BV(vacuum_s_led);

//     PORTJ |= _BV(t134_led);
//     PORTK &= ~_BV(t121_led);
//   }
// }
