#include "AppTaskNet.h"
#include "TcpApp.h"
#include "App.h"
#include "AppPersist.h"
extern unsigned char EMAC_ADDR0;		
extern unsigned char EMAC_ADDR1;		
extern unsigned char EMAC_ADDR2;		
extern unsigned char EMAC_ADDR3;		
extern unsigned char EMAC_ADDR4;		
extern unsigned char EMAC_ADDR5;

extern OS_EVENT * semNetInited;//�����Ƿ��Ѿ���ʼ��

//����ͨ������
void AppTaskNet(void * pdata)
{
    UNS_32 i;
	INT8U err;
    uip_ipaddr_t ipaddr;
	clock_time_t lastTime;
	static uint8_t macInited;
    volatile UNS_32 delay;
    struct timer periodic_timer,arp_timer;
	
    pdata = pdata;
    _DBG_("Init Network settings");
    timer_set(&periodic_timer,CLOCK_SECOND/2);
    timer_set(&arp_timer,CLOCK_SECOND*10);
	/*
    while(!(macInited=tapdev_init()))
    {
        //����ʼ��ʧ�ܺ���ʱһ��ʱ��Ȼ�������³��Գ�ʼ��
        _DBG_("Error during iniializing the emac module\r\n");
        for (delay = 0x100000; delay; delay--);
    }
	*/
	
	//�ȴ����硢������ʼ��
	OSSemPend(semNetInited,0,&err);
	//tapdev_init();//����������ֱ�ӵȴ������ʼ���Ƿ����
    _DBG_("Init UIP");
    //��ʼ��uip TCP/IPЭ��ջ
    uip_init();
    //��ʼ����ַ����ģ��ARP
    uip_arp_init();
    //��ʼ��MAC��ַ
    uip_ethaddr.addr[0] = EMAC_ADDR0;
    uip_ethaddr.addr[1] = EMAC_ADDR1;
    uip_ethaddr.addr[2] = EMAC_ADDR2;
    uip_ethaddr.addr[3] = EMAC_ADDR3;
    uip_ethaddr.addr[4] = EMAC_ADDR4;
    uip_ethaddr.addr[5] = EMAC_ADDR5;
	sprintf(_db,"mac-=%d %d %d %d %d %d ",EMAC_ADDR0,EMAC_ADDR1,EMAC_ADDR2,EMAC_ADDR3,EMAC_ADDR4,EMAC_ADDR5);
	DB;
    //����MAC��ַ��һ̨�����豸ֻ��һ��mac��ַ������ҪԤ�����ú���ص�ַ��Ϣ��Ҳ����ʹ��������õķ�ʽ
    uip_setethaddr(uip_ethaddr);
    uip_ipaddr(ipaddr,APP_Persist_IP_Addr->IP_ADDR1,APP_Persist_IP_Addr->IP_ADDR2,APP_Persist_IP_Addr->IP_ADDR3,APP_Persist_IP_Addr->IP_ADDR4);
    //������ַ
    uip_sethostaddr(ipaddr);
    sprintf(_db, "Set own IP address: %d.%d.%d.%d \n\r", \
            ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
            ((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
    DB;

    uip_ipaddr(ipaddr,APP_Persist_IP_Addr->IP_ADDR1,APP_Persist_IP_Addr->IP_ADDR2,APP_Persist_IP_Addr->IP_ADDR3,1);
    //Ĭ������
    uip_setdraddr(ipaddr);
    sprintf(_db, "Set Router IP address: %d.%d.%d.%d \n\r", \
            ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
            ((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
    DB;
    uip_ipaddr(ipaddr,255,255,255,0);
    //��������
    uip_setnetmask(ipaddr);
    sprintf(_db, "Set Subnet mask: %d.%d.%d.%d \n\r", \
            ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
            ((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
    DB;
    sprintf(_db,"%d\r\n",clock_time());//��ʼ��ʱ������Ҫ900ms����
    DB;
	InitTCP();
    //��ʼ��ʱ������Ҫ1��������,
    //�����豸���ݲ�ѯѭ��
    while(1)
    {
        //������ȡ��������
        //sprintf(_db,"while %d\r\n",OSTimeGet());//��ʼ��ʱ������Ҫ900ms����
        //DB;
        uip_len = tapdev_read(uip_buf);
        if(uip_len>0)
        {
            //sprintf(_db,"have data %d\r\n",clock_time());
            //DB;
            if(BUF->type == htons(UIP_ETHTYPE_IP))
            {
                uip_arp_ipin();//
                uip_input();//
                //��������ݷ�����������ݷ���
                if(uip_len>0)
                {
                    uip_arp_out();
                    tapdev_send(uip_buf,uip_len);
                }
                
            }
            else if(BUF->type == htons(UIP_ETHTYPE_ARP))
            {
                uip_arp_arpin();
                if(uip_len > 0)
                {
					tapdev_send(uip_buf,uip_len);
                }
            }
        }else if(timer_expired(&periodic_timer))
        {
			//sprintf(_db,"period%d\r\n",clock_time());
            //DB;
            timer_reset(&periodic_timer);
            for(i = 0 ;i<UIP_CONNS;i++)
            {
                uip_periodic(i);
                if(uip_len>0)
                {
                    uip_arp_out();
                    tapdev_send(uip_buf,uip_len);
                }
            }
#if UIP_UDP
            for(i=0;i<UIP_UDP_CONNS;i++)
            {
                uip_udp_periodic(i);
                if(uip_len>0)
                {
                    uip_arp_out();
                    tapdev_send(uip_buf,uip_len);
                }
            }

#endif /*end of #if UIP_UDP */
            if(timer_expired(&arp_timer))
            {
                timer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
		
		WDT_Feed();
		//_DBG_("+");
        OSTimeDly(10);//��ʱ10��ϵͳ���ģ�Ϊ��������ճ�ʱ��
		//_DBG_("-");
		
		
    }
    OSTaskDel(OS_PRIO_SELF);//���������񴴽����ɾ��������
}

