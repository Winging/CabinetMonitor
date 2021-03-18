#include "AppTaskKeyScan.h"
#include "AppDebug.h"
extern unsigned char menuOne[8][5];
extern OS_EVENT * mBoxKeyCode;//短消息，按键编码
#define LONGPressTime 700
#define DoubleClickTime 400
void AppTaskKeyScan(void * pdata)
{
	/***************************
	 *keyCode 1 up 2 down 3 left 4 right 5 ok 6 cancel
	 **************************/
	
	uint8_t keyCode;
	uint32_t uTemp;
	clock_time_t time;
	
	pdata = pdata;
	while(1)
	{
		//看门狗喂狗
		WDT_Feed();
		
	keyCode = 0;
	if(key_up())
	{
		_DBG_("UP");
		time = OSTimeGet();
		while(key_up())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyUpLong;
			}
		}
		if(keyCode != keyUpLong)
				keyCode = keyUp;
		
		if(keyCode == keyUp)
		{
			my_printf("detect second");
			while(OSTimeGet()-time<DoubleClickTime)
			{
				if(key_up())
				{
					keyCode = keyUpDouble;
					break;
				}
			}
			if(keyCode==keyUpDouble)
			my_printf("up double click ");
		}
	}
	else if(key_down())
	{
		time=OSTimeGet();
		while(key_down())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyDownLong;
			}
		}
		if(keyCode != keyDownLong)
				keyCode = keyDown;
		if(keyCode == keyDown)
		{
			my_printf("detect second");
			while(OSTimeGet()-time<DoubleClickTime)
			{
				if(key_down())
				{
					keyCode = keyDownDouble;
					break;
				}
			}
			if(keyCode==keyDownDouble)
			my_printf("down double click ");
		}
	}
	else if(key_left())
	{
		time = OSTimeGet();
		while(key_left())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyLeftLong;
			}
		}
		if(keyCode != keyLeftLong)
				keyCode = keyLeft;
	}
	else if(key_right())
	{
		time = OSTimeGet();
		while(key_right())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyRightLong;
			}
		}
		if(keyCode != keyRightLong)
				keyCode = keyRight;
	}
	else if(key_ok())
	{

		time = OSTimeGet();
		while(key_ok())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyOkLong;
			}
			
		}
		if(keyCode != keyOkLong)
				keyCode = keyOk;
		if(keyCode == keyOk)
		{
			my_printf("detect second ok");
			while(OSTimeGet()-time<DoubleClickTime)
			{
				if(key_ok())
				{
					keyCode = keyOkDouble;
					break;
				}
			}
			if(keyCode==keyOkDouble)
			my_printf("key cancel click ");
		}
	}
	else if(key_cancel())
	{
		time = OSTimeGet();
		while(key_cancel())
		{
			if(OSTimeGet()-time>LONGPressTime) 
			{ 
				keyCode = keyCancelLong;
				break;
			}
		}
		if(keyCode != keyCancelLong)
			keyCode = keyCancel;
	}
	if(keyCode>0)
	{
		sprintf(_db,"code = %d",keyCode);
		DB;
		OSMboxPost(mBoxKeyCode,(void*)(0xff000000 |keyCode ));
	}
	//_DBG_("C");
	OSTimeDly(10);
	//_DBG_("B");
	}
}