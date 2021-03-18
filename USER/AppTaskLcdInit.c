#include "AppTaskLcdInit.h"
#include "LEDControl.h"

extern OS_EVENT * semLCDInited;//lcd�Ƿ��Ѿ���ʼ��
void AppTaskLCDInit(void * pdata)
{
	INT8U err;
	while(1)
	{
		INILCDConrol();
		OSSemPost(semLCDInited);
		OSTaskDel(OS_PRIO_SELF);//����Ѿ���ʼ����ɾ��������
	}
	OSTaskDel(OS_PRIO_SELF);//���������񴴽����ɾ��������
}
