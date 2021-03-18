#ifndef __RTCTIME_H__
#define __RTCTIME_H__
#include "includes.h"
typedef struct {
    uint32_t RTC_Sec;     /* Second value - [0,59] */
    uint32_t RTC_Min;     /* Minute value - [0,59] */
    uint32_t RTC_Hour;    /* Hour value - [0,23] */
    uint32_t RTC_Mday;    /* Day of the month value - [1,31] */
    uint32_t RTC_Mon;     /* Month value - [1,12] */
    uint32_t RTC_Year;    /* Year value - [0,4095] */
    uint32_t RTC_Wday;    /* Day of week value - [0,6] */
    uint32_t RTC_Yday;    /* Day of year value - [1,365] */
} RTCTime;

void RTCInit();
void RTCStart();
void RTCSetTime(RTCTime Time );
void RTCSetTimeByYDMHMSW(uint32_t year,uint32_t mon,uint32_t day,uint32_t hour,uint32_t min,uint32_t sec,uint32_t dow);
RTCTime RTCGetTime();


#endif
