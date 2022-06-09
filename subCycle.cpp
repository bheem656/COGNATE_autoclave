#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;
// extern uint8_t RS;

extern uint8_t process_status;
extern volatile uint8_t RS;

float steam_generator_temp;
float outer_body_temp;
float chamber_temp;
float pressure;

/************** temp cut off  condition ******************/
uint8_t max_steam_generator_temp = 180;
uint8_t max_outer_body_temp = 90;
uint16_t motor_on_time = 160; // 150 fine
uint16_t motor_off_time = 2000;
uint16_t _stPres = 223;
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
    /********************  new ************************************/


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
            Serial1.print("Intrupt generated");
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


void ST_PROCESS(uint32_t duration, uint32_t _prr)
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
            Serial1.print("Intrupt generated");
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

        if (pressure < _prr)
        {

            if (millis() - motor_time >= 700)
            {
                motor_time = millis(); // load current time

                /*********** turn on motor ****************/
                while (millis() - motor_time < 60) // motor_on_time
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
            Serial1.print("Intrupt generated");
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
            Serial1.print("Intrupt generated");
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
            // PORTJ |= _BV(v3);  // 4th valve
            PORTJ |= _BV(v4);  // 4th valve
        }
        if (rev_time < 440000 && rev_time >= 410000) // if (ct > 100000 && ct <= 130000)
        {
            Serial1.println("second  running.....");
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v3);  // 4th valve
            PORTJ |= _BV(v2);
        }
        if (rev_time < 410000 && rev_time >= 270000) // if (ct > 130000 && ct <= 270000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            // PORTJ |= _BV(v3);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 270000 && rev_time >= 240000) // if (ct > 270000 && ct <= 300000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            // PORTJ |= _BV(v3);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }
        if (rev_time < 240000 && rev_time >= 90000) // if (ct > 300000 && ct <= 450000)
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v3);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 90000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            // PORTJ |= _BV(v3);  // 4th valve
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

void DR_porous_PROCESS(uint32_t duration)
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
            Serial1.print("Intrupt generated");
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

        if (rev_time >= 865000) // 14:25
        {
            Serial1.println("firstt running.....");
            PORTJ &= ~_BV(v2); //
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
        }
        if (rev_time < 865000 && rev_time >= 630000) // 10:30
        {
            Serial1.println("second  running.....");
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);
        }
        if (rev_time < 630000 && rev_time >= 590000) // 9:50
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 590000 && rev_time >= 360000) // 6:00
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }
        if (rev_time < 360000 && rev_time >= 300000) // 5:30
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 300000 && rev_time >= 90000) // 1:30
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }

        if (rev_time < 90000)
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
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

void DR_all_prgm_PROCESS(uint32_t duration)
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
            Serial1.print("Intrupt generated");
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

        if (rev_time >= 2460000) // 41:00 2460000
        {
            Serial1.println("firstt running.....");
            PORTJ &= ~_BV(v2); //
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
        }
        if (rev_time < 2460000 && rev_time >= 840000) // 41:00 840000
        {
            Serial1.println("second  running.....");
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);
        }
        if (rev_time < 840000 && rev_time >= 630000) // 9:50 630000
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 630000 && rev_time >= 600000) // 6:00 600000
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }
        if (rev_time < 600000 && rev_time >= 360000) // 5:30 360000
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }
        if (rev_time < 360000 && rev_time >= 330000) // 1:30 330000
        {
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ |= _BV(v2);  // 4th valve
        }

        if (rev_time < 330000 && rev_time >= 90000) // 1:30 330000
        {

            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
            PORTJ &= ~_BV(v2); //
        }

        if (rev_time < 90000) // 90000
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




void DR_bnd_PROCESS(uint32_t duration)
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
            Serial1.print("Intrupt generated");
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

        if (rev_time >= 85000) // 41:00 2460000
        {
            // Serial1.println("firstt running.....");
            PORTJ &= ~_BV(v2); //
            PORTH |= _BV(vac); // vaccume pump on
            PORTJ |= _BV(v4);  // 4th valve
        }


        if (rev_time < 85000) // 90000
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
    PORTJ |= _BV(v2);
    // delay(10000);
    // PORTJ &= ~_BV(v2);
    print_pass();
}

/****************************** common function *****************************************/

void CRE_PROCESS(uint32_t duration, uint8_t process_num)
{

    PORTJ &= ~_BV(v2);
    PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v4);

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
            Serial1.print("Intrupt generated");
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
                print_re(process_num);
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
    // PORTJ |= _BV(v3);
    // PORTJ |= _BV(v4);
}

void CUA_PROCESS(uint32_t duration, int16_t _pressure, uint8_t process_num)
{

    PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);
    PORTJ &= ~_BV(v2);

    Serial1.print(" duration :");
    Serial1.println(duration);

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
            Serial1.print("Intrupt generated");
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
                print_ua(process_num);
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
        if (pressure < _pressure)
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
    // PORTJ &= ~_BV(v4);
}

void CPR_PROCESS(uint32_t duration, int16_t _pressure, uint8_t process_num)
{

    PORTJ &= ~_BV(v2);

    _cuurent_time = millis();
    _last_time = _cuurent_time;
    motor_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    // pressure = mpx();
    // delay(100);
    while (curr_time < duration) // while (pressure <= _pressure)
    {
        Serial1.println("running..");

        // if (curr_time > 600)
        // {
        //     Serial1.println("error");
        //     break;
        // }

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        // if (!RS)
        // {
        //     Serial1.print("Intrupt generated");
        //     process_status = -1;
        //     break;
        // }

        /********* Display current cycle Status on 7-Segment ******************/
        if (millis() - _last_time >= 5000)
        {
            _last_time = millis();
            MAX7219_Clear(2);
            while (millis() - _last_time < 700)
            {
                print_pr(process_num);
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
        if (pressure > _pressure)
        {
            Serial1.println("presuure cutt off");
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

    // Serial1.println("breaked.....");

    /********* turn off all valve & Relay **********/
    // PORTJ &= ~_BV(v3);
    PORTJ &= ~_BV(v1);    // 1ST
    PORTH &= ~_BV(motor); // 1ST // motor
}


/********************* all test cycle program function ********************/
void first_all_test_process()
{

    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;
    /********************  new ************************************/

    while (curr_time < 40000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            Serial1.print("Intrupt generated");
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_he();
        //     }

        //     _last_time = millis();
        // }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nddigit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        PORTC |= _BV(steam);
        PORTC |= _BV(heat);

        PORTJ |= _BV(v1);
        // PORTJ |= _BV(v2);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
    }
}

void second_all_test_process()
{

    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;
    /********************  new ************************************/

    while (curr_time < 60000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        mm_ = curr_time / 60000; // Total minutes
        ss_ = curr_time % 60000; // Total seconds
        ss_ = ss_ / 1000;

        /********* Check if stop button pressed ******************/

        if (!RS)
        {
            Serial1.print("Intrupt generated");
            process_status = -1;
            break;
        }

        /********* Display current cycle Status on 7-Segment ******************/
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_he();
        //     }

        //     _last_time = millis();
        // }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nddigit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        PORTC |= _BV(steam);
        PORTC |= _BV(heat);

        PORTJ |= _BV(v1);
        // PORTJ |= _BV(v2);
        PORTJ |= _BV(v3);
        PORTJ |= _BV(v4);
    }
}




// error code

// void error_code_check(void)
// {




// }