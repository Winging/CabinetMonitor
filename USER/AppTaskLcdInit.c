#include "AppTaskLcdInit.h"
#include "LEDControl.h"

extern OS_EVENT * semLCDInited;//lcd是否已经初始化
void AppTaskLCDInit(void * pdata)
{
	INT8U err;
	while(1)
	{
		INILCDConrol();
		OSSemPost(semLCDInited);
		OSTaskDel(OS_PRIO_SELF);//如果已经初始化则删除本任务
	}
	OSTaskDel(OS_PRIO_SELF);//当所有任务创建完成删除本任务
}
