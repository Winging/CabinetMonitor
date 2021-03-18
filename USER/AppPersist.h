#ifndef __APP_PERSIST_H__
#define __APP_PERSIST_H__
#include "IAP.h"
#include "LPC17xx.h"

/*******************************************************************************************
 *@description:使用flash存储持久化保存相关的参数
 *@history : 2013-3-14 v1.0
 *@author 黄东润
 *@mail hityouok@qq.com
 *
 *******************************************************************************************/
 /*
  *固定值宏定义
  *
  */
#define MONITOR_MODULE_ADDR 4
 
 /*flash相关**********************************************************************************/
#define SECTOR 28 //公共信息使用第28个扇区
#define END_SECTOR 28
#define Persist_BASE_ADDR (0x00070000UL) //对应SECTOR 的基准地址


//告警记录相关
#define RECORD_INDEX_SECTOR 24 //告警指针扇区

#define RECORD_SECTOR 25 //告警记录扇区 
#define RECORD_END_SECTOR 27 //告警记录结束扇区
#define Alarm_Record_Base_Addr  (0x00058000UL)
#define Alarm_Record_Index_Addr (0x00050000UL)
#define RECORD_MAXIUM_NUM 200

//升级指令相关,STATE扇区
#define SECTOR_STATE 11 

/****************************************************************************
 *告警等信息总数宏定义
 ****************************************************************************/
#define ALARM_REC_TYPE_NUM 43
#define  ALARM_SET_NUM 22
#define ALARM_SHIFT_NUM 8
#define CONTROL_STATE_NUM 1
#define RELAY_STATE_NUM 8
#define SWITCH_STATE_NUM 8
#define FAN_NUM 4 //风扇总数4
#define FIRE_NUM 4 //加热器总数4
/*声明告警类型******************************************************************************/
typedef enum AlarmTypesEnum{
	DCOver,//0 直流过压告警
	DCLack,//1 直流欠压告警
	ACOver,//2 交流过压告警
	ACLack,//3 交流欠压告警
	DCDown,//4 直流掉电告警
	ACDown,//5 交流掉电告警
	Thunder,//6防雷告警
	Water,   //7  水浸告警
	PowerCom,//8  电源通讯告警
	MDF,     //9   MDF告警
	Shake,   //10   撞击告警,
	Leap,    //11   倾斜告警，不用
	UPS,//12
	Temp1Over,//13  温度1过高告警
	Temp1Lack,//14  温度1过低告警
	Wet1Over,//15  湿度1过高告警
	Wet1Lack,//16  湿度1过低告警
	Temp2Over,//17  温度2过高告警
	Temp2Lack,//18  温度2过低告警
	Wet2Over, //19  湿度2过高告警
	Wet2Lack, //20  湿度2过低告警
	Temp3Over,//21  温度3过高告警
	Temp3Lack,//22  温度3过低告警
	Wet3Over,//23  湿度3过高告警
	Wet3Lack,//24  湿度3过低告警
	Temp4Over,//25  温度4过高告警
	Temp4Lack,//26  温度4过低告警
	Wet4Over,//27  湿度4过高告警
	Wet4Lack,//28  湿度4过低告警
	Smoke,   //29  烟雾告警
	SmokeTwo,//30
	SmokeThree,//31
	SmokeFour,//32
	Door,    //33  门禁告警
	DoorTwo,//34    门禁2
	DoorThree,//35  门禁3
	DoorFour,//36   门禁4
	UserS1,  //37  S1/自定义1告警
	UserS2,//38 S2/自定义2告警
} AlarmTypes;

//告警设置类型
typedef enum AlarmSetTypesEnum
{
	DCOverSet,   //0
	DCLackSet,   //1
	DCDownSet,   //2
	ACOverSet,   //3
	ACLackSet,   //4
	ACDownSet,   //5
	Temp1OverSet,//6
	Temp1LackSet,//7
	Wet1OverSet,//8
	Wet1LackSet,//9
	Temp2OverSet,//10
	Temp2LackSet,//11
	Wet2OverSet,//12
	Wet2LackSet,//13
	Temp3OverSet,//14
	Temp3LackSet,//15
	Wet3OverSet,//16
	Wet3LackSet,//17
	Temp4OverSet,//18
	Temp4LackSet,//19
	Wet4OverSet,//20
	Wet4LackSet,//21
} AlarmSetTypes;

//告警偏移增益类型
typedef enum AlarmShiftTypesEnum
{
	TempAdd,
	TempMinus,
	WetAdd,
	WetMinus,
	DCAdd,
	DCMinus,
	ACAdd,
	ACMinus
} AlarmShiftTypes;

//主控控制类型，一般为0 1 
typedef enum ControlStateTypesEnum
{
	BeepState
}ControlStateTypes;

//开关量控制类型
typedef enum SwitchStateTypesEnum
{
	ThunderState,
	WaterState,
	SmokeState,
	DoorState,
	DoorTwoState,
	DoorThreeState,
	DoorFourState,
	SmokeTwoState,
	SmokeThreeState,
	SmokeFourState,
}SwitchStateTypes;

//告警级别
typedef enum AlarmLevelEnum
{
	Forbidden,//禁用   0 
	Argent,//紧急      1
	Low,//低           2
	Resume,//告警恢复   3
	NoAlarm=0xff
} AlarmLevel;

/*定义结构体*************************************************************************/

//IP地址持久化保存结构体
typedef struct
{
	__IO uint8_t  IP_ADDR1;
	__IO uint8_t  IP_ADDR2;
	__IO uint8_t  IP_ADDR3;
	__IO uint8_t  IP_ADDR4;
	__IO uint16_t  ServerPort;
	__IO uint16_t  ClientPort;
	
} App_Persist_IP_Def;

//监控模块地址持久化保存结构体
typedef struct
{
	__IO uint8_t  Module_ADDR1;
	__IO uint8_t  Module_ADDR2;
} App_Persist_Module_Def;


//告警点记录持久化保存结构体
typedef struct alarmSetRec
{
	float alarmSet;
} AlarmSetTypeDef;

//告警量偏移增益持久化保存结构体
typedef struct alarmShiftSetRec
{
	float alarmShift;
} AlarmShiftTypeDef;

//告警类型 级别及继电器关联
typedef struct alarmLevelRelayRec
{
	uint16_t level;//告警级别
	uint16_t relay;//告警关联继电器
}AlarmLevelRelayTypeDef;

//状态类型，比如蜂鸣器状态等等
typedef struct ControlStateRec
{
	uint8_t state;
}ControlStateTypeDef;

//继电器输出类型
typedef struct RelayStateRec
{
	uint8_t state;
}RelayStateTypeDef;

//开关量输出类型
typedef struct SwitchStateRec
{
	uint8_t state;
}SwitchStateTypeDef;

typedef struct EcuipContrtolArgRec
{
	uint8_t relay;
	float open;
	float sense;
}EcuipControlArgTypeDef;//因为字节对齐问题，占用的是12字节
//告警记录读写索引指针
typedef struct sysAlarmIndexRec
{
	uint16_t recordBegin;//指向历史告警最开始的记录
	uint16_t recordEnd;//指向当前可写位置
	uint16_t  allWrite;//所有扇区都已经有告警记录，则开始覆盖 默认为非0，0表示已经覆盖了
}SysAlarmIndexTypeDef;

//flash区域结构体;1024个字节 1KB
typedef struct
{
	App_Persist_IP_Def ipAddr;//8
	App_Persist_Module_Def moduleAddr;//2
	ControlStateTypeDef    controlStates[10];//10*1 = 10bytes 包括一些预留的
	RelayStateTypeDef      relayStates[20];//20 *1 = 20bytes 包括一些预留的
	SwitchStateTypeDef     switchStates[20];//20 * 1 = 20 bytes 包括一些预留的
	
	AlarmSetTypeDef alarmSets[50];//50 * 4 = 200bytes 包括一些预留的
	AlarmShiftTypeDef alarmShifts[30];//30*4 = 120bytes 包括一些预留的
	AlarmLevelRelayTypeDef alarmLevelRelays[50];//50*4 =200bytes 包括一些预留的
	EcuipControlArgTypeDef fanControlArgs[6];//6*12 = 72bytes 包括一些预留的
	EcuipControlArgTypeDef fireControlArgs[6];//6*12 = 72bytes 包括一些预留的
 	uint8_t res[300];//预留300字节
} Flash_Type_Def;
//告警信息持久化保存结构体
typedef struct sysAlarmTypeRec
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t alarmType;//告警类型
	uint8_t level;//告警级别
	uint8_t state;//告警状态
} SysAlarmTypeDef;

//每个扇区使用1kb数据
typedef struct
{
	SysAlarmTypeDef sysAlarms[113];//113*9 = 1017字节
	uint8_t res[7];//保留，读取无效
}Flash_Rec_Type_Def;

//告警记录数据
typedef struct
{
	SysAlarmIndexTypeDef recordIndex[1];//8bytes 
	uint8_t res[250];//250bytes 保留
}Flash_Rec_Index_Type_Def;


/*分配地址******************************************************************/

#define App_Persist_IP_BASE (Persist_BASE_ADDR)//为IP地址分配8个字节，前4个字节作为IP地址，后4个字节分别为服务端和客户端端口
#define App_Persist_Module_BASE (Persist_BASE_ADDR + 8)//为模块地址分配2个字节，前两个字节为模块地址
#define App_Persist_ControlState_Base (Persist_BASE_ADDR+10) //20bytes 为控制状态分配的字节，共20条 每条一个字节
#define App_Persist_RelayState_Base    (Persist_BASE_ADDR+20)//为继电器分配的字节 共20条 20个字节
#define App_Persist_SwitchState_Base    (Persist_BASE_ADDR+40) //为开关量分配的字节 共20条 20个字节
#define App_Persist_AlarmSet_Base (Persist_BASE_ADDR+60)//为告警设置分配200个字节，每条设置4个字节
#define App_Persist_AlarmShift_Base (Persist_BASE_ADDR+260)//为告警偏移分配120个字节，每条4个字节
#define App_Persist_AlarmLevelRelay_Base (Persist_BASE_ADDR+380)//为告警与继电器关联等分配200字节 每条4个字节
#define App_Persist_FanControlArg_Base (Persist_BASE_ADDR+580)//为风扇参数设置分配的72字节，每条12字节
#define App_Persist_FireControlArg_Base (Persist_BASE_ADDR+652)//为加热器参数设置分配的72字节，每条12字节

//告警指针地址基准
#define App_Persist_RecordIndex_Base (Alarm_Record_Index_Addr)//为告警记录指针分配6个字节
//告警记录地址基准
#define App_Persist_SysAlarmRecord_Base (Alarm_Record_Base_Addr)//为告警记录分配的起始地址

/*定义指针***********************************************************************/

#define APP_Persist_IP_Addr                 ((App_Persist_IP_Def        *) App_Persist_IP_BASE  )
#define APP_Persist_Module_Addr             ((App_Persist_Module_Def    *) App_Persist_Module_BASE )
#define APP_Persist_AlarmRec_Addr           ((AlarmTypeDef              *) App_Persist_AlarmRec_Base)
#define APP_Persist_AlarmSet_Addr	        ((AlarmSetTypeDef           *) App_Persist_AlarmSet_Base)
#define APP_Persist_AlarmShift_Addr         ((AlarmShiftTypeDef         *) App_Persist_AlarmShift_Base)
#define APP_Persist_AlarmLevelRelay_Addr    ((AlarmLevelRelayTypeDef    *) App_Persist_AlarmLevelRelay_Base)
#define APP_Persist_ControlState_Addr       ((ControlStateTypeDef       *) App_Persist_ControlState_Base)  
#define APP_Persist_RelayState_Addr         ((RelayStateTypeDef         *) App_Persist_RelayState_Base)  
#define APP_Persist_SwitchState_Addr        ((SwitchStateTypeDef        *) App_Persist_SwitchState_Base)
#define APP_Persist_FanControlArg_Addr      ((EcuipControlArgTypeDef    *) App_Persist_FanControlArg_Base)
#define APP_Persist_FireControlArg_Addr     ((EcuipControlArgTypeDef    *) App_Persist_FireControlArg_Base)
#define APP_Persist_RecordIndex_Addr     	((SysAlarmIndexTypeDef    	*) App_Persist_RecordIndex_Base)

//告警记录起始告警指针
#define APP_Persist_SysAlarmRecord_Addr     ((SysAlarmTypeDef    		*) App_Persist_SysAlarmRecord_Base)

/*接口声明**********************************************************************/
void PrepareSector(void);
void modifyIPAddr(App_Persist_IP_Def  IpType);
void modifyModuleAddr(App_Persist_Module_Def  ModuleType);
void modifyAlarmSet(AlarmSetTypes types,float alarmSet);
void modifyAlarmShift(AlarmShiftTypes types,float alarmShift);
void modifyAlarmLevelRelay(AlarmTypes types,uint8_t level,uint8_t relay);
void modifyControlState(ControlStateTypes types,uint8_t state);
void modifyRelayState(uint8_t types,uint8_t state);
void modifySwitchState(SwitchStateTypes types,uint8_t state);
void modifyFanArg(uint8_t fanNumber,uint8_t relayNumber,float open,float sense);
void modifyFireArg(uint8_t fireNumber,uint8_t relayNumber,float open,float sense);
void modifyRecordIndex(uint32_t begin,uint32_t end,uint8_t allWrite);
void beginSysAlarm(AlarmTypes types,AlarmLevel level);
void resumeSysAlarm(AlarmTypes types);
SysAlarmTypeDef getSysAlarm(uint16_t number);
void DelAllSysAlarm(void);

void updateCmdEraeseState(void);//升级指令，擦写state扇区
#endif
