#ifndef __TEMSENSOR_H__
#define __TEMSENSOR_H__
#include "includes.h"

/************************** PRIVATE VARIABLES *************************/
void Delay_Ms (unsigned long tick);
unsigned char AM2301_CollectCharData(uint8_t number,char *pvHumidity,char *pvTemperature);
/*声明******************************************************/
//温度状态枚举
typedef enum TempStateEnum
{
	TempStateOK,
	TempStateHigh,
	TempStateLow
}TempStateTypeDef;

//湿度状态枚举
typedef enum WetStateEnum
{
	WetStateOK,
	WetStateHigh,
	WetStateLow
}WetStateTypeDef;

/*接口****************************************************/
TempStateTypeDef checkTempAlarm(uint8_t number);
WetStateTypeDef checkWetAlarm(uint8_t number);
char getTemp(uint8_t number);
char getWet(uint8_t numer);

#endif
