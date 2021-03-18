#include "AppTaskNetInit.h"
#include "AppDebug.h"

extern OS_EVENT * semNetInited;//�����Ƿ��Ѿ���ʼ��
void AppTaskNetInit(void * pdata)
{
	INT8U err;
	uint8_t macInited;
	
	pdata = pdata;
	while(1)
	{
		macInited=tapdev_init();
		if(macInited)
		{
			OSSemPost(semNetInited);
			OSTaskDel(OS_PRIO_SELF);//����ʼ��������ʱ�����ɾ��������
		}
	}
	OSTaskDel(OS_PRIO_SELF);//���������񴴽����ɾ��������
}