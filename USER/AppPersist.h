#ifndef __APP_PERSIST_H__
#define __APP_PERSIST_H__
#include "IAP.h"
#include "LPC17xx.h"

/*******************************************************************************************
 *@description:ʹ��flash�洢�־û�������صĲ���
 *@history : 2013-3-14 v1.0
 *@author �ƶ���
 *@mail hityouok@qq.com
 *
 *******************************************************************************************/
 /*
  *�̶�ֵ�궨��
  *
  */
#define MONITOR_MODULE_ADDR 4
 
 /*flash���**********************************************************************************/
#define SECTOR 28 //������Ϣʹ�õ�28������
#define END_SECTOR 28
#define Persist_BASE_ADDR (0x00070000UL) //��ӦSECTOR �Ļ�׼��ַ


//�澯��¼���
#define RECORD_INDEX_SECTOR 24 //�澯ָ������

#define RECORD_SECTOR 25 //�澯��¼���� 
#define RECORD_END_SECTOR 27 //�澯��¼��������
#define Alarm_Record_Base_Addr  (0x00058000UL)
#define Alarm_Record_Index_Addr (0x00050000UL)
#define RECORD_MAXIUM_NUM 200

//����ָ�����,STATE����
#define SECTOR_STATE 11 

/****************************************************************************
 *�澯����Ϣ�����궨��
 ****************************************************************************/
#define ALARM_REC_TYPE_NUM 43
#define  ALARM_SET_NUM 22
#define ALARM_SHIFT_NUM 8
#define CONTROL_STATE_NUM 1
#define RELAY_STATE_NUM 8
#define SWITCH_STATE_NUM 8
#define FAN_NUM 4 //��������4
#define FIRE_NUM 4 //����������4
/*�����澯����******************************************************************************/
typedef enum AlarmTypesEnum{
	DCOver,//0 ֱ����ѹ�澯
	DCLack,//1 ֱ��Ƿѹ�澯
	ACOver,//2 ������ѹ�澯
	ACLack,//3 ����Ƿѹ�澯
	DCDown,//4 ֱ������澯
	ACDown,//5 ��������澯
	Thunder,//6���׸澯
	Water,   //7  ˮ���澯
	PowerCom,//8  ��ԴͨѶ�澯
	MDF,     //9   MDF�澯
	Shake,   //10   ײ���澯,
	Leap,    //11   ��б�澯������
	UPS,//12
	Temp1Over,//13  �¶�1���߸澯
	Temp1Lack,//14  �¶�1���͸澯
	Wet1Over,//15  ʪ��1���߸澯
	Wet1Lack,//16  ʪ��1���͸澯
	Temp2Over,//17  �¶�2���߸澯
	Temp2Lack,//18  �¶�2���͸澯
	Wet2Over, //19  ʪ��2���߸澯
	Wet2Lack, //20  ʪ��2���͸澯
	Temp3Over,//21  �¶�3���߸澯
	Temp3Lack,//22  �¶�3���͸澯
	Wet3Over,//23  ʪ��3���߸澯
	Wet3Lack,//24  ʪ��3���͸澯
	Temp4Over,//25  �¶�4���߸澯
	Temp4Lack,//26  �¶�4���͸澯
	Wet4Over,//27  ʪ��4���߸澯
	Wet4Lack,//28  ʪ��4���͸澯
	Smoke,   //29  ����澯
	SmokeTwo,//30
	SmokeThree,//31
	SmokeFour,//32
	Door,    //33  �Ž��澯
	DoorTwo,//34    �Ž�2
	DoorThree,//35  �Ž�3
	DoorFour,//36   �Ž�4
	UserS1,  //37  S1/�Զ���1�澯
	UserS2,//38 S2/�Զ���2�澯
} AlarmTypes;

//�澯��������
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

//�澯ƫ����������
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

//���ؿ������ͣ�һ��Ϊ0 1 
typedef enum ControlStateTypesEnum
{
	BeepState
}ControlStateTypes;

//��������������
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

//�澯����
typedef enum AlarmLevelEnum
{
	Forbidden,//����   0 
	Argent,//����      1
	Low,//��           2
	Resume,//�澯�ָ�   3
	NoAlarm=0xff
} AlarmLevel;

/*����ṹ��*************************************************************************/

//IP��ַ�־û�����ṹ��
typedef struct
{
	__IO uint8_t  IP_ADDR1;
	__IO uint8_t  IP_ADDR2;
	__IO uint8_t  IP_ADDR3;
	__IO uint8_t  IP_ADDR4;
	__IO uint16_t  ServerPort;
	__IO uint16_t  ClientPort;
	
} App_Persist_IP_Def;

//���ģ���ַ�־û�����ṹ��
typedef struct
{
	__IO uint8_t  Module_ADDR1;
	__IO uint8_t  Module_ADDR2;
} App_Persist_Module_Def;


//�澯���¼�־û�����ṹ��
typedef struct alarmSetRec
{
	float alarmSet;
} AlarmSetTypeDef;

//�澯��ƫ������־û�����ṹ��
typedef struct alarmShiftSetRec
{
	float alarmShift;
} AlarmShiftTypeDef;

//�澯���� ���𼰼̵�������
typedef struct alarmLevelRelayRec
{
	uint16_t level;//�澯����
	uint16_t relay;//�澯�����̵���
}AlarmLevelRelayTypeDef;

//״̬���ͣ����������״̬�ȵ�
typedef struct ControlStateRec
{
	uint8_t state;
}ControlStateTypeDef;

//�̵����������
typedef struct RelayStateRec
{
	uint8_t state;
}RelayStateTypeDef;

//�������������
typedef struct SwitchStateRec
{
	uint8_t state;
}SwitchStateTypeDef;

typedef struct EcuipContrtolArgRec
{
	uint8_t relay;
	float open;
	float sense;
}EcuipControlArgTypeDef;//��Ϊ�ֽڶ������⣬ռ�õ���12�ֽ�
//�澯��¼��д����ָ��
typedef struct sysAlarmIndexRec
{
	uint16_t recordBegin;//ָ����ʷ�澯�ʼ�ļ�¼
	uint16_t recordEnd;//ָ��ǰ��дλ��
	uint16_t  allWrite;//�����������Ѿ��и澯��¼����ʼ���� Ĭ��Ϊ��0��0��ʾ�Ѿ�������
}SysAlarmIndexTypeDef;

//flash����ṹ��;1024���ֽ� 1KB
typedef struct
{
	App_Persist_IP_Def ipAddr;//8
	App_Persist_Module_Def moduleAddr;//2
	ControlStateTypeDef    controlStates[10];//10*1 = 10bytes ����һЩԤ����
	RelayStateTypeDef      relayStates[20];//20 *1 = 20bytes ����һЩԤ����
	SwitchStateTypeDef     switchStates[20];//20 * 1 = 20 bytes ����һЩԤ����
	
	AlarmSetTypeDef alarmSets[50];//50 * 4 = 200bytes ����һЩԤ����
	AlarmShiftTypeDef alarmShifts[30];//30*4 = 120bytes ����һЩԤ����
	AlarmLevelRelayTypeDef alarmLevelRelays[50];//50*4 =200bytes ����һЩԤ����
	EcuipControlArgTypeDef fanControlArgs[6];//6*12 = 72bytes ����һЩԤ����
	EcuipControlArgTypeDef fireControlArgs[6];//6*12 = 72bytes ����һЩԤ����
 	uint8_t res[300];//Ԥ��300�ֽ�
} Flash_Type_Def;
//�澯��Ϣ�־û�����ṹ��
typedef struct sysAlarmTypeRec
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t alarmType;//�澯����
	uint8_t level;//�澯����
	uint8_t state;//�澯״̬
} SysAlarmTypeDef;

//ÿ������ʹ��1kb����
typedef struct
{
	SysAlarmTypeDef sysAlarms[113];//113*9 = 1017�ֽ�
	uint8_t res[7];//��������ȡ��Ч
}Flash_Rec_Type_Def;

//�澯��¼����
typedef struct
{
	SysAlarmIndexTypeDef recordIndex[1];//8bytes 
	uint8_t res[250];//250bytes ����
}Flash_Rec_Index_Type_Def;


/*�����ַ******************************************************************/

#define App_Persist_IP_BASE (Persist_BASE_ADDR)//ΪIP��ַ����8���ֽڣ�ǰ4���ֽ���ΪIP��ַ����4���ֽڷֱ�Ϊ����˺Ϳͻ��˶˿�
#define App_Persist_Module_BASE (Persist_BASE_ADDR + 8)//Ϊģ���ַ����2���ֽڣ�ǰ�����ֽ�Ϊģ���ַ
#define App_Persist_ControlState_Base (Persist_BASE_ADDR+10) //20bytes Ϊ����״̬������ֽڣ���20�� ÿ��һ���ֽ�
#define App_Persist_RelayState_Base    (Persist_BASE_ADDR+20)//Ϊ�̵���������ֽ� ��20�� 20���ֽ�
#define App_Persist_SwitchState_Base    (Persist_BASE_ADDR+40) //Ϊ������������ֽ� ��20�� 20���ֽ�
#define App_Persist_AlarmSet_Base (Persist_BASE_ADDR+60)//Ϊ�澯���÷���200���ֽڣ�ÿ������4���ֽ�
#define App_Persist_AlarmShift_Base (Persist_BASE_ADDR+260)//Ϊ�澯ƫ�Ʒ���120���ֽڣ�ÿ��4���ֽ�
#define App_Persist_AlarmLevelRelay_Base (Persist_BASE_ADDR+380)//Ϊ�澯��̵��������ȷ���200�ֽ� ÿ��4���ֽ�
#define App_Persist_FanControlArg_Base (Persist_BASE_ADDR+580)//Ϊ���Ȳ������÷����72�ֽڣ�ÿ��12�ֽ�
#define App_Persist_FireControlArg_Base (Persist_BASE_ADDR+652)//Ϊ�������������÷����72�ֽڣ�ÿ��12�ֽ�

//�澯ָ���ַ��׼
#define App_Persist_RecordIndex_Base (Alarm_Record_Index_Addr)//Ϊ�澯��¼ָ�����6���ֽ�
//�澯��¼��ַ��׼
#define App_Persist_SysAlarmRecord_Base (Alarm_Record_Base_Addr)//Ϊ�澯��¼�������ʼ��ַ

/*����ָ��***********************************************************************/

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

//�澯��¼��ʼ�澯ָ��
#define APP_Persist_SysAlarmRecord_Addr     ((SysAlarmTypeDef    		*) App_Persist_SysAlarmRecord_Base)

/*�ӿ�����**********************************************************************/
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

void updateCmdEraeseState(void);//����ָ���дstate����
#endif
