#include "AppTaskSample.h"
#include "AppDebug.h"
#include "PacketAnalysis.h"
#include "TcpApp.h"
#include "AppSensor.h"
#include "Beep.h"
#include "TemSensor.h"
#include "AppPersist.h"
#include "Relay.h"
#include "AppRs485.h"
#include "HexToAsccii.h"

extern struct uip_conn * client_conn;//���Ͷ�/�ͻ���TCP����
char data_Humidity1, data_Temperature1,data_Humidity2, data_Temperature2,
data_Humidity3, data_Temperature3,data_Humidity4, data_Temperature4;
uint32_t xSample,ySample,zSample;//x y z ���ϼ��ٵĲ���ֵ����β���ȡƽ�����
uint8_t powerDown;//�����ʶ
uint8_t alarmState[ALARM_REC_TYPE_NUM]={0};
int32_t timeCount,TimeCountMax =100;//RS485���ճ�ʱ����
uint8_t recIndex,rLen;//����ָ��
uint8_t recData[10];//���ջ�����
uint8_t tempRec;//��ʱ�����ַ�
uint8_t voltage1,voltage2,vSum;//���յ��ĵ�ѹ1��2��sum
void DoAlarm(uint8_t type)
{
	CPU_SR cpu_sr;
	//ֻ�з���״̬�л�����ӻָ����澯��д���¼
	if(alarmState[type]==0)
	{
		OS_ENTER_CRITICAL();
		beginSysAlarm(type,Argent);
		OS_EXIT_CRITICAL();
		alarmState[type] =1;//��ǰΪ�澯״̬
	}
}
void DoResumeAlarm(uint8_t type)
{
	CPU_SR cpu_sr;
	//ֻ�з���״̬�л�����Ӹ澯���ָ���д���¼
	if(alarmState[type]==1)
	{
		OS_ENTER_CRITICAL();
		resumeSysAlarm(type);
		OS_EXIT_CRITICAL();
		alarmState[type] =0;//��ǰΪ�澯״̬
	}
}
void AppTaskSample(void* pdata)
{
	INT8U err;
	clock_time_t shakeExamTime,lastTempTime,normalExamTime,shakeSampleTime,shakeSampleExcuteTime;
	CPU_SR cpu_sr;
	uint8_t doBeep,i;
	uint32_t hasShakeSample;
	uint8_t relayAlarms[RELAY_STATE_NUM] ={0};

	lastTempTime = OSTimeGet();
	normalExamTime = OSTimeGet();
	shakeExamTime = OSTimeGet();
	shakeSampleTime = OSTimeGet();
	shakeSampleExcuteTime = 1000;
	hasShakeSample = 0;
	while(1)
	{
		if(OSTimeGet()-shakeSampleTime>shakeSampleExcuteTime)
		{
			//�Լ��ٶȽ��в���
			xSample = readAcceleration(1);
			//for(i=0;i<30;i++)
			//xSample = xSample+ (readAcceleration(1))/30;
			ySample = readAcceleration(2);
			//for(i=0;i<30;i++)
			//ySample =ySample+ ( readAcceleration(2))/30;
			zSample =readAcceleration(3);
			//for(i=0;i<30;i++)
			//zSample = zSample+(readAcceleration(3))/30;
			//sprintf(_db,"you have get [[[[[[x=%d ,y=%d ,z=%d]]]]]]",xSample,ySample,zSample);
			//DB;
			shakeSampleTime = OSTimeGet();
			if(hasShakeSample>10)shakeSampleExcuteTime = 1200;
			hasShakeSample ++;
			
		}
		
		doBeep = 0;
		for(i=0;i<RELAY_STATE_NUM;i++)
			relayAlarms[i] =0;
		//_DBG_("INIT RELAYALARMS");
		for(i=0;i<ALARM_REC_TYPE_NUM;i++)
		{
			//sprintf(_db,"i=%d,relay?id=%d",i,APP_Persist_AlarmLevelRelay_Addr[i].relay-1 );
			//DB;
			if(APP_Persist_AlarmLevelRelay_Addr[i].relay-1 < RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[i].relay-1] ++;
		}
		//��ͨ�澯�������Ӳ���һ��
		if(OSTimeGet()-normalExamTime>4000)
		{
			normalExamTime = OSTimeGet();
		if(DoorAlarm(1)){
			//_DBG_("DOOR");
			doBeep = 1;
			DoAlarm(Door);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Door].relay-1);
			}
		if(DoorAlarm(2)){
			//_DBG_("DoorTwo");
			doBeep = 1;
			DoAlarm(DoorTwo);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[DoorTwo].relay-1);
			}
        if(DoorAlarm(3)){
			//_DBG_("DoorThree");
			doBeep = 1;
			DoAlarm(DoorThree);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[DoorThree].relay-1);
			}
        if(DoorAlarm(4)){
			//_DBG_("DoorFour");
			doBeep = 1;
			DoAlarm(DoorFour);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[DoorFour].relay-1);
			}			
		
		if(SmokeAlarm(1)){
			_DBG_("SMOKE");
			doBeep = 1;
			DoAlarm(Smoke);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Smoke].relay-1);
			}
			
		if(SmokeAlarm(2)){
			_DBG_("SmokeTwo");
			doBeep = 1;
			DoAlarm(SmokeTwo);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[SmokeTwo].relay-1);
			}
		if(SmokeAlarm(3)){
			_DBG_("SmokeThree");
			doBeep = 1;
			DoAlarm(SmokeThree);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[SmokeThree].relay-1);
			}
		if(SmokeAlarm(4)){
			_DBG_("SmokeFour");
			doBeep = 1;
			DoAlarm(SmokeFour);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[SmokeFour].relay-1);
			}
			
			
		if(WaterAlarm()){
			//_DBG_("WATER");
			doBeep = 1;
			DoAlarm(Water);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Water].relay-1);
			}
		if(ThunderAlarm()){
			//_DBG_("THUNDER");
			doBeep = 1;
			DoAlarm(Thunder);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Thunder].relay-1);
			}
		
		if(LeapAlarm()){
			
			//_DBG_("leap");
			doBeep = 1;
			DoAlarm(Leap);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Leap].relay-1);
			}
		
			
		if(checkWetAlarm(1) == WetStateLow)//������ʪ�ȣ�
		{
			//_DBG_("wet1 lack");
			doBeep = 1;
			DoAlarm(Wet1Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet1Lack].relay-1);
		}
		if(checkWetAlarm(1) == WetStateHigh)//������ʪ��
		{
			//_DBG_("wet1 over");
			doBeep = 1;
			DoAlarm(Wet1Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet1Over].relay-1);
		}
		if(checkTempAlarm(1) == TempStateLow)//�������¶�
		{
			//_DBG_("temp1 lack");
			doBeep = 1;
			DoAlarm(Temp1Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp1Lack].relay-1);
		}
		if(checkTempAlarm(1) == TempStateHigh)//�������¶�
		{
			//_DBG_("temp1 over");
			doBeep = 1;
			DoAlarm(Temp1Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp1Over].relay-1);
		}
		
		if(checkWetAlarm(2) == WetStateLow)//������ʪ�ȣ�
		{
			//_DBG_("wet1 lack");
			doBeep = 1;
			DoAlarm(Wet2Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet2Lack].relay-1);
		}
		if(checkWetAlarm(2) == WetStateHigh)//������ʪ��
		{
			//_DBG_("wet1 over");
			doBeep = 1;
			DoAlarm(Wet2Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet2Over].relay-1);
		}
		if(checkTempAlarm(2) == TempStateLow)//�������¶�
		{
			//_DBG_("temp1 lack");
			doBeep = 1;
			DoAlarm(Temp2Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp2Lack].relay-1);
		}
		if(checkTempAlarm(2) == TempStateHigh)//�������¶�
		{
			//_DBG_("temp1 over");
			doBeep = 1;
			DoAlarm(Temp2Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp2Over].relay-1);
		}
		
		if(checkWetAlarm(3) == WetStateLow)//������ʪ�ȣ�
		{
			//_DBG_("wet1 lack");
			doBeep = 1;
			DoAlarm(Wet3Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet3Lack].relay-1);
		}
		if(checkWetAlarm(3) == WetStateHigh)//������ʪ��
		{
			//_DBG_("wet1 over");
			doBeep = 1;
			DoAlarm(Wet3Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet3Over].relay-1);
		}
		if(checkTempAlarm(3) == TempStateLow)//�������¶�
		{
			//_DBG_("temp1 lack");
			doBeep = 1;
			DoAlarm(Temp3Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp3Lack].relay-1);
		}
		if(checkTempAlarm(3) == TempStateHigh)//�������¶�
		{
			//_DBG_("temp1 over");
			doBeep = 1;
			DoAlarm(Temp3Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp3Over].relay-1);
		}
		
		if(checkWetAlarm(4) == WetStateLow)//������ʪ�ȣ�
		{
			//_DBG_("wet1 lack");
			doBeep = 1;
			DoAlarm(Wet4Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet4Lack].relay-1);
		}
		if(checkWetAlarm(4) == WetStateHigh)//������ʪ��
		{
			//_DBG_("wet1 over");
			doBeep = 1;
			DoAlarm(Wet4Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Wet4Over].relay-1);
		}
		if(checkTempAlarm(4) == TempStateLow)//�������¶�
		{
			//_DBG_("temp1 lack");
			doBeep = 1;
			DoAlarm(Temp4Lack);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp4Lack].relay-1);
		}
		if(checkTempAlarm(4) == TempStateHigh)//�������¶�
		{
			//_DBG_("temp1 over");
			doBeep = 1;
			DoAlarm(Temp4Over);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp4Over].relay-1);
		}
		
// 		if(checkTempAlarm(2) == TempStateLow)//ϵͳ���¶�
// 		{
// 			//_DBG_("temp2 lack");
// 			doBeep = 1;
// 			DoAlarm(Temp2Lack);
// 			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp2Lack].relay-1);
// 		}
// 		if(checkTempAlarm(2) == TempStateHigh)//ϵͳ���¶�
// 		{
// 			//_DBG_("temp2 over");
// 			doBeep = 1;
// 			DoAlarm(Temp2Over);
// 			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Temp2Over].relay-1);
// 		}
		
		if(ACPowerDownAlarm())
		{
			doBeep = 1;
			DoAlarm(ACDown);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[ACDown].relay-1);
		}
		
		//���û�и澯
		if(!DoorAlarm(1)){
			//_DBG_("Door Resume");
			DoResumeAlarm(Door);
			if(APP_Persist_AlarmLevelRelay_Addr[Door].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Door].relay-1]--;
		}
		if(!SmokeAlarm(1)){
			//_DBG_("Smoke Resume");
			DoResumeAlarm(Smoke);
			if(APP_Persist_AlarmLevelRelay_Addr[Smoke].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Smoke].relay-1]--;
		}
		if(!WaterAlarm()){
			//_DBG_("Water Resume");
			DoResumeAlarm(Water);
			if(APP_Persist_AlarmLevelRelay_Addr[Water].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Water].relay-1]--;
		}
		if(!ThunderAlarm()){
			//_DBG_("Thunder Resume");
			DoResumeAlarm(Thunder);
			if(APP_Persist_AlarmLevelRelay_Addr[Thunder].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Thunder].relay-1]--;
		}
		if(!DoorAlarm(2)){
			//_DBG_("Open Resume");
			DoResumeAlarm(DoorTwo);
			if(APP_Persist_AlarmLevelRelay_Addr[DoorTwo].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[DoorTwo].relay-1]--;
		}
		if(!LeapAlarm()){
			//_DBG_("Leap Resume");
			DoResumeAlarm(Leap);
			if(APP_Persist_AlarmLevelRelay_Addr[Leap].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Leap].relay-1]--;
		}
		
		if(checkWetAlarm(1) == WetStateOK/* || getWet(1) == 0*/)//����ʪ��OK
		{
			//_DBG_("wet1 resume");
			DoResumeAlarm(Wet1Over);
			if(APP_Persist_AlarmLevelRelay_Addr[Wet1Over].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Wet1Over].relay-1]--;
			DoResumeAlarm(Wet1Lack);
			if(APP_Persist_AlarmLevelRelay_Addr[Wet1Lack].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Wet1Lack].relay-1]--;
			
		}
		if(checkTempAlarm(1) == TempStateOK/* || getTemp(1) == 0*/)//�����¶�OK
		{
			//_DBG_("temp1 resume");
			DoResumeAlarm(Temp1Over);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp1Over].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp1Over].relay-1]--;
			DoResumeAlarm(Temp1Lack);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp1Lack].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp1Lack].relay-1]--;
		}
		if(checkTempAlarm(2) == TempStateOK/* || getTemp(2) == 0*/)//�����¶�OK
		{
			//_DBG_("temp2 resume");
			DoResumeAlarm(Temp2Over);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp2Over].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp2Over].relay-1]--;
			DoResumeAlarm(Temp2Lack);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp2Lack].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp2Lack].relay-1]--;
		}
		if(checkTempAlarm(3) == TempStateOK/* || getTemp(3) == 0*/)//�����¶�OK
		{
			//_DBG_("temp2 resume");
			DoResumeAlarm(Temp3Over);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp3Over].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp3Over].relay-1]--;
			DoResumeAlarm(Temp3Lack);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp3Lack].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp3Lack].relay-1]--;
		}
		if(checkTempAlarm(4) == TempStateOK/* || getTemp(4) == 0*/)//�����¶�OK
		{
			//_DBG_("temp2 resume");
			DoResumeAlarm(Temp4Over);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp4Over].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp4Over].relay-1]--;
			DoResumeAlarm(Temp4Lack);
			if(APP_Persist_AlarmLevelRelay_Addr[Temp4Lack].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Temp4Lack].relay-1]--;
		}
		
		if(!ACPowerDownAlarm())
		{
			DoResumeAlarm(ACDown);
			if(APP_Persist_AlarmLevelRelay_Addr[ACDown].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[ACDown].relay-1]--;
		}
		
	}
	//�𶯴���300ʱ�����ڲ���һ�Σ�����һ��3��
	if(hasShakeSample>=10 && (OSTimeGet()-shakeExamTime>300))
	{
		shakeExamTime = OSTimeGet();
		if(ShakeAlarm()){
			//_DBG_("Shake");
			doBeep = 1;
			DoAlarm(Shake);
			AlarmRelayOut(APP_Persist_AlarmLevelRelay_Addr[Shake].relay-1);
		}
		
		if(!ShakeAlarm())
		{
			//_DBG_("Shake Resume");
			
			DoResumeAlarm(Shake);
			if(APP_Persist_AlarmLevelRelay_Addr[Shake].relay-1<RELAY_STATE_NUM)
				relayAlarms[APP_Persist_AlarmLevelRelay_Addr[Shake].relay-1]--;
		}
		
	}
		
		//�����澯�Ƿ񶼽������������ָ��̵�����ʼ״̬
		for(i=0;i<RELAY_STATE_NUM;i++)
		{
			//sprintf(_db,"alarms[%d]=%d",i,relayAlarms[i]);
			//DB;
			if(relayAlarms[i]<=0)
				AlarmResumeRelayOut(i);
		}
		if(doBeep==1)
		{
			BeepOn();
			OSTimeDly(400);
			BeepOff();
			
		}
		//_DBG_("S");
		//�¶Ȳ��������ֲ���һ��
		if(OSTimeGet()-lastTempTime>3000)
		{
			//_DBG_("TEST");
			data_Humidity1 = 0;
			data_Temperature1 = 0;
			OS_ENTER_CRITICAL();
			AM2301_CollectCharData(1,&data_Humidity1,&data_Temperature1);
			OS_EXIT_CRITICAL();
			
			OS_ENTER_CRITICAL();
			AM2301_CollectCharData(2,&data_Humidity2,&data_Temperature2);
			OS_EXIT_CRITICAL();
			
			OS_ENTER_CRITICAL();
			AM2301_CollectCharData(3,&data_Humidity3,&data_Temperature3);
			OS_EXIT_CRITICAL();
			
			OS_ENTER_CRITICAL();
			AM2301_CollectCharData(4,&data_Humidity4,&data_Temperature4);
			OS_EXIT_CRITICAL();
			
			sprintf(_db,"tempwet:%d,%d,%d,%d,%d,%d,%d,%d",data_Temperature1,data_Humidity1,
			data_Temperature2,data_Humidity2,data_Temperature3,data_Humidity3,data_Temperature4,data_Humidity4);
			DB;
			lastTempTime = OSTimeGet();
			
			Rs485SendInfo();//��RS485���͸澯��Ϣ
			//my_printf();
			timeCount = 0;
			my_printf("____");
			
			while(1)
			{
				timeCount++;
				lowCtrl();
				OS_ENTER_CRITICAL();
				rLen = UART_Receive(LPC_UART2,&tempRec,1,NONE_BLOCKING);
				OS_EXIT_CRITICAL();
				if(rLen>0)
				{
			 	my_printf("{%c,%d}",tempRec,tempRec);
					if(tempRec==0x7e)
					{ _DBG_("a");
						recIndex = 0;
						recData[recIndex] = tempRec;
					}
					else
					{ 
						if(tempRec != 0x00)
						{ _DBG_("b");
							recIndex++;
							recData[recIndex] = tempRec;
						}
						if(recIndex>7)
						{
							recIndex = 0;
						}
					}
					if(tempRec == 0x1d)
					{   _DBG_("c");
						if(recIndex==7)
						{ _DBG_("d");
							voltage1 = (uint8_t)((uint8_t)(getCharToHex(recData[1])*16)+(uint8_t)(getCharToHex(recData[2])));
							voltage2 = (uint8_t)((uint8_t)(getCharToHex(recData[3])*16)+(uint8_t)(getCharToHex(recData[4])));
							vSum = (uint8_t)((uint8_t)(getCharToHex(recData[5])*16)+(uint8_t)(getCharToHex(recData[6])));
							my_printf("v1=%d v2=%d sum=%d",voltage1,voltage2,vSum);
							if(voltage1^voltage2 == vSum)
							{
								//����������ȷ
								my_printf("vOk");
								if(voltage1==1 && voltage2==1)//˵���е�ѹ�澯
								{
									setACPowerDown(1);
								}else
								{
									setACPowerDown(0);
								}
							}
							highCtrl();
							break;
						}
					}
				}
				
				if(timeCount>TimeCountMax) break;
				OSTimeDly(1);
			}
			
		}
		WDT_Feed();
		//_DBG_("A");
		OSTimeDly(DETECTTIME_DURATION);
		//_DBG_("M");
		
	}
}
