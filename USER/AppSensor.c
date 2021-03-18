#include "AppSensor.h"
#include "TemSensor.h"
#include "AppPersist.h"
#include "AppDebug.h"

extern uint32_t xSample,ySample,zSample;//x y z 轴上加速的采样值，多次采样取平均获得
static uint8_t ACPowerDown;
//内部函数
static void  InitSmokeInput();
static void InitDoorInput();
static uint8_t SwitchAlarm(uint8_t port,uint8_t pin,SwitchStateTypes state);

void InitSensor()
{
//	LPC_GPIO1->FIODIR &= ~(0X3F << 18);    //传感器端口设置为输入
//	LPC_GPIO0->FIODIR &= ~(0X01 << 27);    //倾斜传感
    LPC_GPIO1->FIODIR &= ~(0X01 << 19);    //水禁	
	LPC_GPIO1->FIODIR &= ~(0X01 << 20);    //防雷
	LPC_GPIO1->FIODIR &= ~(0X01 << 21);    //MDF
	LPC_GPIO1->FIODIR &= ~(0X01 << 22);    //UPS
	
	InitDoorInput();//初始化门禁管脚
	InitSmokeInput();//初始化烟雾管脚
	initLM();//初始化LM75A传感器
}
static void  InitSmokeInput()
{
	LPC_GPIO3->FIODIR &= ~(0X01 << 25);   //烟雾 1 
	LPC_GPIO0->FIODIR &= ~(0X01 << 29);   
	LPC_GPIO0->FIODIR &= ~(0X01 << 30);
	LPC_GPIO1->FIODIR &= ~(0X01 << 18);   //烟雾 4
}
static void InitDoorInput()
{
	LPC_GPIO1->FIODIR &= ~(0X01 << 30);    //门禁 1
	LPC_GPIO0->FIODIR &= ~(0X01 << 28);
	LPC_GPIO0->FIODIR &= ~(0X01 << 27);
	LPC_GPIO3->FIODIR &= ~(0X01 << 26);    //门禁 4    
}
uint8_t HasAlarm(uint8_t type)
{
	switch(type)
	{
		
	case Leap:
	{
		return LeapAlarm();
	}
	case Door:
	{
		return DoorAlarm(1);
	}
	case DoorTwo:
	{
		return DoorAlarm(2);
	}
	case DoorThree:
	{
		return DoorAlarm(3);
	}
	case DoorFour:
	{
		return DoorAlarm(4);
	}
	case Smoke:
	{
		return SmokeAlarm(1);
	}
	case SmokeTwo:
	{
		return SmokeAlarm(2);
	}
	case SmokeThree:
	{
		return SmokeAlarm(3);
	}
	case SmokeFour:
	{
		return SmokeAlarm(4);
	}
	case Thunder:
	{
		return ThunderAlarm();
	}
// 	case DoorTwo:
// 	{
// 		return DoorAlarm(2);
// 	}
	case Water:
	{
		return WaterAlarm();
	}
	case Shake:
	{
		return ShakeAlarm();
	}
	case Temp1Over:
	{
		return (checkTempAlarm(1)==TempStateHigh)?1:0;
	}
	case Temp1Lack:
	{
		return (checkTempAlarm(1)==TempStateLow)?1:0;
	}
	case Temp2Over:
	{
		return (checkTempAlarm(2)==TempStateHigh)?1:0;
	}
	case Temp2Lack:
	{
		return (checkTempAlarm(2)==TempStateLow)?1:0;
	}
	case Temp3Over:
	{
		return (checkTempAlarm(3)==TempStateHigh)?1:0;
	}
	case Temp3Lack:
	{
		return (checkTempAlarm(3)==TempStateLow)?1:0;
	}
	case Temp4Over:
	{
		return (checkTempAlarm(4)==TempStateHigh)?1:0;
	}
	case Temp4Lack:
	{
		return (checkTempAlarm(4)==TempStateLow)?1:0;
	}
	/*不用
	case Temp5Over:
	{
		return (checkTempAlarm(5)==TempStateHigh)?1:0;
	}
	case Temp5Lack:
	{
		return (checkTempAlarm(5)==TempStateLow)?1:0;
	}
	*/
	case Wet1Over:
	{
		return (checkWetAlarm(1)==WetStateHigh)?1:0;
	}
	case Wet1Lack:
	{
		return (checkWetAlarm(1)==WetStateLow)?1:0;
	}
	case Wet2Over:
	{
		return (checkWetAlarm(2)==WetStateHigh)?1:0;
	}
	case Wet2Lack:
	{
		return (checkWetAlarm(2)==WetStateLow)?1:0;
	}
	case Wet3Over:
	{
		return (checkWetAlarm(3)==WetStateHigh)?1:0;
	}
	case Wet3Lack:
	{
		return (checkWetAlarm(3)==WetStateLow)?1:0;
	}
	case Wet4Over:
	{
		return (checkWetAlarm(4)==WetStateHigh)?1:0;
	}
	case Wet4Lack:
	{
		return (checkWetAlarm(4)==WetStateLow)?1:0;
	}
	/*
	case Wet5Over:
	{
		return (checkWetAlarm(5)==WetStateHigh)?1:0;
	}
	case Wet5Lack:
	{
		return (checkWetAlarm(5)==WetStateLow)?1:0;
	}
	*/
	case ACDown:
	{
		return ACPowerDownAlarm();
	}
	
	}
	return 0;
}

uint8_t SmokeAlarm(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			return SwitchAlarm(3,25,SmokeState);
		}
		case 2:
		{
			return SwitchAlarm(0,29,SmokeTwoState);
		}
		case 3:
		{
			return SwitchAlarm(0,30,SmokeThreeState);
		}
		case 4:
		{
			return SwitchAlarm(1,18,SmokeFourState);
		}
	}
}
uint8_t DoorAlarm(uint8_t number)
{
	switch(number)
	{
		case 1:
		{
			return SwitchAlarm(1,30,DoorState);
		}
		case 2:
		{
			return SwitchAlarm(0,28,DoorTwoState);
		}
		case 3:
		{
			return SwitchAlarm(0,27,DoorThreeState);
		}
		case 4:
		{
			return SwitchAlarm(3,26,DoorFourState);
		}
	}
}
/*
 *@描述：返回开关量告警信息
 */
static uint8_t SwitchAlarm(uint8_t port,uint8_t pin,SwitchStateTypes state)
{
	uint32_t uTemp;
	uint8_t s;
	uTemp = GPIO_ReadValue(port);
	if(!((uTemp>>pin)&0x0001))
	{
		s = 1;
	}
	else
	s = 0;
	switch(s)
	{
		case 0:
		{
			if(APP_Persist_SwitchState_Addr[state].state!=1)
				return 0;
			else 
				return 1;
			break;
		}
		case 1:
			if(APP_Persist_SwitchState_Addr[state].state!=1)
				return 1;
			else 
				return 0;
			break;
	}
}
uint8_t WaterAlarm()
{
	return SwitchAlarm(1,19,WaterState);
}
uint8_t ThunderAlarm()
{
	return SwitchAlarm(1,20,ThunderState);
}
uint8_t LeapAlarm()
{
	uint32_t uTemp;
	uTemp = GPIO_ReadValue(1);
	if(!((uTemp>>27)&0x0001))
	{
		return 1;
	}
	return 0;
}
//获取两个数之差的绝对值
uint8_t theAbs(uint32_t x,uint32_t y)
{
	if(x>y)
		return x-y;
	else 
		return y-x;
}
uint8_t ShakeAlarm()
{
	uint32_t x,y,z;
	x = readAcceleration(1);
	y = readAcceleration(2);
	z = readAcceleration(3);
	//sprintf(_db,"------x=%d y=%d z=%d --------",x,y,z);
	//DB;
	if(theAbs(x,xSample)>100) return 1;
	if(theAbs(y,ySample)>100) return 1;
	if(theAbs(z,zSample)>100) return 1;
	return 0;
}
/*
 *@描述：加速传感器
 *
 *
 */
uint32_t readAcceleration(uint8_t t)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t adc_value, tmp;
	
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0X00;
	switch(t)
	{
		case 1:
		{
			PinCfg.Pinnum = 24;
			PinCfg.Portnum = 0;
			PINSEL_ConfigPin(&PinCfg);
	
			ADC_Init(LPC_ADC, 200000);
			ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,DISABLE);
			ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);
			ADC_StartCmd(LPC_ADC,ADC_START_NOW);
			while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_0,ADC_DATA_DONE)));
			adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);
			break;
		}
		case 2:
		{
			PinCfg.Pinnum = 25;
			PinCfg.Portnum = 0;
			PINSEL_ConfigPin(&PinCfg);
	
			ADC_Init(LPC_ADC, 200000);
			ADC_IntConfig(LPC_ADC,ADC_ADINTEN1,DISABLE);
			ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_1,ENABLE);
			ADC_StartCmd(LPC_ADC,ADC_START_NOW);
			while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_1,ADC_DATA_DONE)));
			adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1);
			break;
		}
		case 3:
		{
			PinCfg.Pinnum = 26;
			PinCfg.Portnum = 0;
			PINSEL_ConfigPin(&PinCfg);
	
			ADC_Init(LPC_ADC, 200000);
			ADC_IntConfig(LPC_ADC,ADC_ADINTEN2,DISABLE);
			ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE);
			ADC_StartCmd(LPC_ADC,ADC_START_NOW);
			while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,ADC_DATA_DONE)));
			adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
			break;
		}
	}
	return adc_value;
}

/*
 *@描述：返回系统告警状态，在获取系统状态时使用
 *
 *
 */
uint8_t GetSysAlarm()
{
	uint8_t t;
	
	t= 0;
	if(checkWetAlarm(2)==WetStateLow)//湿度2低湿度
		t=t | 0x80;
	if(checkWetAlarm(2)==WetStateHigh)//湿度2高湿度
		t=t | 0x40;
	if(checkTempAlarm(2) == TempStateLow)//温度2低温
		t=t | 0x20;
	if(checkTempAlarm(2) == TempStateHigh)//温度2高温
		t=t | 0x10;
	if(LeapAlarm())//倾斜
		t=t | 0x08;
	if(ShakeAlarm())//撞击
		t= t | 0x04;
	if(0)//MDF
		t= t | 0x02;
	if(0)//电源通讯
		t= t | 0x01;
	return t;
}
/*
 *@描述：返回环境告警状态，在获取系统状态时使用
 *
 *
 */
uint8_t GetEnvironAlarm()
{
	uint8_t t;
	
	t= 0;
	if(0)//自定义输入异常
		t=t | 0x80;
	if(DoorAlarm(1))//门磁
		t=t | 0x40;
	if(SmokeAlarm(1))//烟雾
		t=t | 0x20;
	if(WaterAlarm())//水浸
		t=t | 0x10;
	if(checkWetAlarm(1) == WetStateLow)//环境低湿度，
		t=t | 0x08;
	if(checkWetAlarm(1) == WetStateHigh)//环境高湿度
		t= t | 0x04;
	if(checkTempAlarm(1) == TempStateLow)//环境低温度
		t= t | 0x02;
	if(checkTempAlarm(1) == TempStateHigh)//环境高温度
		t= t | 0x01;
	return t;
}
/*
 *@描述：返回电源告警状态，在获取系统状态时使用
 *
 *
 */
uint8_t GetPowerAlarm()
{
	uint8_t t;
	
	t= 0;
	if(0)//电源系统
		t=t | 0x80;
	if(ThunderAlarm())//防雷
		t=t | 0x40;
	if(ACPowerDownAlarm())//交流掉电
		t=t | 0x20;
	if(0)//直流掉电
		t=t | 0x10;
	if(0)//交流欠压
		t=t | 0x08;
	if(0)//交流过压
		t= t | 0x04;
	if(0)//直流欠压
		t= t | 0x02;
	if(0)//直流过压
		t= t | 0x01;
	return t;
}
uint8_t GetTempWetAlarmOne()
{
	uint8_t t;
	
	t= 0;
	if(checkWetAlarm(1)==WetStateLow)//湿度1低湿度
		t=t | 0x80;
	if(checkWetAlarm(1)==WetStateHigh)//湿度1高湿度
		t=t | 0x40;
	if(checkTempAlarm(1) == TempStateLow)//温度1低温
		t=t | 0x20;
	if(checkTempAlarm(1) == TempStateHigh)//温度1高温
		t=t | 0x10;
	if(checkWetAlarm(2)==WetStateLow)//湿度2低湿度
		t=t | 0x08;
	if(checkWetAlarm(2)==WetStateHigh)//湿度2高湿度
		t= t | 0x04;
	if(checkTempAlarm(2) == TempStateLow)//温度2低温
		t= t | 0x02;
	if(checkTempAlarm(2) == TempStateHigh)//温度2高温
		t= t | 0x01;
	return t;
}
uint8_t GetTempWetAlarmTwo()
{
	uint8_t t;
	
	t= 0;
	if(checkWetAlarm(3)==WetStateLow)//湿度3低湿度
		t=t | 0x80;
	if(checkWetAlarm(3)==WetStateHigh)//湿度3高湿度
		t=t | 0x40;
	if(checkTempAlarm(3) == TempStateLow)//温度3低温
		t=t | 0x20;
	if(checkTempAlarm(3) == TempStateHigh)//温度3高温
		t=t | 0x10;
	if(checkWetAlarm(4)==WetStateLow)//湿度4低湿度
		t=t | 0x08;
	if(checkWetAlarm(4)==WetStateHigh)//湿度4高湿度
		t= t | 0x04;
	if(checkTempAlarm(4) == TempStateLow)//温度4低温
		t= t | 0x02;
	if(checkTempAlarm(4) == TempStateHigh)//温度4高温
		t= t | 0x01;
	return t;
}
uint8_t GetSmokeDoorAlarm()
{
	uint8_t t;
	
	t= 0;
	if(DoorAlarm(1))//门禁1
		t=t | 0x80;
	if(DoorAlarm(2))//门禁1
		t=t | 0x40;
	if(DoorAlarm(3))//门禁1
		t=t | 0x20;
	if(DoorAlarm(4))//门禁1
		t=t | 0x10;
	if(SmokeAlarm(1))//烟雾1告警
		t=t | 0x08;
	if(SmokeAlarm(2))//烟雾2告警
		t= t | 0x04;
	if(SmokeAlarm(3))//烟雾3告警
		t= t | 0x02;
	if(SmokeAlarm(4))//烟雾4告警
		t= t | 0x01;
	return t;
}
uint8_t GetNormalAlarm()
{
	uint8_t t;
	
	t= 0;
	if(WaterAlarm())//水浸
		t=t | 0x80;
	if(ThunderAlarm())//防雷
		t=t | 0x40;
	if(HasAlarm(MDF))//MDF
		t=t | 0x20;
	if(HasAlarm(Shake))//撞击告警
		t=t | 0x10;
	if(HasAlarm(UPS))//UPS告警
		t=t | 0x08;
	if(HasAlarm(PowerCom))//电源通讯告警
		t= t | 0x04;
	if(0)//保留
		t= t | 0x02;
	if(0)//保留
		t= t | 0x01;
	return t;
}
/*
 *@描述 设置掉电标识
 */
void setACPowerDown(uint8_t flag)
{
	ACPowerDown = flag;
}
/*
 *@描述：是否掉电告警
 */
uint8_t ACPowerDownAlarm()
{
	if(ACPowerDown==1)
		return 1;
	else
	return 0;
}