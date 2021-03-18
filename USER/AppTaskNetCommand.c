#include "AppTaskNetCommand.h"
#include "RingBuffer.h"
#include "AppDebug.h"
#include "PacketAnalysis.h"

extern uint8_t NetServerDataOut[262];//最大包
extern OS_EVENT * semNetCommand;//是否有网络数据包需要解析 初始为0
extern OS_EVENT * semNetBuf;//网络数据接收缓冲区互斥 初始为1
extern Ring_Buffer_T bufNetRec;//网络接收缓冲区
void AppTaskNetCommand(void * pdata)
{
	INT8U err;
	uint8_t datas[262];
	uint8_t rx;
	uint8_t hasPacket;//是否有数据包到来？
	int len,i;
	uint8_t result;
	pdata = pdata;
	while(1)
	{
		hasPacket = 0;
		_DBG_("N");
		OSSemPend(semNetCommand,0,&err);//是否有数据需要解析
		_DBG_("E");
		OSSemPend(semNetBuf,0,&err);//获得缓冲区的
		len = -1;
		while(!ISBufEmpty(&bufNetRec))
		{
			rx = BufPop(&bufNetRec);
			sprintf(_db,"%d \n\r",rx);
			DB;
			if(len>=0)
			{
				datas[len++] = rx;
				if(rx == PACKET_EOI)
				{
					//一个完整的包接收完毕
					sprintf(_db,"Net Packet len=%d\n\r",len);
					DB;
					hasPacket = 1;
					//将冗余数据从缓冲区剔除
					while(!ISBufEmpty(&bufNetRec))
					{
						if(bufNetRec.buf[bufNetRec.head] == PACKET_SOI) break;//直到有包起止符
						BufPop(&bufNetRec);
					}
				}
			}
			else
			if((rx == PACKET_SOI) && ( len== -1))
			{
				len = 0;
				datas[len++] = rx;
				sprintf(_db,"start =%d(%d)\n\r",datas[0],rx);
				DB;
			}
		}
		OSSemPost(semNetBuf);
		for(i=0;i<len;i++)
		{
			sprintf(_db,"%c \n\r",datas[i]);
			DB;
		}
		_DBG_("?");
		if(hasPacket>0)
		{
			result = PacketAnalyse(datas,len);//开始包解析
			sprintf(_db,"Net Command len=%d",BufLen(&bufNetRec));
			DB;
		}
		OSTimeDly(5);
	}
}
