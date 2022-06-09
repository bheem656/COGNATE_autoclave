

#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;
// extern uint8_t RS;

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
    uint32_t _timeout = 0;

    switch (process_status)
    {
        // case 1:
        //     start_process_led_glow();
        //     _timeout = 00; // 0:00
        //     Serial1.print(" current process :");
        //     Serial1.println(process_status);
        //     HE_PROCESS(_timeout);
        //     process_status = 2;
        //     break;

        // case 2:
        //     vaccume_process_led_glow();
        //     _timeout = 150000; // 2:30
        //     Serial1.print(" current process :");
        //     Serial1.println(process_status);
        //     CUA_PROCESS(_timeout, -60, 1);
        //     // UA1_PROCESS(_timeout);
        //     process_status = 3;
        //     break;

        // case 3:

        //     vaccume_process_led_glow();
        //     _timeout = 260000; // 220000; // 7:25 670000
        //     Serial1.print(" current process :");
        //     Serial1.println(process_status);
        //     // PR1_PROCESS(_timeout);
        //     CPR_PROCESS(_timeout, 90, 1);
        //     process_status = 4;
        //     break;

        // case 4:
        //     vaccume_process_led_glow();
        //     _timeout = 30000; // 30 sec
        //     CRE_PROCESS(_timeout, 1);
        //     // RE1_PROCESS(_timeout);
        //     process_status = 5;
        //     break;

        // case 5:
        //     vaccume_process_led_glow();
        //     _timeout = 150000; // 2:30
        //     CUA_PROCESS(_timeout, -65, 2);
        //     // UA2_PROCESS(_timeout);
        //     process_status = 6;
        //     break;

    case 1:
        start_process_led_glow();
        _timeout = 00; // 6:40  // 400000
        Serial1.print(" current process :");
        Serial1.println(process_status);
        HE_PROCESS(_timeout);
        process_status = 2;
        break;
    case 2:
        vaccume_process_led_glow();
        _timeout = 150000; // 3:40 220000
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CUA_PROCESS(_timeout, -80, 1);
        // UA1_PROCESS(_timeout);
        process_status = 3;
        break;
    case 3:
        vaccume_process_led_glow();
        _timeout = 260000; // 7:25 670000
        Serial1.print(" current process :");
        Serial1.println(process_status);
        // PR1_PROCESS(_timeout);
        CPR_PROCESS(_timeout, 90, 1);
        process_status = 4;
        break;
    case 4:
        vaccume_process_led_glow();
        _timeout = 3000; // 45sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CRE_PROCESS(_timeout, 1);
        // RE1_PROCESS(_timeout);
        process_status = 5;
        break;

    case 5:
        vaccume_process_led_glow();
        _timeout = 150000; // 2:30
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CUA_PROCESS(_timeout, -65, 1);
        // UA1_PROCESS(_timeout);
        process_status = 3;
        break;

    case 6:
        vaccume_process_led_glow();
        _timeout = 110000; // 1:50
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CPR_PROCESS(_timeout, 60, 2);
        // PR2_PROCESS(_timeout);
        process_status = 6;
        break;

        // case 6:
        //     vaccume_process_led_glow();
        //     _timeout = 170000; // 1:50
        //     CPR_PROCESS(_timeout, 60, 2);
        //     // PR2_PROCESS(_timeout);
        //     process_status = 7;
        //     break;

    case 7:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        CRE_PROCESS(_timeout, 2);
        // RE2_PROCESS(_timeout);
        process_status = 8;
        break;

    case 8:
        vaccume_process_led_glow();
        _timeout = 750000; // 1:15
        CUA_PROCESS(_timeout, -65, 3);
        // UA3_PROCESS(_timeout);
        process_status = 9;
        break;

    case 9:
        vaccume_process_led_glow();
        _timeout = 102000; // 1:42
        CPR_PROCESS(_timeout, 60, 3);
        // PR3_PROCESS(_timeout);
        process_status = 10;
        break;

    case 10:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        CRE_PROCESS(_timeout, 3);
        // RE3_PROCESS(_timeout);
        process_status = 11;
        break;

    case 11:
        vaccume_process_led_glow();
        _timeout = 75000; // 1:15
        CUA_PROCESS(_timeout, -65, 4);
        // UA4_PROCESS(_timeout);
        process_status = 12;
        break;

    case 12:
        vaccume_process_led_glow();
        _timeout = 100000; // 1:40
        CPR_PROCESS(_timeout, 60, 4);
        // PR4_PROCESS(_timeout);
        process_status = 13;
        break;

    case 13:
        vaccume_process_led_glow();
        _timeout = 30000; // 30 sec
        CRE_PROCESS(_timeout, 4);
        // RE4_PROCESS(_timeout);
        process_status = 14;
        break;

    case 14:
        vaccume_process_led_glow();
        _timeout = 84000; // 1:24
        CUA_PROCESS(_timeout, -60, 5);
        // UA5_PROCESS(_timeout);
        process_status = 15;
        break;

    case 15:
        vaccume_process_led_glow();
        _timeout = 100000; // 1:40
        CPR_PROCESS(_timeout, 120, 5);
        // PR5_PROCESS(_timeout);
        process_status = 16;
        break;

    case 16:
        sterilize_process_led_glow();
        _timeout = 2220000; // 37:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        ST_PROCESS(_timeout, 120);
        // ST_PROCESS(_timeout);
        process_status = 17;
        break;

    case 17:
        sterilize_process_led_glow();
        _timeout = 40000; // 40 sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
        RE_PROCESS(_timeout);
        process_status = 18;
        break;

    case 18:
        dry_process_led_glow();
        _timeout = 2460000; // 41:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        DR_PROCESS(_timeout);
        process_status = 19;

        break;

    case 19:
        end_process_led_glow();
        PASS_PROCESS();
        RS = 0;
        break;
    default:
        break;
    }
}

// void cycle::unwrapped_cycle()
