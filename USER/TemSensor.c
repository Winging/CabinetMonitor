#include "TemSensor.h"
#include "AppPersist.h"
#include "LM75A.h"
/************************** PRIVATE functions *************************/
//功能：操作与am2301的数据位相连的单片机端口与相应位，包括：输入、输出、置1和清0

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
/*声明*****************************************/
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
函数名：AM2301_CollectCharData(int *pvHumidity,int *pvTemperature)
功  能：获得数据（湿度、温度），获得的数据保存于形参所提供的指针所指向的变量中
返回值：
                0：数据校验正确；
                1：数据校验错误
                2：获取数据错误
                3: 等待响应信号错误
参  数：
                pvHumidity:   湿度变量的指针，用于保存采集到的湿度，数据类型为：signed char，得到的数据仅为湿度的整数部分，小数部分自动舍弃
                pvTemperature:温度变量的指针，用于保存采集到的温度，数据类型为：signed char，得到的数据仅为温度的整数部分，小数部分自动舍弃


说  明：建议执行此函数的间隔为2秒，绝不能小1秒。若间隔时间很长，建议采集2次，以2次为准
                当发生错误，即返回值为1、2或3时，应间隔2秒后重新读取数据
注  意：因为：  WinAVR默认将char 视为unsigned char 
                所以：参数必须定义为signed char，-128<signed char<127，足以保存所得到的数据
原  理：详见AM2301的数据手册
*********************************************************************************/
unsigned char AM2301_CollectCharData(uint8_t number,char *pvHumidity,char *pvTemperature)
{
    unsigned char lvReturn=3,lvCount;
    unsigned int  lvTemp16;
    int lvTemp;

    unsigned char  lvHumidityHigh;            //湿度高位
    unsigned char  lvHumidityLow;             //湿度低位
    unsigned char  lvTemperatureHigh;         //温度高位
    unsigned char  lvTemperatureLow;          //温度低位
    unsigned char  lvCheck;                   //校验位

    //主机的操作
    SetDataBitOutput(number);
    ClearDataBit(number);                             //拉低总线
	//Delay_Us(500*2);						    //持续500us
	Delay_Us(5000);						    //持续500us
    SetDataBit(number);                               //释放总线

    //主机检测从机
    //SetDataBitInput();
    Delay_Us(40);
	SetDataBitInput(number);

    if(!TestDataBit(number))
    {
        lvCount=1;
        while(!(TestDataBit(number)) && (lvCount > 0)) //判断丛机是否发送了80us的低电平信号，是否结束
                lvCount++;
        if(lvCount==0)
                return 3;
        lvCount=1;
        while((TestDataBit(number)) && (lvCount > 0))  //判断丛机是否发送了80us的高电平信号，如发出则进入数据接收状态
           lvCount++;
        if(lvCount==0)
								return 3;  
		        
        if(AM2301_ReadByte(number,&lvHumidityHigh))       //获得湿度高位
           return 2;

        if(AM2301_ReadByte(number,&lvHumidityLow))        //获得湿度高位
           return 2;

        if(AM2301_ReadByte(number,&lvTemperatureHigh))    //获得湿度高位
           return 2;

        if(AM2301_ReadByte(number,&lvTemperatureLow))     //获得湿度高位
           return 2;

        if(AM2301_ReadByte(number,&lvCheck))               //获得湿度高位
           return 2;


		SetDataBitInput(number);	//CF
     	SetDataBit(number);   	//CF

        lvCount=lvHumidityHigh + lvHumidityLow + lvTemperatureHigh + lvTemperatureLow; //计算湿度高位+湿度低位+温度高位+温度低位的和

        if(lvCount==lvCheck)           //数据校验，若正确则获得正确的湿度和温度，并返回1
        {
            //lvTemperatureHigh |=0x80;            
            //把湿度的高位和地位组合，得到完整的湿度
            lvTemp16=lvHumidityHigh;
            lvTemp16 <<=8;
            lvTemp16 |= lvHumidityLow;
            *pvHumidity=lvTemp16/10;

            //把温度的高位和地位组合，得到完整的温度
            lvTemp16=lvTemperatureHigh;
            lvTemp16 <<=8;
            lvTemp16 |= lvTemperatureLow;
            if(lvTemp16 & 0x8000)                  //如果温度是负值，则转化
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
        else                                      //数据校验，若错误，则返回1
            lvReturn=1;
    }

    //SetDataBitInput();	//CF
		//SetDataBit();   	//CF

    return lvReturn;
}

//函数实现
//说明：共3个函数，分别是：
//AM2301_ReadByte(unsigned char *pvData)
//AM2301_CollectCharData(int *pvHumidity,int *pvTemperature)
//AM2301_CollectFloatData(float *pvHumidity,float *pvTemperature)
/********************************************************************************
函数名：AM2301_ReadByte(unsigned char *pvData)
功  能：读取1个字节的数据
返回值：0：成功；2：读数据错误
参  数：pvData:所读取的数据
条  件：与am2301的数据位相连的单片机端口为“输入模式”
注  意:此函数为辅助函数，仅供本文件使用，外部无法访问
原  理：数据是从高位往低位依次读取，读得数据的顺序为：湿度高位+湿度低位+温度高位+温度低位+校验位
*********************************************************************************/
unsigned char AM2301_ReadByte(uint8_t number,unsigned char *pvData)
{
   unsigned char lvReturn=0,lvI,lvCount,lvBit;

   for(lvI=0;lvI<8;lvI++)                 //循环8次，得到1个字节（含8个数据位）的数据
   {
    lvCount=1;
    while(!(TestDataBit(number))&& (lvCount>0)) //判断丛机是否发来50us的低电平信号，若超时返回2
        lvCount++;
    if(lvCount==0)
    {
	    lvReturn=2;
	    break;
    }
	//Delay_Us(35);		                  //延时35us
	Delay_Us(60);		                  //延时60us
		
    lvBit=0;		                      
    if(TestDataBit(number))	                  //判断数据是0或1
       lvBit=1;

    lvCount=1;
    while((TestDataBit(number))&& (lvCount>0))     //判断丛机是否发来26-28us的高电平信号，若超时返回2
      lvCount++;

    if(lvCount==0)
    {
	   lvReturn=2;
	   break;
    }   
    if(lvBit)					          //把得到的每一位数据保存于pvData中
       *pvData |= 1<<(7-lvI);
    else
       *pvData &= ~1<<(7-lvI);
  }
  return lvReturn;
}
/*
 *@描述：是否有温度报警
 *@参数：number 温度传感器编号 温度1 或温度2 
 *@返回：TempStateTypeDef 
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
 *@描述：是否有湿度报警
 *@参数：number 湿度度传感器编号 湿度1 或湿度2 
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
 *@描述：获得温度数据
 *@参数：传感器编号
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
		/*不用
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
 *@描述：获得温度数据
 *@参数：传感器编号
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