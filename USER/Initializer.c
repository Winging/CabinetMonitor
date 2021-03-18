#include "Initializer.h"
#include "AppPersist.h"
#include "AppDebug.h"
#include "Beep.h"
#include "RTCTime.h"
uint8_t serialMac[6];//为了提高网络初始化的实时性，先将序列号转化为MAC地址存储在全局变量之中
void InitFlashSetting(void)
{
	CPU_SR cpu_sr;
	App_Persist_IP_Def ip;
	App_Persist_Module_Def mod;
	uint32_t b[4];
	RTCTime time;
	float alarmSet;
	uint8_t i;
	uint8_t state;
	
	_DBG_("TE");
	if(APP_Persist_IP_Addr->IP_ADDR1==255)
	{
		ip.IP_ADDR1 = 192;
		ip.IP_ADDR2 = 168;
		ip.IP_ADDR3 = 0;
		ip.IP_ADDR4 = 105;
		ip.ServerPort = 2001;
		_DBG_("TEm");
		OS_ENTER_CRITICAL();
		modifyIPAddr(ip);
		OS_EXIT_CRITICAL();
		_DBG_("TE2");
	}
	_DBG_("TE1");
	/*
	if(APP_Persist_Module_Addr->Module_ADDR1!=4)
	{
		_DBG_("TE4");
		mod.Module_ADDR1 = 0;
		mod.Module_ADDR2 = 4;
		OS_ENTER_CRITICAL();
		modifyModuleAddr(mod);
		OS_EXIT_CRITICAL();
		_DBG_("TE3");
	}
	*/
	_DBG_("OK?");
	OS_ENTER_CRITICAL();
	u32IAP_ReadSerialNumber(&b[0],&b[1],&b[2],&b[3]);
	OS_EXIT_CRITICAL();
	_DBG_("OK ");
	//sprintf(_db,"serial number %d-%d-%d-%d",b[0],b[1],b[2],b[3]);
	//DB;
	serialMac[5] = (uint8_t)((b[0]&0x000000ff));
	serialMac[4] = (uint8_t)((b[0]&0x0000ff00)>>8);
	serialMac[3] = (uint8_t)((b[0]&0x00ff0000)>>16);
	serialMac[2] = (uint8_t)((b[0]&0xff000000)>>24);
	serialMac[1] = (uint8_t)((b[1]&0x000000ff));
	serialMac[0] = (uint8_t)((b[1]&0x0000ff00)>>8);
	
	serialMac[5] ^= (uint8_t)((b[1]&0x00ff0000))>>16;
	serialMac[4] ^= (uint8_t)((b[1]&0xff000000)>>24);
	serialMac[3] ^= (uint8_t)((b[2]&0x00ff0000)>>16);
	serialMac[2] ^= (uint8_t)((b[2]&0xff000000)>>24);
	serialMac[1] ^= (uint8_t)((b[2]&0x000000ff));
	serialMac[0] ^= (uint8_t)((b[2]&0x0000ff00)>>8);
	
	OS_ENTER_CRITICAL();
	if(GetBeepState()>3)
	BeepOpen();//蜂鸣器开
	OS_EXIT_CRITICAL();
	//程序控制allwrite只可能是0 或1 如果是大于1则说明是第一次使用，进行系统初始化
	if(APP_Persist_RecordIndex_Addr[0].allWrite>1)
	{
		modifyRecordIndex(0,0,1);
	}
	//如果年份小于2012说明是第一次使用，进行时钟初始化
	if(LPC_RTC->YEAR<2012)
	{
		RTCSetTimeByYDMHMSW(13,4,8,0,0,0,0);
	}
	//初始化告警门限为 0
	for(i=0;i<ALARM_SET_NUM;i++)
	{
		alarmSet = APP_Persist_AlarmSet_Addr[i].alarmSet;
		my_printf("org = %f",alarmSet);
		if(alarmSet>8000||alarmSet<-8000)
		{
			if(i==Temp1OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,80);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp1LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,-40);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet1OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,98);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet1LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,0);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp2OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,80);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp2LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,-40);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet2OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,98);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet2LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,0);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			
			else if(i==Temp3OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,80);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp3LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,-40);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet3OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,98);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet3LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,0);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp4OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,80);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Temp4LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,-40);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet4OverSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,98);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
			else if(i==Wet4LackSet)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(i,0);
				OS_EXIT_CRITICAL();
				alarmSet = 100;
			}
		}
	}
	//初始化告警偏移为0
	for(i=0;i<8;i++)
	{
		alarmSet = APP_Persist_AlarmShift_Addr[i].alarmShift;
		my_printf("org = %f",alarmSet);
		if(alarmSet>8000||alarmSet<-8000)
		{
			OS_ENTER_CRITICAL();
			modifyAlarmShift(i,0);
			OS_EXIT_CRITICAL();
			alarmSet = 100;
		}
	}
	
	OS_ENTER_CRITICAL();
	modifySwitchState(DoorState,1);
	modifySwitchState(DoorTwoState,1);
	modifySwitchState(DoorThreeState,1);
	modifySwitchState(DoorFourState,1);
	modifySwitchState(SmokeThreeState,0);
	modifySwitchState(SmokeFourState,0);
	my_printf("statte-%d",APP_Persist_SwitchState_Addr[SmokeFourState],state);
	//if(APP_Persist_AlarmLevelRelay_Addr[Smoke].relay>200)
		modifyAlarmLevelRelay(Smoke,Argent,4);
	OS_EXIT_CRITICAL();
	//初始化开关闭合类型，默认为常开
	/*
	for(i=0;i<5;i++)
	{
		state = APP_Persist_SwitchState_Addr[i].state;
		if(state!=0 && state!=1)
		{
			OS_ENTER_CRITICAL();
			if(i!=DoorState)
				modifySwitchState(i,0);
			else
				modifySwitchState(DoorState,1);
	
			OS_ENTER_CRITICAL();
			
		}
	}
	*/
	
}