

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

void vaccume_test_cycle(void)
{

    switch (process_status)
    {
    case 1:
        VACCUME_PROCESS();
        break;
    case 2:
        STEADY_PROCESS();
        break;
    case 3:
        TESTING_PROCESS();
        break;
    case 4:
        HOLDING_PROCESS();
        break;

    case 12:
        PASS_PROCESS();
        break;

    default:
        break;
    }
}
// void cycle::unwrapped_cycle()


void VACCUME_PROCESS(void)
{

}

void STEADY_PROCESS(void)
{
    
}

void TESTING_PROCESS(void)
{
    
}

void HOLDING_PROCESS(void)
{
    
}

