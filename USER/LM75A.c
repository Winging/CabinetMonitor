#include "LM75A.h"

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device as slave definition */
#define USEDI2CDEV_M		1
/** Own Slave address in Slave I2C device */
//#define I2CDEV_S_ADDR	(0x90>>1)

#define READ_LM75				0x01
#define LM75_ADDR				(0x90>>1)
#define LM75_TEMP				0x00
#define LM75_CONFIG				0x01
#define LM75_THYST				0x02
#define LM75_TOS				0x03

#define I2C_IDLE				0
#define I2C_STARTED				1
#define I2C_RESTARTED			        2
#define I2C_REPEATED_START		        3
#define DATA_ACK				4
#define DATA_NACK				5
#define I2C_BUSY				6
#define I2C_NO_DATA				7
#define I2C_NACK_ON_ADDRESS		        8
#define I2C_NACK_ON_DATA		        9
#define I2C_ARBITRATION_LOST	                10
#define I2C_TIME_OUT		        	11
#define I2C_OK					12

/** Max buffer length */
#define BUFFER_SIZE			6

#if (USEDI2CDEV_M == 0)
#define I2CDEV_M LPC_I2C0
#elif (USEDI2CDEV_M == 1)
#define I2CDEV_M LPC_I2C1
#elif (USEDI2CDEV_M == 2)
#define I2CDEV_M LPC_I2C2
#else
#error "Master I2C device not defined!"
#endif

/** These global variables below used in interrupt mode - Slave device ----------------*/
uint8_t Master_Buf[BUFFER_SIZE];
uint8_t Receive_Buf[2];
void initLM(void)
{
	PINSEL_CFG_Type PinCfg;
	I2C_M_SETUP_Type transferMCfg;
	/*
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
#if ((USEDI2CDEV_M == 0))
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
#endif
#if ((USEDI2CDEV_M == 1))
	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
#endif
#if ((USEDI2CDEV_M == 2))
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#endif

	// Initialize Slave I2C peripheral
	I2C_Init(I2CDEV_M, 100000);

	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_M, ENABLE);
	
}
int16_t getLMWet(void)
{
	I2C_M_SETUP_Type transferMCfg;
	int16_t tempp;
	
	/* Initialize buffer */
    Master_Buf[0] = LM75_CONFIG;
    Master_Buf[1] = 0x00;
	
	/* Start I2C slave device first */
	
	transferMCfg.sl_addr7bit = LM75_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
        
    Master_Buf[0] = LM75_TEMP;
    Master_Buf[1] = LM75_ADDR | READ_LM75;
    Receive_Buf[0] = 0X00;
    Receive_Buf[1] = 0X00;
        
    transferMCfg.sl_addr7bit = LM75_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = Receive_Buf;
	transferMCfg.rx_length = 2;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
        
    tempp = ( ( ( Receive_Buf[0] << 8 ) | Receive_Buf[1] ) >> 3 )*0.03125;
	
	return tempp;
	
	//return 24;
}