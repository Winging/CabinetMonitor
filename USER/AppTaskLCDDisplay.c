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

#define MenuOne_Setting 11   //������
#define MenuTwo_Setting_Begin 3
#define MenuTwo_Setting_AlarmSetS 24 //�澯�����õ�������Ŀ
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


//contextMenu[0] �������һ���˵���ǰѡ�
//contextMenu[1]������Ƕ����˵���ǰѡ�
//contextMenu[2]������������˵���ǰѡ��
//contextMenu[3]��������ļ��˵���ǰѡ��
int8_t contextMenu[4]={0};
int8_t currentMenuIndex;
int8_t menuThreeAlarmContentUpdate = 0;//�澯���ݸ��£�0δ���£�4-17
static float alarmSet;
static uint8_t addr1,addr2,addr3,addr4,editNow;
static uint16_t port;
static int16_t numberRec;
static uint8_t noRec;
static uint8_t switchState;
extern OS_EVENT * mBoxKeyCode;//����Ϣ����������
extern OS_EVENT * semLCDInited;//lcd�Ƿ��Ѿ���ʼ��

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
	//OSSemPend(semLCDInited,0,&err);//���������ʲô���õİ취���Խ��LCD��ʼ�����⣬�������pend
	_DBG_("LCD INIT");
	//�ȴ�LCD��ʼ��
	
	INILCDConrol();
	_DBG_("LCD START");
	//����и澯������ʾ�澯��¼
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
		//��ȡ������Ϣ
		keyCode = (uint32_t)(OSMboxAccept(mBoxKeyCode))&0xffff;
		//���1����û�а�����رձ����
		if(keyCode==0 && (OSTimeGet()-hitTime)>60000)
			CLR_BKEN;
		
		//�������0 ��˵���а���
		if(keyCode>0)
		{
			SET_BKEN;//������ ����������
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
					DisplayBelowInfo(menuState[0],2);//�澯
				else
					DisplayBelowInfo(menuState[1],2);//����
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
//		��ת,��ѹģ�黹δ�������Բ���ʾ��ѹ
// 		if(contextMenu[0] == MenuOne_AC)
// 			contextMenu[0] = MenuOne_SYSTIME;
		sprintf(_db,"context =%d",contextMenu[0]);
		DB;
		if(contextMenu[0]>MenuOne_End) contextMenu[0] = MenuOne_Begin;
		DisplayMenu(menuOne[contextMenu[0]],5);
		//��ʾ��ʪ�ȵ���Ϣ
		displaySysState(contextMenu[0]);
	}
	else if(keyCode==keyDown)
	{
		clrscr();
		contextMenu[0] --;
//		��ת,��ѹģ�黹δ�������Բ���ʾ��ѹ
// 		if(contextMenu[0] == MenuOne_DC)
// 			contextMenu[0] = MenuOne_AC;
		if(contextMenu[0]<MenuOne_Begin) contextMenu[0] = MenuOne_End;
		DisplayMenu(menuOne[contextMenu[0]],5);
		//��ʾ��ʪ�ȵ���Ϣ
		displaySysState(contextMenu[0]);
	}
	else if(keyCode==keyOk)
	{
		clrscr();
		//����澯��¼-�澯����--һ���˵���
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			currentMenuIndex = 2;
			contextMenu[1] = MenuTwo_Alarm_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		//����-------һ���˵���
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
		//����ϼ��Ǹ澯
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			contextMenu[1] ++;		
			if(contextMenu[1]>MenuTwo_Alarm_End) contextMenu[1] = MenuTwo_Alarm_Begin;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		else //����ϼ�������
		if(contextMenu[0]==MenuOne_Setting)
		{
			contextMenu[1] ++;
			if(contextMenu[1]>MenuTwo_Setting_End) contextMenu[1] = MenuTwo_Setting_Begin;
//			ʵ����ת��������ѹ
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
				//contextMenu[1] - MenuTwo_Setting_IP - 1��0��ʼ��4
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState>1||switchState<0)
				{
					OS_ENTER_CRITICAL();
					modifySwitchState(contextMenu[1] - MenuTwo_Setting_IP-1,0);
					OS_EXIT_CRITICAL();
					switchState=0;
				}
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
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
		//����ϼ��Ǹ澯
		if(contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm)
		{
			contextMenu[1] --;
			if(contextMenu[1]<MenuTwo_Alarm_Begin) contextMenu[1] = MenuTwo_Alarm_End;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		else //����ϼ�������
		if(contextMenu[0]==MenuOne_Setting)
		{
			contextMenu[1] --;
			if(contextMenu[1]<MenuTwo_Setting_Begin) contextMenu[1] = MenuTwo_Setting_End;
//			ʵ����ת��������ѹ
// 			if(contextMenu[1] == MenuTwo_Alarm_AC)
// 				contextMenu[1] = MenuTwo_Setting_Return;
			DisplayMenu(menuTwo[contextMenu[1]],9);
			
			//��ʾ�澯��
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
				//contextMenu[1] - MenuTwo_Setting_IP-1��0��ʼ��4
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState>1||switchState<0)
				{
					OS_ENTER_CRITICAL();
					modifySwitchState(contextMenu[1] - MenuTwo_Setting_IP-1,0);
					OS_EXIT_CRITICAL();
					switchState=0;
				}
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
			}
			//��ʾIP��ַ
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
		//����ϼ��Ǹ澯����ʱѡ�񷵻��򷵻�
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Return))
		{
			clrscr();
			currentMenuIndex = 1;
			DisplayMenu(menuOne[contextMenu[0]],5);
		}
		//����ϼ��Ǹ澯����ʱѡ��澯���ݣ�����������˵�
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			currentMenuIndex = 3;
			contextMenu[2] = MenuThree_Alarm_Content_Begin;
			contextMenu[2] = MenuThree_Alarm_Content_FL;
			DisplayMenu(menuThree[contextMenu[2]],6);
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//�澯
			else
				DisplayBelowInfo(menuState[1],2);//����
			menuThreeAlarmContentUpdate = 1;
		}
		//����ϼ��Ǹ澯����ʱѡ����Ǹ澯��¼������������˵�
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			clrscr();
			currentMenuIndex = 3;
			//DisplayMenu(menuThree[contextMenu[2]],6);
			numberRec=0;
			if(DisplayAlarmRecord(numberRec,6)==0)
			{
				//�޸���澯��¼
				noRec = 1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
			}
			else
			{
				noRec = 0;
			}
			
		}
		//����ϼ������ã��򰴷���ʱ��������
		else if(contextMenu[0]==MenuOne_Setting && contextMenu[1]==MenuTwo_Setting_Return)
		{
			clrscr();
			currentMenuIndex = 1;
			DisplayMenu(menuOne[contextMenu[0]],5);
		}
		//����ϼ������ã�ѡ�е������������������˵� �Ը����澯���������
		else if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			currentMenuIndex = 3;
			//������ֵ����ʾ�ڵڶ���Ŀ
			//���ѡ�е��ǵ�ַ
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
				//contextMenu[1] - MenuTwo_Setting_IP-1��0��ʼ��4��
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
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
		//����ϼ��Ǹ澯���� ����ʾ�澯����
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			contextMenu[2]++;
			if(contextMenu[2]>MenuThree_Alarm_Content_End) contextMenu[2] = MenuThree_Alarm_Content_Begin;
//		��ת,��ѹģ�黹δ�������Բ���ʾ��ѹ
// 			if(contextMenu[2] == MenuThree_Alarm_Content_Begin)
// 				contextMenu[2] = MenuThree_Alarm_Content_AC;	
			
			DisplayMenu(menuThree[contextMenu[2]],6);
			
			if(contextMenu[2]!=MenuThree_Alarm_Content_Return)
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//�澯
			else
				DisplayBelowInfo(menuState[1],2);//����
		}
		//����ϼ��Ǹ澯����ʱѡ����Ǹ澯��¼������������˵�
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			my_printf("numberRec=%d",numberRec);
			if(numberRec<0)
				noRec = 0;
			if(noRec==0)
				numberRec++;
			if(DisplayAlarmRecord(numberRec,6)==0)
			{
				//�޸���澯��¼
				noRec=1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
				
			}
			else
			{
				noRec = 0;
			}
		}
		//����ϼ��˵������ã���ѡ������������¼����Խ��е���
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
				//contextMenu[1] - MenuTwo_Setting_IP��0��ʼ��4
				switchState = (switchState+1)%2;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
				my_printf("type=%d s=%d",contextMenu[1] - MenuTwo_Setting_IP-1,switchState);
			}
			
		}
	}
	else if(keyCode == keyDown)
	{
		
		//����ϼ��Ǹ澯���� ����ʾ�澯����
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content))
		{
			clrscr();
			contextMenu[2]--;
			if(contextMenu[2]<MenuThree_Alarm_Content_Begin) contextMenu[2] = MenuThree_Alarm_Content_End;
//			��ת,��ѹģ�黹δ�������Բ���ʾ��ѹ
// 			if(contextMenu[2] == MenuThree_Alarm_Content_DC)
// 				contextMenu[2] = MenuThree_Alarm_Content_Return;
			DisplayMenu(menuThree[contextMenu[2]],6);
			if(contextMenu[2]!=MenuThree_Alarm_Content_Return)
			if(HasAlarm(contextMenu[2]-MenuThree_Alarm_Content_Begin))
				DisplayBelowInfo(menuState[0],2);//�澯
			else
				DisplayBelowInfo(menuState[1],2);//����
		}
		//����ϼ��Ǹ澯����ʱѡ����Ǹ澯��¼������������˵�
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
				//�޸���澯��¼
				noRec = 1;
				DisplayMenu(menuThree[MenuThree_Alarm_Content_Return],6);
			}
			else
			{
				noRec = 0;
			}
		}
		//����ϼ��˵������ã���ѡ������������¼����Խ��е���
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
				//contextMenu[1] - MenuTwo_Setting_IP��1��ʼ��5��0��Ӧ�ķ�������
				switchState = (switchState+1)%2;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
			}
		}
	}
	else if(keyCode == keyOk)
	{
		//����ϼ��Ǹ澯���� ѡ�񷵻��򷵻���һ���˵�
		if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Content)&&(contextMenu[2]==MenuThree_Alarm_Content_Return))
		{
			clrscr();
			currentMenuIndex = 2;
			DisplayMenu(menuTwo[contextMenu[1]],9);
			//menuThreeAlarmContentUpdate == 0;
		}
		//����ϼ��Ǹ澯����ʱѡ����Ǹ澯��¼������������˵�
		else if((contextMenu[0] >=0 && contextMenu[0]<=MenuOne_Alarm) && (contextMenu[1]==MenuTwo_Alarm_Record))
		{
			//����޸���澯����Է���
			if(noRec==1)
			{
				clrscr();
				currentMenuIndex = 2;
				DisplayMenu(menuTwo[contextMenu[1]],9);
			}
		}
		//����ϼ��˵������ã���ѡ���������OK�����뱣��˵�ѡ��
		else  if(contextMenu[0]==MenuOne_Setting && (contextMenu[1]>=MenuTwo_Setting_Begin&&contextMenu[1]<MenuTwo_Setting_Return))
		{
			//�����IP����
			if(contextMenu[1]==MenuTwo_Setting_IP)
			{
				sprintf(_db,"save ip");
				DB;
				clrscr();
				currentMenuIndex = 4;
				contextMenu[3] = MenuFour_Item_Begin;
				DisplayMenu(menuFour[contextMenu[3]],2);
			}
			//����Ǹ澯���ã���OK��ֱ�ӽ����ļ��˵��������˳�ѡ��
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
		//����ϼ��˵������ã���ѡ���������left���л���ͬ�����ݶ�
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
		//����ϼ��˵������ã���ѡ���������right���л���ͬ�����ݶ�
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
		//����ϼ��˵������ã���ѡ������������¼����Խ��е���
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
		//����ϼ��˵������ã���ѡ������������¼����Խ��е���
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
		//����Ǹ澯���ݡ���¼������
		if(contextMenu[1] >=MenuTwo_Alarm_Begin && contextMenu[1] <= MenuTwo_Alarm_End )
		{
			clrscr();
			currentMenuIndex = 2;		
			//contextMenu[1] = 0;
			DisplayMenu(menuTwo[contextMenu[1]],9);
		}
		//��������ø澯��
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
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
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
		//���һ���˵������ã���ѡ������������Ѿ�����޸��������뱣��˵�
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
					//contextMenu[1]-MenuTwo_Setting_Begin ��Ӧ����� �������Ƕ�Ӧ��
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
					//contextMenu[1] - MenuTwo_Setting_IP��1��ʼ��5��0��Ӧ�ķ�������
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
			
			//��ʾ�澯��
			if(contextMenu[1] >= MenuTwo_Setting_Begin &&contextMenu[1] <= MenuTwo_Setting_AlarmSetS )
			{
				alarmSet = APP_Persist_AlarmSet_Addr[contextMenu[1]-MenuTwo_Setting_Begin].alarmSet;
				DisplayAlarmSetting(alarmSet,contextMenu[1]-MenuTwo_Setting_Begin);
			}
			//��ʾIP��ַ
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
				//contextMenu[1] - MenuTwo_Setting_IP-1��0��ʼ��4��
				switchState = APP_Persist_SwitchState_Addr[contextMenu[1] - MenuTwo_Setting_IP-1].state;
				if(switchState==0)
					DisplayBelowInfo(menuState[2],2);//����
				else
					DisplayBelowInfo(menuState[3],2);//����
			}
		}
	}
	else if(keyCode == keyUp)
	{
		//���һ���˵������ã���ѡ������������Ѿ�����޸��������뱣��˵�
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
		//���һ���˵������ã���ѡ������������Ѿ�����޸��������뱣��˵�
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
 *@��������ʾϵͳ״̬��������ʪ�Ⱥ͵�ѹ����Ϣ
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