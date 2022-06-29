
#include "BoardConfig.h"
#include "max.h"

// #define debug 1
extern uint8_t dev;
// extern uint8_t RS;

extern int8_t process_status;
extern uint32_t cuurent_time;
extern uint32_t last_time;
extern volatile uint8_t RS;

extern uint32_t clock_running;
extern float pres;
extern float tmp4;
extern float tmp2;
extern float tmp3;

void unwrapped_cycle(void)
{
    uint32_t _timeout = 0;
    // delay(1000);

    //     PORTJ |= _BV(v1);
    // PORTJ |= _BV(v2);
    // PORTJ |= _BV(v3);
    // PORTJ |= _BV(v4);

    // delay(10000);

    // PORTJ &=~ _BV(v1);
    // PORTJ &=~ _BV(v2);
    // PORTJ &=~_BV(v3);
    // PORTJ &=~ _BV(v4);

    Serial1.println(" unwrapped cycle started..............");

    switch (process_status)
    {
    case 1:
        start_process_led_glow();
        _timeout = 400000; // 6:40  // 400000
        Serial1.print(" current process :");
        Serial1.println(process_status);
       HE_PROCESS(_timeout);
        process_status = 2;
        break;
    case 2:
        vaccume_process_led_glow();
        _timeout = 240000; // 3:40 220000
        Serial1.print(" current process :");
        Serial1.println(process_status);
       CUA_PROCESS(_timeout, -81, 1);
        // UA1_PROCESS(_timeout);
        process_status = 3;
        break;
    case 3:
        vaccume_process_led_glow();
        _timeout = 670000; // 7:25 670000
        Serial1.print(" current process :");
        Serial1.println(process_status);
        // PR1_PROCESS(_timeout);
        CPR_PROCESS(_timeout, 151, 1);
        process_status = 4;
        break;
    case 4:
        vaccume_process_led_glow();
        _timeout = 45000; // 45sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CRE_PROCESS(_timeout, 1);
        // RE1_PROCESS(_timeout);
        process_status = 5;
        break;
    case 5:
        vaccume_process_led_glow();
        _timeout = 390000; // 6:30  390000
        Serial1.print(" current process :");
        Serial1.println(process_status);
        CPR_PROCESS(_timeout, 219, 2);
        // PR2_PROCESS(_timeout);
        process_status = 6;
        break;
    case 6:
        sterilize_process_led_glow();
        _timeout = 240000; // 4:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        ST_PROCESS(_timeout, 222);
        process_status = 7;
        break;

    case 7:
        sterilize_process_led_glow();
        _timeout = 45000; // 45 sec
        Serial1.print(" current process :");
        Serial1.println(process_status);
        RE_PROCESS(_timeout);
        process_status = 8;
        break;
    case 8:
        dry_process_led_glow();
        _timeout = 540000; // 9:00
        Serial1.print(" current process :");
        Serial1.println(process_status);
        DR_PROCESS(_timeout);
        process_status = 9;
        break;
    case 9:
        end_process_led_glow();
        Serial1.print(" current process :");
        Serial1.println(process_status);
        PASS_PROCESS();
        RS = 0;
        process_status = 0;
        break;

    default:

        break;
    }
}
