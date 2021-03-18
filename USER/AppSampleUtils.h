#ifndef __APP_SAMPLE_UTILS_H__
#define __APP_SAMPLE_UTILS_H__

#include "includes.h"


//电压结构体。1、每个成员值位字符类型，0-9；2、当 三个值位 "AAA"时表示未用
//例  "123"表示12.3V
typedef struct VoltageRec
{
    char a1,a2,a3;
} Type_Of_Voltage;
//温度结构体，每个成员为字符类型，0-9；
//例如 "050"表示50 C；"240" 表示-40 C；
typedef struct  TempratureRec
{
		char a1,a2,a3;
} Type_Of_Temprature;
/*内部函数，比如读取电压可以调用这个函数*****************************************************************/

/*读取状态************************************************************/
Type_Of_Voltage      ReadStatus_GetVoltage();
Type_Of_Temprature   ReadStatus_GetTemprate();
uint8_t              ReadStatus_GetDoorState();
uint8_t              ReadStatus_GetElecState();
uint8_t              ReadStatus_GetAlarmFlag();
/* 设置门限信息*************************************/
uint8_t              SetDoor_SetVoltageOver(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetVoltageLack(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetVoltageLess(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetFanTemp(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetMainCold(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetMainHot(uint8_t a1,uint8_t a2,uint8_t a3);
#endif
