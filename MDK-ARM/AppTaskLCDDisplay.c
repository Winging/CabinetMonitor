#include "AppTaskLCDDisplay.h"
#include "AppPersist.h"

#define MenuOne_Begin 0
#define MenuOne_End 7
#define MenuOne_Alarm 6
#define MenuOne_TEMP1 0
#define MenuOne_WET1  1
#define MenuOne_TEMP2 2
#define MenuOne_WET2  3
#define MenuOne_AC    4
#define MenuOne_DC    5
#define MenuOne_SYSTIME 6
#define MenuTwo_Alarm_Begin 0
#define MenuTwo_Alarm_End 2
#define MenuTwo_Alarm_Content 0
#define MenuTwo_Alarm_Record 1
#define MenuTwo_Alarm_Return 2
#define MenuThree_Alarm_Content_Begin 0
#define MenuThree_Alarm_Content_End 25
#define MenuThree_Alarm_Content_Return 25

#define MenuOne_Setting 7
#define MenuTwo_Setting_Begin 3
#define MenuTwo_Setting_AlarmSetS 16
#define MenuTwo_Setting_IP 17
#define MenuTwo_Setting_Switch 22
#define MenuTwo_Setting_End 23
#define MenuTwo_Setting_Return 23

#define MenuFour_Item_Begin 0
#define MenuFour_Item_End 2
#define MenuFour_Item_Save 0
#define MenuFour_Item_Exit 1
#define MenuFour_Item_Return 2

extern char data_Humidity;
extern char data_Temperature;

extern unsigned char menuOne[8][5];
extern unsigned char menuTwo[24][9];
extern unsigned  char menuThree[26][6];
extern unsigned  char menuFour[3][2];
extern unsigned char menuState[4][2];
extern unsigned char menuWelcome[1][4];
uint8_t menuOneItems = 7;
uint8_t menuTwoItems = 17;
uint8_t menuThreeItems = 25;
//contextMenu[0] 保存的是一级菜单当前选项，
//contextMenu[1]保存的是二级菜单当前选项，
//contextMenu[2]保存的是三级菜单当前选项
//contextMenu[3]保存的是四级菜单当前选项
int8_t contextMenu[4]={0};
int8_t currentMenuIndex;
static float alarmSet;
static uint8_t addr1,addr2,addr3,addr4,editNow;
static uint16_t port;
static int16_t numberRec;
static uint8_t noRec;
static uint8_t switchState;
extern OS_EVENT * mBoxKeyCode;//短消息，按键编码
extern OS_EVENT * semLCDInited;//lcd是否已经初始化

void menuOneOperate(uint32_t keyCode);
void menuTwoOperate(uint32_t keyCode);
void menuThreeOperate(uint32_t keyCode);
void menuFourOperate(uint32_t keyCode);
void AppTaskLCDDisplay(void * pdata)
{
	uint32_t keyCode;
	CPU_SR cpu_sr;
	INT8U err;
	clock_time_t hitTime,disTime,clockDisTime;
	
	pdata = pdata;
	//OSSemPend(semLCDInited,0,&err);//如果后面有什么更好的办法可以解决LCD初始化问题，则可以用pend
	_DBG_("LCD INIT");
	//等待LCD初始化
	
	INILCDConrol();
	_DBG_("LCD START");
	//如果有告警，则显示告警记录
	clrscr();
	DisplayMenu(menuWelcome[0],4);
	DisplayVersion(SOFTWARE_VERSION_MAIN,SOFTWARE_VERSION_SECOND);
	OSTimeDly(2000);
	if(DisplayAlarmRecord(0,6)==1)
	{
		
		currentMenuIndex = 3;
		noRec = 0;
		numberRec = 0;
		contextMenu[1]=MenuTwo_Alarm_Record;
	}
	else
	{
		currentMenuIndex = 1;
		DisplayMenu(menuOne[0],5);
		DisplaySetting(getTemp(1),1);
	}
	
	/*
	currentMenuIndex = 1;
	contextMenu[0] = MenuOne_Setting;
	DisplayMenu(menuOne[contextMenu[0]],5);
	_DBG_("D?");
	*/
	clockDisTime = OSTimeGet();
	while(1)
	{
		keyCode = (uint32_t)(OSMboxAccept(mBoxKeyCode))&0xffff;
		if(keyCode==0 && (OSTimeGet()-hitTime)>60000)
			CLR_BKEN;
		
		//如果大于0 则说明有按键
		if(keyCode>0)
		{
			SET_BKEN;//开灯啦 啦啦啦啦啦
			hitTime = OSTimeGet();
			sprintf(_db,"keycode =%d cur=%d",keyCode,currentMenuIndex);
			DB;
			if(currentMenuIndex == 1)
			{
				menuOneOperate(keyCode);
			}
			else if(currentMenuIndex == 2)
			{
				menuTwoOperate(keyCode);
			}
			else if(currentMenuIndex == 3)
			{
				sprintf(_db,"[%d %d %d]",contextMenu[0],contextMenu[1],contextMenu[2]);
				DB;
				menuThreeOperate(keyCode);
			}
			else if(currentMenuIndex == 4)
			{
				menuFourOperate(keyCode);
			}
		}
		if(currentMenuIndex==1&&contextMenu[0]==MenuOne_SYSTIME)
		{
			if(OSTimeGet()-clockDisTime>100)
			{
				clockDisTime = OSTimeGet();
				DisplayTime(LPC_RTC->YEAR,LPC_RTC->MONTH,LPC_RTC->DOM,
						LPC_RTC->HOUR,LPC_RTC->MIN,LPC_RTC->SEC);
			}
		}
		else
		if(currentMenuIndex==1&&contextMenu[0]!=MenuOne_SYSTIME)
		{
			if(OSTimeGet()-disTime>500)
			{
				disTime = OSTimeGet();
			switch(contextMenu[0])
			{
				case MenuOne_TEMP1:
				{
					DisplaySetting(getTemp(1),1);
					break;
				}
				case MenuOne_WET1:
				{
					DisplaySetting(getWet(1),2);
					break;
				}
				case MenuOne_TEMP2:
				{
					DisplaySetting(getTemp(2),1);
					break;
				}
				case MenuOne_WET2:
				{
					DisplaySetting(getWet(2),2);
					break;
				}
				case MenuOne_AC:
				{
					DisplaySetting(0,3);
					break;
				}
				case MenuOne_DC:
				{
					DisplaySetting(0,3);
					break;
				}
				
			}
			}
		
		}
		
		WDT_Feed();
		//_DBG_("t");
		OSTimeDly(5);
		//_DBG_("r");
	}
}
void menuOneOperate(uint32_t keyCode)
{
	CPU_SR cpu_sr;
	
	if(keyCode==keyUp)
	{
		clrscr();
		contextMenu[0] ++;
		sprintf(_db,"context =%d",contextMenu[0]);
		DB;
		if(contextMenu[0]>MenuOne_End) contextMenu[0] = MenuOne_Begin;
		DisplayMenu(menuOne[contextMenu[0]],5);
		switch(contextMenu[0])
		{
			case MenuOne_TEMP1:
			{
				DisplaySetting(getTemp(1),1);
				break;
			}
			case MenuOne_WET1:
			{
				DisplaySetting(getWet(1),1);
				break;
			}
			case MenuOne_TEMP2:
			{
				DisplaySetting(getTemp(2),1);
				break;
			}
			case MenuOne_WET2:
			{
				DisplaySetting(getWet(2),2);
				break;
			}
			case MenuOne_AC:
			{
				DisplaySetting(0,3);
				break;
			}
			case MenuOne_DC:
			{
				DisplaySetting(0,3);
				break;
			}
		}
	}
	else if(keyCode==keyDown)
	{
		clrscr();
		contextMenu[0] --;
		if(contextMenu[0]<MenuOne_Begin) contextMenu[0] = MenuOne_End;
		DisplayMenu(menuOne[contextMenu[0]],5);
		switch(contextMenu[0])
		{
			case MenuOne_TEMP1:
			{
				DisplaySetting(getTemp(1),1);
				break;
			}
			case MenuOne_WET1:
			{
				DisplaySetting(getWet(1),2);
				break;
			}
			case MenuOne_TEMP2:
			{
				DisplaySetting(getTemp(2),1);
				break;
			}
			case MenuOne_WET2:
			{
				DisplaySetting(getWet(2),2);
				break;
			}
			case MenuOne_AC:
			{
				DisplaySetting(0,3);
				break;
			}
			case MenuOne_DC:
			{
				DisplaySetting(0,3);
				break;
			}
		}
	}
	else if(keyCode==keyOk)
	{
		clrscr();
		//进入告警记录-告警内容--一级菜单项
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			currentMenuIndex = 2;
			contextMenu[1] = MenuTwo_Alarm_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		//设置-------一级菜单项
		else if(contextMenu[0] == MenuOne_Setting)
		{
			currentMenuIndex = 2;
			contextMenu[1] = MenuTwo_Setting_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
			alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
			my_printf("org = %f",alarmSet);
			if(alarmSet>8000||alarmSet<-8000)
			{
				OS_ENTER_CRITICAL();
				modifyAlarmSet(contextMenu[1]-MenuTwo_Setting_Begin,0);
				OS_EXIT_CRITICAL();
				alarmSet = 0;
			}
			DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			
		}
	}
	/*
	else if(keyCode==keyLeft)
	{
		DisplaySetting(245,5,1);
	}
	else if(keyCode == keyRight)
	{
		DisplaySetting(-20,3,2);
	}
	else if(keyCode == keyCancel)
	{
		DisplaySetting(1,3,3);
		//clrBelow();
	}
	*/
}
void menuTwoOperate(uint32_t keyCode)
{
	App_Persist_IP_Def ip;
	CPU_SR cpu_sr;
	
	if(keyCode==keyUp)
	{
		clrscr();
		
		//如果上级是告警
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			contextMenu[1] ++;
			if(contextMenu[1]>MenuTwo_Alarm_End) contextMenu[1] = MenuTwo_Alarm_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		else //如果上级是设置
		if(contextMenu[0]==MenuOne_Setting)
		{
			contextMenu[1] ++;
			if(contextMenu[1]>MenuTwo_Setting_End) contextMenu[1] = MenuTwo_Setting_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
			if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				DisplayMenu(menuTwo[contextMenu[1]],9);
				alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
				my_printf("org = %f",alarmSet);
				if(alarmSet>8000||alarmSet<-8000)
				{
					OS_ENTER_CRITICAL();
					modifyAlarmSet(contextMenu[1]-MenuTwo_Setting_Begin,0);
					OS_EXIT_CRITICAL();
					alarmSet = 0;
				}
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
				
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				DisplayMenu(menuTwo[contextMenu[1]],9);
				//contextMenu[1] - MenuTwo_Setting_IP - 1从0开始到4
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState>1||switchState<0)
				{
					OS_ENTER_CRITICAL();
					modifySwitchState(contextMenu[1] - MenuTwo_Setting_IP-1,0);
					OS_EXIT_CRITICAL();
					switchState=0;
				}
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
			}
			else if(contextMenu[1]== MenuTwo_Setting_IP)
			{
				addr1=APP_Persist_IP_Addr->IP_ADDR1;
				addr2=APP_Persist_IP_Addr->IP_ADDR2;
				addr3=APP_Persist_IP_Addr->IP_ADDR3;
				addr4=APP_Persist_IP_Addr->IP_ADDR4;
				port=APP_Persist_IP_Addr->ServerPort;
				if(port>9999)
				{
					port = 1234;
					ip.IP_ADDR1 = addr1;
					ip.IP_ADDR2 = addr2;
					ip.IP_ADDR3 = addr3;
					ip.IP_ADDR4 = addr4;
					ip.ServerPort = port;
					OS_ENTER_CRITICAL();
					modifyIPAddr(ip);
					OS_EXIT_CRITICAL();
				}
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
				
			}

		}
	}
	else if(keyCode==keyDown)
	{
		clrscr();
		//如果上级是告警
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			contextMenu[1] --;
			if(contextMenu[1]<MenuTwo_Alarm_Begin) contextMenu[1] = MenuTwo_Alarm_End;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		else //如果上级是设置
		if(contextMenu[0]==MenuOne_Setting)
		{
			contextMenu[1] --;
			if(contextMenu[1]<MenuTwo_Setting_Begin) contextMenu[1] = MenuTwo_Setting_End;
			DisplayMenu(menuTwo[contextMenu[1]],9);
			
			//显示告警点
			if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				DisplayMenu(menuTwo[contextMenu[1]],9);
				alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
				my_printf("org = %f",alarmSet);
				if(alarmSet>8000||alarmSet<-8000)
				{
					OS_ENTER_CRITICAL();
					modifyAlarmSet(contextMenu[1]-MenuTwo_Setting_Begin,0);
					OS_EXIT_CRITICAL();
					alarmSet = 0;
				}
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				DisplayMenu(menuTwo[contextMenu[1]],9);
				//contextMenu[1] - MenuTwo_Setting_IP-1从0开始到4
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState>1||switchState<0)
				{
					OS_ENTER_CRITICAL();
					modifySwitchState(contextMenu[1] - MenuTwo_Setting_IP-1,0);
					OS_EXIT_CRITICAL();
					switchState=0;
				}
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
			}
			//显示IP地址
			else if(contextMenu[1]== MenuTwo_Setting_IP)
			{
				addr1=APP_Persist_IP_Addr->IP_ADDR1;
				addr2=APP_Persist_IP_Addr->IP_ADDR2;
				addr3=APP_Persist_IP_Addr->IP_ADDR3;
				addr4=APP_Persist_IP_Addr->IP_ADDR4;
				port=APP_Persist_IP_Addr->ServerPort;
				if(port>9999)
				{
					port = 1234;
					ip.IP_ADDR1 = addr1;
					ip.IP_ADDR2 = addr2;
					ip.IP_ADDR3 = addr3;
					ip.IP_ADDR4 = addr4;
					ip.ServerPort = port;
					OS_ENTER_CRITICAL();
					modifyIPAddr(ip);
					OS_EXIT_CRITICAL();
				}
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
		}
	}
	else if(keyCode==keyOk)
	{
		//如果上级是告警，此时选择返回则返回
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Return))
		{
			clrscr();
			currentMenuIndex = 1;
			DisplayMenu(menuOne[contextMenu[0]],5);
		}
		//如果上级是告警，此时选择告警内容，则进入三级菜单
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			currentMenuIndex = 3;
			contextMenu[2] = MenuThree_Alarm_Content_Begin;
			DisplayMenu(menuThree[contextMenu[2]],6);
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//告警
			else
				DisplayBelowInfo(menuState[1],2);//正常
		}
		//如果上级是告警，此时选择的是告警记录，则进入三级菜单
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			clrscr();
			currentMenuIndex = 3;
			//DisplayMenu(menuThree[contextMenu[2]],6);
			numberRec=0;
			if(DisplayAlarmRecord(numberRec,6)==0)
			{
				//无更多告警记录
				noRec = 1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
			}
			else
			{
				noRec = 0;
			}
			
		}
		//如果上级是设置，则按返回时返回设置
		else if(contextMenu[0]==MenuOne_Setting && contextMenu[1]==MenuTwo_Setting_Return)
		{
			clrscr();
			currentMenuIndex = 1;
			DisplayMenu(menuOne[contextMenu[0]],5);
		}
		//如果上级是设置，选中的是设置项，则进入三级菜单 对各个告警点进行设置
		else if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			currentMenuIndex = 3;
			//获得相关值并显示在第二栏目
			//如果选中的是地址
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				addr1=APP_Persist_IP_Addr->IP_ADDR1;
				addr2=APP_Persist_IP_Addr->IP_ADDR2;
				addr3=APP_Persist_IP_Addr->IP_ADDR3;
				addr4=APP_Persist_IP_Addr->IP_ADDR4;
				port=APP_Persist_IP_Addr->ServerPort;
				if(port>9999)
				{
					port = 1234;
					ip.IP_ADDR1 = addr1;
					ip.IP_ADDR2 = addr2;
					ip.IP_ADDR3 = addr3;
					ip.IP_ADDR4 = addr4;
					ip.ServerPort = port;
					OS_ENTER_CRITICAL();
					modifyIPAddr(ip);
					OS_EXIT_CRITICAL();
				}
					
				sprintf(_db,"%d %d %d %d :%d",addr1,addr2,addr3,addr4,port);
				DB;
				editNow = 1;
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				DisplayMenu(menuTwo[contextMenu[1]],9);
				//contextMenu[1] - MenuTwo_Setting_IP-1从0开始到4；
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
				my_printf("type=%d s=%d",contextMenu[1] - MenuTwo_Setting_IP-1,switchState);
			}
		}
		
	}
}
void menuThreeOperate(uint32_t keyCode)
{
	if(keyCode==keyUp)
	{
		//如果上级是告警内容 则显示告警内容
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			contextMenu[2]++;
			if(contextMenu[2]>MenuThree_Alarm_Content_End) contextMenu[2] = MenuThree_Alarm_Content_Begin;
			DisplayMenu(menuThree[contextMenu[2]],6);
			
			if(contextMenu[2]!=MenuThree_Alarm_Content_Return)
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//告警
			else
				DisplayBelowInfo(menuState[1],2);//正常
		}
		//如果上级是告警，此时选择的是告警记录，则进入三级菜单
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			my_printf("numberRec=%d",numberRec);
			if(numberRec<0)
				noRec = 0;
			if(noRec==0)
				numberRec++;
			if(DisplayAlarmRecord(numberRec,6)==0)
			{
				//无更多告警记录
				noRec=1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
				
			}
			else
			{
				noRec = 0;
			}
		}
		//如果上级菜单是设置，并选中设置项，则按上下键可以进行调整
		else  if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				switch(editNow)
				{
					case 1:
						addr1++;break;
					case 2:
						addr2++;break;
					case 3:
						addr3++;break;
					case 4:
						addr4++;break;
					case 5:
						port++;break;
				}
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				alarmSet = alarmSet+0.1;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				//contextMenu[1] - MenuTwo_Setting_IP从0开始到4
				switchState = (switchState+1)%2;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
				my_printf("type=%d s=%d",contextMenu[1] - MenuTwo_Setting_IP-1,switchState);
			}
			
		}
	}
	else if(keyCode == keyDown)
	{
		
		//如果上级是告警内容 则显示告警内容
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			contextMenu[2]--;
			if(contextMenu[2]<MenuThree_Alarm_Content_Begin) contextMenu[2] = MenuThree_Alarm_Content_End;
			DisplayMenu(menuThree[contextMenu[2]],6);
			if(contextMenu[2]!=MenuThree_Alarm_Content_Return)
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//告警
			else
				DisplayBelowInfo(menuState[1],2);//正常
		}
		//如果上级是告警，此时选择的是告警记录，则进入三级菜单
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			clrscr();
			my_printf("numberRec=%d",numberRec);
			if(numberRec>=0)
			numberRec--;
			if(numberRec==-1)
			{
				noRec = 1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
			}
			else
			if(DisplayAlarmRecord(numberRec,6)==0)
			{
				//无更多告警记录
				noRec = 1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
			}
			else
			{
				noRec = 0;
			}
		}
		//如果上级菜单是设置，并选中设置项，则按上下键可以进行调整
		else  if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				switch(editNow)
				{
					case 1:
						addr1--;break;
					case 2:
						addr2--;break;
					case 3:
						addr3--;break;
					case 4:
						addr4--;break;
					case 5:
						port--;break;
				}
				
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				alarmSet = alarmSet-0.1;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				//contextMenu[1] - MenuTwo_Setting_IP从1开始到5；0对应的蜂鸣器的
				switchState = (switchState+1)%2;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
			}
		}
	}
	else if(keyCode == keyOk)
	{
		//如果上级是告警内容 选择返回则返回上一级菜单
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content)&&(contextMenu[2]==MenuThree_Alarm_Content_Return))
		{
			clrscr();
			currentMenuIndex = 2;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		//如果上级是告警，此时选择的是告警记录，则进入三级菜单
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			//如果无更多告警则可以返回
			if(noRec==1)
			{
				clrscr();
				currentMenuIndex = 2;
				DisplayMenu(menuTwo[contextMenu[1]],9);
			}
		}
		//如果上级菜单是设置，并选中设置项，则按OK键进入保存菜单选项
		else  if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			//如果是IP设置
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				sprintf(_db,"save ip");
				DB;
				clrscr();
				currentMenuIndex = 4;
				contextMenu[3] = MenuFour_Item_Begin;
				DisplayMenu(menuFour[contextMenu[3]],2);
			}
			//如果是告警设置，按OK键直接进入四级菜单，保存退出选项
			else
			{
				sprintf(_db,"save setting");
				DB;
				clrscr();
				currentMenuIndex = 4;
				contextMenu[3] = MenuFour_Item_Begin;
				DisplayMenu(menuFour[contextMenu[3]],2);
			}
		}
	}
	else if(keyCode==keyLeft)
	{
		//如果上级菜单是设置，并选中设置项，则按left键切换不同的数据段
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
					if(editNow>1)
					editNow--;
			}
			else
			{
				
			}
			
		}
	}
	else if(keyCode==keyRight)
	{
		//如果上级菜单是设置，并选中设置项，则按right键切换不同的数据段
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
					if(editNow<5)
						editNow++;
			}
			else
			{
				
			}
		}
	}
	else if(keyCode==keyUpLong)
	{
		//如果上级菜单是设置，并选中设置项，则按上下键可以进行调整
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				switch(editNow)
				{
					case 1:
						addr1+=20;break;
					case 2:
						addr2+=20;break;
					case 3:
						addr3+=20;break;
					case 4:
						addr4+=20;break;
					case 5:
						port+=20;break;
				}
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else 
			{
				alarmSet = alarmSet+1;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
		}
	}
	else if(keyCode == keyDownLong)
	{
		//如果上级菜单是设置，并选中设置项，则按上下键可以进行调整
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				switch(editNow)
				{
					case 1:
						addr1-=20;break;
					case 2:
						addr2-=20;break;
					case 3:
						addr3-=20;break;
					case 4:
						addr4-=20;break;
					case 5:
						port-=20;break;
				}
				
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else 
			{
				alarmSet = alarmSet-1;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
		}
	}
}
void menuFourOperate(uint32_t keyCode)
{
	App_Persist_IP_Def ip;
	CPU_SR cpu_sr;
	
	if(keyCode == keyOk)
	{
		//如果一级菜单是设置，并选中设置项，并且已经点击修改完毕则进入保存菜单
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			if(contextMenu[3] == MenuFour_Item_Save)
			{
				if(contextMenu[1]==MenuTwo_Setting_IP)
				{
					ip.IP_ADDR1 = addr1;
					ip.IP_ADDR2 = addr2;
					ip.IP_ADDR3 = addr3;
					ip.IP_ADDR4 = addr4;
					ip.ServerPort = port;
					OS_ENTER_CRITICAL();
					modifyIPAddr(ip);
					OS_EXIT_CRITICAL();
					clrscr();
					currentMenuIndex = 2;
					DisplayMenu(menuTwo[contextMenu[1]],9);
				}
				else if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
				{
					//contextMenu[1]-MenuTwo_Setting_Begin 对应的序号 和类型是对应的
					alarmSet = alarmSet+0.000001;
					OS_ENTER_CRITICAL();
					modifyAlarmSet(contextMenu[1]-MenuTwo_Setting_Begin,alarmSet);
					OS_EXIT_CRITICAL();
					my_printf("[%d %f]",contextMenu[1]-MenuTwo_Setting_Begin,alarmSet);
					clrscr();
					currentMenuIndex = 2;
					DisplayMenu(menuTwo[contextMenu[1]],9);
				}
				else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
				{
					//contextMenu[1] - MenuTwo_Setting_IP从1开始到5；0对应的蜂鸣器的
					
					OS_ENTER_CRITICAL();
					modifySwitchState(contextMenu[1] - MenuTwo_Setting_IP-1,switchState);
					OS_EXIT_CRITICAL();
					currentMenuIndex = 2;
					DisplayMenu(menuTwo[contextMenu[1]],9);
				}
			}
			else if(contextMenu[3] == MenuFour_Item_Exit)
			{
				clrscr();
				currentMenuIndex = 2;
				DisplayMenu(menuTwo[contextMenu[1]],9);
			}
			else if(contextMenu[3] == MenuFour_Item_Return)
			{
				clrscr();
				currentMenuIndex = 2;
				DisplayMenu(menuTwo[contextMenu[1]],9);
			}
			
			//显示告警点
			if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			//显示IP地址
			else if(contextMenu[1]== MenuTwo_Setting_IP)
			{
				addr1=APP_Persist_IP_Addr->IP_ADDR1;
				addr2=APP_Persist_IP_Addr->IP_ADDR2;
				addr3=APP_Persist_IP_Addr->IP_ADDR3;
				addr4=APP_Persist_IP_Addr->IP_ADDR4;
				port=APP_Persist_IP_Addr->ServerPort;
				if(port>9999)
				{
					port = 1234;
					ip.IP_ADDR1 = addr1;
					ip.IP_ADDR2 = addr2;
					ip.IP_ADDR3 = addr3;
					ip.IP_ADDR4 = addr4;
					ip.ServerPort = port;
					OS_ENTER_CRITICAL();
					modifyIPAddr(ip);
					OS_EXIT_CRITICAL();
				}
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				//contextMenu[1] - MenuTwo_Setting_IP-1从0开始到4；
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
			}
		}
	}
	else if(keyCode == keyUp)
	{
		//如果一级菜单是设置，并选中设置项，并且已经点击修改完毕则进入保存菜单
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			contextMenu[3] ++;
			if(contextMenu[3]>MenuFour_Item_End)
				contextMenu[3] = MenuFour_Item_Begin;
			DisplayMenu(menuFour[contextMenu[3]],2);
		}
	}
	else if(keyCode == keyDown)
	{
		//如果一级菜单是设置，并选中设置项，并且已经点击修改完毕则进入保存菜单
		if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			contextMenu[3] --;
			if(contextMenu[3]<MenuFour_Item_Begin)
				contextMenu[3] = MenuFour_Item_End;
			DisplayMenu(menuFour[contextMenu[3]],2);
		}
	}
}