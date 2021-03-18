#include "Beep.h"
#include "AppDebug.h"

void BeepInit()
{
	GPIO_SetDir(3, (1<<26), 1);
}
void BeepOn()
{
	if(GetBeepState()==1)
	{
		//GPIO_SetValue(3, (1<<26));//¾É°å×Ó25 ÐÂ°à×Ó 26
		_DBG_("BEEP ON");
	}
	//GPIO_SetValue(3, (1<<25));
	//_DBG_("BEEP ON");
}
void BeepOff()
{
	 GPIO_ClearValue(3, (1<<26));
	_DBG_("BEEP Off");
}
void BeepOpen()
{
	modifyControlState(BeepState,1);
}
void BeepLock()
{
	modifyControlState(BeepState,0);
}
uint8_t GetBeepState()
{
	return APP_Persist_ControlState_Addr[BeepState].state;
}
