#include "Relay.h"
#include "AppPersist.h"
#include "AppDebug.h"

void InitRelay(void)
{
	GPIO_SetDir(0, (1<<19), 1);//k1
	GPIO_SetDir(0, (1<<20), 1);//k2
	GPIO_SetDir(0, (1<<21), 1);//k3
	GPIO_SetDir(0, (1<<22), 1);//k4
/*	
	if(APP_Persist_RelayState_Addr[0].state==1)
	{
		GPIO_ClearValue(2, (1<<13));
	}
	else
	{
		GPIO_SetValue(2, (1<<13));
	}
	
	if(APP_Persist_RelayState_Addr[1].state==1)
	{
		GPIO_ClearValue(2, (1<<12));
	}
	else 
	{
		GPIO_SetValue(2, (1<<12));
	}
	
	if(APP_Persist_RelayState_Addr[2].state==1)
	{
		GPIO_ClearValue(2, (1<<11));
	}
	else
	{
		GPIO_SetValue(2, (1<<11));
	}
	
	if(APP_Persist_RelayState_Addr[3].state==1)
	{
		GPIO_ClearValue(2, (1<<10));
	}
	else
	{
		GPIO_SetValue(2, (1<<10));
	}
	
	if(APP_Persist_RelayState_Addr[4].state==1)
	{
		GPIO_ClearValue(0, (1<<18));
	}
	else
	{
		GPIO_SetValue(0, (1<<18));
	}
	
	if(APP_Persist_RelayState_Addr[5].state==1)
	{
		GPIO_ClearValue(0, (1<<19));
	}
	else
	{
		GPIO_SetValue(0, (1<<19));
	}
	
	if(APP_Persist_RelayState_Addr[6].state==1)
	{
		GPIO_ClearValue(0, (1<<21));
	}
	else
	{
		GPIO_SetValue(0, (1<<21));
	}
	
	if(APP_Persist_RelayState_Addr[7].state==1)
	{
		GPIO_ClearValue(0, (1<<22));
	}
	else
	{
		GPIO_SetValue(0, (1<<22));
	}
*/	
	_DBG_("OK?");
}
void AlarmRelayOut(uint8_t number)
{
	//_DBG_("ALARM TO relay");
	if(APP_Persist_RelayState_Addr[number].state==1)
	{
		RelayOutput(number+1,1);
	//	sprintf(_db,"open relay = %d ",number);
		//DB;
	}
	else
	{
		RelayOutput(number+1,0);
	//	sprintf(_db,"close relay = %d ",number);
		//DB;
	}
}
void AlarmResumeRelayOut(uint8_t number)
{
	//_DBG_("resume TO relay");
	if(APP_Persist_RelayState_Addr[number].state==1)
	{
		RelayOutput(number+1,0);
	//	sprintf(_db,"close relay = %d ",number);
		//DB;
	}
	else
	{
		RelayOutput(number+1,1);
		//sprintf(_db,"open relay = %d ",number);
		//DB;
	}
}
void RelayOutput(uint8_t number,uint8_t output)
{

	if(output==1)
	{
		switch(number)
		{
			case 1:
			{
				//GPIO_SetDir(2, (1<<13), 1);//k1
				GPIO_ClearValue(0, (1<<19));
				break;
			}
			case 2:
			{
				//GPIO_SetDir(2, (1<<12), 1);//k2
				GPIO_ClearValue(0, (1<<20));
				break;
			}
			case 3:
			{
				//GPIO_SetDir(2, (1<<11), 1);//k3
				GPIO_ClearValue(0, (1<<21));
				break;
			}
			case 4:
			{
				//GPIO_SetDir(2, (1<<10), 1);//k4
				GPIO_ClearValue(0, (1<<22));
				break;
			}
	/*		case 5:
			{
				//GPIO_SetDir(0, (1<<18), 1);//k5
				GPIO_ClearValue(0, (1<<18));
				break;
			}
			case 6:
			{
				//GPIO_SetDir(0, (1<<19), 1);//k6
				GPIO_ClearValue(0, (1<<19));
				break;
			}
			case 7:
			{
				//GPIO_SetDir(0, (1<<21), 1);//k7
				GPIO_ClearValue(0, (1<<21));
				break;
			}
			case 8:
			{
				//GPIO_SetDir(0, (1<<22), 1);//k7
				GPIO_ClearValue(0, (1<<22));
				break;
			}*/
		}
	}
	else if(output==0)
	{
		switch(number)
		{
			case 1:
			{
				//GPIO_SetDir(2, (1<<13), 1);//k1
				GPIO_SetValue(0, (1<<19));
				break;
			}
			case 2:
			{
				//GPIO_SetDir(2, (1<<12), 1);//k2
				GPIO_SetValue(0, (1<<20));
				break;
			}
			case 3:
			{
				//GPIO_SetDir(2, (1<<11), 1);//k3
				GPIO_SetValue(0, (1<<21));
				break;
			}
			case 4:
			{
				//GPIO_SetDir(2, (1<<10), 1);//k4
				GPIO_SetValue(0, (1<<22));
				break;
			}
		/*	case 5:
			{
				//GPIO_SetDir(0, (1<<18), 1);//k5
				GPIO_SetValue(0, (1<<18));
				break;
			}
			case 6:
			{
				//GPIO_SetDir(0, (1<<19), 1);//k6
				GPIO_SetValue(0, (1<<19));
				break;
			}
			case 7:
			{
				//GPIO_SetDir(0, (1<<21), 1);//k7
				GPIO_SetValue(0, (1<<21));
				break;
			}
			case 8:
			{
				//GPIO_SetDir(0, (1<<22), 1);//k8
				GPIO_SetValue(0, (1<<22));
				break;
			}*/
		}
		
	}
}