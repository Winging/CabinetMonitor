#include "RTCTime.h"
/*
 *@������ʵʱʱ�ӳ�ʼ��
 */
void RTCInit()
{
	RTC_Init(LPC_RTC);
}
/*
 *@����������ʵʱʱ��
 */
void RTCStart()
{
	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
}
/*
 *@����:����ʱ��
 *@������Time �ṹ��
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
 *@���������ʵʱʱ�ӵ�ʱ�䣬����ʱ��ṹ��
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
 *@����������������ʱ�����趨ʵʱʱ��
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
