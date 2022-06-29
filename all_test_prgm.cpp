

#include "BoardConfig.h"
#include "max.h"
extern int8_t process_status;
extern uint8_t RS;

void all_test_prgm_cycle(void)
{
    first_all_test_process();
    second_all_test_process();
    RS = 0;
    process_status = 0;
}
