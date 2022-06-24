#ifndef ErrorList_H
#define ErrorList_H


#include "BoardConfig.h"

// #include <stdint.h>


uint8_t error_list[20][5] = {
		"Er01",
		"Er02",
		"Er03",
		"Er04",
		"Er05",
		"Er06",
		"Er07",
		"Er08",
		"Er09",
		"Er10",
		"Er12",
		"Er14",
		"Er98",
		"Er99"

};

uint8_t error_details[20][100] = {
		"Steam Generator over temperature", // STG TEMP > 220 ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er01
		"Heating Ring over temperature", // 150 -- ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er02
		"Chamber over temperature", // 150 -- ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er03
		"Fail to maintain temperature and pressure",//  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er04
		"Pressure not exhausted", //   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er05
		"Door open during cycle", //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er06
		"Working overtime", //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er07
		"Over Pressure", //230   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er08
		"In-chamber sensors temp. too high or too low", // High=140, Low= 100   ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er09
		"Temp. and Pressure doesn't match", //  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er10
		"Vacuum fail", // (UA)vaccum cycle pressure not change  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er12
		"In-chamber sensors temp. differs too much",  //---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er14
		"Out of power during cycle", // power shut down during cycle  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er98
		"Forced exit" // force fully cycle close  ---->> TURN OFF RUNNING CYCLE  & DISPLAY ERROR CIDE Er99
};



void Check_Error(void);


#endif