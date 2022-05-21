

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

void prion_cycle(void)
{
    uint32_t _timeout = 0;

    switch (process_status)
    {
    case 1:
        start_process_led_glow();
        _timeout = 140000; // 2:10
        Serial1.print(" current process :");
        Serial1.println(process_status);
        HE_PROCESS(_timeout);
        process_status = 2;
        break;
    case 2:
        vaccume_process_led_glow();
        _timeout = 160000; // 2:40
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CUA_PROCESS(_timeout,-80,1);
        // UA1_PROCESS(_timeout);
        process_status = 3;
        break;
    case 3:
        vaccume_process_led_glow();
        _timeout = 270000; // 4:30
        CPR_PROCESS(_timeout,90,1);
        // PR1_PROCESS(_timeout);
        process_status = 4;
        break;
    case 4:
        vaccume_process_led_glow();
        _timeout = 40000; // 40 sec
        CRE_PROCESS(_timeout,1);
        // RE1_PROCESS(_timeout);
        process_status = 5;
        break;
    case 5:
        vaccume_process_led_glow();
        _timeout = 140000; // 2:20
        CUA_PROCESS(_timeout,-65,2);
        // UA2_PROCESS(_timeout);
        process_status = 6;
        break;
    case 6:
        vaccume_process_led_glow();
        _timeout = 15000; // 2:30
        CPR_PROCESS(_timeout,60,2);
        // PR2_PROCESS(_timeout);
        process_status = 7;
        break;

    case 7:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        CRE_PROCESS(_timeout,2);
        // RE2_PROCESS(_timeout);
        process_status = 8;
        break;
    case 8:
        vaccume_process_led_glow();
        _timeout = 150000; // 2:30
        CUA_PROCESS(_timeout,-65,3);
        // UA3_PROCESS(_timeout);
        process_status = 9;
        break;
    case 9:
        vaccume_process_led_glow();
        _timeout = 450000; // 7:30
        CPR_PROCESS(_timeout,216,3);
        //  PR1_PROCESS(_timeout);
        // PR3_PROCESS(_timeout);
        process_status = 10;
        break;

    case 10:
        sterilize_process_led_glow();
        _timeout = 1080000; // 18:00
        ST_PROCESS(_timeout);
        process_status = 11;
        break;

    case 11:
        sterilize_process_led_glow();
        _timeout = 52000; // 52 sec
        RE_PROCESS(_timeout);
        process_status = 12;
        break;

    case 12:
        dry_process_led_glow();
        _timeout = 540000; // 9:00
        DR_PROCESS(_timeout);
        process_status = 13;
        break;

    case 13:
        end_process_led_glow();

        PASS_PROCESS();
        // process_status = 14;
        break;

    default:
        break;
    }
}
