#include "AppSerialCom.h"
#include "RingBuffer.h"

#define LED_NUM     8                   /* Number of user LEDs                */
const unsigned long led_mask[] = { 1UL<<0, 1UL<<1, 1UL<<2, 1UL<< 3,
                                   1UL<< 4, 1UL<< 5, 1UL<< 6, 1UL<< 7 };

/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void LED_init(void) {

  LPC_GPIO2->FIODIR |= 0x000000ff;  //P2.0...P2.7 Output LEDs on PORT2 defined as Output

  /* Configure the LCD Control pins                                           */
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int num) {
 
  LPC_GPIO2->FIOPIN |= led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int num) {

  LPC_GPIO2->FIOPIN &= ~led_mask[num];
}

extern Ring_Buffer_T bufRec;//引用外部定义的接收缓冲区
//声明
static void UART_IntErr(uint8_t bLSErrType);
static void UART_IntReceive(void);
/*
*@描述：初始化串口
*@参数：void
*@返回：无
*/
void InitUart(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration for UART0
	PINSEL_CFG_Type PinCfg;
	/*
	 * Initialize UART pin connect
	 */
	PinCfg.OpenDrain = UARTOpendrain;
	PinCfg.Pinmode = UARTPinMode;
	PinCfg.Portnum = UARTPortTX;
	PinCfg.Pinnum = UARTPinTX;
	PinCfg.Funcnum = UARTFuncTX;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Portnum = UARTPortRX;
	PinCfg.Pinnum = UARTPinRX;
	PinCfg.Funcnum = UARTFuncRX;
	PINSEL_ConfigPin(&PinCfg);
	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	// Initialize UART peripheral with given to corresponding parameter
	UART_Init((LPC_UART_TypeDef *)_LPC_UART, &UARTConfigStruct);
	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV2;//8 character 
	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig((LPC_UART_TypeDef *)_LPC_UART, &UARTFIFOConfigStruct);


	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef *)_LPC_UART, ENABLE);

    /* Enable UART Rx interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)_LPC_UART, UART_INTCFG_RBR, ENABLE);
	/* Enable UART line status interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)_LPC_UART, UART_INTCFG_RLS, ENABLE);
	/* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(_UART_IRQ, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART channel */
    NVIC_EnableIRQ(_UART_IRQ);
	LED_init();
	LED_On(1);
	BufInit(&bufRec);
}
/*
*@描述：串口中断
*@参数：void
*@返回：无
*/
void _UART_IRQHander(void)
{
	
	uint32_t intsrc, tmp, tmp1;
	OSIntEnter();
	/* Determine the interrupt source */
	intsrc = UART_GetIntId((LPC_UART_TypeDef *)_LPC_UART);
	tmp = intsrc & UART_IIR_INTID_MASK;
	LED_On(2);
	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef *)_LPC_UART);
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

	// Transmit Holding Empty
	if (tmp == UART_IIR_INTID_THRE){
			//UART_IntTransmit();
		LED_On(4);
	}
	OSIntExit();
}

void UART_IntErr(uint8_t bLSErrType)
{
	// Loop forever
	while (1){
		// For testing purpose
	}
}

//发生接收中断时调用，基本上是发生8字节中断后调用，或者超时后调用
void UART_IntReceive(void)
{
	uint32_t rlen;//接收数据的长度
	uint8_t tmpchar;
	static uint8_t chars[100],index=0;
	while(1)
	{
		//接收一个字符
		rlen = UART_Receive((LPC_UART_TypeDef *)_LPC_UART,&tmpchar,1,BLOCKING);
		if(rlen)
		{
			//回发检测，调试用
			
			//BufPush(bufRec,tmpchar);
			chars[index++] = tmpchar;
			//获得包结束位
			if(tmpchar == '-')
			{
				UART_Send((LPC_UART_TypeDef *)_LPC_UART,chars,index,BLOCKING);
				LED_On(5);
			}
			//获得包起始位
			if(tmpchar == '+')
			{
				//命令起始位 +
			}
		}
		else 
			break;
	}
}

/*
*@描述 ： 串口通信发送接口，提供给通过串口给上位机发送同样报文的接口；采用阻塞发送，因为发送数据不多
*@参数：str指向发送的字符串，len 发送的长度；
*返回：无
*/
void UartSend(uint8_t* str,uint32_t len)
{
	UART_Send((LPC_UART_TypeDef *)_LPC_UART,str,len,BLOCKING);
}
