#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__
#include "includes.h"
#include "AppPersist.h"
//深圳RT12232B系列(控制器1520),时序:6800

//BKEN
#define SET_BKEN   LPC_GPIO2->FIOCLR = 0x01<<8      
#define CLR_BKEN   LPC_GPIO2->FIOSET |= 0x01<<8  

/*
 *按键类型
 */
typedef enum keyTypeEnum
{
    keyUp = 1,
	keyDown,
	keyLeft,
	keyRight,
	keyOk,
	keyCancel,
	keyUpLong ,
	keyDownLong,
	keyLeftLong,
	keyRightLong,
	keyOkLong,
	keyCancelLong,
	keyUpDouble,
	keyDownDouble,
	keyOkDouble,
	keyInvalid,
} keyType;

/********************************************************************
 *函数声明
 *******************************************************************/
void INILCDConrol(void);
void INITLCD (void);
void clrscr(void);
void clrBelow(void);
void FILLONE(unsigned char k);
void FILLTOW(unsigned char i,unsigned char k);
void FILLALL(void);
void WRPicture (unsigned char *PICTURE);
void DrawASCII(uint32_t d_where,uint32_t x_add,uint8_t layer);
void Draw_word1(uint32_t d_where,uint32_t start, uint32_t x_add,uint8_t layer,uint8_t width,uint8_t  character[]) ;
void Draw_Menu(uint8_t character[],uint8_t len);
void DisplayMenu(uint8_t menuIndexes[],uint8_t len);
void DisplaySetting(float data, uint8_t type);
void DisplayAlarmSetting(float alarmSet, uint8_t alarmType);
void DisplayIP(uint8_t addr,uint8_t type);
void DisplayPort(uint16_t port);
void DisplayIPBlink(uint8_t type);
void DisplayPortBlink(uint16_t port);
void DisplayVersion(uint32_t main,uint32_t second);
void DisplayBelowInfo(uint8_t menuIndexes[],uint8_t len);
uint8_t DisplayAlarmRecord(int16_t number,uint8_t len);
void DisplayTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second);
//按键接口
void key_init(void);
uint8_t key_ok();
uint8_t key_cancel();
uint8_t key_up();
uint8_t key_down();
uint8_t key_right();
uint8_t key_left();

#endif