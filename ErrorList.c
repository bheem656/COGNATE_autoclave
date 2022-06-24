#include "errorlist.h"
#include "max.h"

extern volatile uint8_t RS;
extern uint8_t current_cycle ;
extern float steam_generator_temp;
extern float outer_body_temp;
extern float chamber_temp;
extern float pressure;

uint8_t over_steam_generator_temp = 220;
uint8_t over_outer_body_temp = 150;
uint8_t over_chamber_temp = 150;
uint8_t over_pressure = 230;

uint8_t InChamberTemp_High = 140;
uint8_t InChamberTemp_Low = 100;

extern volatile uint8_t door_status;

uint32_t total_time_cycle;
uint32_t overtime_total_time_cycle = 10000;

 uint8_t power_failure = 0;

void Check_Error()
{

    pressure = mpx();
    outer_body_temp = TS1();
    chamber_temp = TS2();
    steam_generator_temp = TS3();

    // Staem Generator over temperature
    if (steam_generator_temp > over_steam_generator_temp)
    {

        print_code(0, 1);
        RS = 0;
    }
    // Heating Ring over temperature
    if (outer_body_temp > over_outer_body_temp)
    {

        print_code(0, 2);
        RS = 0;
    }
    // Chamber over temperature
    if (chamber_temp > over_chamber_temp)
    {

        print_code(0, 3);
        RS = 0;
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
    if (door_status == 1 && RS == 1)
    {

        print_code(0, 6);
        RS = 0;
    }
    // Working overtime
    if (total_time_cycle > overtime_total_time_cycle)
    {

        print_code(0, 7);
        RS = 0;
    }
    // Over Pressure
    if (pressure > over_pressure)
    {

        print_code(0, 8);
        RS = 0;
    }
    // In-chamber sensors temp. too high or too low
    if (chamber_temp > InChamberTemp_High || chamber_temp > InChamberTemp_Low)
    {

        print_code(0, 9);
        RS = 0;
    }

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

        print_code(0, 1);
        RS = 0;
    }
    // Forced exit

    if ( RS == 0)
    {
        // uint8_t power_failure;// = EEPROM.read(0); // EEPROM.read()
        
        if (power_failure == 1) 
        {
        print_code(9, 8);
        // EEPROM.update(0, 0);
        RS = 0;

        // CALL PRE HEAT
        }
    }
}