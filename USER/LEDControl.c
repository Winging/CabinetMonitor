#include "LEDControl.h"
#include "AppDebug.h"
#include "math.h"
/********************************************************************** 
 *���������
 **********************************************************************/
#define nop() __nop()//asm("nop")

//RESET
#define SET_RESET  LPC_GPIO0->FIOSET |= 0x01<<8      
#define CLR_RESET  LPC_GPIO0->FIOCLR |= 0x01<<8 

//A0---H -> DATA, L -> ָ�� 
#define SET_A0  LPC_GPIO0->FIOSET |= 0x01<<9         
#define CLR_A0  LPC_GPIO0->FIOCLR |= 0x01<<9


//RW  //DIR?
#define SET_RW LPC_GPIO0->FIOSET |= 0x01<<4    //
#define CLR_RW LPC_GPIO0->FIOCLR |= 0x01<<4    //

//E1
#define SET_E1 LPC_GPIO0->FIOSET |= 0x01<<5        
#define CLR_E1 LPC_GPIO0->FIOCLR |= 0x01<<5

//E2
#define SET_E2 LPC_GPIO0->FIOSET |= 0x01<<6       
#define CLR_E2 LPC_GPIO0->FIOCLR = 0x01<<6



//LEDDISPLY
#define SET_LEDDISPLY  LPC_GPIO2->FIOSET |= 0x01<<9            
#define CLR_LEDDISPLY  LPC_GPIO2->FIOCLR = 0x01<<9  

#define SET_GLCDRW_READ  LPC_GPIO0->FIOSET = 0X01 << 7;//��
#define CLR_GLCDRW_WRITE  LPC_GPIO0->FIOCLR = 0X01 << 7; //д  

/********************************************************************
 *��������
 ********************************************************************/
volatile unsigned long cnt;

//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

typedef unsigned int uint;
typedef unsigned char uchar;
unsigned int i,j,k,t,m;
unsigned char command,Data;
static uint32_t uTemp;
extern unsigned char  ascii[];
extern unsigned char MenuOne_Temp1[];
extern unsigned char fonts[];
extern unsigned  char menuThree[26][6];
/***********************************************************************
 *@description:Һ���ײ���ƽӿ�
 ***********************************************************************/
void BUSYL(void);
void BUSYR(void);
void WRCOMMAND1 (unsigned char command);
void WRCOMMAND2 (unsigned char command);
void WR_DATA1 (unsigned char Data);
void WR_DATA2 (unsigned char Data);
void SetPage(uint8_t page0,uint8_t page1);
void SetAddress(uint8_t address0,uint8_t address1);
void PutCharR(uint8_t ch);
void PutCharL(uint8_t ch);

void TIMER0_IRQHandler(void)
{
	if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)== SET)
	{
		cnt++;
	}
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

void DelayMS(unsigned long tick) 
{
    uint32_t t;
	t=0;
    while(t <= tick*200000) t++;
}

void TIMER_INIT(void)
{
        // Initialize timer 0, prescale count time of 100uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 100;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
	// Set Match value, count value of 10000 (10000 * 100uS = 1000000us = 1s --> 1 Hz)
	TIM_MatchConfigStruct.MatchValue   = 10;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	// To start timer 0
	TIM_Cmd(LPC_TIM0,ENABLE);
}




void BUSYL(void)
{
  unsigned char temp; 
  while(1)
  {
    //LPC_GPIO2->FIODIR |= 0XFF;
    //LPC_GPIO2->FIOPIN0 = 0XFF;          //P1=0xFF;
    LPC_GPIO2->FIODIR &= ~0XFF;
    SET_E1;                             //E1=1;
    CLR_E2;                             //E2=0;
    //SET_RW;                             //R_W=1;
    CLR_RW; 
    SET_GLCDRW_READ;
    
    CLR_A0;                             //A0=0;
    temp = LPC_GPIO2->FIOPIN0;
    if ((temp&0x80)==0)
    {
      LPC_GPIO2->FIODIR |= 0XFF;
      break;
    } 
  }
}

void BUSYR(void)
{
  unsigned char temp; 
  while(1)
  {
    LPC_GPIO2->FIODIR &= ~0XFF;
    //LPC_GPIO2->FIODIR |= 0XFF;
    //LPC_GPIO2->FIOPIN0 = 0XFF;          //P1=0xFF;
    SET_E2;                             //E2=1;
    CLR_E1;                             //E1=0;
    //SET_RW;                             //R_W=1;
    CLR_RW;
    SET_GLCDRW_READ;
    //LPC_GPIO2->FIODIR &= ~0XFF;
    CLR_A0;                             //A0=0;
    temp = LPC_GPIO2->FIOPIN0;
    if ((temp&0x80)==0)
    {
      LPC_GPIO2->FIODIR |= 0XFF;
      break;
    }
  }
}
//дE1ָ���ӳ���
void WRCOMMAND1 (unsigned char command)
{
  CPU_SR cpu_sr;
	
  OS_ENTER_CRITICAL();
  //LPC_GPIO2->FIODIR |= 0XFF;
  BUSYL();
  nop(); 
  CLR_E1;                               //E1=0;

  //CLR_RW;                               //R_W=0;
  SET_RW;
  CLR_GLCDRW_WRITE;
  CLR_A0;                               //A0=0;
  SET_E1;                               //E1=1;
  
  LPC_GPIO2->FIOPIN0 = command;
  nop(); 
  CLR_E1;                               //E1=0;
  OS_EXIT_CRITICAL();
}

//дE2ָ���ӳ���
void WRCOMMAND2 (unsigned char command)
{
  CPU_SR cpu_sr;
	
  OS_ENTER_CRITICAL();
  LPC_GPIO2->FIODIR |= 0XFF;
  BUSYR();
  nop(); 
  CLR_E2;                               //E2=0;
  //CLR_RW;                               //R_W=0;
  SET_RW;
  CLR_GLCDRW_WRITE;
  CLR_A0;                               //A0=0;
  SET_E2;                               //E2=1;
  
  LPC_GPIO2->FIOPIN0 = command;
  nop(); 
  CLR_E2;                               //E2=0;
  OS_EXIT_CRITICAL();
}
//дE1�����ӳ���
void WR_DATA1 (unsigned char Data)
{
  CPU_SR cpu_sr;
	
  OS_ENTER_CRITICAL();
  LPC_GPIO2->FIODIR |= 0XFF;
  BUSYL();
  nop(); 
  CLR_E1;                               //E1=0;
  //CLR_RW;                               //R_W=0;
  SET_RW;
  CLR_GLCDRW_WRITE;
  
  SET_A0;                               //A0=1;
  SET_E1;                               //E1=1;
  
  LPC_GPIO2->FIOPIN0 = Data;
  nop(); 
  CLR_E1;                               //E1=0;
  OS_EXIT_CRITICAL();
}

//дE2�����ӳ���
void WR_DATA2 (unsigned char Data)
{
  CPU_SR cpu_sr;
	
  OS_ENTER_CRITICAL();
  LPC_GPIO2->FIODIR |= 0XFF;
  BUSYR();
  CLR_E2;                               //E2=0;
  //CLR_RW;                               //R_W=0;
  SET_RW;
  CLR_GLCDRW_WRITE;
  SET_A0;                               //A0=1;
  SET_E2;                               //E2=1;
  
  LPC_GPIO2->FIOPIN0 = Data;
  nop(); 
  CLR_E2;                               //E2=0;
  OS_EXIT_CRITICAL();
}
/*------------------------------------------------------------------------------ 
����˵����ͬʱ����������ʾҳΪ0��3ҳ�����ں�����˽�У��û���ֱ�ӵ��ã� 
------------------------------------------------------------------------------*/
void SetPage(uint8_t page0,uint8_t page1)
{
	WRCOMMAND1(0xB8|page1);WRCOMMAND2(0xB8|page0); 
}
/*----------------------------------------------------------------------------- 
����˵����ͬʱ���������е�ַΪ0��121�����ں�����˽�У��û���ֱ�ӵ��ã� 
-----------------------------------------------------------------------------*/ 
void SetAddress(uint8_t address0,uint8_t address1) 
{ 
	WRCOMMAND1(address1);WRCOMMAND2(address0); 
}
/*----------------------------------------------------------------------------- 
���÷�ʽ��void PutChar0(uchar ch) 
����˵��������ҳ��ǰ��ַ��һ���ֽ�8���㡣���ں�����˽�У��û���ֱ�ӵ��ã� 
-----------------------------------------------------------------------------*/ 
void PutCharR(uint8_t ch) 
{ 
	WR_DATA2(ch); 
}
/* 
------------------------------------------------------------------------------------ 
���÷�ʽ��void PutChar1(uchar ch) 
����˵��������ҳ��ǰ��ַ��һ���ֽ�8���㡣���ں�����˽�У��û���ֱ�ӵ��ã� 
*/ 
void PutCharL(uint8_t ch) 
{ 
	WR_DATA1(ch); 
}
/*-------------------------------------------------------------------------------- 
���÷�ʽ��void clrscr(void) 
����˵�������� 
--------------------------------------------------------------------------------*/ 
void clrscr(void) 
{ 
  uint8_t i; 
  uint8_t page; 
  for (page=0;page<4;page++) 
  { 
    SetPage(page,page); 
    SetAddress(0,0); 
    for (i=0;i<61;i++) 
 { 
    PutCharR(0); 
    PutCharL(0); 
 } 
  } 
}

void INITLCD (void)
{
  LPC_GPIO0->FIOSET = 0X01 << 8;            //REST=1;
  nop();
  LPC_GPIO0->FIOCLR = 0X01 << 8;            //REST=0;
  nop();
  LPC_GPIO0->FIOSET = 0X01 << 8;            //REST=1;
  nop();
  WRCOMMAND1 (0xe2);//��λ
  WRCOMMAND2 (0xe2);

  WRCOMMAND1 (0xa0);//����˳��:A0:SEG0~~SEG60��������; A1:SEG60~~SEG0��������
  WRCOMMAND2 (0xa0);

  WRCOMMAND1 (0xa4);//A5:����״̬��,�ڹ���ʾ�����Ч;A4��ֹ����״̬,�ڿ���ʾ����Ч.
  WRCOMMAND2 (0xa4);

  WRCOMMAND1 (0xa9);//A8:1/16ռ�ձ�,��Ƭ1520ʹ��;A9:1/32ռ�ձ�,˫Ƭ1520ʹ��.
  WRCOMMAND2 (0xa9);

  WRCOMMAND1 (0xc0);//��ʾ��ʼ������:C0Ϊ��һ��,��ʱ��ı��������,���Բ����ϻ��¹���Ч��.
  WRCOMMAND2 (0xc0);

  WRCOMMAND1 (0xb8);//ҳ���ַ����:1520��4ҳ,ÿҳ��80���ֽ�.B8~BBΪ1��4ҳ.
  WRCOMMAND2 (0xb8);

  WRCOMMAND1 (0x00);//�е�ַ����:0~4FH,��1~80��Ԫ.
  WRCOMMAND2 (0x00);

  WRCOMMAND1 (0xaf);//AEH,����ʾ;AFH:����ʾ
  WRCOMMAND2 (0xaf);
}

void INILCDConrol(void)
{
	LPC_GPIO2->FIODIR |= 0X3FF; //P2_0-P2_9���
	LPC_GPIO0->FIODIR |= 0X003F0;
//	LPC_GPIO0->FIODIR |= 0X203E0; //P0_5---P0_9���
	//LPC_GPIO3->FIODIR = 0X01 << 26;
	//TIMER_INIT();
	_DBG_("BACK");
	SET_BKEN;
	_DBG_("DELAY");
	DelayMS(40);
	_DBG_("INITDDD");
	INITLCD();
	_DBG_("BACKEND");
	
}

void FILLONE(unsigned char k)
{
  for (i=0;i<4;i++)
  {
    WRCOMMAND1 (i+0xb8);
    WRCOMMAND2 (i+0xb8);//��ʼҳ
    WRCOMMAND1 (0xc0);
    WRCOMMAND2 (0xc0);//��ʼ��
    WRCOMMAND1 (0x00);
    WRCOMMAND2 (0x00);
    for(t=0;t<61;t++)
    {
      WR_DATA1(k);
      WR_DATA2(k);
    }
  }
}


void FILLTOW(unsigned char i,unsigned char k)
{
  for (j=0;j<4;j++)
  {
    WRCOMMAND1 (j+0xb8);
    WRCOMMAND2 (j+0xb8);//��ʼҳ
    WRCOMMAND1 (0xc0);
    WRCOMMAND2 (0xc0);//��ʼ��
   
    WRCOMMAND1 (0x00);
    for (t=0;t<31;t++)  
    {
      WR_DATA1 (i);
      WR_DATA1 (k);
    }
    WRCOMMAND2 (0x00);
    for (m=0;m<31;m++)  
    {
      WR_DATA2 (k);
      WR_DATA2 (i);
    }
 }
}


void FILLALL(void)
{
  FILLONE(0x00);//����
  FILLONE(0xFF);
  DelayMS(5);
  FILLONE(0x55);
  DelayMS(5);
  FILLONE(0xaa);
  DelayMS(5);
  FILLTOW(0xff,0x00);
  DelayMS(5);
  FILLTOW(0x00,0xff);
  DelayMS(5);
  FILLTOW(0x55,0xaa);
  DelayMS(5);
  FILLTOW(0xaa,0x55);
  DelayMS(5);
}



void WRPicture (unsigned char *PICTURE)
{
  for (i=0;i<4;i++)
  {
    WRCOMMAND1 (i+0xb8);
    WRCOMMAND2 (i+0xb8);//��ʼҳ
    WRCOMMAND1 (0xc0);
    WRCOMMAND2 (0xc0);//��ʼ��
   
    WRCOMMAND1 (0x00);
    for (t=122*i;t<61*(2*i+1);t++)  
    {
       WR_DATA1 (PICTURE[t]);
    }
    WRCOMMAND2 (0x00);
    for (j=61*(2*i+1);j<122*(i+1);j++)  
    {
       WR_DATA2 (PICTURE[j]);
    }
  }
}

/*-------------------------------------------------------------------------------- 
���÷�ʽ��void DrawBmp(bit layer,uchar width,uchar *bmp) 
����˵������һ��ͼ,layer��ʾ���²㣬width��ͼ�εĿ��߶���16��bmp��ͼ��ָ�� 
          ʹ��zimo3���������ȡģ���ֽڵ���/240�ֽ� 
--------------------------------------------------------------------------------*/ 
void DrawBmp(uint8_t layer,uint8_t width,uint8_t *bmp) 
{ 
	uint8_t x,address,i=0;     //address��ʾ�Դ�������ַ 
	uint8_t page=0; //page��ʾ������ҳ 
	uint8_t window=0;  //window��ʾ������ҳ 
	//putcharR  //�ұ� 
	//putcharL  //��� 
	for (x=width;x>1;x--) 
	{ 
		if (i>60) {window=1;address=i%61;} 
		else address=i;  
		if(layer==0)  //��ʾһ�а˸��� 
		{ 
			SetPage(0,0); 
			SetAddress(address,address); 
			if(window==1)PutCharR(bmp[i]); 
			else  PutCharL(bmp[i]); 
			SetPage(1,1); 
			SetAddress(address,address); 
			if(window==1)PutCharR(bmp[i+width]); 
			else  PutCharL(bmp[i+width]); 
		} 
		else 
		{  //��ʾ�ڶ��а˸����� 
			SetPage(2,2); 
			SetAddress(address,address); 
			if(window==1)PutCharR(bmp[i]); 
			else  PutCharL(bmp[i]); 
			SetPage(3,3); 
			SetAddress(address,address); 
			if(window==1)PutCharR(bmp[i+width]); 
			else  PutCharL(bmp[i+width]); 
		} 
		i++; 
	} 
} 
//--------------------------------------------------------------- 
//--------------------------------------------------------------- 
//����zimo2  ��ʾ����,�Ժ��ֵ���ʾ��ʽ 
//void Draw_word(uchar d_where,uint x_add,bit layer,uchar width) 
//d_where��ʾ������еڼ������֣�x_add������λ��,layer��ʾ�Ĳ�, width��ʾ�Ŀ�ȡ�start ��ʾ��ʼ�ֽڣ�Ϊ-1ʱ����d_where������ 
//--------------------------------------------------------------- 
void Draw_word(uint32_t d_where,uint32_t start, uint32_t x_add,uint8_t layer,uint8_t width,uint8_t * character) 
{ 
	uint8_t x,i=0,address;     //address��ʾ�Դ�������ַ 
	uint8_t page=0; //page��ʾ������ҳ 
	uint8_t window=0;  //window��ʾ������ҳ 
	//putcharR  //�ұ� 
	//putcharL  //��� 
	if(start==-1)
		d_where=d_where*24;
	else 
		d_where=start;
	
	for (x=width;x>1;x--) 
	{ 
		if (x_add>60) {window=1;address=x_add%61;} 
		else address=x_add; 
 
		if(layer==0)  //��ʾһ�а˸��� 
		{ 
			SetPage(0,0); 
			SetAddress(address,address); 
			if(window==1)PutCharR(character[d_where]);//�ұ� 
			else  PutCharL(character[d_where]);//��� 
			SetPage(1,1); 
			SetAddress(address,address); 
			if(window==1)PutCharR(character[d_where+width]); 
			else  PutCharL(character[d_where+width]); 
		} 
		else 
		{  //��ʾ�ڶ��а˸����� 
			
			SetPage(2,2); 
			SetAddress(address,address); 
			if(window==1)PutCharR(character[d_where]);//�ұ� 
			else  PutCharL(character[d_where]);//��� 
			SetPage(3,3); 
			SetAddress(address,address); 
			if(window==1)PutCharR(character[d_where+width]); 
			else  PutCharL(character[d_where+width]); 
 
		} 
		x_add++; 
		d_where++; 
	} 
}
void Draw_Menu(uint8_t character[],uint8_t len)
{
	uint8_t i;
	INITLCD();
	clrscr();
	for(i=0;i<len;i++)
		Draw_word(i,-1,12*i,1,12,character);
}
/*
 *@��������ʾ�˵�
 *@������menuIndexes ���飬�����˵��ֵĹ��ɣ�len �˵����ֵĸ���
 *@���أ���
 */
void DisplayMenu(uint8_t menuIndexes[],uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		if(menuIndexes[i]==0) break;//���Ϊ�����ʾ�˵������ַ��Ѿ���ʾ�꣬�����
		Draw_word(menuIndexes[i]-1,-1,12*i,1,12,fonts);
	}
}

/*
 *@��������ʾ�˵�
 *@������menuIndexes ���飬�����˵��ֵĹ��ɣ�len �˵����ֵĸ���
 *@���أ���
 */
void DisplayBelowInfo(uint8_t menuIndexes[],uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		if(menuIndexes[i]==0) break;//���Ϊ�����ʾ�˵������ַ��Ѿ���ʾ�꣬�����
		Draw_word(menuIndexes[i]-1,-1,12*i,0,12,fonts);
	}
}

/*
 *@���� ���������Ļ
 *
 */
void clrBelow(void)
{
	uint8_t i;
	for(i=0;i<20;i++)
		DrawASCII(68,6*i,0);
}

/*
 *@��������ʾ���ò���
 *@������integer �������֣�fraction С������(1λ),type ���� 0ֻ��ʾ���� 1 �¶� 2 ʪ�� 3 ��ѹ 
 */
void DisplayAlarmSetting(float alarmSet, uint8_t alarmType)
{
	uint8_t type;
	switch(alarmType)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:type=3;break;
		case 6:
		case 7:
		case 10:
		case 11:
		case 14:
		case 15:
		case 18:
		case 19:
		case 22:
		case 23:type = 1;break;
		case 8:
		case 9:
		case 12:
		case 13:
		case 16:
		case 17:
		case 20:
		case 21:
		case 24:
		case 25:type = 2;break;
	}
	DisplaySetting(alarmSet,type);
}

/*
 *@��������ʾ���ò���
 *@������alarmSet �������� type ���� 0ֻ��ʾ���� 1 �¶� 2 ʪ�� 3 ��ѹ 
 */
void DisplaySetting(float data, uint8_t type)
{
	uint8_t used;//��ʾ��С���ĸ�����ռ�õ��ַ������Ա�����ȷ������λ
	int8_t i;
	uint8_t nums[5],totalNum;
	int32_t integer;
	uint8_t fraction;
	
	clrBelow();
	used = 0;
	//sprintf(_db,"DIS int=%d neg=%d",integer,neg);
	//DB;
	my_printf("----------%d",data);
	my_printf("here data %d",((int32_t)(data*10000)));
	integer = (int32_t)(((data*10000)));
	my_printf("integer_______* %d",integer);
	if(integer>0)
		integer= integer+1;
	else if(integer<0)
		integer= integer-1;	
	integer = integer/1000;
	my_printf("integer||||||||||* %d",integer);
	if(integer<0) 
	{
		my_printf("JUEG int=%d",integer);
		integer = integer*(-1);
		DrawASCII(64,used*6,0);
		used++;
	}
	
	fraction = integer%10;
	integer = integer/10;
	my_printf("dis %d %d",integer,fraction);
	totalNum = 0;//��λ����ʼ��Ϊ0
	if(integer==0)
	{
		nums[totalNum] = 0;
		totalNum ++;
	}
	while(integer>0)
	{
		nums[totalNum] = integer % 10;
		integer = integer /10;
		totalNum++;
	}
	
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],6*used,0);
		used++;
	}
	DrawASCII(62,used*6,0);//����С����
	used++;
	DrawASCII(fraction,used*6,0);
	used++;
	//�¶�����ʾ'C
	if(type==1)
	{
		DrawASCII(67,used*6,0);
		used++;
		DrawASCII(38,used*6,0);
		used++;
	}
	//ʪ����ʾ%
	else if(type == 2)
	{
		DrawASCII(66,used*6,0);
		used++;
	}
	//��ѹ��ʾV
	else if(type == 3)
	{
		DrawASCII(57,used*6,0);
		used++;
	}
	
}
/*
 *@������ʾIP��ַ
 *@���� type 1:addr1 2:addr2  3:addr3  4:addr4
 */
void DisplayIP(uint8_t addr,uint8_t type)
{
	uint8_t nums[3],totalNum;
	uint8_t used;
	int8_t i;
	
	//������ո����ݶε�����
	used = (type-1)*4;
	for(i=0;i<3;i++)
	{
		DrawASCII(68,used*6,0);
		used++;
	}
	
	totalNum = 0;
	if(addr==0) 
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(addr>0)
	{
		nums[totalNum] = addr%10;
		addr=addr/10;
		totalNum++;
	}
	//ÿ��IPλռ���ĸ��ַ� 3������һ��.��
	used = (type-1)*4;
	used = used+3-(totalNum);//Ȼ���ֿ����ұߵ�.��
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	if(type!=4) 
		DrawASCII(62,used*6,0);//��ʾ���
}
/*
 *@������IP��˸
 */
void DisplayIPBlink(uint8_t type)
{
	uint8_t nums[3],totalNum;
	uint8_t used;
	int8_t i;
	//������ո����ݶε�����
	used = (type-1)*4;
	for(i=0;i<3;i++)
	{
		DrawASCII(68,used*6,0);
		used++;
	}
}
/*
 *@�������˿���˸
 */
void DisplayPortBlink(uint16_t port)
{
	uint8_t nums[5],totalNum;
	uint8_t used;
	int8_t i;
	
	//�������
	used = 16;
	for(i=0;i<4;i++)
	{
		DrawASCII(68,used*6,0);
		used++;
	}
}
/*
 *@��������ʾ�˿ں�
 *@������port,�˿ں�
 */
void DisplayPort(uint16_t port)
{
	uint8_t nums[5],totalNum;
	uint8_t used;
	int8_t i;
	
	//�������
	used = 16;
	for(i=0;i<4;i++)
	{
		DrawASCII(68,used*6,0);
		used++;
	}
	totalNum = 0;
	if(port==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(port>0)
	{
		nums[totalNum] = port%10;
		port = port /10;
		totalNum++;
	}
	DrawASCII(65,15*6,0);
	used = 16;//4��ip��һ��ð��
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	
}
/*
 *@��������ʾ�澯
 *@������index ��� 
 *@���أ�0û�ж�Ӧ�ĸ澯 
 */
uint8_t DisplayAlarmRecord(int16_t number,uint8_t len)
{
	uint8_t alarmType,state,level;
	uint8_t nums[5],totalNum,used;
	uint16_t temp;	
	int8_t i;
	SysAlarmTypeDef alarm;
	
	clrscr();
	if(number<0)
		return 0;
	alarm = getSysAlarm(number);
	if(alarm.alarmType==0xff)
		return 0;
	alarmType = alarm.alarmType-1;
	//��ʾ�澯��¼�ţ�ռ���ĸ�λ��
	used = 0;
	temp = number+1;
	totalNum = 0;
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	DrawASCII(69,used*6,1);//����#
	used++;
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,1);
		used++;
	}
	if(alarm.state==0)
	{
		DrawASCII(40,used*6,1);//����ǽ���������ʾE
		used++;
	}
	for(i=0;i<len;i++)
	{
		if(menuThree[alarmType][i]==0) break;//���Ϊ�����ʾ�˵������ַ��Ѿ���ʾ�꣬�����
		Draw_word(menuThree[alarmType][i]-1,-1,12*i+30,1,12,fonts);
	}
	
	DisplayTime(alarm.year+2000,alarm.month,alarm.day,alarm.hour,alarm.minute,alarm.second);
	return 1;
}

void DisplayTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second)
{
	uint8_t nums[5],totalNum,used;
	uint16_t temp;
	int8_t i;
	
	clrBelow();
	//��ʾ��
	temp = year;
	totalNum = 0;
	used = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	DrawASCII(64,used*6,0);
	used++;
	
	//��ʾ��
	temp = month;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	if(totalNum == 1)
	{
		nums[1] = 1;
		totalNum = 2;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	DrawASCII(64,used*6,0);//����-
	used++;
	
	//��ʾ��
	temp = day;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	if(totalNum == 1)
	{
		nums[1] = 1;
		totalNum = 2;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	DrawASCII(68,used*6,0);
	used++;
	
	//��ʾʱ
	temp = hour;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	if(totalNum == 1)
	{
		nums[1] = 1;
		totalNum = 2;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	DrawASCII(65,used*6,0);
	used++;
	
	//��ʾ��
	temp = minute;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	if(totalNum == 1)
	{
		nums[1] = 1;
		totalNum = 2;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	DrawASCII(65,used*6,0);
	used++;
	
	//��ʾ��
	temp = second;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	if(totalNum == 1)
	{
		nums[1] = 1;
		totalNum = 2;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],used*6,0);
		used++;
	}
	used++;
	
}
void DisplayVersion(uint32_t main,uint32_t second)
{
	uint8_t nums[5],totalNum,used,shift;
	uint16_t temp;
	int8_t i;
	
	//��ʾ���汾��
	temp = main;
	totalNum = 0;
	used = 0;
	shift = 12*5;
	
	DrawASCII(57,shift+used*6,1);
	used++;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],shift+used*6,1);
		used++;
	}
	DrawASCII(64,shift+used*6,1);
	used++;
	
	//��ʾ�ΰ汾��
	temp = second;
	totalNum = 0;
	if(temp==0)
	{
		nums[totalNum] = 0;
		totalNum++;
	}
	while(temp>0)
	{
		nums[totalNum] = temp%10;
		temp = temp/10;
		totalNum++;
	}
	for(i=totalNum-1;i>=0;i--)
	{
		DrawASCII(nums[i],shift+used*6,1);
		used++;
	}
	used++;
}
//--------------------------------------------------------------- 
//--------------------------------------------------------------- 
//����zimo2  ��ʾASCII��ֵ0-9a-zA-Z
//void DrawASCII(uint32_t d_where,uint32_t x_add,uint8_t layer)  
//d_where��ʾ������еڼ���ascii����x_add������λ��,layer��ʾ�Ĳ�
//--------------------------------------------------------------- 
void DrawASCII(uint32_t d_where,uint32_t x_add,uint8_t layer) 
{ 
	uint8_t x,i=0,address;     //address��ʾ�Դ�������ַ 
	uint8_t page=0; //page��ʾ������ҳ 
	uint8_t window=0;  //window��ʾ������ҳ
	uint8_t width = 6;	
	//putcharR  //�ұ� 
	//putcharL  //��� 
	d_where=d_where * 12;
	for (x=width;x>1;x--) 
	{ 
		if (x_add>60) {window=1;address=x_add%61;} 
		else address=x_add; 
		if(layer==0)  //��ʾһ�а˸��� 
		{ 
 
			SetPage(0,0); 
			SetAddress(address,address); 
			if(window==1)PutCharR(ascii[d_where]);//�ұ� 
			else  PutCharL(ascii[d_where]);//��� 
			SetPage(1,1); 
			SetAddress(address,address); 
			if(window==1)PutCharR(ascii[d_where+width]); 
			else  PutCharL(ascii[d_where+width]); 
		} 
		else 
		{  //��ʾ�ڶ��а˸����� 
			SetPage(2,2); 
			SetAddress(address,address); 
			if(window==1)PutCharR(ascii[d_where]);//�ұ� 
			else  PutCharL(ascii[d_where]);//��� 
			SetPage(3,3); 
			SetAddress(address,address); 
			if(window==1)PutCharR(ascii[d_where+width]); 
			else  PutCharL(ascii[d_where+width]); 
 
		} 
		x_add++; 
		d_where++; 
	} 
}

/*********************************************************************
 *��������ʵ��
 *********************************************************************/
void key_init(void)
{
	LPC_GPIO2->FIODIR &= ~(0X07 << 10);    //�����˿���Ϊ����
}
uint8_t key_ok()
{
	//P2_10   key_ok
	//����ɨ��
	uTemp = GPIO_ReadValue(2);
	if(!((uTemp>>10)&0x0001))
	{
		DelayMS(10);
		//����ɨ��
		uTemp = GPIO_ReadValue(2);
		if(!((uTemp>>10)&0x0001))
       {
          return 1;
       }
   }
   return 0;
	
}

uint8_t key_cancel()
{
	return 0;
}

uint8_t key_up()
{
	//����ɨ��
    uTemp = GPIO_ReadValue(2);
    
    //P2_12   key_up
    if(!((uTemp>>12)&0x0001))
    {
		DelayMS(10);
		uTemp = GPIO_ReadValue(2);
		if(!((uTemp>>12)&0x0001))
		{
			return 1;
		}
    }
	return 0;
}
uint8_t key_down()
{
	uTemp = GPIO_ReadValue(2);
	//P2_11   key_down
   if(!((uTemp>>11)&0x0001))
   {
       DelayMS(10);
	   uTemp = GPIO_ReadValue(2);
       if(!((uTemp>>11)&0x0001))
       {
          return 1;
       }
   }
   return 0;
}
//����
uint8_t key_right()
{
   return 0;
}
//����
uint8_t key_left()
{
   return 0;
}

