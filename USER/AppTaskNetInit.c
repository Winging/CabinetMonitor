#include "AppTaskNetInit.h"
#include "AppDebug.h"

extern OS_EVENT * semNetInited;//网络是否已经初始化
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
			OSTaskDel(OS_PRIO_SELF);//当初始化完网络时则可以删除此任务
		}
	}
	OSTaskDel(OS_PRIO_SELF);//当所有任务创建完成删除本任务
}