#ifndef __APP_SENSOR_H__
#define __APP_SENSOR_H__
#include "includes.h"

void InitSensor();
uint8_t DoorAlarm(uint8_t number);
uint8_t SmokeAlarm(uint8_t number);

uint8_t WaterAlarm();
uint8_t ThunderAlarm();
uint8_t LeapAlarm();
uint8_t ShakeAlarm();
uint8_t ACPowerDownAlarm();
void setACPowerDown(uint8_t flag);
/*获得告警数据*************************************/
uint8_t GetSysAlarm();
uint8_t GetEnvironAlarm();
uint8_t GetPowerAlarm();
uint8_t GetTempWetAlarmOne();
uint8_t GetTempWetAlarmTwo();
uint8_t GetSmokeDoorAlarm();
uint8_t GetNormalAlarm();
uint8_t HasAlarm(uint8_t type);
uint32_t readAcceleration(uint8_t t);
#endif
