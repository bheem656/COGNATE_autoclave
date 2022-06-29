

#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;
// extern uint8_t RS;

#define print_debug 1

extern int8_t process_status;
extern uint32_t _cuurent_time;
extern uint32_t _last_time;
extern uint32_t curr_time;
extern uint32_t rev_time;
// uint32_t duration

extern float steam_generator_temp;
extern float pressure;
extern volatile uint8_t RS;

extern float outer_body_temp;

extern uint32_t clock_running;
extern float pres;
extern float tmp4;
extern float tmp2;
extern float tmp3;

extern uint32_t mm_;
extern uint32_t ss_;

extern uint8_t _m4;
extern uint8_t _m3;
extern uint8_t _s2;
extern uint8_t _s1;

void vaccume_test_cycle(void)
{

    switch (process_status)
    {
    case 1:
        VACCUME_PROCESS(); // 4 min
        process_status = 2;
        break;
    case 2:
        STEADY_PROCESS(); // 5 min
        process_status = 3;
        break;
    case 3:
        TESTING_PROCESS(); // 10 min
        process_status = 4;
        break;
    case 4:
        VAC_PASS_PROCESS(); // 30 sec
        RS = 0;
        process_status = 0;
        // break;

    default:
        break;
    }
}
// void cycle::unwrapped_cycle()

void VACCUME_PROCESS(void)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < 240000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = 240000 - curr_time;
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
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_ua(3);
        //     }

        //     _last_time = millis();
        // }

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

            // Serial1.print("current process running :");
            // Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            // Serial1.print("Outer Body :");
            // Serial1.println(outer_body_temp);

            // Serial1.print("Steam Generator : ");
            // Serial1.println(steam_generator_temp);

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
            PORTJ &= ~_BV(v3); //.....................
            break;
        }

        /*****************************  Valve Control **************************/
        PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v4);  // 4th valve on
    }

    /********* turn off all valve & Relay **********/
    PORTH &= ~_BV(vac);
    PORTJ &= ~_BV(v4);
    PORTJ |= _BV(v4);
}

void STEADY_PROCESS(void)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < 300000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = 300000 - curr_time;
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
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_ua(3);
        //     }

        //     _last_time = millis();
        // }

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

            // Serial1.print("current process running :");
            // Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            // Serial1.print("Outer Body :");
            // Serial1.println(outer_body_temp);

            // Serial1.print("Steam Generator : ");
            // Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure < -80)
        // {
        //     PORTH &= ~_BV(vac);
        //     PORTJ &= ~_BV(v4);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v3); // 4th valve on
                          /***********************  Relay control **********************/
    }

    /********* turn off all valve & Relay **********/
    // PORTH &= ~_BV(vac);
    // PORTJ &= ~_BV(v4);
}

void TESTING_PROCESS(void)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < 600000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = 600000 - curr_time;
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
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_ua(3);
        //     }

        //     _last_time = millis();
        // }

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

            // Serial1.print("current process running :");
            // Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            // Serial1.print("Outer Body :");
            // Serial1.println(outer_body_temp);

            // Serial1.print("Steam Generator : ");
            // Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure < -80)
        // {
        //     PORTH &= ~_BV(vac);
        //     PORTJ &= ~_BV(v4);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v3); // 4th valve on
                          /***********************  Relay control **********************/
    }

    /********* turn off all valve & Relay **********/

    PORTJ &= ~_BV(v3);
}

void VAC_PASS_PROCESS(void)
{
    _cuurent_time = millis();
    _last_time = _cuurent_time;

    /***************************  Run for duration time ********************/
    curr_time = millis() - _cuurent_time;

    while (curr_time < 30000)
    {

        /******************  convert time  in minutes and seconds **********************/
        curr_time = millis() - _cuurent_time;
        rev_time = 30000 - curr_time;
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
        // if (millis() - _last_time >= 5000)
        // {
        //     _last_time = millis();
        //     MAX7219_Clear(2);
        //     while (millis() - _last_time < 700)
        //     {
        //         print_ua(3);
        //     }

        //     _last_time = millis();
        // }

        /************ Show Time on 7-Segment *******************/

        _m4 = mm_ / 10; // 4th digit
        _m3 = mm_ % 10; // 3rd digit
        _s2 = ss_ / 10; // 2nd digit
        _s1 = ss_ % 10; // 1st digit

        show_time(_m4, _m3, _s2, _s1);

        /************ Get current Temp & Pressure *******************/
        pressure = mpx();
        // outer_body_temp = TS1();
        // steam_generator_temp = TS3();

        if (print_debug)
        {

            // Serial1.print("current process running :");
            // Serial1.print(process_status);
            // // Serial1.println("...HE...");

            Serial1.print("pressure : ");
            Serial1.print(pressure);
            Serial1.println(" kPa");

            // Serial1.print("Outer Body :");
            // Serial1.println(outer_body_temp);

            // Serial1.print("Steam Generator : ");
            // Serial1.println(steam_generator_temp);

            Serial1.println("....................... ");
            Serial1.print("Time : ");
            Serial1.print(mm_);
            Serial1.print(" : "); // minutes
            Serial1.println(ss_);
        }

        /*******************  Break Condition *******************/
        // if (pressure < -80)
        // {
        //     PORTH &= ~_BV(vac);
        //     PORTJ &= ~_BV(v4);
        //     break;
        // }

        /*****************************  Valve Control **************************/
        // PORTH |= _BV(vac); // vaccume pump on
        PORTJ |= _BV(v2); // 4th valve on
                          /***********************  Relay control **********************/
    }

    /********* turn off all valve & Relay **********/

    PORTJ &= ~_BV(v2);
}
