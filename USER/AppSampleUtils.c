#include "AppSampleUtils.h"

/*�ڲ���ʶ�����������ȡ��ѹ���Ե����������******************************************************/


/*��ȡ״̬***���鶨��*********************************************************/
/*
 * ��������ȡ48V��ѹ
 *���� "000"-"999" "AAA"��ʾδ�ã� ��535 ��ʾ53.5v 
 */
Type_Of_Voltage      ReadStatus_GetVoltage()
{
	Type_Of_Voltage v;
	v.a1 = 'A';
	v.a2 = 'A';
	v.a3 = 'A';
	return v;
}
/*
 *��������ȡ�¶� 1
 *���� "000"-"999" �� "050" ��ʾ50C "250"��ʾ-50C
 */
Type_Of_Temprature   ReadStatus_GetTemprate()
{
	Type_Of_Temprature t;
	t.a1 = '0';
	t.a2 = '0';
	t.a3 = '0';
	return t;
}
/*
 *�������Ž�״̬
 *���أ�0 ������1 ͣ�� 'A' δ����
 */
uint8_t               ReadStatus_GetDoorState()
{
	return 0;
}
/*
 *�������е�״̬
 *���أ�0 ������1 ͣ�� 'A' δ����
 */
uint8_t              ReadStatus_GetElecState()
{

}
/*
 *�������澯��־
 *���أ�0 �� ��1 ��δ�ϴ�
 */
uint8_t              ReadStatus_GetAlarmFlag()
{
	return 0;
}



/* ����������Ϣ*************************************/
/*
 *����������48v��ѹ��ѹֵ
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "480" Ϊ48.0V
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetVoltageOver(uint8_t a1,uint8_t a2,uint8_t a3)
{
	//ʵ��
	return 1;
}
/*
 *����������48v��ѹǷѹֵ
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "480" Ϊ48.0V
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetVoltageLack(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *����������48v��ѹ����ѹֵ
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "480" Ϊ48.0V
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetVoltageLess(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *���������÷����Ƚ����¶�
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "080" Ϊ80C "250"Ϊ-50C
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetFanTemp(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *���������豸������
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "080" Ϊ80C "250"Ϊ-50C
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetMainCold(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *���������豸������
 *���� a1a2a3 Ϊ�ַ� "000"-"999" ���� "080" Ϊ80C "250"Ϊ-50C
 *���أ�1��ʾ�ɹ� 0��ʾʧ��
 *
 */
uint8_t              SetDoor_SetMainHot(uint8_t a1,uint8_t a2,uint8_t a3);
