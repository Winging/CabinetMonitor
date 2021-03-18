#include "AppPersist.h"
#include "AppDebug.h"
#include "RTCTime.h"

uint8_t GetInteger(uint8_t fraction)
{
	switch(fraction)
	{
		case 0 : return 0;
		case 1 : return 1;
		case 2 : return 3;
		case 3 : return 4;
		case 4 : return 5;
		case 5 : return 6;
		case 6 : return 8;
		case 7 : return 9;
	}
}
/*内部函数声明***********************************************************/
void InitFlash(Flash_Type_Def * flash);
void InitAlarmRecordIndexFlash(Flash_Rec_Index_Type_Def * flash);
/*内部函数定义***********************************************************/
void PrepareSector(void)
{
	u32IAP_PrepareSectors(SECTOR,END_SECTOR);
}
void InitFlash(Flash_Type_Def * flash)
{
	uint8_t i;
	(flash->ipAddr).IP_ADDR1 = APP_Persist_IP_Addr->IP_ADDR1;
	(flash->ipAddr).IP_ADDR2 = APP_Persist_IP_Addr->IP_ADDR2;
	(flash->ipAddr).IP_ADDR3 = APP_Persist_IP_Addr->IP_ADDR3;
	(flash->ipAddr).IP_ADDR4 = APP_Persist_IP_Addr->IP_ADDR4;
	(flash->ipAddr).ServerPort = APP_Persist_IP_Addr->ServerPort;
	(flash->ipAddr).ClientPort = APP_Persist_IP_Addr->ClientPort;
	(flash->moduleAddr).Module_ADDR1 = APP_Persist_Module_Addr->Module_ADDR1;
	(flash->moduleAddr).Module_ADDR2 = APP_Persist_Module_Addr->Module_ADDR2;
	//_DBG_("Init Flash ASSGIN OK");
	for(i=0;i<ALARM_SET_NUM;i++)
	{
		(flash->alarmSets[i]).alarmSet = APP_Persist_AlarmSet_Addr[i].alarmSet;
		//my_printf("org alarmset %f",APP_Persist_AlarmSet_Addr[i].alarmSet);

	}
	//_DBG_("alarmSET ok");
	for(i=0;i<ALARM_SHIFT_NUM;i++)
	{
		(flash->alarmShifts[i]).alarmShift = APP_Persist_AlarmShift_Addr[i].alarmShift;

	}
	//_DBG_("alarmShift ok");
	for(i=0;i<ALARM_REC_TYPE_NUM;i++)
	{
		(flash->alarmLevelRelays[i]) = APP_Persist_AlarmLevelRelay_Addr[i];
	}
	
	//_DBG_("alarmLevelRealy ok");
	for(i=0;i<CONTROL_STATE_NUM;i++)
	{
		(flash->controlStates[i]) = APP_Persist_ControlState_Addr[i];
	}
	//_DBG_("control OK");
	for(i=0;i<RELAY_STATE_NUM;i++)
	{
		(flash->relayStates[i]) = APP_Persist_RelayState_Addr[i];
	}
	//_DBG_("relayState ok");
	
	for(i=0;i<SWITCH_STATE_NUM;i++)
	{
		(flash->switchStates[i]) = APP_Persist_SwitchState_Addr[i];
	}
	//_DBG_("swtichState ok");
	for(i=0;i<FAN_NUM;i++)
	{
		(flash->fanControlArgs[i]).relay = APP_Persist_FanControlArg_Addr[i].relay;
		(flash->fanControlArgs[i]).open = APP_Persist_FanControlArg_Addr[i].open;
		(flash->fanControlArgs[i]).sense = APP_Persist_FanControlArg_Addr[i].sense;
	}
	//_DBG_("fan ok");
	for(i=0;i<FIRE_NUM;i++)
	{
		(flash->fireControlArgs[i]).relay = APP_Persist_FireControlArg_Addr[i].relay;
		(flash->fireControlArgs[i]).open = APP_Persist_FireControlArg_Addr[i].open;
		(flash->fireControlArgs[i]).sense = APP_Persist_FireControlArg_Addr[i].sense;
	}
	//_DBG_("fire ok");
	
}
/*接口定义*************************************************************/
void modifyIPAddr(App_Persist_IP_Def  IpType)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	_DBG_("Try Init Flash");
	InitFlash(&flash);
	flash.ipAddr = IpType;
	_DBG_("pre");
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
}
void modifyModuleAddr(App_Persist_Module_Def ModuleType)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	InitFlash(&flash);
	flash.moduleAddr = ModuleType;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		//_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		//_DBG_("copy ok");
	}
}
/*
 *@描述 修改告警设置
 *@参数 types告警点类型，alarmSet告警点
 *@返回 无
 */
void modifyAlarmSet(AlarmSetTypes types,float alarmSet)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	my_printf("Modify alarmset %d",(int32_t)(alarmSet*1000));
	InitFlash(&flash);
	(flash.alarmSets[types]).alarmSet = alarmSet;
	my_printf("Why?%d",(int32_t)((flash.alarmSets[types]).alarmSet*1000));
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	my_printf("address=%d",APP_Persist_AlarmSet_Addr);
	my_printf("type%d test=%f--%d",types,APP_Persist_AlarmSet_Addr[types].alarmSet,(int32_t)(APP_Persist_AlarmSet_Addr[types].alarmSet*1000));
}
void modifyAlarmShift(AlarmShiftTypes types,float alarmShift)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.alarmShifts[types]).alarmShift = alarmShift;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
}
void modifyAlarmLevelRelay(AlarmTypes types,uint8_t level,uint8_t relay)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.alarmLevelRelays[types]).level = level;
	(flash.alarmLevelRelays[types]).relay = relay;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"after types=%d,level:%d relay:%d",types,APP_Persist_AlarmLevelRelay_Addr[types].level,APP_Persist_AlarmLevelRelay_Addr[types].relay);
	DB;
}

void modifyControlState(ControlStateTypes types,uint8_t state)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.controlStates[types]).state = state;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"after types=%d,state:%d",types,APP_Persist_ControlState_Addr[types].state);
	DB;
}
void modifyRelayState(uint8_t types,uint8_t state)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.relayStates[types]).state = state;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"after types=%d,state:%d",types,APP_Persist_RelayState_Addr[types].state);
	DB;
}
void modifySwitchState(SwitchStateTypes types,uint8_t state)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.switchStates[types]).state = state;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"after types=%d,state:%d",types,APP_Persist_SwitchState_Addr[types].state);
	DB;
}
void modifyFanArg(uint8_t fanNumber,uint8_t relayNumber,float open,float sense)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.fanControlArgs[fanNumber]).relay = relayNumber;
	(flash.fanControlArgs[fanNumber]).open = open;
	(flash.fanControlArgs[fanNumber]).sense = sense;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"fan after types=%d,state:%d",fanNumber,APP_Persist_FanControlArg_Addr[fanNumber].relay);
	DB;
}
void modifyFireArg(uint8_t fireNumber,uint8_t relayNumber,float open,float sense)
{
	uint32_t error_num;
	Flash_Type_Def flash;
	
	InitFlash(&flash);
	(flash.fireControlArgs[fireNumber]).relay = relayNumber;
	(flash.fireControlArgs[fireNumber]).open = open;
	(flash.fireControlArgs[fireNumber]).sense =sense;
	PrepareSector();
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(SECTOR,END_SECTOR))
	{
		_DBG_("earase");
	}
	PrepareSector();
	error_num = u32IAP_CopyRAMToFlash(App_Persist_IP_BASE,(uint32_t)&flash.ipAddr,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("copy ok");
	}
	sprintf(_db,"fire after types=%d,state:%d",fireNumber,APP_Persist_FireControlArg_Addr[fireNumber].relay);
	DB;
}
void modifyRecordIndex(uint32_t begin,uint32_t end,uint8_t allWrite)
{
	uint32_t error_num;
	Flash_Rec_Index_Type_Def flash;
	InitAlarmRecordIndexFlash(&flash);
	//(flash.fanControlArgs[fireNumber]).relay = relayNumber;
	(flash.recordIndex[0]).recordBegin = begin;
	(flash.recordIndex[0]).recordEnd = end;
	(flash.recordIndex[0]).allWrite = allWrite;
	my_printf("inside %d %d",(flash.recordIndex[0]).recordBegin,(flash.recordIndex[0]).recordEnd);
	u32IAP_PrepareSectors(RECORD_INDEX_SECTOR,RECORD_INDEX_SECTOR);
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(RECORD_INDEX_SECTOR,RECORD_INDEX_SECTOR))
	{
		_DBG_("index earase");
	}
	u32IAP_PrepareSectors(RECORD_INDEX_SECTOR,RECORD_INDEX_SECTOR);
	error_num = u32IAP_CopyRAMToFlash(App_Persist_RecordIndex_Base,(uint32_t)&flash.recordIndex[0],256);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("index copy ok");
	}
}

void InitAlarmRecordFlash(Flash_Rec_Type_Def * flash,uint8_t sector)
{
	int i;
	SysAlarmTypeDef * recordAddr;
	
	recordAddr = (SysAlarmTypeDef *)(App_Persist_SysAlarmRecord_Base + sector*0x8000);
	for(i=0;i<113;i++)
	{
		flash->sysAlarms[i] = recordAddr[i];
		//my_printf("i=%d type=%d   ",i,recordAddr[i].alarmType);
	}
}
void InitAlarmRecordIndexFlash(Flash_Rec_Index_Type_Def * flash)
{
	flash->recordIndex[0] = APP_Persist_RecordIndex_Addr[0];
}
void beginSysAlarm(AlarmTypes types,AlarmLevel level)
{
	//当产生响应的告警时首先判断是否之前已经有该告警，如果有则更新第二个位置的最新告警
	uint32_t error_num;
	uint16_t begin,end,write;
	uint8_t sector;
	Flash_Rec_Type_Def recflash;
	SysAlarmTypeDef alarm;
	RTCTime LocalTime;
	
	LocalTime = RTCGetTime();//获得当前时间
	
	alarm.minute = LocalTime.RTC_Min;
	alarm.second = LocalTime.RTC_Sec;
	alarm.day    = LocalTime.RTC_Mday;
	alarm.hour   = LocalTime.RTC_Hour;
	alarm.year   = LocalTime.RTC_Year-2000 ;
	alarm.month  = LocalTime.RTC_Mon ;
	alarm.alarmType = types+1;
	alarm.level = level;
	alarm.state = 1;
	my_printf("%d-%d-%d-%d-%d-%d",alarm.alarmType,alarm.month,alarm.day,alarm.hour,alarm.minute,alarm.second);
	
	begin=APP_Persist_RecordIndex_Addr[0].recordBegin;
	end=APP_Persist_RecordIndex_Addr[0].recordEnd;//当前可写入告警序号
	write=APP_Persist_RecordIndex_Addr[0].allWrite;
	sector = end/113 +RECORD_SECTOR;//一个扇区113条记录,序号整除113获得的扇区的编号
	my_printf("begin%d end%d write%d sector%d",begin,end,write,sector);
	InitAlarmRecordFlash(&recflash,sector-RECORD_SECTOR);//初始化写flash
	my_printf("? check");
	recflash.sysAlarms[end%113] = alarm;//取余数获得在sector扇区中对应的告警编号 每个扇区113条
	my_printf("%d type ===%d",end%113,alarm.alarmType);
	if(write==0)//所有扇区都已经存在告警记录
	{
		modifyRecordIndex((begin+1)%RECORD_MAXIUM_NUM,(end+1)%RECORD_MAXIUM_NUM,0);//起始序号begin和写指针end同时移动
	}
	else
	{
		//my_printf("begin index");
		//OS_ENTER_CRITICAL();
		if(end+1!=RECORD_MAXIUM_NUM)
			modifyRecordIndex(begin,end+1,1);//写指针end移动即可，写指针end指向下一个可写告警的序号
		else
			modifyRecordIndex(begin,0,0);//所有区域已经填满则将写指针指向开始
		//OS_EXIT_CRITICAL();
		//my_printf("end index");
	}
	u32IAP_PrepareSectors(sector,sector);
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(sector,sector))
	{
		_DBG_("test earase");
	}
	u32IAP_PrepareSectors(sector,sector);
	error_num = u32IAP_CopyRAMToFlash(App_Persist_SysAlarmRecord_Base+(sector-RECORD_SECTOR)*(0x8000),(uint32_t)&recflash.sysAlarms[0].year,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("test copy ok");
	}
	my_printf("has written type=%d",getSysAlarm(end).alarmType);
}
SysAlarmTypeDef getSysAlarm(uint16_t number)
{
	SysAlarmTypeDef * recordAddr;
	uint16_t index;
	uint8_t sector ;
	SysAlarmTypeDef alarm;
	
	alarm.alarmType = 0xff;//0xff作为识别空的标识
	index = APP_Persist_RecordIndex_Addr[0].recordBegin+number;//对应编号
	index = index % RECORD_MAXIUM_NUM;//循环到开始点
	
	//如果是未覆盖全部，则直接判断是否有对应的告警序号
	if(APP_Persist_RecordIndex_Addr[0].allWrite==1)
	{
		_DBG_("NONE ONE");
		my_printf("get index=%d end=%d",index,APP_Persist_RecordIndex_Addr[0].recordEnd);
		if(index >= APP_Persist_RecordIndex_Addr[0].recordEnd) return alarm;
	}
	else
	{
		_DBG_("NONE TWO");
		my_printf("get index=%d end=%d",index,APP_Persist_RecordIndex_Addr[0].recordEnd);
		if(number>=RECORD_MAXIUM_NUM) return alarm;
	}
	
	sector = index/113;
	//my_printf("get index=%d sector=%d",index,sector);
	recordAddr = (SysAlarmTypeDef *)(App_Persist_SysAlarmRecord_Base + sector*0x8000);
	my_printf("xxxxx type=%d",recordAddr[index%113].alarmType);
	return recordAddr[index%113];
}
void resumeSysAlarm(AlarmTypes types)
{
	//当产生响应的告警时首先判断是否之前已经有该告警，如果有则更新第二个位置的最新告警
	uint32_t error_num;
	uint16_t begin,end,write;
	uint8_t sector;
	SysAlarmTypeDef alarm;
	RTCTime LocalTime;
	Flash_Rec_Type_Def recflash;
	
	LocalTime = RTCGetTime();//获得当前时间
	
	alarm.minute = LocalTime.RTC_Min;
	alarm.second = LocalTime.RTC_Sec;
	alarm.day    = LocalTime.RTC_Mday;
	alarm.hour   = LocalTime.RTC_Hour;
	alarm.year   = LocalTime.RTC_Year-2000 ;
	alarm.month  = LocalTime.RTC_Mon ;
	alarm.alarmType = types+1;
	alarm.level = Low;
	alarm.state = 0;
	//my_printf("%d-%d-%d-%d-%d-%d",alarm.year,alarm.month,alarm.day,alarm.hour,alarm.minute,alarm.second);
	
	begin=APP_Persist_RecordIndex_Addr[0].recordBegin;
	end=APP_Persist_RecordIndex_Addr[0].recordEnd;//当前可写入告警序号
	write=APP_Persist_RecordIndex_Addr[0].allWrite;
	sector = end/113 +RECORD_SECTOR;//一个扇区113条记录,序号整除113获得的扇区的编号
	//my_printf("begin%d end%d write%d sector%d",begin,end,write,sector);
	InitAlarmRecordFlash(&recflash,sector-RECORD_SECTOR);//初始化写flash
	//my_printf("? check");
	recflash.sysAlarms[end%113] = alarm;//取余数获得在sector扇区中对应的告警编号 每个扇区113条
	//my_printf("? ===");
	if(write==0)//所有扇区都已经存在告警记录
	{
		modifyRecordIndex((begin+1)%RECORD_MAXIUM_NUM,(end+1)%RECORD_MAXIUM_NUM,0);//起始序号begin和写指针end同时移动
	}
	else
	{
		//my_printf("begin index");
		//OS_ENTER_CRITICAL();
		if(end+1!=RECORD_MAXIUM_NUM)
			modifyRecordIndex(begin,end+1,1);//写指针end移动即可，写指针end指向下一个可写告警的序号
		else
			modifyRecordIndex(begin,0,0);//所有区域已经填满则将写指针指向开始
		//OS_EXIT_CRITICAL();
		//my_printf("end index");
	}
	u32IAP_PrepareSectors(sector,sector);
	if( IAP_STA_CMD_SUCCESS==u32IAP_EraseSectors(sector,sector))
	{
		_DBG_("test earase");
	}
	u32IAP_PrepareSectors(sector,sector);
	error_num = u32IAP_CopyRAMToFlash(App_Persist_SysAlarmRecord_Base+(sector-RECORD_SECTOR)*(0x8000),(uint32_t)&recflash.sysAlarms[0].year,1024);
	if(error_num == IAP_STA_CMD_SUCCESS)
	{
		_DBG_("test copy ok");
	}
}
void DelAllSysAlarm(void)
{
	modifyRecordIndex(0,0,1);	
}

void updateCmdEraeseState(void)//升级指令，擦写state扇区
{
	u32IAP_PrepareSectors(SECTOR_STATE,SECTOR_STATE);
	u32IAP_EraseSectors(SECTOR_STATE,SECTOR_STATE);
}
