#include "TcpApp.h"
#include "RingBuffer.h"
#include "AppDebug.h"
#include "AppPersist.h"

struct uip_conn * client_conn;//���Ͷ�/�ͻ���TCP����
struct uip_conn * server_conn;//���ն�/�����TCP����
extern Ring_Buffer_T bufNetRec;//������ջ�����
extern OS_EVENT * semNetCommand;//�Ƿ����������ݰ���Ҫ���� ��ʼΪ0
extern OS_EVENT * semNetBuf;//�������ݽ��ջ��������� ��ʼΪ1
extern uint8_t NetServerDataOut[265];//���������Ͱ�
extern uint32_t NetServerDataOutLen;
//extern uint8_t NetClientDataOut[265];//�ͻ��˷��Ͱ�
//extern uint32_t NetClientDataOutLen;
extern OS_EVENT * semServerDataOut;//����˷������ݻ���
extern OS_EVENT * semClientDataOut;//�ͻ��˷������ݻ���
extern OS_EVENT * mBoxServerDataLen;//����Ϣ����Ҫ���͵����ݳ���
extern OS_EVENT * mBoxClientDataLen;//����Ϣ����Ҫ���͵����ݳ���
void ClientInit(void);
void ServerInit(void);
void ClientApp(void);
void ServerApp(void);
//��ʼ���ͻ��˷�������
void ClientInit(void)
{
	uip_ipaddr_t ipaddr[2];
	struct CommandState * state;
	_DBG_("begin client INit\n\r");
	uip_ipaddr(ipaddr,192,168,0,3);
	client_conn = uip_connect(ipaddr,HTONS(2015));
	state = (struct CommandState *) &(uip_conn->appstate);//Ӧ��״̬
	state->state = IDLE;
	if(client_conn != NULL)
	_DBG_("Client INit\n\r");
	else
	{
		_DBG_("Client fail\n\r");
	}
}
//��ʼ�����ط�������������
void ServerInit()
{
	_DBG_("begin SEREVER INit\n\r");
	uip_listen(HTONS(APP_Persist_IP_Addr->ServerPort));
	_DBG_("Server INit\n\r");
}
//��ʼ��TCP����
void InitTCP(void)
{
	ServerInit();
	BufInit(&bufNetRec);
}
/*
 *@����������Զ�̷�����,׼����������
 */
void ConnectServer(void)
{
	//OSMboxPost(mBoxClientStart,(void*)(0xff000000 | 1));
	ClientInit();
}
//�ͻ��˴��� ��TcpApp�лص�
void ClientApp(void)
{
	#if 0
	char words[300];
	char receives[300];
	char * appdata;
	unsigned int recIndex;
	struct CommandState * state;
	uint32_t sendOutLen;
	INT8U err;
	state = (struct CommandState *) &(uip_conn->appstate);//Ӧ��״̬
	if(uip_aborted())
	{
		//��������Ѿ���ֹ
		_DBG("aborted\n\r");
		uip_close();
	}
	if(uip_timedout())
	{
		_DBG_("timeout\n\r");
		//��ʱ��ر�����
		uip_close();
	}
	if(uip_closed())
	{
		_DBG_("closed\n\r");
	}
	
	if(uip_connected())
	{
		state->state = CONNECTED;
		state->leftDataLen = 100;
		_DBG_("Client Connected\n\r");
	}
	//�յ�Ӧ��
	if(uip_acked())
	{

		if(state->state == RESPONSE_SEND)
		{
			uip_close();
			_DBG_("Close client Connection\n\r");
		}
	}
	if(uip_newdata())
	{
		if(state->state == RESPONSE_SEND && uip_datalen()!=0)
		{
			//�Ƿ���Ҫ����ظ��أ�
		}
	}
	//�Ѿ����ӵ���������ͨ����ѯ�������Ƿ�׼���ã�׼�����������ݵ�������
	if(uip_poll()) 
	{
		//��ѯ
		//�鿴�Ƿ���������Ҫ����
		sendOutLen = (uint32_t)(OSMboxAccept(mBoxClientDataLen))&0xffff;
		if(sendOutLen != 0)
		{
			state->leftDataLen = sendOutLen;
			OSSemPend(semClientDataOut,0,&err);
			uip_send(NetClientDataOut,sendOutLen);
			OSSemPost(semClientDataOut);
			state->leftDataLen = sendOutLen;//��Ҫ���͵����ݳ���Ϊ leftDataLen;
			state->state = RESPONSE_SEND;
			sprintf(_db,"client send len:%d\m\n\r",sendOutLen);
			DB;
		}
	}
	/*
	if(uip_poll() && uip_closed())
	{
		//��������Ѿ��رգ���鿴�Ƿ���Ҫ���¿���,����һЩsendoutLen����
		sendOutLen = (uint32_t)(OSMboxAccept(mBoxClientDataLen))&0xffff;
		if(sendOutLen != 0)
		{
			uip_restart();
			_DBG("Restart\n\r");
		}
	}
	*/
	#endif	
}
//����˴��� ��TcpApp�лص�
void ServerApp(void)
{
	char words[300];
	char receives[300];
	char * appdata;
	unsigned int recIndex;
	struct CommandState * state;
	uint32_t sendOutLen;
	INT8U err;
	state = (struct CommandState *) &(uip_conn->appstate);
	//_DBG_("(1)");
	if(uip_aborted())
	{
		//��������Ѿ���ֹ
		_DBG_(" server ABORTED\n\r");
		uip_close();
	}
	if(uip_timedout() )
	{
		_DBG_("server timeout\n\r");
		uip_close();
	}
	if(uip_closed())
	{
		_DBG_("server closed\n\r");
	}
	if(uip_connected())
	{
		state->state = CONNECTED;
		state->leftDataLen = 100;
		_DBG_("There is a client To Server\n\r");
	}
	//_DBG_("(2)");
	if(uip_acked())
	{
		if(state->state == RESPONSE_SEND)
		{
			//���ݷ�����ɣ���ر�����
			//uip_close();
			_DBG_("Close Server Connection\n\r");
		}
		//_DBG_("server Acked\n\r");
	}
	//_DBG_("(3)");
	//���յ��µ�����
	if(uip_newdata()||uip_connected()) 
	{
		//ȥ����־��֤
		//if(state->state == CONNECTED)
		{
			//�����ݸ��Ƶ�ѭ������������Ҫ��֤����
			appdata = (char * )uip_appdata;
			//_DBG_("10 ");
			OSSemPend(semNetBuf,0,&err);//��û�������
			//_DBG_("11 ");
			for(recIndex=0;recIndex<uip_datalen();recIndex++)
			{
				if(! ISBufFull(&bufNetRec))
				{
					//���������δ�� 
					BufPush(&bufNetRec,appdata[recIndex]);
				}
			}
			OSSemPost(semNetBuf);
			//_DBG_("12 ");
			//���ȷʵ��������������
			if(!ISBufEmpty(&bufNetRec))
			{
				//_DBG_("14 ");
				OSSemPost(semNetCommand);
				//_DBG_("15 ");
				state->state = COMMAND_RECEIVED;
			}
			
		}

	}
	//_DBG_("(4)");
	//��Ϊ ��Ҫ�ȴ�����׼���ã����Բ�ȡ��ѯ�ķ�ʽ��������
	if(uip_poll())
	{
		//_DBG_("(5)");
		//��ѯ
		//�鿴�Ƿ���������Ҫ����
		sendOutLen = (uint32_t)(OSMboxAccept(mBoxServerDataLen))&0xffff;
		if(sendOutLen != 0)
		{
			//_DBG_("(6)");
			state->leftDataLen = sendOutLen;
			OSSemPend(semServerDataOut,0,&err);
			//_DBG_("(7)");
			uip_send(NetServerDataOut,sendOutLen);
			OSSemPost(semServerDataOut);
			//_DBG_("(8)");
			state->leftDataLen = sendOutLen;//��Ҫ���͵����ݳ���Ϊ leftDataLen;
			state->state = RESPONSE_SEND;
		}			
	}

}
//uip TCP �ص�����
void TcpApp(void)
{
	
	if(uip_conn->lport== HTONS(APP_Persist_IP_Addr->ServerPort))
	{
		ServerApp();
	}
	else
	{
		ClientApp();
	}
}
