#include "AppSampleUtils.h"

/*内部函识ㄒ妪，比如读取电压可以调用这个函数******************************************************/


/*读取状态***分组定义*********************************************************/
/*
 * 描述：读取48V电压
 *返回 "000"-"999" "AAA"表示未用； 例535 表示53.5v 
 */
Type_Of_Voltage      ReadStatus_GetVoltage()
{
	Type_Of_Voltage v;
	v.a1 = 'A';
	v.a2 = 'A';
	v.a3 = 'A';
	return v;
}
/*
 *描述：读取温度 1
 *返回 "000"-"999" 例 "050" 表示50C "250"表示-50C
 */
Type_Of_Temprature   ReadStatus_GetTemprate()
{
	Type_Of_Temprature t;
	t.a1 = '0';
	t.a2 = '0';
	t.a3 = '0';
	return t;
}
/*
 *描述：门禁状态
 *返回：0 正常，1 停用 'A' 未启用
 */
uint8_t               ReadStatus_GetDoorState()
{
	return 0;
}
/*
 *描述：市电状态
 *返回：0 正常，1 停用 'A' 未启用
 */
uint8_t              ReadStatus_GetElecState()
{

}
/*
 *描述：告警标志
 *返回：0 无 ；1 有未上传
 */
uint8_t              ReadStatus_GetAlarmFlag()
{
	return 0;
}



/* 设置门限信息*************************************/
/*
 *描述：设置48v电压过压值
 *参数 a1a2a3 为字符 "000"-"999" 例如 "480" 为48.0V
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetVoltageOver(uint8_t a1,uint8_t a2,uint8_t a3)
{
	//实现
	return 1;
}
/*
 *描述：设置48v电压欠压值
 *参数 a1a2a3 为字符 "000"-"999" 例如 "480" 为48.0V
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetVoltageLack(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *描述：设置48v电压过低压值
 *参数 a1a2a3 为字符 "000"-"999" 例如 "480" 为48.0V
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetVoltageLess(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *描述：设置风扇热交换温度
 *参数 a1a2a3 为字符 "000"-"999" 例如 "080" 为80C "250"为-50C
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetFanTemp(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *描述：主设备仓制冷
 *参数 a1a2a3 为字符 "000"-"999" 例如 "080" 为80C "250"为-50C
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetMainCold(uint8_t a1,uint8_t a2,uint8_t a3)
{
	return 1;
}
/*
 *描述：主设备仓制热
 *参数 a1a2a3 为字符 "000"-"999" 例如 "080" 为80C "250"为-50C
 *返回：1表示成功 0表示失败
 *
 */
uint8_t              SetDoor_SetMainHot(uint8_t a1,uint8_t a2,uint8_t a3);
