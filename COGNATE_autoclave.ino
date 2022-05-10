
#include "BoardConfig.h"
#include "max.h"

// #include <SPI.h>

uint8_t test_sw = 0;
uint8_t process_status = 0;
uint8_t prgrm_sw = 0;
bool start_sw = 0;
volatile uint8_t RS = 0;

uint32_t cuurent_time = 0;
uint32_t clock_running = 0;
uint8_t temp = 0;

float pres;
float tmp4;
float tmp2;
float tmp3;

volatile uint8_t int_flag = 1;
volatile int count;

uint32_t last_time;
uint8_t err_time;
/************** Global Variable  ********************/

ISR(TIMER1_OVF_vect)
{

  count++;
  if (count >= 40)
  {
    count = 0;
    //  MAX7219_Clear(1);

    //  temp++;
    float pres = mpx();
    lcd2_press(pres);

    float temp = TS2();
    lcd1_temp(temp);

    int_flag = 1;
    TIFR1 |= 0x01;
  }
  TIFR1 |= 0x01; // clear the interrupt flag
}

float get_sensor_data()
{
  pres = mpx();
  Serial1.print("pressure : ");
  Serial1.print(pres);
  Serial1.println(" kPa");

  tmp4 = TS1();
  Serial1.print("Outer Body :");
  Serial1.println(tmp4);

  tmp2 = TS2();
  Serial1.print("Chamber : ");
  Serial1.println(tmp2);

  tmp3 = TS3();
  Serial1.print("Steam Generator : ");
  Serial1.println(tmp3);

  return pres;
}

void print_load()
{
  set_char(1, 68, 1, false); // 68
  set_char(2, 65, 1, false); // 65
  set_char(3, 79, 1, false); // 79
  set_char(4, 76, 1, false); // 76
  // MAX7219_Clear(2);
}

void setup()
{
  Serial1.begin(9600);
  board_init();
  Timer1_init();
  Disp_board_config();

  ///* Radiator Fan On */
  PORTH |= _BV(fan);
  ///* Heat Ring  On */
  //   PORTC |= _BV(heat);/

  MAX7219_Clear(1);
  MAX7219_Clear(2);

  // PORTA |= _BV(start_led);
  // delay(3000);
  // PORTK |= _BV(vacuum_led);
  // delay(3000);
  // PORTK |= _BV(serilize_led);
  // delay(3000);
  // PORTK |= _BV(dry_led);
  // delay(3000);
  // PORTK |= _BV(end_led);

  // float t = -17.17;
  // lcd1_temp(t);
  //   MAX7219_Clear(1);
  // float p = 281.17;
  // lcd2_press(p);
  print_load();
  // PORTJ |= _BV(v3);  // 4th valve
  // PORTJ |= _BV(v4);  // 4th valve
  //  PORTH |= _BV(vac);

  //  delay(60000);
  //   delay(60000);
  //    delay(60000);
  //     delay(60000);
}

void loop()
{

  uint8_t fresh = 0, drain = 0;
  uint8_t door_status = 0;
  // get_sensor_data(); // print Data

  /*************************************************************************************************************/
  /********** water drain status *********************/
  if (PINE & (1 << 5))
  {
    //  Serial1.println("lower sensor WATER empty");
    PORTA |= _BV(water_status_led);
    fresh = 1;
  }
  else
  {
    PORTA &= ~_BV(water_status_led);
    fresh = 0;
  }

  if (!(PINE & (1 << 6)))
  {
    PORTA |= _BV(drain_status_led);
    drain = 1;
  }

  else
  {
    PORTA &= ~_BV(drain_status_led);
    drain = 0;
  }
  /********** Switches current status*********************/

  // delay(500);
  // MAX7219_Clear(2);
  // delay(100);
  /********** Check program btn SWA*********************/
  if (!RS)
  {

    if (!(PINL & (1 << 4)))
    {

      //  Serial1.print("PRGM BTN PRESSED :  ");
      //  Serial1.println(prgrm_sw);
      if (prgrm_sw >= 5)
      {
        prgrm_sw = 1;
      }

      else
      {
        prgrm_sw++;
      }
      test_sw = 0;
      delay(100);
    }

    /********** Test program btn SWC *********************/

    if (!(PINL & (1 << 6)))
    {

      prgrm_sw = 0;
      if (test_sw >= 4)
      {
        test_sw = 1;
      }

      else
      {
        test_sw++;
      }

      delay(100);

      //      Serial1.print("test PRGM BTN PRESSED :  ");
      //  Serial1.println(test_sw);
    }
  }

  /********** cycle program status / start button *********************/
  if (!(PINL & (1 << 5)))
  {
    //  if(runningStatus)
    //    runningStatus = 0;
    //  else

    if (!(PINE & (1 << 7)) && (fresh == 0) && (drain == 0))
    {

      RS = 1;
      Serial1.println("............Door Closed .............");
    }

    else
    {

      Serial1.println("..........Please close the Door first  and check water status...........");
    }

    Serial1.print("Program Status : ");
    Serial1.println(RS);
    delay(200);
  }

  if (prgrm_sw)
  {

    //  test_sw = 0;

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
      PORTK &= ~_BV(t121_led);
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
  ///********** Switches current status end*********************/
  //
  ///********** Switches current status end*********************/
  //
  Serial1.print("RS Status : ");
  Serial1.println(RS);
  //
  //
  // Serial1.print("cycle Program  Status : ");
  // Serial1.println(prgrm_sw);
  //
  // Serial1.print("program running Status : ");
  // Serial1.println(start_sw);
  //
  // Serial1.println("...................................");
  RS = 1;
  if (RS)
  {
    Serial1.print("RS Status ....: ");
    Serial1.println(RS);
    delay(300);

         tmp4 = TS1();
      tmp3 = TS3();

      if (tmp3 > 180 && tmp4 > 100)
      {
        process_status = 2;

      }

      else{

        process_status =1;
      }

    // process_status = 1;
    while (RS)
    {

      /* check start stop */
      if (!(PINL & (1 << 5)))
      {
        RS = 0;
        delay(300);
        break;
      }

      /* check pressure and temp */
      // get_sensor_data(); // print Data

      /* pre heating   -- HEAT , STEAM */

      /* ##################################### PRE HEATING #########################################################*/
      /* ##################################### HE #########################################################*/
      /********  HEAT , STEAM START WITH 00 END WITH 6:40 **********/

      if (process_status == 1)
      {

        

        PORTA |= _BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        float chamber_pressure;

        last_time = millis();
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 400000) // 400000
        {
           
        PORTA |= _BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...HE...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();

          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          //  if (tmp3 > 180 && tmp4 > 100)
          //  {
          //    process_status = 2;
          //    break;
          //  }

          // chamber_pressure = get_sensor_data(); // print Data
          // uint32_t last_time =
          clock_running = millis() - cuurent_time;

          // if( (millis() - last_time)  >= 5000)
          // {
          //   err_time = 1;
          //   last_time = millis();

          // }

          // else
          // {

          //   err_time = 0;
          // }

          // if(!err_time)
          // {

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          uint16_t clk = clock_running / 60000;
          // Serial1.print(clk); // minutes

          // MAX7219_Clear(1);
          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);
          // MAX7219_Clear(2);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);
          //
          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);
          MAX7219_Clear(2);

          // }

          // else

          // {

          //     set_char(1, 16, 1, false); //
          //     set_char(2, 14, 1, false); // 68 e
          //     set_char(3, 72, 1, false); // 72 h
          //     set_char(4, 16, 1, false); // 76
          //     MAX7219_Clear(2);

          // }

          /* Steam Generatoe */
          if (tmp3 >= 200)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }
        }

        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }
        process_status = 2;
      }
      /* ##################################### 1st vaccume #########################################################*/
      /* ##################################### UA1 #########################################################*/
      /********  V4 + VACCUME PUMP + STRAT FROM 3:40 + END AT -80 KPA **********/
      if (process_status == 2)
      {
        PORTJ &= ~_BV(v3); // 4th valve
        PORTJ &= ~_BV(v4); // 4th valve

        cuurent_time = millis();
        /* AC compnents status */
        PORTC &= ~_BV(heat);
        PORTC &= ~_BV(steam);

        /* LED  status */
        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        /*  buzzer */

        uint32_t ct = 0;

        // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 220000) // 220000 :: 3:40
        {
                   PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);
          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...UA1...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* Get pressure and Temperature */
          // chamber_pressure = get_sensor_data(); // print Data

          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /********** BREAK CONDITION ***************/
          if (pres < -80)
          {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            process_status = 3;
            break;
          }

          /* Steam Generatoe */
          if (tmp3 >= 200)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp3 >= 120)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          /* clock running */
          clock_running = millis() - cuurent_time;
          ct = 220000 - clock_running;
          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(ct / 60000); // minutes

          uint16_t clk = ct / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          ct = ct % 60000;
          ct = ct / 1000;
          Serial1.println(ct, 1);

          uint8_t s1 = ct / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          // Serial1.println(".....HE........");
          /* VA! cycle */
          PORTH |= _BV(vac); // vaccume pump on
          PORTJ |= _BV(v4);  // 4th valve
          // PORTJ |= _BV(v1);  // 4th valve
          // PORTJ |= _BV(v2);  // 4th valve
          // PORTJ |= _BV(v3);  // 4th valve
        }

        PORTH &= ~_BV(vac);
        PORTJ &= ~_BV(v4);
        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }

        process_status = 3;
      }

      /* Raise pressure */
      /* ##################################### PR1 #########################################################*/
      /* ##################################### PR1 #########################################################*/
      /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA **********/

      if (process_status == 3)
      {

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 445000) // 445000 // 7:25
        {

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...PR1...");
          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;
          /* Get pressure and Temperature */
          // chamber_pressure = get_sensor_data(); // print Data

          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /* Steam Generatoe */
          // PORTC |= _BV(steam);
          /* Heat Ring */
          if (tmp3 >= 180)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          if (pres > 150)
          {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            process_status = 4;
            break;
          }

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(clock_running / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          /* Get pressure and Temperature */
          // get_sensor_data(); // print Data
          // PORTJ &= ~_BV(v4);   // 1ST

          PORTJ |= _BV(v3);
          PORTJ |= _BV(v1); // 1ST

          PORTH |= _BV(motor); // 1ST // motor
          delay(50);
          PORTH &= ~_BV(motor); // 1ST // motor

          delay(2000);
          // PORTC |= _BV(PORTC4); // 4TH  // steam

          // PORTJ |= _BV(v3);
          // PORTJ |= _BV(v1);    // 1ST

          // PORTH |= _BV(motor); // 1ST // motor
          // delay(100);
          // PORTH &= ~_BV(motor); // 1ST // motor

          // delay(3000);
          // PORTC |= _BV(PORTC4); // 4TH  // steam
          // // PORTC |= _BV(PORTC4); // 4TH  // steam
        }

        PORTJ &= ~_BV(v3);
        PORTJ &= ~_BV(v1);    // 1ST
        PORTH &= ~_BV(motor); // 1ST // motor

        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }
        process_status = 4;
        // PORTC &= ~_BV(PORTC4); // 4TH  // steam
      }
      /* Exaust */
      /* ##################################### RE1 #########################################################*/
      /* ##################################### RE1 #########################################################*/
      /********  V2 +  STRAT FROM 00:00 + END AT 45 SEC **********/

      if (process_status == 4)
      {

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        /* Get pressure and Temperature */
        // chamber_pressure = get_sensor_data(); // print Data
        cuurent_time = millis();
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 45000) // 45sec  45000
        {

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...RE1...");
          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;

          pres = mpx();
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /* Steam Generatoe */
          if (tmp3 >= 150)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }
          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(clock_running / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          /* Get pressure and Temperature */
          // get_sensor_data(); // print Data
          // PORTJ &= ~_BV(v4);   // 1ST
          PORTJ |= _BV(v2);
        }
        PORTJ &= ~_BV(v2);

        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }
        process_status = 5;
      }

      /* Raise pressure */

      /* ##################################### PR2 #########################################################*/
      /* ##################################### RR2 #########################################################*/
      /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA **********/

      if (process_status == 5)
      {

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 445000) // 445000 // 7:25
        {

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...PR2...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;
          /* Get pressure and Temperature */
          // chamber_pressure = get_sensor_data(); // print Data

          pres = mpx();

          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /* Steam Generatoe */
          // PORTC |= _BV(steam);
          // if (tmp2 >= 70)
          // {

          //   PORTC &= ~_BV(steam);
          // }
          // else
          // {
          //   PORTC |= _BV(steam);
          // }

          /* Heat Ring */
          // if (tmp3 >= 68)
          // {

          //   PORTC &= ~_BV(heat);
          // }
          // else
          // {
          //   PORTC |= _BV(heat);
          // }

          if (tmp3 >= 180)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 134)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          if (pres > 217)
          {

            PORTJ &= ~_BV(v1); // 1ST
            // PORTJ &= ~_BV(v3);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            process_status = 6;
            break;
          }

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(clock_running / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          /* Get pressure and Temperature */
          // get_sensor_data(); // print Data
          // PORTJ &= ~_BV(v4);   // 1ST

          PORTJ |= _BV(v3);
          PORTJ |= _BV(v1); // 1ST

          PORTH |= _BV(motor); // 1ST // motor
          delay(50);
          PORTH &= ~_BV(motor); // 1ST // motor

          delay(2000);
          // PORTC |= _BV(PORTC4); // 4TH  // steam
        }

        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }
        PORTJ &= ~_BV(v1);
        process_status = 6;
      }
      /* Sterillization  */
      /* ##################################### &&&&&&&&&&&&&&& ST &&&&&&&&&&&&&&& #########################################################*/
      /* ##################################### &&&&&&&&&&&&&&& ST &&&&&&&&&&&&&&& #########################################################*/
      /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 04:00 + END AT 00:00 with maintain pressure of 218 kpa **********/

      if (process_status == 6)
      {

        PORTA &= ~_BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK |= _BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 240000) // 240000 // 7:25
        {

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...ST...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;
          /* Get pressure and Temperature */
          // chamber_pressure = get_sensor_data(); // print Data

          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /* Steam Generatoe */
          if (tmp3 >= 150)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          if (pres < 218)
          {

            PORTH |= _BV(motor); // 1ST // motor
            delay(70);
            PORTH &= ~_BV(motor); // 1ST // motor
            delay(4000);
          }

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(clock_running / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          PORTJ |= _BV(v3);
          PORTJ |= _BV(v1); // 1ST
        }
        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }

        PORTJ &= ~_BV(v1);
        process_status = 7;
      }
      /* Exaust */
      /* ##################################### RE #########################################################*/
      /* ##################################### RE #########################################################*/
      /********  V2 +  STRAT FROM 00:00 + END AT 45 SEC **********/

      else if (process_status == 7)
      {

        PORTA &= ~_BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK |= _BV(dry_led);
        PORTK &= ~_BV(end_led);

        /* Get pressure and Temperature */
        // chamber_pressure = get_sensor_data(); // print Data

        cuurent_time = millis();
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 45000) // 45sec  45000
        {

          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...RE...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;

          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          if (tmp3 >= 150)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(clock_running / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = clock_running % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          /* Get pressure and Temperature */
          // get_sensor_data(); // print Data
          // PORTJ &= ~_BV(v4);   // 1ST
          PORTJ |= _BV(v2);
        }
        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }

        PORTJ &= ~_BV(v1);

        process_status = 8;
      }
      /* Drying */
      /* ##################################### DR #########################################################*/
      /* ##################################### DR #########################################################*/
      /********  V2 +  STRAT FROM 00:00 + END AT 45 SEC **********/
      /*
      9:00 - v4+vacc
      --- 1:40 ----  #100000
      7:20 - v4+vacc+v2
       --- 0:30 ----#30000
      6:50 - v4+vacc
      --- 2:20---- #140000
      4:30 - v4+vacc+v2
      --- 00:30 ----#30000
      4:00 - v4+vacc
      --- 2:30 ----#150000
      1:30 - v4+vacc+v2
      --- 1:30 ----#90000

      */

      if (process_status == 8)
      {

        PORTJ &= ~_BV(v1); //
        PORTJ &= ~_BV(v2); //
        PORTJ &= ~_BV(v3); //
        PORTJ &= ~_BV(v4); //

        PORTA &= ~_BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK |= _BV(end_led);

        uint32_t ct = 0;

        cuurent_time = millis();
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 540000) // 9min  540000
        {
          Serial1.print("current process running :");
          Serial1.print(process_status);
          Serial1.println("...DR...");

          if (!(PINL & (1 << 5)))
          {
            RS = 0;
            delay(300);
            break;
          }
          /* clock running */
          clock_running = millis() - cuurent_time;
          ct = -clock_running;
          // Serial1.print("CT value.....");
          // Serial1.println(ct);

          pres = mpx();
          // lcd2_press(pres);
          Serial1.print("pressure : ");
          Serial1.print(pres);
          Serial1.println(" kPa");

          tmp4 = TS1();
          Serial1.print("Outer Body :");
          Serial1.println(tmp4);

          tmp2 = TS2();
          Serial1.print("Chamber : ");
          Serial1.println(tmp2);

          tmp3 = TS3();
          Serial1.print("Steam Generator : ");
          Serial1.println(tmp3);

          /* Steam Generatoe */
          if (tmp3 >= 150)
          {

            PORTC &= ~_BV(steam);
          }
          else
          {
            PORTC |= _BV(steam);
          }

          /* Heat Ring */
          if (tmp2 >= 100)
          {

            PORTC &= ~_BV(heat);
          }
          else
          {
            PORTC |= _BV(heat);
          }

          Serial1.print("....................... ");
          Serial1.print("Time : ");
          Serial1.print(ct / 60000); // minutes
          uint16_t clk = clock_running / 60000;

          uint8_t m1 = clk / 10; // 4th digit
          set_char(4, m1, 1, false);
          uint8_t m2 = clk % 10; // 3rd digit
          set_char(3, m2, 1, true);

          Serial1.print(" : "); // minutes
          clock_running = ct % 60000;
          clock_running = clock_running / 1000;
          Serial1.println(clock_running);

          uint8_t s1 = clock_running / 10; // 2nd digit
          set_char(2, s1, 1, false);
          uint8_t s2 = clock_running % 10; // 1st digit
          set_char(1, s2, 1, false);

          if (ct >= 439051) // if (ct <= 100000 && ct > 0)
          {
            Serial1.println("firstt running.....");
            PORTJ &= ~_BV(v2); //
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
          }
          if (ct < 439051 && ct >= 409372) // if (ct > 100000 && ct <= 130000)
          {
            Serial1.println("second  running.....");
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);
          }
          if (ct < 409372 && ct >= 269265) // if (ct > 130000 && ct <= 270000)
          {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
          }
          if (ct < 269265 && ct >= 239586) // if (ct > 270000 && ct <= 300000)
          {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
          }
          if (ct < 239586 && ct >= 89240) // if (ct > 300000 && ct <= 450000)
          {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
          }
          if (ct < 89240)
          {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
          }
        }

        if (RS == 0)
        {
          process_status = 9;
          Serial1.println("Process Stoped");
        }

        process_status = 9;
      }
    }
  }

  delay(100);
}

// void preheating_unwrapped()
// {
// }

/********** Switches current status end*********************/

// Serial1.print("Test Program Status : ");
// Serial1.println(test_sw);
//
//
// Serial1.print("cycle Program  Status : ");
// Serial1.println(prgrm_sw);
//
// Serial1.print("program running Status : ");
// Serial1.println(start_sw);

// Serial1.println("...................................");

/*************************************************************************************************************/
// if(int_flag)
//{
//   Serial1.println("...................................................");
//   float pres = mpx();
//   Serial1.print("pressure : " );
//   Serial1.print(pres);
//   Serial1.println(" kPa" );
//// delay(1000);
//  volatile float tmp4 = TS1();
//  Serial1.print("TS1: ");
//  Serial1.println(tmp4);
//// delay(1000);
//  volatile float tmp2 = TS2();
//  Serial1.print("TS2: ");
//  Serial1.println(tmp2);
//// delay(1000);
//    volatile float tmp3 = TS3();
//  Serial1.print("TS3: ");
//  Serial1.println(tmp3);
////     delay(1000);
//  int_flag = 0;
//  }

/*************************************************************************************************************/

// PORTJ &= ~ _BV(PORTJ6); // 2ND
// PORTJ &= ~ _BV(PORTJ5); // 3RD
// PORTJ &= ~ _BV(PORTJ4); // 4TH
// PORTJ &= ~ _BV(PORTJ3); // 1ST

// PORTJ |=  _BV(PORTJ6); // 1st
// PORTJ |= _BV(PORTJ5); // 3RD
// PORTJ |=  _BV(PORTJ4); // 4TH
// PORTJ |=  _BV(PORTJ3); // 2nd

// PORTH |= _BV(PORTH3); // 3RD FAN
// PORTH |= _BV(PORTH4); // 2ND // vaccume
// PORTH |= _BV(PORTH5); // 1ST // motor
// PORTC |= _BV(PORTC4); // 4TH  // steam
// PORTC |= _BV(PORTC5); // 5TH  // heat

//  PORTJ |= _BV(v4); //
//  PORTH |= _BV(vac); //
//  delay(10000);
//  PORTJ &= ~_BV(v4); //
//  PORTH &= ~_BV(vac); //

// if(PINE & (1<<6))
//   Serial1.println("drain water  chamber empty");
// else
//   Serial1.println("drain water chamber full");

//
// if(PINE & (1<<7))
//  Serial1.println("upper sensor WATER full");
// else
//  Serial1.println("upper sensor water empty");

// if(PINE & (1<<5))
//   Serial1.println("lower sensor WATER empty");
// else
//   Serial1.println("lower sensor water full");

// PORTJ |= _BV(v2); //

//#define SW4 PORTE4 // DOOR SENSOR
//#define SW3 PORTE5  // LOWER SENSOR
//#define SW2 PORTE6   // USED WATER FLOAT SENSOR
//#define SW1 PORTE7   // UPPER SENSOR

// put your main code here, to run repeatedly:
//  Serial1.print("PE5 : ");
////  Serial1.println(PINE & (1<<5)); // PINE & (1<<5)
//   Serial1.println(digitalRead(PE5)); // EMPTY HIGH
//
//  Serial1.print("PE6 : ");
////  Serial1.println(PINE & (1<<6)); // FLOAT EMPTY HIGH
//   Serial1.println(digitalRead(PE6)); //
//
//  Serial1.print("PE7 : ");
////  Serial1.println(PINE & (1<<7)); // EMPTY LOW
// Serial1.println(digitalRead(PE7));

// PORTH |= _BV(PORTH3); // 3RD
// PORTH |= _BV(PORTH4); // 2ND
// PORTH |= _BV(PORTH5); // 1ST
// PORTC |= _BV(PORTC4); // 4TH
// PORTC |= _BV(PORTC5); // 5TH
//
//
// PORTJ |= _BV(PORTJ6); // 2ND
// PORTJ |= _BV(PORTJ5); // 3RD
// PORTJ |= _BV(PORTJ4); // 4TH
// PORTJ |= _BV(PORTJ3); // 1ST
// PORTB |= _BV(PORTB6);

/////*************************************************************************************************************/
//////if(int_flag)
//////{
////  Serial1.println("...................................................");
//   pres = mpx();
////  lcd2_press(pres);
//  Serial1.print("pressure : " );
//  Serial1.print(pres);
//  Serial1.println(" kPa" );
//// delay(1000);
// tmp4 = TS1();
//  Serial1.print("TS1: ");
//  Serial1.println(tmp4);
//// delay(1000);
//  tmp2 = TS2();
//  lcd1_temp(tmp2);//.....................
//  Serial1.print("TS2: ");
//  Serial1.println(tmp2);
//// delay(1000);
// tmp3 = TS3();
//  Serial1.print("TS3: ");
//  Serial1.println(tmp3);
//////     delay(1000);
////  int_flag = 0;
//////  }
