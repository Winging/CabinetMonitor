#include "RTCTime.h"
/*
 *@描述：实时时钟初始化
 */
void RTCInit()
{
	RTC_Init(LPC_RTC);
}
/*
 *@描述：启动实时时钟
 */
void RTCStart()
{
	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
}
/*
 *@描述:设置时间
 *@参数：Time 结构体
 */
void RTCSetTime(RTCTime Time )
{
  LPC_RTC->SEC = Time.RTC_Sec;
  LPC_RTC->MIN = Time.RTC_Min;
  LPC_RTC->HOUR = Time.RTC_Hour;
  LPC_RTC->DOM = Time.RTC_Mday;
  LPC_RTC->DOW = Time.RTC_Wday;
  LPC_RTC->DOY = Time.RTC_Yday;
  LPC_RTC->MONTH = Time.RTC_Mon;
  LPC_RTC->YEAR = Time.RTC_Year;    
  return;
}
/*
 *@描述：获得实时时钟的时间，返回时间结构体
 */
RTCTime RTCGetTime()
{
  RTCTime LocalTime;
    
  LocalTime.RTC_Sec = LPC_RTC->SEC;
  LocalTime.RTC_Min = LPC_RTC->MIN;
  LocalTime.RTC_Hour = LPC_RTC->HOUR;
  LocalTime.RTC_Mday = LPC_RTC->DOM;
  LocalTime.RTC_Wday = LPC_RTC->DOW;
  LocalTime.RTC_Yday = LPC_RTC->DOY;
  LocalTime.RTC_Mon = LPC_RTC->MONTH;
  LocalTime.RTC_Year = LPC_RTC->YEAR;
  return ( LocalTime );  
}
/*
 *@描述：按照年月日时分秒设定实时时钟
 */
void RTCSetTimeByYDMHMSW(uint32_t year,uint32_t mon,uint32_t day,uint32_t hour,uint32_t min,uint32_t sec,uint32_t dow)
{
	LPC_RTC->SEC = sec;
	LPC_RTC->MIN = min;
	LPC_RTC->HOUR = hour;
	LPC_RTC->DOM = day;
	LPC_RTC->MONTH = mon;
	LPC_RTC->YEAR = year+2000;
	LPC_RTC->DOW = dow;
	return;
}
