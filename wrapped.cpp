

#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;

extern uint8_t process_status;
extern uint32_t cuurent_time;
extern uint32_t last_time;
extern volatile uint8_t RS;

extern uint32_t clock_running;
extern float pres;
extern float tmp4;
extern float tmp2;
extern float tmp3;

// bnd cycle

void wrapped_cycle(void)
{

    uint8_t count = 0;
    if (process_status == 1)
    {

        PORTA |= _BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        // float chamber_pressure;

        last_time = millis();
        // clock_running = millis() - cuurent_time;   6:40 sec
        while ((millis() - cuurent_time) < 400000) // 400000
        {
            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(3, 72, dev, false); // 72 h
                    set_char(2, 69, dev, false); // 69 e
                    // set_char(1, 1, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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

            // // MAX7219_Clear(1);
            uint8_t m1 = clk / 10; // 4th digit
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);
            // // MAX7219_Clear(2);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);
            //
            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);
            // MAX7219_Clear(2);

            // }

            // else

            // {

            //       set_char(1, 16, 1, false); //
            //       set_char(2, 14, 1, false); // 68 e
            //       set_char(3, 72, 1, false); // 72 h
            //       set_char(4, 16, 1, false); // 76
            //     // MAX7219_Clear(2);

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

            if (RS == 0)
            {
                process_status = 13;
                Serial1.println("Process Stoped");
            }
        }
        process_status = 2;
    }
    /* ##################################### 1st vaccume #########################################################*/
    /* ##################################### UA1 #########################################################*/
    /********  V4 + VACCUME PUMP + STRAT FROM 2:40 + END AT -80 KPA **********/
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

        // // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 257000) // 4:17
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(1, 85, dev, false); // 85 u
                    set_char(2, 65, dev, false); // 65 a
                    set_char(3, 1, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            ct = ct % 60000;
            ct = ct / 1000;
            Serial1.println(ct, 1);

            uint8_t s1 = ct / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        process_status = 3;
    }

    /* Raise pressure */
    /* ##################################### PR1 #########################################################*/
    /* ##################################### PR1 #########################################################*/
    /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA (3:30)**********/

    if (process_status == 3)
    {

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        //        // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 95000) // 210000 // 1:35
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(3, 80, dev, false); // 80 P
                    set_char(2, 82, dev, false); // 62 R
                    set_char(1, 2, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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

            if (pres > 90)
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }
        process_status = 4;
        // PORTC &= ~_BV(PORTC4); // 4TH  // steam
    }
    /* Exaust */
    /* ##################################### RE1 #########################################################*/
    /* ##################################### RE1 #########################################################*/
    /********  V2 +  STRAT FROM 00:00 + END AT 40 SEC **********/

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
        while ((millis() - cuurent_time) < 26000) // 26 sec
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(2, 80, dev, false); // 69 e
                    set_char(3, 82, dev, false); // 62 R
                    set_char(1, 1, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

            /* Get pressure and Temperature */
            // get_sensor_data(); // print Data
            // PORTJ &= ~_BV(v4);   // 1ST
            PORTJ |= _BV(v2);
        }
        PORTJ &= ~_BV(v2);

        if (RS == 0)
        {
            process_status = 13;
            Serial1.println("Process Stoped");
        }
        process_status = 5;
    }

    /* ##################################### 2nd vaccume #########################################################*/
    /* ##################################### UA2 #########################################################*/
    /********  V4 + VACCUME PUMP + STRAT FROM 2:40 + END AT -80 KPA **********/
    if (process_status == 5)
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

        // // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 157000) // 140000 :: 2:37
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(1, 85, dev, false); // 85 u
                    set_char(2, 65, dev, false); // 65 a
                    set_char(3, 2, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            if (pres < -65)
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            ct = ct % 60000;
            ct = ct / 1000;
            Serial1.println(ct, 1);

            uint8_t s1 = ct / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        process_status = 6;
    }

    /* Raise pressure */

    /* ##################################### PR2 #########################################################*/
    /* ##################################### RR2 #########################################################*/
    /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA **********/

    if (process_status == 6)
    {

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 150000) // 115000 // 2:30
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(3, 80, dev, false); // 80 P
                    set_char(2, 82, dev, false); // 62 R
                    set_char(1, 2, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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

            if (tmp3 >= 60)
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }
        PORTJ &= ~_BV(v1);
        process_status = 7;
    }

    /* Exaust */
    /* ##################################### RE2 #########################################################*/
    /* ##################################### RE2 #########################################################*/
    /********  V2 +  STRAT FROM 00:00 + END AT 30 SEC **********/

    if (process_status == 7)
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
        while ((millis() - cuurent_time) < 28000) // 28 sec
        {
            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(2, 80, dev, false); // 69 e
                    set_char(3, 82, dev, false); // 62 R
                    set_char(1, 2, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

            /* Get pressure and Temperature */
            // get_sensor_data(); // print Data
            // PORTJ &= ~_BV(v4);   // 1ST
            PORTJ |= _BV(v2);
        }
        PORTJ &= ~_BV(v2);

        if (RS == 0)
        {
            process_status = 13;
            Serial1.println("Process Stoped");
        }
        process_status = 8;
    }

    /* ##################################### 3rd vaccume #########################################################*/
    /* ##################################### UA3 #########################################################*/
    /********  V4 + VACCUME PUMP + STRAT FROM 2:40 + END AT -80 KPA **********/
    if (process_status == 8)
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

        // // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 223000) // 140000 :: 3:43
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(1, 85, dev, false); // 85 u
                    set_char(2, 65, dev, false); // 65 a
                    set_char(3, 3, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

            // PORTA &= ~_BV(start_led);
            // PORTK |= _BV(vacuum_led);
            // PORTK &= ~_BV(serilize_led);
            // PORTK &= ~_BV(dry_led);
            // PORTK &= ~_BV(end_led);
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
            if (pres < -65)
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            ct = ct % 60000;
            ct = ct / 1000;
            Serial1.println(ct, 1);

            uint8_t s1 = ct / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        process_status = 9;
    }

    /* Raise pressure */

    /* ##################################### PR3 #########################################################*/
    /* ##################################### PR3 #########################################################*/
    /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 00:00 + END AT 150 KPA **********/

    if (process_status == 9)
    {

        count++;

        if (count >= 5)
        {
            count = 0;
            uint32_t tt1 = millis();
            MAX7219_Clear(2);
            while (millis() - tt1 < 1500)
            {
                set_char(3, 80, dev, false); // 80 P
                set_char(2, 82, dev, false); // 62 R
                set_char(1, 3, dev, false);  // 1
            }
            // MAX7219_Clear(2);
        }

        PORTA &= ~_BV(start_led);
        PORTK |= _BV(vacuum_led);
        PORTK &= ~_BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 360000) //// 6:00
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

    
            if (tmp3 >= 60)
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

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
            process_status = 13;
            Serial1.println("Process Stoped");
        }
        PORTJ &= ~_BV(v1);
        process_status = 10;
    }

    /* Sterillization  */
    /* ##################################### &&&&&&&&&&&&&&& ST &&&&&&&&&&&&&&& #########################################################*/
    /* ##################################### &&&&&&&&&&&&&&& ST &&&&&&&&&&&&&&& #########################################################*/
    /********  V1 + MOTOR + V3 + STEAM +  STRAT FROM 04:00 + END AT 00:00 with maintain pressure of 218 kpa **********/

    if (process_status == 10)
    {

        count++;

        if (count >= 5)
        {
            count = 0;
            uint32_t tt1 = millis();
            MAX7219_Clear(2);
            while (millis() - tt1 < 1500)
            {
                set_char(3, 83, dev, false); // 83 s
                set_char(2, 84, dev, false); // 84 t

                // set_char(1, 1, dev, false);  // 1
            }
            // MAX7219_Clear(2);
        }

        PORTA &= ~_BV(start_led);
        PORTK &= ~_BV(vacuum_led);
        PORTK |= _BV(serilize_led);
        PORTK &= ~_BV(dry_led);
        PORTK &= ~_BV(end_led);

        cuurent_time = millis();
        // float chamber_pressure;
        // clock_running = millis() - cuurent_time;
        while ((millis() - cuurent_time) < 300000) // 4 min
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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

            PORTJ |= _BV(v3);
            PORTJ |= _BV(v1); // 1ST
        }
        if (RS == 0)
        {
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        PORTJ &= ~_BV(v1);
        process_status = 11;
    }

    /* Exaust */
    /* ##################################### RE #########################################################*/
    /* ##################################### RE #########################################################*/
    /********  V2 +  STRAT FROM 00:00 + END AT 45 SEC **********/

    if (process_status == 11)
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
        while ((millis() - cuurent_time) < 59000) // 59 sec
        {
            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(2, 80, dev, false); // 69 e
                    set_char(3, 82, dev, false); // 62 R

                    // set_char(1, 1, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = clock_running % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

            /* Get pressure and Temperature */
            // get_sensor_data(); // print Data
            // PORTJ &= ~_BV(v4);   // 1ST
            PORTJ |= _BV(v2);
        }
        if (RS == 0)
        {
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        PORTJ &= ~_BV(v1);

        process_status = 12;
    }

    /* Drying */
    /* ##################################### DR #########################################################*/
    /* ##################################### DR #########################################################*/
    /********  V2 +  STRAT FROM 00:00 + END AT 3:00 **********/
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

    if (process_status == 12)
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
        while ((millis() - cuurent_time) < 540000) // 9 min
        {

            count++;

            if (count >= 5)
            {
                count = 0;
                uint32_t tt1 = millis();
                MAX7219_Clear(2);
                while (millis() - tt1 < 1500)
                {
                    set_char(3, 68, dev, false); // 68 d
                    set_char(2, 82, dev, false); // 82 r
                    // set_char(1, 1, dev, false);  // 1
                }
                // MAX7219_Clear(2);
            }

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
            set_char(4, m1, dev, false);
            uint8_t m2 = clk % 10; // 3rd digit
            set_char(3, m2, dev, true);

            Serial1.print(" : "); // minutes
            clock_running = ct % 60000;
            clock_running = clock_running / 1000;
            Serial1.println(clock_running);

            uint8_t s1 = clock_running / 10; // 2nd digit
            set_char(2, s1, dev, false);
            uint8_t s2 = clock_running % 10; // 1st digit
            set_char(1, s2, dev, false);

            if (ct >= 95000) // if (ct <= 100000 && ct > 0)
            {
                Serial1.println("firstt running.....");
                PORTJ &= ~_BV(v2); //
                PORTH |= _BV(vac); // vaccume pump on
                PORTJ |= _BV(v4);  // 4th valve
            }

            if (ct < 95000)
            {
                PORTH |= _BV(vac); // vaccume pump on
                PORTJ |= _BV(v4);  // 4th valve
                PORTJ |= _BV(v2);  // 4th valve
            }
        }

        if (RS == 0)
        {
            process_status = 13;
            Serial1.println("Process Stoped");
        }

        process_status = 14;
    }
    if (process_status == 13)
    {
        Serial1.println("Abort");
    }

    if (process_status == 14)
    {
        Serial1.println("successfully run cycle");
    }
}
