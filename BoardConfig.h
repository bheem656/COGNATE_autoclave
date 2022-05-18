#ifndef BoardConfig_H
#define BoardConfig_h

#include "Arduino.h"

#define v1 PORTJ6
#define v2 PORTJ3
#define v3 PORTJ5
#define v4 PORTJ4

#define fan PORTH3
#define vac PORTH4

#define steam PORTC4
#define heat PORTC5
#define motor PORTH5

#define rtd1 A4 // pressure
#define rtd2 A5 // TS1
#define rtd3 A6 // TS2
#define rtd4 A7 // TS3
#define rtd5 A8 // TS3

#define SW4 PORTE4 // DOOR SENSOR
#define SW3 PORTE5 // LOWER SENSOR
#define SW2 PORTE6 // USED WATER FLOAT SENSOR
#define SW1 PORTE7 // UPPER SENSOR

#define water_status_led PORTA6 // water_status_LED
#define drain_status_led PORTA4 // d13  drain_status_led

#define bnd_s__led PORTA2   // d15  b&d_led
#define vacuum_s_led PORTK4 // d6 vacuum_led

#define unwrapped_led PORTA7 // d2  unwrappped_LED
#define wrapped_led PORTA5   // d16 wrapped_led
#define prion_led PORTA3     // d14 prion_led
#define porous_led PORTA1    // d12  porous_led

#define start_led PORTA0    // d17 start_led
#define vacuum_led PORTK7   // d3 vacuum_led
#define serilize_led PORTK5 // d5 serilize_led
#define dry_led PORTK3      // d7 dry_led
#define end_led PORTK2      // d8 end_led

#define t134_led PORTJ7 // d10 t134_led
#define t121_led PORTK6 // d11 t121_led

/* SWITCH */
#define SW_program PORTL4 // program a
#define SW_start PORTL5   // start  b
#define SW_start1 PORTK1  // start  63/A0
#define SW_test PORTL6    // test SW_start  c
#define SW_D PORTL7       // na

float TS1(void);
float TS2(void);
float TS3(void);
void board_init(void);
float mpx(void);

void Timer1_init(void);

/* cycles */

enum sub_cycle
{
    HE = 0,
    UA1,
    PR1,
    RE1,
    UA2,
    PR2,
    RE2,
    UA3,
    PR3,
    ST,
    RE,
    DR,
    PASS
};

/***********  program cycle process methods ************/
void HE_PROCESS(uint32_t duration);
void UA1_PROCESS(uint32_t duration );
void PR1_PROCESS(uint32_t duration);
void RE1_PROCESS(uint32_t duration);

void UA2_PROCESS(uint32_t duration);
void PR2_PROCESS(uint32_t duration);
void RE2_PROCESS(uint32_t duration);

void UA3_PROCESS(uint32_t duration);
void PR3_PROCESS(uint32_t duration);
void RE3_PROCESS(uint32_t duration);

void UA4_PROCESS(uint32_t duration);
void PR4_PROCESS(uint32_t duration);
void RE4_PROCESS(uint32_t duration);

void UA5_PROCESS(uint32_t duration);
void PR5_PROCESS(uint32_t duration);

void ST_PROCESS(uint32_t duration);

void RE_PROCESS(uint32_t duration);
void DR_PROCESS(uint32_t duration);
void PASS_PROCESS(void);
// void verify_process(void);

void PR_PROCESS(uint32_t duration , uint16_t _pressure);

/*********** running prgram led  status methods ************/
void start_process_led_glow(void);
void vaccume_process_led_glow(void);
void sterilize_process_led_glow(void);
void dry_process_led_glow(void);
void end_process_led_glow(void);

/*********** work program cycle methods ************/
void unwrapped_cycle(void);
void wrapped_cycle(void);
void prion_cycle(void);
void porous_cycle(void);
void all_prgm_cycle(void);

/*********** test program cycle methods ************/
void bnd_test_cycle(void);
void vaccume_test_cycle(void);
void all_test_prgm_cycle(void);

/*********** vacume test methods ************/
void VACCUME_PROCESS(void);
void STEADY_PROCESS(void);
void TESTING_PROCESS(void);
void VAC_PASS_PROCESS(void);

/***********  program select  led status method ************/
void status_led_glow(void);
// void get_btn_status(void);

#endif
