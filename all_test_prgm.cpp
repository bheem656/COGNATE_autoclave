

#include "BoardConfig.h"
#include "max.h"

extern uint8_t RS;

void all_test_prgm_cycle(void)
{
    first_all_test_process();
    second_all_test_process();
    RS = 0;
}
