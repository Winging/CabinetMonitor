#include "PacketAnalysis.h"
#include "AppDebug.h"
#include "RTCTime.h"
#include "App.h"
#include "HexToAsccii.h"
#include "AppPersist.h"
#include "Beep.h"
#include "AppSensor.h"
#include "IAP.h"

typedef struct floatRec
{
	int32_t f;
	uint8_t t;
	int8_t n;//-1 Ϊ���� 1 Ϊ����
} FloatType;

typedef enum ComExpEnum
{
	ComOk = 0,
	VerErr = 1,
	CheckSumErr = 2,
	LCheckSumErr = 3,
	CID2Err = 4,
	InvalidCmd = 5,
	InvalidData = 6
	
} RTNTypeEnum;

//����
extern uint8_t NetServerDataOut[265];//���������Ͱ�
extern uint32_t NetServerDataOutLen;
extern OS_EVENT * semServerDataOut;//����˷������ݻ���
//OS_EVENT * semClientDataOut;//�ͻ��˷������ݻ���
extern OS_EVENT * mBoxServerDataLen;//����Ϣ����Ҫ���͵����ݳ���
//OS_EVENT * mBoxClientDataLen;//����Ϣ����Ҫ���͵����ݳ���

extern char data_Humidity, data_Temperature;

void InitPackage(Package * pack,uint16_t infoLen);
void handleComExpception(RTNTypeEnum type);
uint8_t getOriginalChkSum(uint8_t packet[],uint8_t infolen);
uint8_t readSysStatus(uint8_t packet[],uint16_t infoLen);//4.1

uint8_t setEnvironmentShift(uint8_t packet[],uint16_t infoLen);//4.2
uint8_t readEnvironmentShift(uint8_t packet[],uint16_t infoLen);//4.3

uint8_t setVoltageShift(uint8_t packet[],uint16_t infoLen);//4.4
uint8_t readVoltageShift(uint8_t packet[],uint16_t infoLen);//4.5

uint8_t setDCAlarm(uint8_t packet[],uint16_t infoLen);//4.6
uint8_t readDCAlarm(uint8_t packet[],uint16_t infoLen);//4.7

uint8_t setACAlarm(uint8_t packet[],uint16_t infoLen);//4.8
uint8_t readACAlarm(uint8_t packet[],uint16_t infoLen);//4.9

uint8_t setEnvironmentAlarm(uint8_t packet[],uint16_t infoLen);//4.10
uint8_t readEnvironmentAlarm(uint8_t packet[],uint16_t infoLen);//4.11

uint8_t setAlarmLevelRelay(uint8_t packet[],uint16_t infoLen);//4.12
uint8_t readAlarmLevelRelay(uint8_t packet[],uint16_t infoLen);//4.13

uint8_t setRelay(uint8_t packet[],uint16_t infoLen);//4.14
uint8_t readRelay(uint8_t packet[],uint16_t infoLen);//4.15

uint8_t readTime(uint8_t packet[],uint16_t infoLen);//4.18
uint8_t setTime(uint8_t packet[],uint16_t infoLen);//4.19

uint8_t readProductNumber(uint8_t packet[],uint16_t infoLen);//4.20

uint8_t setModuleAddr(uint8_t packet[],uint16_t infoLen);//4.21
uint8_t readModuleAddr(uint8_t packet[],uint16_t infoLen);//4.22

uint8_t setBeepState(uint8_t packet[],uint16_t infoLen);//4.23
uint8_t readBeepState(uint8_t packet[],uint16_t infoLen);//4.24

uint8_t getAlarmHistory(uint8_t packet[],uint16_t infoLen);//4.25
uint8_t delAlarmHistory(uint8_t packet[],uint16_t infoLen);//4.26

uint8_t setSwitchState(uint8_t packet[],uint16_t infoLen);//4.27
uint8_t readSwitchState(uint8_t packet[],uint16_t infoLen);//4.28

uint8_t setFanArg(uint8_t packet[],uint16_t infoLen);//4.31
uint8_t readFanArg(uint8_t packet[],uint16_t infoLen);//4.32

uint8_t setFireArg(uint8_t packet[],uint16_t infoLen);//4.33
uint8_t readFireArg(uint8_t packet[],uint16_t infoLen);//4.34

uint8_t restartMachine(uint8_t packet[],uint16_t infoLen);//����ָ��豸��λ

void handleComExpception(uint8_t type)
{
	INT8U err;
	Package pack;
	RTNTypeEnum rtn;
	
	switch(type)
	{
		case CheckSumErr:
		{
			rtn = CheckSumErr;
			break;
		}
	}
	//У��ʹ���
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	pack.cCid2 = rtn;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
}
uint8_t getOriginalChkSum(uint8_t packet[],uint8_t infolen)
{
	uint8_t sumPos;
	sumPos = infolen*2+15;
	return (uint8_t)((getCharToHex(packet[sumPos])<<4) + getCharToHex(packet[sumPos+1]));
}
uint8_t PacketAnalyse(uint8_t packet[],uint32_t len)
{
	uint8_t sum,orgSum;
	uint16_t cidOne,cidTwo;
	uint32_t infoLen;
	
	
	infoLen = (uint8_t)(getCharToHex(packet[11])<<12) + (uint8_t)(getCharToHex(packet[12])<<8) + (uint8_t)(getCharToHex(packet[13])<<4) + getCharToHex(packet[14]);
	sum = GetCheckSum(packet,infoLen);
	orgSum = getOriginalChkSum(packet,infoLen);
	cidOne = (uint8_t)(getCharToHex(packet[7])<<4) + getCharToHex(packet[8]);//
	cidTwo = (uint8_t)(getCharToHex(packet[9])<<4) + getCharToHex(packet[10]);
	sprintf(_db,"infolen = %d cidOne=%d cidTwo=%d",infoLen,cidOne,cidTwo);
	DB;
	//����У����ж�
	/*
	if(sum!=orgSum)
	{
		sprintf(_db,"check err sum=%d org=%d",sum,orgSum);
		DB;
		handleComExpception(CheckSumErr);
		return CheckSumErr;
	}
	*/
	//���cidOne === 
	my_printf("compare cidOne= %d cidTwo=%d =%d",cidOne,cidTwo,cidOne==238);
	if(cidOne ==238)
	{
		//
		_DBG_("enter");
		if(cidTwo == 9)
		{
			return restartMachine(packet,infoLen);
		}
	}
	
	switch(cidTwo)
	{
		//��ȡϵͳ״̬
		case 0x01: return readSysStatus(packet,infoLen);
			break;
		
		//���û���ƫ����������
		case 0x02: return setEnvironmentShift(packet,infoLen);
			break;
		
		//��û���ƫ����������
		case 0x03:return  readEnvironmentShift(packet,infoLen);
			break;
		
		//���õ�ѹ�������ƫ��
		case 0x04: return setVoltageShift(packet,infoLen);
			break;
		
		//��õ�ѹ�������ƫ��
		case 0x05:return  readVoltageShift(packet,infoLen);
			break;
		
		//����ֱ����ѹ�澯����
		case 0x06: return setDCAlarm(packet,infoLen);
			break;
		
		//���ֱ����ѹ�澯����
		case 0x07:return  readDCAlarm(packet,infoLen);
			break;
			
		//���ý�����ѹ�澯����
		case 0x08: return setACAlarm(packet,infoLen);
			break;
			
		//��ý�����ѹ�澯����
		case 0x09:return  readACAlarm(packet,infoLen);
			break;
		
		//���û������澯����
		case 0x0A: return setEnvironmentAlarm(packet,infoLen);
			break;
			
		//��û������澯����
		case 0x0B:return  readEnvironmentAlarm(packet,infoLen);
			break;
			
		//���ø澯���𼰹������
		case 0x0C:return  setAlarmLevelRelay(packet,infoLen);
			break;
		
		//��ȡ�澯���𼰹������
		case 0x0D:return  readAlarmLevelRelay(packet,infoLen);
			break;
		
		//���ü̵����������
		case 0x0E:return  setRelay(packet,infoLen);
			break;
		
		//��ȡ�̵����������
		case 0x0F:return  readRelay(packet,infoLen);
			break;
		
		//����ʱ��
		case 0x12: return setTime(packet,infoLen);
			break;
		
		//��ȡʱ��
		case 0x13: return readTime(packet,infoLen);
			break;
			
		//��ò�Ʒ���к�
		case 0x14:return readProductNumber(packet,infoLen);
			break;
		
		//���ü��ģ���ַ
		case 0x15: return setModuleAddr(packet,infoLen);
			break;
		
		//��ü��ģ���ַ
		case 0x16: return readModuleAddr(packet,infoLen);
			break;
		
		//���÷�����״̬
		case 0x17: return setBeepState(packet,infoLen);
			break;	
		
		//��ȡ������״̬
		case 0x18: return readBeepState(packet,infoLen);
			break;
		
		//��ѯ��ʷ��¼
		case 0x19: return getAlarmHistory(packet,infoLen);
			break;
		
		//ɾ����ʷ��¼
		case 0x1A: return delAlarmHistory(packet,infoLen);
			break;
		
		//���ÿ������������
		case 0x1B: return setSwitchState(packet,infoLen);
			break;	
		
		//��ȡ�������������
		case 0x1C: return readSwitchState(packet,infoLen);
			break;
		
		//���÷��ȿ��Ʋ���
		case 0x1F: return setFanArg(packet,infoLen);
			break;
		
		//��ȡ���ȿ��Ʋ���
		case 0x20: return readFanArg(packet,infoLen);
			break;
			
		//���ü��������Ʋ���
		case 0x21: return setFanArg(packet,infoLen);
			break;
		
		//��ȡ���������Ʋ���
		case 0x22: return readFanArg(packet,infoLen);
			break;
	}
	
	return 2;
}
/*
 *@���� У���
 *
 */
uint8_t GetCheckSum(uint8_t packet[],uint32_t infoLen)
{
	uint8_t sum ;
	uint16_t i;
	sum = 0 ;
	for(i = 1 ; i <= 14+infoLen; i++ )
	{
		sum += packet[i];
	}
	sprintf(_db,"sum = %d ",sum);
	DB;
	return (sum % 255);
}
/*
 *@���� ��ʼ����
 *
 */
void InitPackage(Package * pack,uint16_t infoLen)
{
	uint8_t i;
	pack->cSoi = 0x7E;//��ʼ��
	pack->cVer = 0x02;//�汾��
	pack->AddrHigh = CON_ADDR1;//���Ƶ�ַ�߰�λ
	pack->AddrLow = CON_ADDR2;//���Ƶ�ַ�Ͱ�λ
	pack->cCid1 = 0xe1;//cidֵ Ĭ��Ϊ0xE1
	pack->cCid2 = 0x00;//����״��Ϊ 0x00,�ڲ���������´���
	pack->InfoLen = infoLen;//���ȳ�ʼ��ΪinfoLen;
	pack->cEoi = 0x0D;//������
	for(i=0;i<infoLen;i++)
	pack->Info[i]=0x00;
}
/*
 *@�������������ͼ��������ֽ� 
 *@���� �� intPart�������֣�dotPartС������ ��Ҫת��Ϊ������ٸ�0.125 ��0.5 dotPartΪ4
 *
 */
uint8_t integerToFraction(uint8_t dotPart)
{
	uint8_t u;
	switch(dotPart)
	{
		case 0:u=0;break;
		case 1:u=1;break;
		case 2:u=2;break;
		case 3:u=3;break;
		case 4:u=3;break;
		case 5:u=4;break;
		case 6:u=5;break;
		case 7:u=6;break;
		case 8:u=7;break;
		case 9:u=7;break;
	}
	return u;
}
void PushDataToInfo(Package * pack,int32_t intPart,uint8_t dotPart,int8_t neg,uint16_t start)
{
	uint8_t bits,intBits,negative;
	int32_t tempInt;
	//����Ǹ�����תΪ����
	if(intPart<0) 
	{
		intPart = (-1)*intPart;
		negative = 1;
	}
	else 
		negative = 0;
	if(neg == -1)
		negative = 1;
	//С������ת0.125�ĸ���
	dotPart = integerToFraction(dotPart);
	
	//������������
	tempInt = intPart;
	bits = 0;
	while(tempInt !=0 ) 
	{
		bits++;
		tempInt = tempInt/2;
	}
	intBits = bits + 3;//������λС����λ��
	bits = bits-1 + 127;//����Լ������ ��127
	//β������ ����������С����λ
	tempInt = (uint32_t)((intPart<<3)) + dotPart;
	sprintf(_db,"intPart = %d intBits=%d bits = %d,TEMP=%d",intPart,intBits,bits,tempInt);
	DB;
	//������λΪ1������
	if(negative == 1)
		pack->Info[start] = (uint8_t)((1<<7)+(bits>>1));
	else
		pack->Info[start] = (uint8_t)(bits>>1);
	//β����λ�ֽ�,����tempInt��λ1����
	if(intBits-7 >0)
		pack->Info[start+1] = (uint8_t)( (uint8_t)((bits%2)<<7) + (uint8_t)((tempInt>>(intBits - 8))&0x0000007f));
	else
		pack->Info[start+1] = (uint8_t)((uint8_t)((bits%2)<<7) + (uint8_t)((tempInt<<(8 - intBits ))&0x0000007f));
	
	//sprintf(_db,"  GET=%d  %d   ",(tempInt<<(7 - intBits + 1)),(uint8_t)((tempInt<<(7 - intBits + 1))&0x0000007f));
	//DB;
	//sprintf(_db,"  ADD=%d  ",((uint8_t)((bits%2)<<7) + (uint8_t)((tempInt<<(7 - intBits + 1))&0x0000007f)));
	//DB;
	
	//β����λ�ֽ�
	if(intBits - 16 > 0)
		pack->Info[start+2] = (uint8_t)((tempInt>>(intBits -16 ))&0x000000ff);
	else 
		pack->Info[start+2] = (uint8_t)((tempInt<<(16 - intBits))&0x000000ff);
	//��λ�ֽ�
	if(intBits - 24 > 0)
		pack->Info[start+3] = (uint8_t)((tempInt>>(intBits -24))&0x000000ff);
	else
		pack->Info[start+3] = (uint8_t)((tempInt<<(24- intBits))&0x000000ff);
	
	//sprintf(_db,"b1=%d b2=%d b3=%d b4=%d\n\r",pack->Info[start],pack->Info[start+1],pack->Info[start+2],pack->Info[start+3]);
	//DB;
}
/*
 *@�����������ֽڵ�����ת��Ϊ�������֣�������С������
 *@���� �� bytesΪԭʼ�ַ��������ַ�����һ��ʮ������
 *
 */
FloatType getFloat(uint8_t packet[],uint8_t start)
{
	FloatType ft;
	uint8_t byte1,byte2,byte3,byte4,jieMa;
	uint32_t res;
	byte1 = (uint8_t)((uint8_t)getCharToHex(packet[start])<<4) + getCharToHex(packet[start+1]);
	byte2 = (uint8_t)((uint8_t)getCharToHex(packet[start+2])<<4) + getCharToHex(packet[start+3]);
	byte3 = (uint8_t)((uint8_t)getCharToHex(packet[start+4])<<4) + getCharToHex(packet[start+5]);
	byte4 = (uint8_t)((uint8_t)getCharToHex(packet[start+6])<<4) + getCharToHex(packet[start+7]);
	jieMa = (uint8_t)((byte1<<1)+(uint8_t)(byte2>>7));
	//���β�����֣���0x80����Ϊ���������λ1��ʡ�ԣ�����Ҫ����
	res = (uint32_t)((byte2|0x80)<<16) + (uint32_t)(byte3<<8)+(uint32_t)byte4;
	//sprintf(_db,"B1=%d,%c%cB2=%d,b3=%d,b4=%d,jiema [%d %d]  \n\r",byte1,packet[start+2],packet[start+3],byte2,byte3,byte4,jieMa,res);
	//DB;
	//���ݽ����ȡ��������
	ft.f = res >>(150-jieMa);//��������
	//���С������
	ft.t  = (uint8_t)((res - (uint32_t)(ft.f<<(150-jieMa)))>>(150-jieMa -3));//������λ������С��������Ϊ0.125
	//sprintf(_db,"get float={%d,%d ,%d,%d}",ft.f<<(150-jieMa),(res - (uint32_t)(ft.f<<(150-jieMa))),(uint8_t)(res - ft.f<<(150-jieMa)),ft.t);
	//DB;
	ft.n = 1;
	if(byte1>127) 
	{
		ft.f =(-1)*ft.f;
		ft.n = -1;
		sprintf(_db,"is -(%d)",ft.f);
		DB;
	}
	return ft;
}
void pushFloatDataToInfo(Package * pack,float ft,uint16_t start)
{
	unsigned int * bytes;
	uint32_t theData;
	float data;
	
	data = ft;
	bytes=(unsigned int *)&data;
	theData =*bytes;
	my_printf("yousee is %d ",*bytes);
	//my_printf("yousee here is %d %d",((uint32_t)theData)%10,(uint32_t)theData/10);
	pack->Info[start]   = (uint8_t)((theData&0xff000000)>>24);
	pack->Info[start+1] = (uint8_t)(((theData)&0x00ff0000)>>16);
	pack->Info[start+2] = (uint8_t)(((theData)&0x0000ff00)>>8);
	pack->Info[start+3] = (uint8_t)(((theData)&0x000000ff));
}
float getFloatData(uint8_t packet[],uint8_t start)
{
	float* data;
	float resFloat;
	uint8_t byte1,byte2,byte3,byte4,jieMa;
	uint32_t res;
	int32_t test;
	byte1 = (uint8_t)((uint8_t)getCharToHex(packet[start])<<4) + getCharToHex(packet[start+1]);
	byte2 = (uint8_t)((uint8_t)getCharToHex(packet[start+2])<<4) + getCharToHex(packet[start+3]);
	byte3 = (uint8_t)((uint8_t)getCharToHex(packet[start+4])<<4) + getCharToHex(packet[start+5]);
	byte4 = (uint8_t)((uint8_t)getCharToHex(packet[start+6])<<4) + getCharToHex(packet[start+7]);
	my_printf("bytes %d %d %d %d ",byte1,byte2,byte3,byte4);
	res  = (uint32_t)(byte1<<24)+(uint32_t)(byte2<<16)+(uint32_t)(byte3<<8)+(uint32_t)(byte4);
	my_printf("res=%d",res);
	data = (float*)&res;
	my_printf("data=%d",data);
	resFloat = (*data);
	test=resFloat*10;
	my_printf("you float = %d",test);
	return resFloat;
}
/*
 *@����������ϵͳʱ��
 *
 */
uint8_t setTime(uint8_t packet[],uint16_t infoLen)
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t year,month,day,hour,minute,second,dayOfWeek;
	Package pack;
	second = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	minute = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	hour = (uint8_t)((uint8_t)getCharToHex(packet[19])<<4) + getCharToHex(packet[20]);
	day = (uint8_t)((uint8_t)getCharToHex(packet[21])<<4) + getCharToHex(packet[22]);
	dayOfWeek = (uint8_t)((uint8_t)getCharToHex(packet[23])<<4) + getCharToHex(packet[24]);
	month = (uint8_t)((uint8_t)getCharToHex(packet[25])<<4) + getCharToHex(packet[26]);
	year = (uint8_t)((uint8_t)getCharToHex(packet[27])<<4) + getCharToHex(packet[28]);
	//sprintf(_db,"time = %d %d %d %d %d %d \n\r",year,month,day,hour,minute,second);
	//DB;
	RTCSetTimeByYDMHMSW(year,month,day,hour,minute,second,dayOfWeek);//���ñ���ʱ��
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set time packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
/*
 *@��������ȡϵͳʱ��
 *
 */
uint8_t readTime(uint8_t packet[],uint16_t infoLen)
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	RTCTime LocalTime;
	LocalTime = RTCGetTime();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,7);//�������ݳ���Ϊ0
	pack.Info[0] = LocalTime.RTC_Sec;
	pack.Info[1] = LocalTime.RTC_Min;
	pack.Info[2] = LocalTime.RTC_Hour;
	pack.Info[3] = LocalTime.RTC_Mday;
	pack.Info[4] = LocalTime.RTC_Wday;
	pack.Info[5] = LocalTime.RTC_Mon;
	pack.Info[6] = LocalTime.RTC_Year - 2000;
	sprintf(_db,"second %d",pack.Info[0]);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"readTime return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}


/*
 *@��������ȡϵͳ״̬
 *
 */
uint8_t readSysStatus(uint8_t packet[],uint16_t infoLen)//4.1
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,45);//�������ݳ���Ϊ45�ֽ�
	pushFloatDataToInfo(&pack,0,0);//ֱ����ѹ
	pushFloatDataToInfo(&pack,0,4);//������ѹ
	pushFloatDataToInfo(&pack,getTemp(1),8);
	pushFloatDataToInfo(&pack,getWet(1),12);
	pushFloatDataToInfo(&pack,getTemp(2),16);
	pushFloatDataToInfo(&pack,getWet(2),20);
	
	pushFloatDataToInfo(&pack,getTemp(3),24);
	pushFloatDataToInfo(&pack,getWet(3),28);
	pushFloatDataToInfo(&pack,getTemp(4),32);
	pushFloatDataToInfo(&pack,getWet(4),36);
	
	//�澯״̬
	pack.Info[41] = GetTempWetAlarmOne();
	pack.Info[42] = GetTempWetAlarmTwo();
	pack.Info[43] = GetSmokeDoorAlarm();
	pack.Info[44] = GetNormalAlarm();
	pack.Info[45] = GetPowerAlarm();
	
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read sys status return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}
/*
 *@���������û���ƫ����
 *
 */
uint8_t setEnvironmentShift(uint8_t packet[],uint16_t infoLen)//4.2
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	FloatType ft;
	Package pack;
	CPU_SR cpu_sr;
	float data;
	
	OS_ENTER_CRITICAL();
	data = getFloatData(packet,15);
	my_printf("TEMP add %f",data);
	modifyAlarmShift(TempAdd,data);
	data = getFloatData(packet,23);
	my_printf("TEMP minus %f",data);
	modifyAlarmShift(TempMinus,data);
	data = getFloatData(packet,31);
	my_printf("Wet add %f",data);
	modifyAlarmShift(WetAdd,data);
	data = getFloatData(packet,39);
	my_printf("wet minus %f",data);
	modifyAlarmShift(WetMinus,data);
	OS_EXIT_CRITICAL();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set environment shift packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
/*
 *@��������ȡ����ƫ����
 *
 */
uint8_t readEnvironmentShift(uint8_t packet[],uint16_t infoLen)//4.3
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,16);//�������ݳ���Ϊ16�ֽ�
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[TempAdd].alarmShift,0);
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[TempMinus].alarmShift,4);
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[WetAdd].alarmShift,8);
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[WetMinus].alarmShift,12);
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read environ shift return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setVoltageShift(uint8_t packet[],uint16_t infoLen)//4.4
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	FloatType ft;
	Package pack;
	CPU_SR cpu_sr;
	float data;
	
	OS_ENTER_CRITICAL();
	/*
	ft = getFloat(packet,15);//ֱ����ѹ����
	sprintf(_db,"DC increase %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmShift(DCAdd,ft.f,ft.t,ft.n);
	ft = getFloat(packet,23);//ֱ����ѹƫ��
	sprintf(_db,"DC shift %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmShift(DCMinus,ft.f,ft.t,ft.n);
	ft = getFloat(packet,31);//������ѹ����
	sprintf(_db,"AC increase %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmShift(ACAdd,ft.f,ft.t,ft.n);
	ft = getFloat(packet,39);//������ѹƫ��
	sprintf(_db,"AC shift %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmShift(ACMinus,ft.f,ft.t,ft.n);
	*/
	data = getFloatData(packet,15);
	my_printf("DC add %f",data);
	modifyAlarmShift(DCAdd,data);
	data = getFloatData(packet,23);
	my_printf("DC minus %f",data);
	modifyAlarmShift(DCMinus,data);
	data = getFloatData(packet,31);
	my_printf("AC add %f",data);
	modifyAlarmShift(ACAdd,data);
	data = getFloatData(packet,39);
	my_printf("AC minus %f",data);
	modifyAlarmShift(ACMinus,data);
	OS_EXIT_CRITICAL();
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set voltage shift packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t readVoltageShift(uint8_t packet[],uint16_t infoLen)//4.5
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,16);//�������ݳ���Ϊ16�ֽ�
	
	/*
	PushDataToInfo(&pack,APP_Persist_AlarmShift_Addr[DCAdd].integer,APP_Persist_AlarmShift_Addr[DCAdd].fraction,APP_Persist_AlarmShift_Addr[DCAdd].negative,0);//ֱ����ѹ����
	PushDataToInfo(&pack,APP_Persist_AlarmShift_Addr[DCMinus].integer,APP_Persist_AlarmShift_Addr[DCMinus].fraction,APP_Persist_AlarmShift_Addr[DCMinus].negative,4);//ֱ����ѹƫ��
	PushDataToInfo(&pack,APP_Persist_AlarmShift_Addr[ACAdd].integer,APP_Persist_AlarmShift_Addr[ACAdd].fraction,APP_Persist_AlarmShift_Addr[ACAdd].negative,8);//������ѹ����
	PushDataToInfo(&pack,APP_Persist_AlarmShift_Addr[ACMinus].integer,APP_Persist_AlarmShift_Addr[ACMinus].fraction,APP_Persist_AlarmShift_Addr[ACMinus].negative,12);//������ѹƫ��
	*/
	
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[DCAdd].alarmShift,0);//ֱ������
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[DCMinus].alarmShift,4);//ֱ��ƫ��
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[ACAdd].alarmShift,8);//��������
	pushFloatDataToInfo(&pack,APP_Persist_AlarmShift_Addr[ACMinus].alarmShift,12);//����ƫ��
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read voltage shift return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setDCAlarm(uint8_t packet[],uint16_t infoLen)//4.6
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	FloatType ft;
	Package pack;
	CPU_SR cpu_sr;
	float data;
	OS_ENTER_CRITICAL();
	/*
	ft = getFloat(packet,15);//ֱ����ѹ�澯��
	sprintf(_db,"DC high %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(DCOverSet,ft.f,ft.t,ft.n);
	
	ft = getFloat(packet,23);//ֱ��Ƿѹ�澯��
	sprintf(_db,"DC less %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(DCLackSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,31);//ֱ������澯��
	sprintf(_db,"dC down %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(DCDownSet,ft.f,ft.t,ft.n);
	*/
	data = getFloatData(packet,15);
	my_printf("DC over %f",data);
	modifyAlarmSet(DCOverSet,data);
	data = getFloatData(packet,23);
	my_printf("DC lack %f",data);
	modifyAlarmSet(DCLackSet,data);
	data = getFloatData(packet,31);
	my_printf("DC %f",data);
	modifyAlarmSet(DCDownSet,data);
	
	OS_EXIT_CRITICAL();

	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set DC Alarm packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t readDCAlarm(uint8_t packet[],uint16_t infoLen)//4.7
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,12);//�������ݳ���Ϊ12�ֽ�
	/*
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCOverSet].integer,APP_Persist_AlarmSet_Addr[DCOverSet].fraction,APP_Persist_AlarmSet_Addr[DCOverSet].negative,0);//ֱ����ѹ�澯��
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCLackSet].integer,APP_Persist_AlarmSet_Addr[DCLackSet].fraction,APP_Persist_AlarmSet_Addr[DCLackSet].negative,4);//ֱ��Ƿѹ�澯��
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCDownSet].integer,APP_Persist_AlarmSet_Addr[DCDownSet].fraction,APP_Persist_AlarmSet_Addr[DCDownSet].negative,8);//ֱ������澯��
	*/
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCOverSet].alarmSet,0);//ֱ����ѹ
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCLackSet].alarmSet,4);//ֱ��Ƿѹ
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[DCDownSet].alarmSet,8);//ֱ������
	
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read DC Alarm return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setACAlarm(uint8_t packet[],uint16_t infoLen)//4.8
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	FloatType ft;
	Package pack;
	CPU_SR cpu_sr;
	float data;
	
	OS_ENTER_CRITICAL();
	/*
	ft = getFloat(packet,15);//������ѹ�澯��
	sprintf(_db,"AC increase %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(ACOverSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,23);//����Ƿѹ�澯��
	sprintf(_db,"AC less %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(ACLackSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,31);//��������澯��
	sprintf(_db,"AC down %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(ACDownSet,ft.f,ft.t,ft.n);
	*/
	
	data = getFloatData(packet,15);
	my_printf("AC over %f",data);
	modifyAlarmSet(ACOverSet,data);
	data = getFloatData(packet,23);
	my_printf("AC lack %f",data);
	modifyAlarmSet(ACLackSet,data);
	data = getFloatData(packet,31);
	my_printf("AC %f",data);
	modifyAlarmSet(ACDownSet,data);
	OS_EXIT_CRITICAL();
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set DC Alarm packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readACAlarm(uint8_t packet[],uint16_t infoLen)//4.9
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,12);//�������ݳ���Ϊ12�ֽ�
	/*
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACOverSet].integer,APP_Persist_AlarmSet_Addr[ACOverSet].fraction,APP_Persist_AlarmSet_Addr[ACOverSet].negative,0);//������ѹ�澯��
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACLackSet].integer,APP_Persist_AlarmSet_Addr[ACLackSet].fraction,APP_Persist_AlarmSet_Addr[ACLackSet].negative,4);//����Ƿѹ�澯��
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACDownSet].integer,APP_Persist_AlarmSet_Addr[ACDownSet].fraction,APP_Persist_AlarmSet_Addr[ACDownSet].negative,8);//��������澯��
	*/
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACOverSet].alarmSet,0);//������ѹ
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACLackSet].alarmSet,4);//����Ƿѹ
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[ACDownSet].alarmSet,8);//��������
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read AC Alarm return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setEnvironmentAlarm(uint8_t packet[],uint16_t infoLen)//4.10
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	FloatType ft;
	Package pack;
	CPU_SR cpu_sr;
	float data;
	
	OS_ENTER_CRITICAL();
	/*
	ft = getFloat(packet,15);
	sprintf(_db,"TEMP1 high %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Temp1OverSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,23);
	sprintf(_db,"TEMP1 low %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Temp1LackSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,31);
	sprintf(_db,"wet1 high %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Wet1OverSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,39);
	sprintf(_db,"wet1 low %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Wet1LackSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,47);
	sprintf(_db,"TEMP2 high %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Temp2OverSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,55);
	sprintf(_db,"TEMP2 low %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Temp2LackSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,63);
	sprintf(_db,"wet2 high %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Wet2OverSet,ft.f,ft.t,ft.n);
	ft = getFloat(packet,71);
	sprintf(_db,"wet2 low %d.%d",ft.f,ft.t);
	DB;
	modifyAlarmSet(Wet2LackSet,ft.f,ft.t,ft.n);
	*/
	
	data = getFloatData(packet,15);
	my_printf("temp1 high %f",data);
	modifyAlarmSet(Temp1OverSet,data);
	data = getFloatData(packet,23);
	my_printf("temp1 low %f",data);
	modifyAlarmSet(Temp1LackSet,data);
	data = getFloatData(packet,31);
	my_printf("wet1 high %f",data);
	modifyAlarmSet(Wet1OverSet,data);
	data = getFloatData(packet,39);
	my_printf("wet1 low %f",data);
	modifyAlarmSet(Wet1LackSet,data);
	data = getFloatData(packet,47);
	my_printf("temp2 high %f",data);
	modifyAlarmSet(Temp2OverSet,data);
	data = getFloatData(packet,55);
	my_printf("temp2 low %f",data);
	modifyAlarmSet(Temp2LackSet,data);
	data = getFloatData(packet,63);
	my_printf("wet2 high %f",data);
	modifyAlarmSet(Wet2OverSet,data);
	data = getFloatData(packet,71);
	my_printf("wet2 low %f",data);
	modifyAlarmSet(Wet2LackSet,data);
	
	data = getFloatData(packet,79);
	my_printf("temp3 high %f",data);
	modifyAlarmSet(Temp3OverSet,data);
	data = getFloatData(packet,87);
	my_printf("temp3 low %f",data);
	modifyAlarmSet(Temp3LackSet,data);
	data = getFloatData(packet,95);
	my_printf("wet3 high %f",data);
	modifyAlarmSet(Wet3OverSet,data);
	data = getFloatData(packet,103);
	my_printf("wet3 low %f",data);
	modifyAlarmSet(Wet3LackSet,data);
	
	data = getFloatData(packet,111);
	my_printf("temp4 high %f",data);
	modifyAlarmSet(Temp4OverSet,data);
	data = getFloatData(packet,119);
	my_printf("temp4 low %f",data);
	modifyAlarmSet(Temp4LackSet,data);
	data = getFloatData(packet,127);
	my_printf("wet4 high %f",data);
	modifyAlarmSet(Wet4OverSet,data);
	data = getFloatData(packet,135);
	my_printf("wet4 low %f",data);
	modifyAlarmSet(Wet4LackSet,data);
	OS_EXIT_CRITICAL();
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set environment shift packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t readEnvironmentAlarm(uint8_t packet[],uint16_t infoLen)//4.11
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,64);//�������ݳ���Ϊ64�ֽ�
	/*
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp1OverSet].integer,APP_Persist_AlarmSet_Addr[Temp1OverSet].fraction,APP_Persist_AlarmSet_Addr[Temp1OverSet].negative,0);//�¶�1����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp1LackSet].integer,APP_Persist_AlarmSet_Addr[Temp1LackSet].fraction,APP_Persist_AlarmSet_Addr[Temp1LackSet].negative,4);//�¶�1����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet1OverSet].integer,APP_Persist_AlarmSet_Addr[Wet1OverSet].fraction,APP_Persist_AlarmSet_Addr[Wet1OverSet].negative,8);//ʪ��1����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet1LackSet].integer,APP_Persist_AlarmSet_Addr[Wet1LackSet].fraction,APP_Persist_AlarmSet_Addr[Wet1LackSet].negative,12);//ʪ��1����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp2OverSet].integer,APP_Persist_AlarmSet_Addr[Temp2OverSet].fraction,APP_Persist_AlarmSet_Addr[Temp2OverSet].negative,16);//�¶�2����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp2LackSet].integer,APP_Persist_AlarmSet_Addr[Temp2LackSet].fraction,APP_Persist_AlarmSet_Addr[Temp2LackSet].negative,20);//�¶�2����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet2OverSet].integer,APP_Persist_AlarmSet_Addr[Wet2OverSet].fraction,APP_Persist_AlarmSet_Addr[Wet2OverSet].negative,24);//ʪ��2����
	PushDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet2LackSet].integer,APP_Persist_AlarmSet_Addr[Wet2LackSet].fraction,APP_Persist_AlarmSet_Addr[Wet2LackSet].negative,28);//ʪ��2����
	*/
	
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp1OverSet].alarmSet,0);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp1LackSet].alarmSet,4);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet1OverSet].alarmSet,8);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet1LackSet].alarmSet,12);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp2OverSet].alarmSet,16);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp2LackSet].alarmSet,20);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet2OverSet].alarmSet,24);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet2LackSet].alarmSet,28);//
	
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp3OverSet].alarmSet,32);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp3LackSet].alarmSet,36);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet3OverSet].alarmSet,40);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet3LackSet].alarmSet,44);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp4OverSet].alarmSet,48);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Temp4LackSet].alarmSet,52);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet4OverSet].alarmSet,56);//
	pushFloatDataToInfo(&pack,APP_Persist_AlarmSet_Addr[Wet4LackSet].alarmSet,60);//
	
	
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read environ shift return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setAlarmLevelRelay(uint8_t packet[],uint16_t infoLen)//4.12
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	CPU_SR cpu_sr;
	uint8_t level,relay,types;
	Package pack;

	types = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	level = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	relay = (uint8_t)((uint8_t)getCharToHex(packet[19])<<4) + getCharToHex(packet[20]);
	
	//���ø澯���𼰼̵������
	OS_ENTER_CRITICAL();
	modifyAlarmLevelRelay(types,level,relay);
	OS_EXIT_CRITICAL();
	/*
	for(err=0;err<24;err++)
	{
		resumeAlarm(err);
	}
	*/
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set alarm relay len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readAlarmLevelRelay(uint8_t packet[],uint16_t infoLen)//4.13
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	uint8_t types;
	
	//��ȡ�澯���
	types = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,3);//�������ݳ���Ϊ3
	pack.Info[0] = types;
	pack.Info[1] = APP_Persist_AlarmLevelRelay_Addr[types].level;
	pack.Info[2] = APP_Persist_AlarmLevelRelay_Addr[types].relay;
	sprintf(_db,"type relay {%d,%d,%d }",pack.Info[0],pack.Info[1],pack.Info[2]);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read alarm relay return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setRelay(uint8_t packet[],uint16_t infoLen)//4.14
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	CPU_SR cpu_sr;
	uint8_t relay,state;
	Package pack;

	relay = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	state = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	
	//���ü̵����������
	OS_ENTER_CRITICAL();
	modifyRelayState(relay,state);
	OS_EXIT_CRITICAL();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set  relay len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t readRelay(uint8_t packet[],uint16_t infoLen)//4.15
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	uint8_t relay;
	
	//��ȡ�̵������
	relay = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,2);//�������ݳ���Ϊ2
	pack.Info[0] = relay;
	pack.Info[1] = APP_Persist_RelayState_Addr[relay].state;
	sprintf(_db,"relay state{%d,%d}",pack.Info[0],pack.Info[1]);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read relay return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readProductNumber(uint8_t packet[],uint16_t infoLen)//4.20
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	uint32_t b[4];
	
	
	u32IAP_ReadSerialNumber(&b[0],&b[1],&b[2],&b[3]);//��ȡоƬ���к�
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,23);//�������ݳ���Ϊ2
	pack.Info[0] = 0;
	pack.Info[1] = 0;
	pack.Info[2] = 0;
	pack.Info[3] = SOFTWARE_VERSION_MAIN;
	pack.Info[4] = SOFTWARE_VERSION_SECOND;//����汾��
	pack.Info[5] = (uint8_t)((b[3]&0xff000000)>>24);
	pack.Info[6] = (uint8_t)((b[3]&0x00ff0000)>>16);
	pack.Info[7] = (uint8_t)((b[3]&0x0000ff00)>>8);
	pack.Info[8] = (uint8_t)((b[3]&0x000000ff));
	pack.Info[9] = (uint8_t)((b[2]&0xff000000)>>24);
	pack.Info[10] = (uint8_t)((b[2]&0x00ff0000)>>16);
	pack.Info[11] = (uint8_t)((b[2]&0x0000ff00)>>8);
	pack.Info[12] = (uint8_t)((b[2]&0x000000ff));
	pack.Info[13] = (uint8_t)((b[1]&0xff000000)>>24);
	pack.Info[14] = (uint8_t)((b[1]&0x00ff0000)>>16);
	pack.Info[15] = (uint8_t)((b[1]&0x0000ff00)>>8);
	pack.Info[16] = (uint8_t)((b[1]&0x000000ff));
	pack.Info[17] = (uint8_t)((b[0]&0xff000000)>>24);
	pack.Info[18] = (uint8_t)((b[0]&0x00ff0000)>>16);
	pack.Info[19] = (uint8_t)((b[0]&0x0000ff00)>>8);
	pack.Info[20] = (uint8_t)((b[0]&0x000000ff));
	pack.Info[21] = 0;
	pack.Info[22] = 0;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read product number return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setModuleAddr(uint8_t packet[],uint16_t infoLen)//4.21
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	CPU_SR cpu_sr;
	uint8_t addr1,addr2;
	Package pack;
	App_Persist_Module_Def mod;
	
	addr1 = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	addr2 = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	//���õ�ַ
	mod.Module_ADDR1 = addr1;
	mod.Module_ADDR2 = addr2;
	modifyModuleAddr(mod);
	/*
	for(err=0;err<24;err++)
	{
		resumeAlarm(err);
	}
	*/
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set module packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readModuleAddr(uint8_t packet[],uint16_t infoLen)//4.22
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	RTCTime LocalTime;
	LocalTime = RTCGetTime();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,2);//�������ݳ���Ϊ0
	pack.Info[0] = 0;
	pack.Info[1] = MONITOR_MODULE_ADDR;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read module return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setBeepState(uint8_t packet[],uint16_t infoLen)//4.23
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	CPU_SR cpu_sr;
	uint8_t state;
	Package pack;
	App_Persist_Module_Def mod;
	
	//״̬
	state = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	sprintf(_db,"state=%d",state);
	DB;
	
	OS_ENTER_CRITICAL();
	
	//���÷�����״̬
	if(state==1)
		BeepOpen();
	else if(state == 0)
		BeepLock();
	
	OS_EXIT_CRITICAL();
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set beep packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readBeepState(uint8_t packet[],uint16_t infoLen)//4.24
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;

	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,1);//�������ݳ���Ϊ0
	pack.Info[0] = GetBeepState();
	sprintf(_db,"beep {%d,}",pack.Info[0]);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read beep return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t getAlarmHistory(uint8_t packet[],uint16_t infoLen)//4.25
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t number,high,low,state,level;
	Package pack;
	SysAlarmTypeDef alarm;
	CPU_SR cpu_sr;
	
	high = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);//���ֽ�
	low = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	number = (uint8_t)((uint8_t)(high<<4) +low );//�����ֽڱ�ʾ���
	
	OS_ENTER_CRITICAL();
	alarm = getSysAlarm(number);
	OS_EXIT_CRITICAL();
	//alarm = APP_Persist_SysAlarmRecord_Addr[3];
	//�����Ӧ��¼�������򲻷�������
	sprintf(_db,"here you get alarmType=%d",alarm.alarmType);
	DB;
	//���alarmType==255��˵���޶�Ӧ�澯
	if(alarm.alarmType==0xff)
	{
		high = 0xff;
		low = 0xff;
		alarm.level = 0x30;
		alarm.alarmType = 0x34;
		alarm.state = 0x36;
	}
	
	//������Ӧ�ĸ澯��¼
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,12);//�������ݳ���Ϊ12
	pack.Info[0] = high;
	pack.Info[1] = low;
	pack.Info[2] = alarm.year;
	pack.Info[3] = alarm.month;
	pack.Info[4] = alarm.day;
	pack.Info[5] = alarm.hour;
	pack.Info[6] = alarm.minute;
	pack.Info[7] = alarm.second;
	pack.Info[8] = alarm.level;
	pack.Info[9] = alarm.alarmType-1;
	pack.Info[10] = alarm.state;
	pack.Info[11] = MONITOR_MODULE_ADDR;//�̶�Ϊ MONITOR_MODULE_ADDR
	sprintf(_db,"num=%d{%d,%d},%d,%d,%d,%d,%d,%d,{%d,%d,%dmod:%d}\n\r",number,high,low,pack.Info[2],pack.Info[3],pack.Info[4],
	pack.Info[5],pack.Info[6],pack.Info[7],pack.Info[8],pack.Info[9],pack.Info[10],(uint8_t)APP_Persist_Module_Addr->Module_ADDR2);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"get alarm history len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t delAlarmHistory(uint8_t packet[],uint16_t infoLen)//4.26
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	DelAllSysAlarm();
	OS_EXIT_CRITICAL();
	my_printf("begin%d end%d write%d sector%d",APP_Persist_RecordIndex_Addr[0].allWrite,APP_Persist_RecordIndex_Addr[0].recordBegin,APP_Persist_RecordIndex_Addr[0].recordEnd);
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"delete history len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setSwitchState(uint8_t packet[],uint16_t infoLen)//4.27
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	CPU_SR cpu_sr;
	uint8_t switchType,state;
	Package pack;

	switchType = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	state = (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	
	//���ü̵����������
	OS_ENTER_CRITICAL();
	modifySwitchState(switchType,state);
	OS_EXIT_CRITICAL();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set  switch len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t readSwitchState(uint8_t packet[],uint16_t infoLen)//4.28
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	Package pack;
	uint8_t switchType;
	int16_t switchState;
	CPU_SR cpu_sr;
	
	//��ȡ���������
	switchType = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	switchState = APP_Persist_SwitchState_Addr[switchType].state;
	if(switchState>1||switchState<0)
	{
		OS_ENTER_CRITICAL();
		modifySwitchState(switchType,0);
		OS_EXIT_CRITICAL();
		switchState=0;
	}
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,2);//�������ݳ���Ϊ2
	pack.Info[0] = switchType;
	pack.Info[1] = switchState;
	sprintf(_db,"switch state{%d,%d}",pack.Info[0],pack.Info[1]);
	DB;
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read switch return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}

uint8_t setFanArg(uint8_t packet[],uint16_t infoLen)//4.31
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t fanNumber,elecNumber;
	float open,sense;
	Package pack;
	CPU_SR cpu_sr;
	
	fanNumber = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	elecNumber= (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	//���ֽڱ���19 20 21 22 ����
	open = getFloatData(packet,23);//�������¶�
	my_printf("open  %f",open);
	sense = getFloatData(packet,31);//������
	my_printf("open  %f",sense);
	
	OS_ENTER_CRITICAL();
	modifyFanArg(fanNumber,elecNumber,open,sense);
	OS_EXIT_CRITICAL();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set fan arg packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;	
}
uint8_t readFanArg(uint8_t packet[],uint16_t infoLen)//4.32
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t fanNumber;
	Package pack;
	fanNumber = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);//���ȱ��
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,32);//�������ݳ���Ϊ12�ֽ�
	pack.Info[0] = fanNumber;//���ȱ��
	pack.Info[1] = APP_Persist_FanControlArg_Addr[fanNumber].relay;//
	pack.Info[2] = 0x00;
	pack.Info[3] = 0x00;
	pushFloatDataToInfo(&pack,APP_Persist_FanControlArg_Addr[fanNumber].open,4);
	pushFloatDataToInfo(&pack,APP_Persist_FanControlArg_Addr[fanNumber].sense,8);
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read fan arg return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t setFireArg(uint8_t packet[],uint16_t infoLen)//4.33
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t fireNumber,elecNumber;
	float open,sense;
	Package pack;
	CPU_SR cpu_sr;
	
	fireNumber = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);
	elecNumber= (uint8_t)((uint8_t)getCharToHex(packet[17])<<4) + getCharToHex(packet[18]);
	//���ֽڱ���19 20 21 22 ����
	open = getFloatData(packet,23);//�������¶�
	my_printf("open  %f",open);
	sense = getFloatData(packet,31);//������
	my_printf("open  %f",sense);
	
	OS_ENTER_CRITICAL();
	modifyFireArg(fireNumber,elecNumber,open,sense);
	OS_EXIT_CRITICAL();
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,0);//�������ݳ���Ϊ0
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"set fan arg packet len=%d\n\r",NetServerDataOutLen);
	DB;
	return 1;	
}
uint8_t readFireArg(uint8_t packet[],uint16_t infoLen)//4.34
{
	//packet�е�15���ֽڿ�ʼΪ�������ݣ��䳤��ΪinfoLen
	INT8U err;
	uint8_t fireNumber;
	Package pack;
	fireNumber = (uint8_t)((uint8_t)getCharToHex(packet[15])<<4) + getCharToHex(packet[16]);//���ȱ��
	
	OSSemPend(semServerDataOut,0,&err);
	//�����
	InitPackage(&pack,32);//�������ݳ���Ϊ12�ֽ�
	pack.Info[0] = fireNumber;//���ȱ��
	pack.Info[1] = APP_Persist_FireControlArg_Addr[fireNumber].relay;//
	pack.Info[2] = 0x00;
	pack.Info[3] = 0x00;
	pushFloatDataToInfo(&pack,APP_Persist_FireControlArg_Addr[fireNumber].open,4);
	pushFloatDataToInfo(&pack,APP_Persist_FireControlArg_Addr[fireNumber].sense,8);
	Packet2DataOut(pack);
	OSSemPost(semServerDataOut);
	OSMboxPost(mBoxServerDataLen,(void*)(0xff000000 |NetServerDataOutLen ));
	sprintf(_db,"read fan arg return len = %d",NetServerDataOutLen);
	DB;
	return 1;
}
uint8_t restartMachine(uint8_t packet[],uint16_t infoLen)
{
	CPU_SR cpu_sr;
	
	_DBG_("RESTART MACHINE");
	OS_ENTER_CRITICAL();
	updateCmdEraeseState();
	OS_EXIT_CRITICAL();
	_DBG_("erased");
	OSSchedLock();
	//���Ź��Ŀ���ʱ��
	WDT_UpdateTimeOut(10);
	OSSchedUnlock();
	my_printf("previos=%d timeout=%d",WDT_GetCurrentCount(),LPC_WDT->WDTC);
	_DBG_("HERE");
	OSSchedLock();
	while(1)
		my_printf("cur=%d timeout=%d",WDT_GetCurrentCount(),LPC_WDT->WDTC);//��ѭ��ʵ�ֿ��Ź���λ��ʵ����λ
	OSSchedUnlock();
}