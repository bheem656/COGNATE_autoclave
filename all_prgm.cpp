

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

void all_prgm_cycle(void)
{
    uint16_t _timeout = 0;

    switch (process_status)
    {
    case 1:
        start_process_led_glow();
        _timeout = 00; // 0:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        HE_PROCESS(_timeout);
        process_status = 2;
        break;

    case 2:
        vaccume_process_led_glow();
        _timeout = 150000; // 2:30
        Serial1.print(" current process :");
        Serial1.println(process_status);
        UA1_PROCESS(_timeout);
        process_status = 3;
        break;

    case 3:
        vaccume_process_led_glow();
        _timeout = 100000; // 1:40
        PR1_PROCESS(_timeout);
        process_status = 4;
        break;

    case 4:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        RE1_PROCESS(_timeout);
        process_status = 5;
        break;

    case 5:
        vaccume_process_led_glow();
        _timeout = 150000; // 2:30
        UA2_PROCESS(_timeout);
        process_status = 6;
        break;

    case 6:
        vaccume_process_led_glow();
        _timeout = 110000; // 1:50
        PR2_PROCESS(_timeout);
        process_status = 7;
        break;

    case 7:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        RE2_PROCESS(_timeout);
        process_status = 8;
        break;

    case 8:
        vaccume_process_led_glow();
        _timeout = 750000; // 1:15
        UA3_PROCESS(_timeout);
        process_status = 9;
        break;

    case 9:
        vaccume_process_led_glow();
        _timeout = 102000; // 1:42
        PR3_PROCESS(_timeout);
        process_status = 10;
        break;

    case 10:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        RE3_PROCESS(_timeout);
        process_status = 8;
        break;

    case 11:
        vaccume_process_led_glow();
        _timeout = 75000; // 1:15
        UA4_PROCESS(_timeout);
        process_status = 6;
        break;

    case 12:
        vaccume_process_led_glow();
        _timeout = 100000; // 1:40
        PR4_PROCESS(_timeout);
        process_status = 10;
        break;

    case 13:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        RE4_PROCESS(_timeout);
        process_status = 5;
        break;

    case 14:
        vaccume_process_led_glow();
        _timeout = 84000; // 1:24
        UA5_PROCESS(_timeout);
        process_status = 6;
        break;

    case 15:
        vaccume_process_led_glow();
        _timeout = 100000; // 1:40
        PR5_PROCESS(_timeout);
        process_status = 10;
        break;

    case 16:
        sterilize_process_led_glow();
        _timeout = 2220000; // 37:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        ST_PROCESS(_timeout);
        process_status = 7;
        break;

    case 17:
        sterilize_process_led_glow();
        _timeout = 40000; // 40 sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
        RE_PROCESS(_timeout);
        process_status = 9;
        break;

    case 18:
        dry_process_led_glow();
        _timeout = 2460000; // 41:00 sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
         DR_PROCESS(_timeout);
        RS = 0;
        break;

    case 19:
        PASS_PROCESS();
        break;
    default:
        break;
    }
}

// void cycle::unwrapped_cycle()
