/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The UCOSII application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              �ƶ���
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

/*ȫ�ֶ���***********************************************************************************************/
#define WDT_TIMEOUT 3000000
/* ����  ****************************************************************************************/
//��������
char _db[128];//�ñ����������������Ϣ,AppDebug����

Ring_Buffer_T bufRec;//���ڽ��ջ�����
Ring_Buffer_T bufNetRec;//������ջ�����
uint8_t NetServerDataOut[265];//���������Ͱ�
uint32_t NetServerDataOutLen;
//uint8_t NetClientDataOut[265];//�ͻ��˷��Ͱ�
//uint32_t NetClientDataOutLen;


/* uc/os-ii�����������******************************************************************/
//��ջ����
static  OS_STK  App_TaskStartStk[APP_TASK_START_STK_SIZE];  //��ʼ�������ջ����
static  OS_STK  App_TaskNetStk[APP_TASK_NET_STK_SIZE];      //����ͨ�������ջ����
static OS_STK   App_TaskNetCommandStk[APP_TASK_NETCOMMAND_STK_SIZE];//�������ݰ����������ջ����
static OS_STK   App_TaskSampleStk[APP_TASK_SAMPLE_STK_SIZE];//���������ջ����
static OS_STK   App_TaskKeyScanStk[APP_TASK_KEYSCAN_SIZE];//����ɨ������
static OS_STK   App_TaskLCDDisplayStk[APP_TASK_LCDDISPLAY_SIZE];//LCD��ʾ����
static OS_STK   App_TaskNetInitStk[APP_TASK_NETINIT_SIZE];//�����ʼ������

//������������
static void AppTaskStart(void * pdata);

//����֮��ͨ���������
OS_EVENT * semNetCommand;//�Ƿ����������ݰ���Ҫ���� ��ʼΪ0
OS_EVENT * semNetBuf;//�������ݽ��ջ��������� ��ʼΪ1
OS_EVENT * semServerDataOut;//����˷������ݻ���
OS_EVENT * semClientDataOut;//�ͻ��˷������ݻ���
OS_EVENT * semTempWet;//�¶�ʪ�����ݻ���
OS_EVENT * mBoxServerDataLen;//����Ϣ����Ҫ���͵����ݳ���
OS_EVENT * mBoxClientDataLen;//����Ϣ����Ҫ���͵����ݳ���
OS_EVENT * mBoxKeyCode;//����Ϣ����������
OS_EVENT * semNetInited;//�����Ƿ��Ѿ���ʼ��
OS_EVENT * semLCDInited;//lcd�Ƿ��Ѿ���ʼ��
/* ����*************************************************************/

/* uc/os-ii ������ض��� ******************************************/
//��ʼ������
static void AppTaskStart(void * pdata)
{
    INT8U os_err;
	App_Persist_Module_Def mod;
	CPU_SR cpu_sr;
	uint16_t theState,i;
	SysAlarmTypeDef alarm;
	
    os_err = os_err;
	SystemInit();
    pdata = pdata;//��������������Լ����ֱ���������
	//����֮��ͨ��
	semNetCommand = OSSemCreate(0);
	semNetBuf = OSSemCreate(1);
	semServerDataOut = OSSemCreate(1);
	semClientDataOut = OSSemCreate(1);
	mBoxServerDataLen = OSMboxCreate((void*)0);
	mBoxClientDataLen = OSMboxCreate((void*)0);
	mBoxKeyCode       = OSMboxCreate((void*)0);
	semNetInited = OSSemCreate(0);
	/* ���Ӳ����ʼ��******/
    //��ʼ��ϵͳʱ��
    clock_init();
    //��ʼ�����Թ���
    debug_frmwrk_init();
	_DBG_("Init hardware");
	//��ʼ������ͨ��
	//InitUart();
	_DBG_("UART INIT");
	//��ʼ��ʵʱʱ��
	RTCInit();
	_DBG_("RTC INIT");
	//����ʵʱʱ��
	RTCStart();
	_DBG_("RTC START");
	
	
	//��ʼ������
	key_init();
	_DBG_("KEY INIT");
	//��ʼ��������
	BeepInit();
	_DBG_("BEEP INIT");
	BeepOff();
	//��ʼ���̵���
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
	
	//��ʼ��������
	_DBG_("INIT SENSOR");
	
	InitSensor();
	
	_DBG_("sensor inited");
	//��ʼ������
	InitFlashSetting();
	
	/*ȷ��ϵͳ����ԭ��*****************/
	if (WDT_ReadTimeOutFlag()){
		_DBG_("wdt timeout");
		// Clear WDT TimeOut
		WDT_ClrTimeOutFlag();
	} else{
		_DBG_("manual reset");
	}
	//��ʼ�����Ź�
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_RESET);
	//���Ź��Ŀ���ʱ��
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
	
    OSTaskDel(OS_PRIO_SELF);//���������񴴽����ɾ��������
}


int c_entry()
{
    INT8U  os_err;
    os_err = os_err;
	
    OSInit();//��ʼ��uc-os/ii
    
    os_err = OSTaskCreate(AppTaskStart,
                            (void*)0,
                            &App_TaskStartStk[APP_TASK_START_STK_SIZE-1],
                            APP_TASK_START_PRIO);
    
    OSStart();//����uc-os/ii
	return 0;
}
int main()
{
    return c_entry();
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
