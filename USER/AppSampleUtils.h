#ifndef __APP_SAMPLE_UTILS_H__
#define __APP_SAMPLE_UTILS_H__

#include "includes.h"


//��ѹ�ṹ�塣1��ÿ����Աֵλ�ַ����ͣ�0-9��2���� ����ֵλ "AAA"ʱ��ʾδ��
//��  "123"��ʾ12.3V
typedef struct VoltageRec
{
    char a1,a2,a3;
} Type_Of_Voltage;
//�¶Ƚṹ�壬ÿ����ԱΪ�ַ����ͣ�0-9��
//���� "050"��ʾ50 C��"240" ��ʾ-40 C��
typedef struct  TempratureRec
{
		char a1,a2,a3;
} Type_Of_Temprature;
/*�ڲ������������ȡ��ѹ���Ե����������*****************************************************************/

/*��ȡ״̬************************************************************/
Type_Of_Voltage      ReadStatus_GetVoltage();
Type_Of_Temprature   ReadStatus_GetTemprate();
uint8_t              ReadStatus_GetDoorState();
uint8_t              ReadStatus_GetElecState();
uint8_t              ReadStatus_GetAlarmFlag();
/* ����������Ϣ*************************************/
uint8_t              SetDoor_SetVoltageOver(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetVoltageLack(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetVoltageLess(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetFanTemp(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetMainCold(uint8_t a1,uint8_t a2,uint8_t a3);
uint8_t              SetDoor_SetMainHot(uint8_t a1,uint8_t a2,uint8_t a3);
#endif
