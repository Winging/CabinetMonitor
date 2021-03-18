#include "TemSensor.h"
#include "AppPersist.h"
#include "LM75A.h"
/************************** PRIVATE functions *************************/
//���ܣ�������am2301������λ�����ĵ�Ƭ���˿�����Ӧλ�����������롢�������1����0

static void SetDataBitOutput(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			GPIO_SetDir(1, (1<<23), 1);
			break;
		}
		case 2:
		{
			GPIO_SetDir(1, (1<<24), 1);
			break;
		}
		case 3:
		{
			GPIO_SetDir(1, (1<<25), 1);
			break;
		}
		case 4:
		{
			GPIO_SetDir(1, (1<<26), 1);
			break;
		}
	}
}
static void SetDataBitInput(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			GPIO_SetDir(1, (1<<23), 0);
			break;
		}
		case 2:
		{
			GPIO_SetDir(1, (1<<24), 0);
			break;
		}
		case 3:
		{
			GPIO_SetDir(1, (1<<25), 0);
			break;
		}
		case 4:
		{
			GPIO_SetDir(1, (1<<26), 0);
			break;
		}
	}
}
static void SetDataBit(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			GPIO_SetValue(1, (1<<23));
			break;
		}
		case 2:
		{
			GPIO_SetValue(1, (1<<24));
			break;
		}
		case 3:
		{
			GPIO_SetValue(1, (1<<25));
			break;
		}
		case 4:
		{
			GPIO_SetValue(1, (1<<26));
			break;
		}
	}
}
static void ClearDataBit(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			GPIO_ClearValue(1, (1<<23));
			break;
		}
		case 2:
		{
			GPIO_ClearValue(1, (1<<24));
			break;
		}
		case 3:
		{
			GPIO_ClearValue(1, (1<<25));
			break;
		}
		case 4:
		{
			GPIO_ClearValue(1, (1<<26));
			break;
		}
	}
}
/*����*****************************************/
extern char data_Humidity1, data_Temperature1,data_Humidity2, data_Temperature2,
data_Humidity3, data_Temperature3,data_Humidity4, data_Temperature4;

unsigned char TestDataBit(uint8_t number );
unsigned char TestDataBit(uint8_t number)
{
	unsigned int tmp;
	unsigned data;
	switch(number)
	{
		case 1:
		{
			tmp =  GPIO_ReadValue(1);
			data = ((tmp>>23) & 0x01); 
			return data;
			break;
		}
		case 2:
		{
			tmp =  GPIO_ReadValue(1);
			data = ((tmp>>24) & 0x01); 
			return data;
			break;
		}
		case 3:
		{
			tmp =  GPIO_ReadValue(1);
			data = ((tmp>>25) & 0x01); 
			return data;
			break;
		}
		case 4:
		{
			tmp =  GPIO_ReadValue(1);
			data = ((tmp>>26) & 0x01); 
			return data;
			break;
		}
	}
}
void Delay_Us (unsigned long tick) 
{
    unsigned int t;
	t=0;
    while(t <= tick*13) t++;
	//myprintf("times=%d",t);
}
unsigned char AM2301_ReadByte(uint8_t number,unsigned char *pvData);

/********************************************************************************
��������AM2301_CollectCharData(int *pvHumidity,int *pvTemperature)
��  �ܣ�������ݣ�ʪ�ȡ��¶ȣ�����õ����ݱ������β����ṩ��ָ����ָ��ı�����
����ֵ��
                0������У����ȷ��
                1������У�����
                2����ȡ���ݴ���
                3: �ȴ���Ӧ�źŴ���
��  ����
                pvHumidity:   ʪ�ȱ�����ָ�룬���ڱ���ɼ�����ʪ�ȣ���������Ϊ��signed char���õ������ݽ�Ϊʪ�ȵ��������֣�С�������Զ�����
                pvTemperature:�¶ȱ�����ָ�룬���ڱ���ɼ������¶ȣ���������Ϊ��signed char���õ������ݽ�Ϊ�¶ȵ��������֣�С�������Զ�����


˵  ��������ִ�д˺����ļ��Ϊ2�룬������С1�롣�����ʱ��ܳ�������ɼ�2�Σ���2��Ϊ׼
                ���������󣬼�����ֵΪ1��2��3ʱ��Ӧ���2������¶�ȡ����
ע  �⣺��Ϊ��  WinAVRĬ�Ͻ�char ��Ϊunsigned char 
                ���ԣ��������붨��Ϊsigned char��-128<signed char<127�����Ա������õ�������
ԭ  �����AM2301�������ֲ�
*********************************************************************************/
unsigned char AM2301_CollectCharData(uint8_t number,char *pvHumidity,char *pvTemperature)
{
    unsigned char lvReturn=3,lvCount;
    unsigned int  lvTemp16;
    int lvTemp;

    unsigned char  lvHumidityHigh;            //ʪ�ȸ�λ
    unsigned char  lvHumidityLow;             //ʪ�ȵ�λ
    unsigned char  lvTemperatureHigh;         //�¶ȸ�λ
    unsigned char  lvTemperatureLow;          //�¶ȵ�λ
    unsigned char  lvCheck;                   //У��λ

    //�����Ĳ���
    SetDataBitOutput(number);
    ClearDataBit(number);                             //��������
	//Delay_Us(500*2);						    //����500us
	Delay_Us(5000);						    //����500us
    SetDataBit(number);                               //�ͷ�����

    //�������ӻ�
    //SetDataBitInput();
    Delay_Us(40);
	SetDataBitInput(number);

    if(!TestDataBit(number))
    {
        lvCount=1;
        while(!(TestDataBit(number)) && (lvCount > 0)) //�жϴԻ��Ƿ�����80us�ĵ͵�ƽ�źţ��Ƿ����
                lvCount++;
        if(lvCount==0)
                return 3;
        lvCount=1;
        while((TestDataBit(number)) && (lvCount > 0))  //�жϴԻ��Ƿ�����80us�ĸߵ�ƽ�źţ��緢����������ݽ���״̬
           lvCount++;
        if(lvCount==0)
								return 3;  
		        
        if(AM2301_ReadByte(number,&lvHumidityHigh))       //���ʪ�ȸ�λ
           return 2;

        if(AM2301_ReadByte(number,&lvHumidityLow))        //���ʪ�ȸ�λ
           return 2;

        if(AM2301_ReadByte(number,&lvTemperatureHigh))    //���ʪ�ȸ�λ
           return 2;

        if(AM2301_ReadByte(number,&lvTemperatureLow))     //���ʪ�ȸ�λ
           return 2;

        if(AM2301_ReadByte(number,&lvCheck))               //���ʪ�ȸ�λ
           return 2;


		SetDataBitInput(number);	//CF
     	SetDataBit(number);   	//CF

        lvCount=lvHumidityHigh + lvHumidityLow + lvTemperatureHigh + lvTemperatureLow; //����ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ�ĺ�

        if(lvCount==lvCheck)           //����У�飬����ȷ������ȷ��ʪ�Ⱥ��¶ȣ�������1
        {
            //lvTemperatureHigh |=0x80;            
            //��ʪ�ȵĸ�λ�͵�λ��ϣ��õ�������ʪ��
            lvTemp16=lvHumidityHigh;
            lvTemp16 <<=8;
            lvTemp16 |= lvHumidityLow;
            *pvHumidity=lvTemp16/10;

            //���¶ȵĸ�λ�͵�λ��ϣ��õ��������¶�
            lvTemp16=lvTemperatureHigh;
            lvTemp16 <<=8;
            lvTemp16 |= lvTemperatureLow;
            if(lvTemp16 & 0x8000)                  //����¶��Ǹ�ֵ����ת��
            {
                    lvTemp16 &= 0x7FFF;
                    lvTemp = 0-lvTemp16;
										*pvTemperature=lvTemp /10 ;
            }
						else
						{
										*pvTemperature=lvTemp16/10 ;
						}
            //*pvTemperature=lvTemp /10 ;
            lvReturn=0;
        }
        else                                      //����У�飬�������򷵻�1
            lvReturn=1;
    }

    //SetDataBitInput();	//CF
		//SetDataBit();   	//CF

    return lvReturn;
}

//����ʵ��
//˵������3���������ֱ��ǣ�
//AM2301_ReadByte(unsigned char *pvData)
//AM2301_CollectCharData(int *pvHumidity,int *pvTemperature)
//AM2301_CollectFloatData(float *pvHumidity,float *pvTemperature)
/********************************************************************************
��������AM2301_ReadByte(unsigned char *pvData)
��  �ܣ���ȡ1���ֽڵ�����
����ֵ��0���ɹ���2�������ݴ���
��  ����pvData:����ȡ������
��  ������am2301������λ�����ĵ�Ƭ���˿�Ϊ������ģʽ��
ע  ��:�˺���Ϊ�����������������ļ�ʹ�ã��ⲿ�޷�����
ԭ  �������ǴӸ�λ����λ���ζ�ȡ���������ݵ�˳��Ϊ��ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ+У��λ
*********************************************************************************/
unsigned char AM2301_ReadByte(uint8_t number,unsigned char *pvData)
{
   unsigned char lvReturn=0,lvI,lvCount,lvBit;

   for(lvI=0;lvI<8;lvI++)                 //ѭ��8�Σ��õ�1���ֽڣ���8������λ��������
   {
    lvCount=1;
    while(!(TestDataBit(number))&& (lvCount>0)) //�жϴԻ��Ƿ���50us�ĵ͵�ƽ�źţ�����ʱ����2
        lvCount++;
    if(lvCount==0)
    {
	    lvReturn=2;
	    break;
    }
	//Delay_Us(35);		                  //��ʱ35us
	Delay_Us(60);		                  //��ʱ60us
		
    lvBit=0;		                      
    if(TestDataBit(number))	                  //�ж�������0��1
       lvBit=1;

    lvCount=1;
    while((TestDataBit(number))&& (lvCount>0))     //�жϴԻ��Ƿ���26-28us�ĸߵ�ƽ�źţ�����ʱ����2
      lvCount++;

    if(lvCount==0)
    {
	   lvReturn=2;
	   break;
    }   
    if(lvBit)					          //�ѵõ���ÿһλ���ݱ�����pvData��
       *pvData |= 1<<(7-lvI);
    else
       *pvData &= ~1<<(7-lvI);
  }
  return lvReturn;
}
/*
 *@�������Ƿ����¶ȱ���
 *@������number �¶ȴ�������� �¶�1 ���¶�2 
 *@���أ�TempStateTypeDef 
 */

TempStateTypeDef checkTempAlarm(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(1))>APP_Persist_AlarmSet_Addr[Temp1OverSet].alarmSet)
				return TempStateHigh;
			else if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(1))<APP_Persist_AlarmSet_Addr[Temp1LackSet].alarmSet)
				return TempStateLow;
			else 
				return TempStateOK;
			break;
		}
		case 2:
		{
			if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(2))>APP_Persist_AlarmSet_Addr[Temp2OverSet].alarmSet)
				return TempStateHigh;
			else if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(2))<APP_Persist_AlarmSet_Addr[Temp2LackSet].alarmSet)
				return TempStateLow;
			else 
				return TempStateOK;
			break;
		}
		case 3:
		{
			if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(3))>APP_Persist_AlarmSet_Addr[Temp3OverSet].alarmSet)
				return TempStateHigh;
			else if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(3))<APP_Persist_AlarmSet_Addr[Temp3LackSet].alarmSet)
				return TempStateLow;
			else 
				return TempStateOK;
			break;
		}
		case 4:
		{
			if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(4))>APP_Persist_AlarmSet_Addr[Temp4OverSet].alarmSet)
				return TempStateHigh;
			else if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(4))<APP_Persist_AlarmSet_Addr[Temp4LackSet].alarmSet)
				return TempStateLow;
			else 
				return TempStateOK;
			break;
		}
		/*
		case 5:
		{
			if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(5))>APP_Persist_AlarmSet_Addr[Temp5OverSet].alarmSet)
				return TempStateHigh;
			else if((APP_Persist_AlarmShift_Addr[TempAdd].alarmShift-APP_Persist_AlarmShift_Addr[TempMinus].alarmShift+getTemp(5))<APP_Persist_AlarmSet_Addr[Temp5LackSet].alarmSet)
				return TempStateLow;
			else 
				return TempStateOK;
			break;
		}
		*/
	}
	return TempStateOK;
}
/*
 *@�������Ƿ���ʪ�ȱ���
 *@������number ʪ�ȶȴ�������� ʪ��1 ��ʪ��2 
 */
WetStateTypeDef checkWetAlarm(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(1))>APP_Persist_AlarmSet_Addr[Wet1OverSet].alarmSet)
				return WetStateHigh;
			else if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(1))<APP_Persist_AlarmSet_Addr[Wet1LackSet].alarmSet)
				return WetStateLow;
			else 
				return WetStateOK;
			break;
		}
		case 2:
		{
			if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(2))>APP_Persist_AlarmSet_Addr[Wet2OverSet].alarmSet)
				return WetStateHigh;
			else if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(2))<APP_Persist_AlarmSet_Addr[Wet2LackSet].alarmSet)
				return WetStateLow;
			else 
				return WetStateOK;
			break;
		}
		case 3:
		{
			if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(3))>APP_Persist_AlarmSet_Addr[Wet3OverSet].alarmSet)
				return WetStateHigh;
			else if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(3))<APP_Persist_AlarmSet_Addr[Wet3LackSet].alarmSet)
				return WetStateLow;
			else 
				return WetStateOK;
			break;
		}
		case 4:
		{
			if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(4))>APP_Persist_AlarmSet_Addr[Wet4OverSet].alarmSet)
				return WetStateHigh;
			else if((APP_Persist_AlarmShift_Addr[WetAdd].alarmShift-APP_Persist_AlarmShift_Addr[WetMinus].alarmShift+getWet(4))<APP_Persist_AlarmSet_Addr[Wet4LackSet].alarmSet)
				return WetStateLow;
			else 
				return WetStateOK;
			break;
		}
	}
	return WetStateOK;
}
/*
 *@����������¶�����
 *@���������������
 */
char getTemp(uint8_t number)
{
	CPU_SR cpu_sr;
	char temp;
	temp = 0;
	switch(number)
	{
		case 1:
		{
			OS_ENTER_CRITICAL();
			temp = data_Temperature1;
			OS_EXIT_CRITICAL();
			break;
		}
		case 2:
		{
			OS_ENTER_CRITICAL();
			temp = data_Temperature2;
			OS_EXIT_CRITICAL();
			break;
		}
		case 3:
		{
			OS_ENTER_CRITICAL();
			temp = data_Temperature3;
			OS_ENTER_CRITICAL();
			break;
		}
		case 4:
		{
			OS_ENTER_CRITICAL();
			temp = data_Temperature4;
			OS_ENTER_CRITICAL();
		}
		/*����
		case 5:
		{
			OS_ENTER_CRITICAL();
			temp = 13;
			OS_EXIT_CRITICAL();
		}
		*/
	}
	return temp;
}
/*
 *@����������¶�����
 *@���������������
 */
char getWet(uint8_t number)
{
	CPU_SR cpu_sr;
	char wet;
	wet = 0;
	switch(number)
	{
		case 1:
		{
			OS_ENTER_CRITICAL();
			wet = data_Humidity1;
			OS_EXIT_CRITICAL();
			break;
		}
		case 2:
		{
			OS_ENTER_CRITICAL();
			wet = data_Humidity2;
			OS_EXIT_CRITICAL();
			break;
		}
		case 3:
		{
			OS_ENTER_CRITICAL();
			wet = data_Humidity3;
			OS_EXIT_CRITICAL();
			break;
		}
		case 4:
		{
			OS_ENTER_CRITICAL();
			wet = data_Humidity4;
			OS_EXIT_CRITICAL();
			break;
		}
		
	}
	return wet;
}