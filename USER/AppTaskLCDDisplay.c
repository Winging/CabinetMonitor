#include "AppTaskLCDDisplay.h"
#include "AppPersist.h"

#define MenuOne_Begin 0
#define MenuOne_End 11
#define MenuOne_Alarm 10
#define MenuOne_TEMP1 0
#define MenuOne_WET1  1
#define MenuOne_TEMP2 2
#define MenuOne_WET2  3
#define MenuOne_TEMP3 4
#define MenuOne_WET3  5
#define MenuOne_TEMP4 6
#define MenuOne_WET4  7
#define MenuOne_AC    8
#define MenuOne_DC    9
#define MenuOne_SYSTIME 10


#define MenuTwo_Alarm_Begin 0
#define MenuTwo_Alarm_End 2
#define MenuTwo_Alarm_Content 0
#define MenuTwo_Alarm_Record 1
#define MenuTwo_Alarm_Return 2
#define MenuThree_Alarm_Content_Begin 0
#define MenuThree_Alarm_Content_End 39
#define MenuThree_Alarm_Content_Return 39

#define MenuOne_Setting 11   //设置项
#define MenuTwo_Setting_Begin 3
#define MenuTwo_Setting_AlarmSetS 24 //告警点设置的最大的项目
#define MenuTwo_Setting_IP 25
#define MenuTwo_Setting_Switch 26
#define MenuTwo_Setting_End 36
#define MenuTwo_Setting_Return 36

#define MenuFour_Item_Begin 0
#define MenuFour_Item_End 2
#define MenuFour_Item_Save 0
#define MenuFour_Item_Exit 1
#define MenuFour_Item_Return 2

#define MenuTwo_Setting_Temp 9
#define MenuTwo_Alarm_AC 8
#define MenuThree_Alarm_Content_FL 6 
#define MenuThree_Alarm_Content_AC 5
#define MenuThree_Alarm_Content_DC 4
extern char data_Humidity;
extern char data_Temperature;

extern unsigned char menuOne[14][5];
extern unsigned char menuTwo[43][9];
extern unsigned  char menuThree[44][6];
extern unsigned  char menuFour[3][2];
extern unsigned char menuState[4][2];
extern unsigned char menuWelcome[1][4];


//contextMenu[0] 保存的是一级菜单当前选项，
//contextMenu[1]保存的是二级菜单当前选项，
//contextMenu[2]保存的是三级菜单当前选项
//contextMenu[3]保存的是四级菜单当前选项
int8_t contextMenu[4]={0};
int8_t currentMenuIndex;
int8_t menuThreeAlarmContentUpdate = 0;//告警内容更新（0未更新）4-17
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

void displaySysState(uint8_t context);
void AppTaskLCDDisplay(void * pdata)
{
	uint32_t keyCode;
	CPU_SR cpu_sr;
	INT8U err;
	clock_time_t hitTime,disTime,clockDisTime,ipEditTime;
	int8_t ipBlink;
	
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
//	clockDisTime = OSTimeGet();
	ipEditTime = OSTimeGet();
	ipBlink=1;

	while(1)
	{
		//获取按键信息
		keyCode = (uint32_t)(OSMboxAccept(mBoxKeyCode))&0xffff;
		//如果1分钟没有按键则关闭背光灯
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
				displaySysState(contextMenu[0]);
			}
		
		}
		
		if((currentMenuIndex==3) && (contextMenu[0]==MenuOne_Setting ) && (contextMenu[1]==MenuTwo_Setting_IP))
		{
			my_printf("blinking calculate ");
			if((OSTimeGet()-ipEditTime)>400)
			{
				ipEditTime = OSTimeGet();
				ipBlink = ipBlink * (-1);
				my_printf("400ms reached ");
			}
			if(ipBlink==1)
			{
				
				switch(editNow)
				{
					case 1:
						DisplayIPBlink(1);
						break;
					case 2:
						DisplayIPBlink(2);
						break;
					case 3:
						DisplayIPBlink(3);
						break;
					case 4:
						DisplayIPBlink(4);
						break;
					case 5:
						DisplayPortBlink(port);
						break;
				}
			}else
			{
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			
		}


		
		if(menuThreeAlarmContentUpdate == 1)
		{
			if(contextMenu[2]!=MenuThree_Alarm_Content_Return)
			{
				if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
					DisplayBelowInfo(menuState[0],2);//告警
				else
					DisplayBelowInfo(menuState[1],2);//正常
			}
		}
		WDT_Feed();
		//_DBG_("t");
		OSTimeDly(5);
		//_DBG_("r");
	}
}
/***************************************************/
void menuOneOperate(uint32_t keyCode)
{
	CPU_SR cpu_sr;
	
	if(keyCode==keyUp)
	{
		clrscr();
		contextMenu[0] ++;
//		跳转,电压模块还未做，所以不显示电压
// 		if(contextMenu[0] == MenuOne_AC)
// 			contextMenu[0] = MenuOne_SYSTIME;
		sprintf(_db,"context =%d",contextMenu[0]);
		DB;
		if(contextMenu[0]>MenuOne_End) contextMenu[0] = MenuOne_Begin;
		DisplayMenu(menuOne[contextMenu[0]],5);
		//显示温湿度等信息
		displaySysState(contextMenu[0]);
	}
	else if(keyCode==keyDown)
	{
		clrscr();
		contextMenu[0] --;
//		跳转,电压模块还未做，所以不显示电压
// 		if(contextMenu[0] == MenuOne_DC)
// 			contextMenu[0] = MenuOne_AC;
		if(contextMenu[0]<MenuOne_Begin) contextMenu[0] = MenuOne_End;
		DisplayMenu(menuOne[contextMenu[0]],5);
		//显示温湿度等信息
		displaySysState(contextMenu[0]);
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
			//contextMenu[1] = MenuTwo_Setting_Begin;
			contextMenu[1] = MenuTwo_Setting_Temp;
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
//			实现跳转，跳过电压
// 			if(contextMenu[1] == MenuTwo_Setting_Begin)
// 				contextMenu[1] = MenuTwo_Setting_Temp;
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
//			实现跳转，跳过电压
// 			if(contextMenu[1] == MenuTwo_Alarm_AC)
// 				contextMenu[1] = MenuTwo_Setting_Return;
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
			contextMenu[2] = MenuThree_Alarm_Content_FL;
			DisplayMenu(menuThree[contextMenu[2]],6);
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//告警
			else
				DisplayBelowInfo(menuState[1],2);//正常
			menuThreeAlarmContentUpdate = 1;
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
	else if(keyCode == keyOkLong)
	{
		clrscr();
		currentMenuIndex = 1;		
		//contextMenu[0] = 0;
		DisplayMenu(menuOne[contextMenu[0]],5);	
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
//		跳转,电压模块还未做，所以不显示电压
// 			if(contextMenu[2] == MenuThree_Alarm_Content_Begin)
// 				contextMenu[2] = MenuThree_Alarm_Content_AC;	
			
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
				my_printf("increase %d",(int32_t)(10000*alarmSet));
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
//			跳转,电压模块还未做，所以不显示电压
// 			if(contextMenu[2] == MenuThree_Alarm_Content_DC)
// 				contextMenu[2] = MenuThree_Alarm_Content_Return;
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
			//menuThreeAlarmContentUpdate == 0;
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
	else if(keyCode==keyUpDouble)
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
	else if(keyCode==keyDownDouble)
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
	else if(keyCode == keyOkLong)
	{
		//如果是告警内容、记录、返回
		if(contextMenu[1] >=MenuTwo_Alarm_Begin && contextMenu[1] <= MenuTwo_Alarm_End )
		{
			clrscr();
			currentMenuIndex = 2;		
			//contextMenu[1] = 0;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		//如果是设置告警点
		else
		{
			clrscr();
			currentMenuIndex = 2;		
			DisplayMenu(menuTwo[contextMenu[1]],9);
			alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{		
				addr1=APP_Persist_IP_Addr->IP_ADDR1;
				addr2=APP_Persist_IP_Addr->IP_ADDR2;
				addr3=APP_Persist_IP_Addr->IP_ADDR3;
				addr4=APP_Persist_IP_Addr->IP_ADDR4;
				port=APP_Persist_IP_Addr->ServerPort;				
				DisplayIP(addr1,1);
				DisplayIP(addr2,2);
				DisplayIP(addr3,3);
				DisplayIP(addr4,4);
				DisplayPort(port);
			}
			else if(contextMenu[1]>MenuTwo_Setting_IP && contextMenu[1] < MenuTwo_Setting_End )
			{
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//常开
				else
					DisplayBelowInfo(menuState[3],2);//常闭
			}
			else 
			{
				//alarmSet = alarmSet-1;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
		}
		menuThreeAlarmContentUpdate = 0;
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
					my_printf("try to save %f",alarmSet);
					my_printf("try save %d",(int32_t)alarmSet);
					OS_ENTER_CRITICAL();
					modifyAlarmSet(contextMenu[1]-MenuTwo_Setting_Begin,alarmSet);
					OS_EXIT_CRITICAL();
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
/*	else if(keyCode == keyCancel)
	{
		clrscr();
		//contextMenu[2] = 0;
		DisplayMenu(menuThree[contextMenu[2]],6);
	}*/
}
/*
 *@描述：显示系统状态，包括温湿度和电压等信息
 *
 */
void displaySysState(uint8_t context)
{
	switch(context)
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
		case MenuOne_TEMP3:
		{
			DisplaySetting(getTemp(3),1);
			break;
		}
		case MenuOne_WET3:
		{
			DisplaySetting(getWet(3),2);
			break;
		}
		case MenuOne_TEMP4:
		{
			DisplaySetting(getTemp(4),1);
			break;
		}
		case MenuOne_WET4:
		{
			DisplaySetting(getWet(4),2);
			break;
		}
		/*
		case MenuOne_TEMP5:
		{
			DisplaySetting(getTemp(5),1);
			break;
		}
		case MenuOne_WET5:
		{
			DisplaySetting(getWet(5),2);
			break;
		}
		*/
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