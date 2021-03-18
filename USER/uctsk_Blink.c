/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_Blink.c
** Descriptions:            The uctsk_Blink application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <includes.h>            


/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static  void  uctsk_Blink        (void);
static  void  GPIO_Configuration (void) ;
static  void  LED_On             (uint32_t led) ;
static  void  LED_Off            (uint32_t led) ;


void  App_BlinkTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task LED Blink", &os_err);
	#endif

}

static void uctsk_Blink (void) 
{                 
    uint8_t  location;  

    GPIO_Configuration();

   	for(;;)
   	{   
	    LED_On (0xff);
	    OSTimeDlyHMSM(0, 0, 0, 500);
	    LED_Off(0xff);

        for(location=0;location<8;location++)
	    {
    	   LED_On ((1<<location));                     /* Turn on the LED. */
    	   OSTimeDlyHMSM(0, 0, 0, 100);                /* delay  100 Msec */
    	   LED_Off ((1<<location));                    /* Turn off the LED. */
    	   OSTimeDlyHMSM(0, 0, 0, 100);                /* delay  100 Msec */
	    }
   }
}

static void GPIO_Configuration(void) 
{
  LPC_GPIO2->FIODIR = 0x000000ff;               /* LEDs PORT2 are Output */
}

static void LED_On (uint32_t led) {

  LPC_GPIO2->FIOPIN |=  (led);                  /* Turn On  LED */
}

static void LED_Off (uint32_t led) {

  LPC_GPIO2->FIOPIN &= ~(led);                  /* Turn Off LED */
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
