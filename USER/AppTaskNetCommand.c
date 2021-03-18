#include "AppTaskNetCommand.h"
#include "RingBuffer.h"
#include "AppDebug.h"
#include "PacketAnalysis.h"

extern uint8_t NetServerDataOut[262];//����
extern OS_EVENT * semNetCommand;//�Ƿ����������ݰ���Ҫ���� ��ʼΪ0
extern OS_EVENT * semNetBuf;//�������ݽ��ջ��������� ��ʼΪ1
extern Ring_Buffer_T bufNetRec;//������ջ�����
void AppTaskNetCommand(void * pdata)
{
	INT8U err;
	uint8_t datas[262];
	uint8_t rx;
	uint8_t hasPacket;//�Ƿ������ݰ�������
	int len,i;
	uint8_t result;
	pdata = pdata;
	while(1)
	{
		hasPacket = 0;
		_DBG_("N");
		OSSemPend(semNetCommand,0,&err);//�Ƿ���������Ҫ����
		_DBG_("E");
		OSSemPend(semNetBuf,0,&err);//��û�������
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
					//һ�������İ��������
					sprintf(_db,"Net Packet len=%d\n\r",len);
					DB;
					hasPacket = 1;
					//���������ݴӻ������޳�
					while(!ISBufEmpty(&bufNetRec))
					{
						if(bufNetRec.buf[bufNetRec.head] == PACKET_SOI) break;//ֱ���а���ֹ��
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
			result = PacketAnalyse(datas,len);//��ʼ������
			sprintf(_db,"Net Command len=%d",BufLen(&bufNetRec));
			DB;
		}
		OSTimeDly(5);
	}
}
