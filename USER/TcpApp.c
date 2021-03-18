#include "TcpApp.h"
#include "RingBuffer.h"
#include "AppDebug.h"
#include "AppPersist.h"

struct uip_conn * client_conn;//发送端/客户端TCP链接
struct uip_conn * server_conn;//接收端/服务端TCP链接
extern Ring_Buffer_T bufNetRec;//网络接收缓冲区
extern OS_EVENT * semNetCommand;//是否有网络数据包需要解析 初始为0
extern OS_EVENT * semNetBuf;//网络数据接收缓冲区互斥 初始为1
extern uint8_t NetServerDataOut[265];//服务器发送包
extern uint32_t NetServerDataOutLen;
//extern uint8_t NetClientDataOut[265];//客户端发送包
//extern uint32_t NetClientDataOutLen;
extern OS_EVENT * semServerDataOut;//服务端发送数据互斥
extern OS_EVENT * semClientDataOut;//客户端发送数据互斥
extern OS_EVENT * mBoxServerDataLen;//短消息，需要发送的数据长度
extern OS_EVENT * mBoxClientDataLen;//短消息，需要发送的数据长度
void ClientInit(void);
void ServerInit(void);
void ClientApp(void);
void ServerApp(void);
//初始化客户端发送链接
void ClientInit(void)
{
	uip_ipaddr_t ipaddr[2];
	struct CommandState * state;
	_DBG_("begin client INit\n\r");
	uip_ipaddr(ipaddr,192,168,0,3);
	client_conn = uip_connect(ipaddr,HTONS(2015));
	state = (struct CommandState *) &(uip_conn->appstate);//应用状态
	state->state = IDLE;
	if(client_conn != NULL)
	_DBG_("Client INit\n\r");
	else
	{
		_DBG_("Client fail\n\r");
	}
}
//初始化本地服务器接收链接
void ServerInit()
{
	_DBG_("begin SEREVER INit\n\r");
	uip_listen(HTONS(APP_Persist_IP_Addr->ServerPort));
	_DBG_("Server INit\n\r");
}
//初始化TCP链接
void InitTCP(void)
{
	ServerInit();
	BufInit(&bufNetRec);
}
/*
 *@描述：链接远程服务器,准备发送数据
 */
void ConnectServer(void)
{
	//OSMboxPost(mBoxClientStart,(void*)(0xff000000 | 1));
	ClientInit();
}
//客户端处理 在TcpApp中回调
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
	state = (struct CommandState *) &(uip_conn->appstate);//应用状态
	if(uip_aborted())
	{
		//如果链接已经终止
		_DBG("aborted\n\r");
		uip_close();
	}
	if(uip_timedout())
	{
		_DBG_("timeout\n\r");
		//超时则关闭链接
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
	//收到应答
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
			//是否需要处理回复呢？
		}
	}
	//已经链接到服务器则通过轮询看数据是否准备好，准备好则发送数据到服务器
	if(uip_poll()) 
	{
		//轮询
		//查看是否有数据需要发送
		sendOutLen = (uint32_t)(OSMboxAccept(mBoxClientDataLen))&0xffff;
		if(sendOutLen != 0)
		{
			state->leftDataLen = sendOutLen;
			OSSemPend(semClientDataOut,0,&err);
			uip_send(NetClientDataOut,sendOutLen);
			OSSemPost(semClientDataOut);
			state->leftDataLen = sendOutLen;//需要发送的数据长度为 leftDataLen;
			state->state = RESPONSE_SEND;
			sprintf(_db,"client send len:%d\m\n\r",sendOutLen);
			DB;
		}
	}
	/*
	if(uip_poll() && uip_closed())
	{
		//如果链接已经关闭，则查看是否需要重新开启,借用一些sendoutLen变量
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
//服务端处理 在TcpApp中回调
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
		//如果链接已经终止
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
			//数据发送完成，则关闭链接
			//uip_close();
			_DBG_("Close Server Connection\n\r");
		}
		//_DBG_("server Acked\n\r");
	}
	//_DBG_("(3)");
	//接收到新的数据
	if(uip_newdata()||uip_connected()) 
	{
		//去掉标志认证
		//if(state->state == CONNECTED)
		{
			//将数据复制到循环缓冲区，需要保证互斥
			appdata = (char * )uip_appdata;
			//_DBG_("10 ");
			OSSemPend(semNetBuf,0,&err);//获得缓冲区的
			//_DBG_("11 ");
			for(recIndex=0;recIndex<uip_datalen();recIndex++)
			{
				if(! ISBufFull(&bufNetRec))
				{
					//如果缓冲区未满 
					BufPush(&bufNetRec,appdata[recIndex]);
				}
			}
			OSSemPost(semNetBuf);
			//_DBG_("12 ");
			//如果确实有数据则传送命令
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
	//因为 需要等待数据准备好，所以采取轮询的方式发送数据
	if(uip_poll())
	{
		//_DBG_("(5)");
		//轮询
		//查看是否有数据需要发送
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
			state->leftDataLen = sendOutLen;//需要发送的数据长度为 leftDataLen;
			state->state = RESPONSE_SEND;
		}			
	}

}
//uip TCP 回调函数
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
