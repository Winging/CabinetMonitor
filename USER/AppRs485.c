#include "AppRs485.h"
#include "AppSensor.h"
#include "TemSensor.h"
#include "HexToAsccii.h"

void UART_IntReceive(void);
void UART_IntErr(uint8_t bLSErrType);
void AnalyseData(uint8_t data[],uint8_t len);
void dataToPacket(uint8_t sendData[],uint8_t byte1,uint8_t byte2);

void UART1_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART0);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {
			UART_IntErr(tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
		UART_IntReceive();
	}

}
void UART_IntReceive(void)
{
	uint8_t tmpc;
	uint32_t rLen;
	static uint8_t dataSize;
	static uint8_t data[5];
	dataSize =0;
	while(1){
		// Call UART read function in UART driver
		rLen = UART_Receive((LPC_UART_TypeDef *)LPC_UART1, &tmpc, 1, NONE_BLOCKING);
		// If data received
		if (rLen){
			/* Check if buffer is more space
			 * If no more space, remaining character will be trimmed out
			 */
			data[dataSize] = tmpc;
			dataSize ++;
			if(dataSize==3)
			{
				//接收到3字节说明已接收到完整的包。
				dataSize = 0;
				AnalyseData(data,3);
			}
		}
		// no more data
		else {
			break;
		}
	}
}
void UART_IntErr(uint8_t bLSErrType)
{
	if (bLSErrType & UART_LSR_PE){
		
	}

	if (bLSErrType & UART_LSR_FE){
		
	}
}
void setCtrl()
{
	LPC_GPIO0->FIODIR |= (1<<26);
}
void lowCtrl()
{
	LPC_GPIO0->FIOCLR = (1<<26);
}
void highCtrl()
{
	LPC_GPIO0->FIOSET = (1<<26);
}

void Rs485Init(void)
{
	uint32_t tt,i,j;
	uint8_t tempchar,a1,a2,a3;
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration
	PINSEL_CFG_Type PinCfg;
	// RS485 configuration
	UART1_RS485_CTRLCFG_Type rs485cfg;	
	
	// UART0 section ----------------------------------------------------
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 115200 bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = 9600;

	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init(LPC_UART2, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(LPC_UART2, ENABLE);

	setCtrl();
	my_printf("RS485 has inited now\n\r");
}

void dataToPacket(uint8_t sendData[],uint8_t byte1,uint8_t byte2)
{
	uint8_t xorByte;
	xorByte = byte1 ^ byte2;
	my_printf("p4");
	sendData[0]=0x7e;
	sendData[1]=getHexToChar(byte1>>4);
	sendData[2]=getHexToChar(byte1&0x0f);
	sendData[3]=getHexToChar(byte2>>4);
	sendData[4]=getHexToChar(byte2&0x0f);
	sendData[5]=getHexToChar(xorByte>>4);
	sendData[6]=getHexToChar(xorByte&0x0f);
	sendData[7]=0x1d;
	my_printf("p5");
}
void Rs485SendInfo(void)
{
	uint8_t data[10],byte1,byte2;
	uint8_t temp,tt;
	uint32_t i,j;
	
	my_printf("p1");
	//第一个字节
	temp = 0;
	if(ThunderAlarm())//防雷	  
		temp |=0x08;
	if(DoorAlarm(1)||DoorAlarm(2))//门磁
		temp|=0x04;
	if(SmokeAlarm(1))//烟雾
		temp|=0x02;
	if(WaterAlarm())//水浸
		temp|=0x01;
	if((checkTempAlarm(1)==TempStateHigh) || (checkTempAlarm(1)==TempStateLow)
	   || (checkTempAlarm(2)==TempStateHigh) || (checkTempAlarm(2)==TempStateLow))//温度
		temp|=0x80;

	if((checkWetAlarm(1)==WetStateHigh) || (checkWetAlarm(1)==WetStateLow)
	   || (checkWetAlarm(2)==WetStateHigh) || (checkWetAlarm(2)==WetStateLow))//湿度
		temp|=0x40;

	
	my_printf("p2");
	byte1 = temp; //第一个字节的内容
	byte2 = 0x03; //第二个字节
	dataToPacket(data,byte1,byte2);
	my_printf("p3");
	highCtrl();
	my_printf("z3");
	for(tt=0;tt<=7;tt++)
	{
		my_printf("t%d=[%d]",tt,data[tt]);
		UART_Send(LPC_UART2, &data[tt], 1,BLOCKING);
		for(i=0;i<=100000;i++);
	}
	lowCtrl();
	my_printf("has sent data-----------------------%d %d %d \n\r",data[0],data[1],data[2]);
}
void AnalyseData(uint8_t data[],uint8_t len)
{
	//如果异或校验出错,则直接忽略该报文，允许丢包存在。
	if(data[0]^data[1] != data[2])
	{
		return;
	}else
	{
		if(data[0]&0x01 != 0 )
		{
			//主电告警
			my_printf("main electricity alarm");
		}
		if(data[0]&0x02 != 0)
		{
			//整流模块告警
			my_printf("aaa");
		}
		if(data[0]&0x04 != 0)
		{
			//熔丝告警
			my_printf("bbb");
		}
		if(data[0] & 0x08 != 0)
		{
			//低压告警
			my_printf("ccc");
		}
	}
}