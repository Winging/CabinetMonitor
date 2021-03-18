/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The UCOSII application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              黄东润
** Created date:            2012-2-19
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
/* Includes ********************************************************************************************/
#include "includes.h"
#include "tasks.h"
#include "TcpApp.h"
#include "AppDebug.h"
#include "AppSerialCom.h"
#include "RingBuffer.h"
#include "AppTaskNetCommand.h"
#include "AppTaskSample.h"
#include "AppTaskKeyScan.h"
#include "AppTaskLCDDisplay.h"
#include "App.h"
#include "RTCTime.h"
#include "AppPersist.h"
#include "LEDControl.h"
#include "Beep.h"
#include "Relay.h"
#include "AppSensor.h"
#include "Initializer.h"
#include "AppTaskNetInit.h"
#include "AppRs485.h"

/*全局定义***********************************************************************************************/
#define WDT_TIMEOUT 3000000
/* 声明  ****************************************************************************************/
//调试声明
char _db[128];//该变量用于输出调试信息,AppDebug引用

Ring_Buffer_T bufRec;//串口接收缓冲区
Ring_Buffer_T bufNetRec;//网络接收缓冲区
uint8_t NetServerDataOut[265];//服务器发送包
uint32_t NetServerDataOutLen;
//uint8_t NetClientDataOut[265];//客户端发送包
//uint32_t NetClientDataOutLen;


/* uc/os-ii任务相关声明******************************************************************/
//堆栈声明
static  OS_STK  App_TaskStartStk[APP_TASK_START_STK_SIZE];  //初始化任务堆栈定义
static  OS_STK  App_TaskNetStk[APP_TASK_NET_STK_SIZE];      //网络通信任务堆栈定义
static OS_STK   App_TaskNetCommandStk[APP_TASK_NETCOMMAND_STK_SIZE];//网络数据包解析任务堆栈定义
static OS_STK   App_TaskSampleStk[APP_TASK_SAMPLE_STK_SIZE];//采样任务堆栈定义
static OS_STK   App_TaskKeyScanStk[APP_TASK_KEYSCAN_SIZE];//按键扫描任务
static OS_STK   App_TaskLCDDisplayStk[APP_TASK_LCDDISPLAY_SIZE];//LCD显示任务
static OS_STK   App_TaskNetInitStk[APP_TASK_NETINIT_SIZE];//网络初始化任务

//启动任务声明
static void AppTaskStart(void * pdata);

//任务之间通信相关声明
OS_EVENT * semNetCommand;//是否有网络数据包需要解析 初始为0
OS_EVENT * semNetBuf;//网络数据接收缓冲区互斥 初始为1
OS_EVENT * semServerDataOut;//服务端发送数据互斥
OS_EVENT * semClientDataOut;//客户端发送数据互斥
OS_EVENT * semTempWet;//温度湿度数据互斥
OS_EVENT * mBoxServerDataLen;//短消息，需要发送的数据长度
OS_EVENT * mBoxClientDataLen;//短消息，需要发送的数据长度
OS_EVENT * mBoxKeyCode;//短消息，按键编码
OS_EVENT * semNetInited;//网络是否已经初始化
OS_EVENT * semLCDInited;//lcd是否已经初始化
/* 定义*************************************************************/

/* uc/os-ii 任务相关定义 ******************************************/
//初始化任务
static void AppTaskStart(void * pdata)
{
    INT8U os_err;
	App_Persist_Module_Def mod;
	CPU_SR cpu_sr;
	uint16_t theState,i;
	SysAlarmTypeDef alarm;
	
    os_err = os_err;
	SystemInit();
    pdata = pdata;//避免编译器警告以及部分编译器报错
	//任务之间通信
	semNetCommand = OSSemCreate(0);
	semNetBuf = OSSemCreate(1);
	semServerDataOut = OSSemCreate(1);
	semClientDataOut = OSSemCreate(1);
	mBoxServerDataLen = OSMboxCreate((void*)0);
	mBoxClientDataLen = OSMboxCreate((void*)0);
	mBoxKeyCode       = OSMboxCreate((void*)0);
	semNetInited = OSSemCreate(0);
	/* 相关硬件初始化******/
    //初始化系统时钟
    clock_init();
    //初始化调试工具
    debug_frmwrk_init();
	_DBG_("Init hardware");
	//初始化串口通信
	//InitUart();
	_DBG_("UART INIT");
	//初始化实时时钟
	RTCInit();
	_DBG_("RTC INIT");
	//启动实时时钟
	RTCStart();
	_DBG_("RTC START");
	
	
	//初始化按键
	key_init();
	_DBG_("KEY INIT");
	//初始化蜂鸣器
	BeepInit();
	_DBG_("BEEP INIT");
	BeepOff();
	//初始化继电器
	_DBG_("RELAY");
	_DBG_("iNIT RS485");
	Rs485Init();
	InitRelay();
	while(1)
	{
		if(SmokeAlarm(1))
		{
			my_printf("1");
		}
		if(SmokeAlarm(2))
		{
			my_printf("2");
		}
		if(SmokeAlarm(3))
		{
			my_printf("3");
		}
		if(SmokeAlarm(4))
		{
			my_printf("4");
		}
		_DBG_("?");
	}
	
	//初始化传感器
	_DBG_("INIT SENSOR");
	
	InitSensor();
	
	_DBG_("sensor inited");
	//初始化设置
	InitFlashSetting();
	
	/*确定系统重启原因*****************/
	if (WDT_ReadTimeOutFlag()){
		_DBG_("wdt timeout");
		// Clear WDT TimeOut
		WDT_ClrTimeOutFlag();
	} else{
		_DBG_("manual reset");
	}
	//初始化看门狗
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_RESET);
	//看门狗的看门时间
	WDT_Start(WDT_TIMEOUT);
	/*
	while(1)
	{
		sprintf(_db," now is %d",i);
		DB;
		AlarmRelayOut(i-1);
		OSTimeDly(1500);
		AlarmResumeRelayOut(i-1);
		OSTimeDly(1500);
		i++;
		if(i>6) i = 1;
	}
	*/
	/*
	mod.Module_ADDR1 = 89;
	mod.Module_ADDR2 = 90;
	modifyModuleAddr(mod);
	_DBG_("OK");
	sprintf(_db,"%d-%d-%d-%d:%d|%d mod:%d-%d",(APP_Persist_IP_Addr->IP_ADDR1),(APP_Persist_IP_Addr->IP_ADDR2),(APP_Persist_IP_Addr->IP_ADDR3),
	(APP_Persist_IP_Addr->IP_ADDR4),(APP_Persist_IP_Addr->ServerPort),(APP_Persist_IP_Addr->ClientPort),
	APP_Persist_Module_Addr->Module_ADDR1,APP_Persist_Module_Addr->Module_ADDR2
	);
    DB;
	*/
	/*
	my_printf("size=%d",sizeof(EcuipControlArgTypeDef));
	//my_printf("org size=%d",sizeof(TESTControlArgTypeDef));
	//modifyFanArg(2,6,4,3,1,7,9,1);
	//modifyFireArg(2,6,4,3,1,7,9,1);
	my_printf("the result =%d - %d - %d ",APP_Persist_RecordIndex_Addr[0].allWrite,APP_Persist_RecordIndex_Addr[0].recordEnd,APP_Persist_RecordIndex_Addr[0].recordBegin);
	OS_ENTER_CRITICAL();
	beginSysAlarm(12,1);
	OS_EXIT_CRITICAL();
	for(i=0;i<50;i++)
	my_printf("i=%d alarm = %d - %d",i,APP_Persist_SysAlarmRecord_Addr[i].state,APP_Persist_SysAlarmRecord_Addr[i].alarmType);
	alarm = getSysAlarm(30);
	my_printf("get alarm here state=%d,type=%d",alarm.state,alarm.alarmType);
	*/
	/*
	for(i=0;i<25;i++)
	{
		my_printf("<%d - %d - %d >",APP_Persist_RecordIndex_Addr[0].allWrite,APP_Persist_RecordIndex_Addr[0].recordEnd,APP_Persist_RecordIndex_Addr[0].recordBegin);
		OS_ENTER_CRITICAL();
		beginSysAlarm(APP_Persist_RecordIndex_Addr[0].recordEnd,Argent);
		OS_EXIT_CRITICAL();
		my_printf(" (%d)  ",i);
		my_printf("[%d - %d - %d ]",APP_Persist_RecordIndex_Addr[0].allWrite,APP_Persist_RecordIndex_Addr[0].recordEnd,APP_Persist_RecordIndex_Addr[0].recordBegin);
		//OSTimeDly(5);
	}
	*/
	//printf("size=%d",sizeof(Flash_Type_Def));
	//modifyAlarmSet(0,3.4);
	//my_printf("test=%f--%d",APP_Persist_AlarmSet_Addr[0].alarmSet,(int32_t)(APP_Persist_AlarmSet_Addr[0].alarmSet*1000));
    
	os_err =  OSTaskCreate(AppTaskNet,
                            (void*)0,
                            &App_TaskNetStk[APP_TASK_NET_STK_SIZE-1],
                            APP_TASK_NET_PRIO
                            );						
	
	os_err =  OSTaskCreate(AppTaskNetCommand,
                            (void*)0,
                            &App_TaskNetCommandStk[APP_TASK_NETCOMMAND_STK_SIZE-1],
                            APP_TASK_NETCOMMAND_PRIO
                            );
							
							
	os_err =  OSTaskCreate(AppTaskSample,
                            (void*)0,
                            &App_TaskSampleStk[APP_TASK_SAMPLE_STK_SIZE-1],
                            APP_TASK_SAMPLE_PRIO
                            );
							
	os_err =  OSTaskCreate(AppTaskKeyScan,
                            (void*)0,
                            &App_TaskKeyScanStk[APP_TASK_KEYSCAN_SIZE-1],
                            APP_TASK_KEYSCAN_PRIO
                            );
							
							
	os_err =  OSTaskCreate(AppTaskLCDDisplay,
                            (void*)0,
                            &App_TaskLCDDisplayStk[APP_TASK_LCDDISPLAY_SIZE-1],
                            APP_TASK_LCDDISPLAY_PRIO
                            );
								
	os_err =  OSTaskCreate(AppTaskNetInit,
                            (void*)0,
                            &App_TaskNetInitStk[APP_TASK_NETINIT_SIZE-1],
                            APP_TASK_NETINIT_PRIO
                            );								
	
    OSTaskDel(OS_PRIO_SELF);//当所有任务创建完成删除本任务
}


int c_entry()
{
    INT8U  os_err;
    os_err = os_err;
	
    OSInit();//初始化uc-os/ii
    
    os_err = OSTaskCreate(AppTaskStart,
                            (void*)0,
                            &App_TaskStartStk[APP_TASK_START_STK_SIZE-1],
                            APP_TASK_START_PRIO);
    
    OSStart();//启动uc-os/ii
	return 0;
}
int main()
{
    return c_entry();
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
