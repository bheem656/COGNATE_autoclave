#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;

extern uint8_t process_status;
extern volatile uint8_t RS;

float steam_generator_temp;
float outer_body_temp;
float chamber_temp;
float pressure;

/************** temp cut off  condition ******************/
uint8_t max_steam_generator_temp = 180;
uint8_t max_outer_body_temp = 120;
uint16_t motor_on_time = 80;
uint16_t motor_off_time = 2000;
/***************  variable for display time on 7-segment *******************/
uint32_t _cuurent_time;
uint32_t curr_time;
uint32_t _last_time;
uint32_t motor_time;
uint32_t rev_time;
uint32_t mm_;
uint32_t ss_;

uint8_t _m4;
uint8_t _m3;
uint8_t _s2;
uint8_t _s1;

/***************  HE_PROCESS  *******************/

#define print_debug 1

void HE_PROCESS(uint32_t duration)
{

    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_he();
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nddigit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*****************************  Valve Control **************************/

        // All valve should be off

        /***********************  Relay control **********************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }
}

/***************  UA1_PROCESS  *******************/
/********  V4 + VACCUME PUMP + STRAT FROM 3:40 + END AT -80 KPA **********/
void UA1_PROCESS(uint32_t duration)
{

    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_ua(1);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure < -80)
        {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
        /***********************  Relay control **********************/

        /********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
}

/***************  PR1_PROCESS  *******************/
 /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA **********/

void PR1_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(1);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure > 150)
        {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            break;
        }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v1);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
        /***********************  Relay control **********************/
        // PORTH |= _BV(motor); // 1ST // motor motor_time
        // delay(motor_on_time);
        // PORTH &= ~_BV(motor); // 1ST // motor

        if (millis() - motor_time >= motor_off_time)
        {
            motor_time = millis(); // load current time

            /*********** turn on motor ****************/
            while (millis() - motor_time < motor_on_time)
            {
                PORTH |= _BV(motor);
            }
            /*********** turn off motor ****************/
            PORTH &= ~_BV(motor);
            motor_time = millis();
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}

      /********  RE1 **********/
            /********  V2 +  STRAT FROM 00:00 + END AT 45 SEC **********/
void RE1_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_re(1);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTJ |= _BV(v2);

        PORTJ &= ~_BV(v3);
        PORTJ &= ~_BV(v4);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTJ |= _BV(v3);
    PORTJ |= _BV(v4);
}

void UA2_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_ua(2);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure < -80)
        {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
        /***********************  Relay control **********************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
}
void PR2_PROCESS(uint32_t duration)
{
      _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(2);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure > 217)
        {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            break;
        }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v1);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
        /***********************  Relay control **********************/
        // PORTH |= _BV(motor); // 1ST // motor motor_time
        // delay(motor_on_time);
        // PORTH &= ~_BV(motor); // 1ST // motor

        if (millis() - motor_time >= motor_off_time)
        {
            motor_time = millis(); // load current time

            /*********** turn on motor ****************/
            while (millis() - motor_time < motor_on_time)
            {
                PORTH |= _BV(motor);
            }
            /*********** turn off motor ****************/
            PORTH &= ~_BV(motor);
            motor_time = millis();
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}
void RE2_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_re(2);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v2);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTJ &= ~_BV(v2);
}

void UA3_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_ua(3);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure < -80)
        {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
        /***********************  Relay control **********************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
}
void PR3_PROCESS(uint32_t duration)
{
      _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(3);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure > 150)
        {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            break;
        }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v1);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
        /***********************  Relay control **********************/
        // PORTH |= _BV(motor); // 1ST // motor motor_time
        // delay(motor_on_time);
        // PORTH &= ~_BV(motor); // 1ST // motor

        if (millis() - motor_time >= motor_off_time)
        {
            motor_time = millis(); // load current time

            /*********** turn on motor ****************/
            while (millis() - motor_time < motor_on_time)
            {
                PORTH |= _BV(motor);
            }
            /*********** turn off motor ****************/
            PORTH &= ~_BV(motor);
            motor_time = millis();
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}
void RE3_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_re(3);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v2);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTJ &= ~_BV(v2);
}

void UA4_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_ua(4);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure < -80)
        {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
        /***********************  Relay control **********************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
}
void PR4_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(1);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure > 150)
        {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            break;
        }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v1);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
        /***********************  Relay control **********************/
        // PORTH |= _BV(motor); // 1ST // motor motor_time
        // delay(motor_on_time);
        // PORTH &= ~_BV(motor); // 1ST // motor

        if (millis() - motor_time >= motor_off_time)
        {
            motor_time = millis(); // load current time

            /*********** turn on motor ****************/
            while (millis() - motor_time < motor_on_time)
            {
                PORTH |= _BV(motor);
            }
            /*********** turn off motor ****************/
            PORTH &= ~_BV(motor);
            motor_time = millis();
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}
void RE4_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_re(4);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v2);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTJ &= ~_BV(v2);
}

void UA5_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_ua(5);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure < -80)
        {
            PORTH &= ~_BV(vac);
            PORTJ &= ~_BV(v4);
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
        /***********************  Relay control **********************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
}
void PR5_PROCESS(uint32_t duration)
{
     _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(1);
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        if (pressure > 150)
        {
            PORTJ &= ~_BV(v3);
            PORTJ &= ~_BV(v1);    // 1ST
            PORTH &= ~_BV(motor); // 1ST // motor
            break;
        }

        /*****************************  Valve Control **************************/
        PORTJ |= _BV(v1);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
        /***********************  Relay control **********************/
        // PORTH |= _BV(motor); // 1ST // motor motor_time
        // delay(motor_on_time);
        // PORTH &= ~_BV(motor); // 1ST // motor

        if (millis() - motor_time >= motor_off_time)
        {
            motor_time = millis(); // load current time

            /*********** turn on motor ****************/
            while (millis() - motor_time < motor_on_time)
            {
                PORTH |= _BV(motor);
            }
            /*********** turn off motor ****************/
            PORTH &= ~_BV(motor);
            motor_time = millis();
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}

void ST_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_st();
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/

        if (pressure < 230)
        {

            if (millis() - motor_time >= motor_off_time)
            {
                motor_time = millis(); // load current time

                /*********** turn on motor ****************/
                while (millis() - motor_time < motor_on_time)
                {
                    PORTH |= _BV(motor);
                }
                /*********** turn off motor ****************/
                PORTH &= ~_BV(motor);
                motor_time = millis();
            }
        }

        /*****************************  Valve Control **************************/
        // PORTJ |= _BV(v2);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }

        PORTJ |= _BV(v3);
        PORTJ |= _BV(v1); // 1ST
    }

    /********* turn off all valve & Relay **********/
    PORTJ &= ~_BV(v1);
}

void RE_PROCESS(uint32_t duration)
{
     _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_RE();
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTJ |= _BV(v2);

        PORTJ &= ~_BV(v3);
        PORTJ &= ~_BV(v4);
        /***********************  Relay control **********************/

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTJ |= _BV(v3);
    PORTJ |= _BV(v4);
}
void DR_PROCESS(uint32_t duration)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < duration)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = duration - curr_time;
        mm_ = rev_time / 60000; // Total minutes
        ss_ = rev_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_dr();
            }

            _last_time = millis();
        }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        outer_body_temp = TS1();
        steam_generator_temp = TS3();

        if (print_debug)
        {

            Serial1.print("current process running :");
            Serial1.print(process_status);
            // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            Serial1.print("Outer Body :");
            Serial1.println(outer_body_temp);

            Serial1.print("Steam Generator : ");
            Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure > 150)
        // {
        //     PORTJ &= ~_BV(v2);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTJ |= _BV(v2);
        /***********************  Relay control **********************/

        if (rev_time >= 440000) // if (ct <= 100000 && ct > 0)
        {
            Serial1.println("firstt running.....");
            PORTJ &= ~_BV(v2); //
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
        }
        if (rev_time < 440000 && rev_time >= 410000) // if (ct > 100000 && ct <= 130000)
        {
            Serial1.println("second  running.....");
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);
        }
        if (rev_time < 410000 && rev_time >= 270000) // if (ct > 130000 && ct <= 270000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 270000 && rev_time >= 240000) // if (ct > 270000 && ct <= 300000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }
        if (rev_time < 240000 && rev_time >= 90000) // if (ct > 300000 && ct <= 450000)
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 90000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }

        /***********************************************************************/
        if (steam_generator_temp >= max_steam_generator_temp)
        {

            PORTC &= ~_BV(steam);
        }
        else
        {
            PORTC |= _BV(steam);
        }

        /* Heat Ring */
        if (outer_body_temp >= max_outer_body_temp)
        {

            PORTC &= ~_BV(heat);
        }
        else
        {
            PORTC |= _BV(heat);
        }
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac); // vaccume pump on
    PORTJ &= ~_BV(v4);  // 4th valve
    PORTJ &= ~_BV(v2);  // 4th valve
}

void PASS_PROCESS(void)
{
    print_pass();
}
