#include "HexToAsccii.h"
#include "PacketAnalysis.h"
#include "AppDebug.h"

extern uint8_t NetServerDataOut[265];//服务器发送包
extern uint32_t NetServerDataOutLen;
char getHexToChar(uint8_t hex)
{
	if(hex>=0 && hex <= 9) return hex +'0';
	else 
		return hex-10+'A';
}
uint8_t getCharToHex(char ch)
{
	if(ch >= '0' && ch<='9') return ch-'0';
	else
		return ch - 'A'+10;
}
void Packet2DataOut(Package pack)
{
	int len,i,dataLen;
	uint8_t chkSum;
	NetServerDataOut[0] = 0x7E;
	//版本号转换成ascii
	NetServerDataOut[1] = getHexToChar(pack.cVer >> 4) ;
	NetServerDataOut[2] =  getHexToChar(pack.cVer & 0x0f) ;
	//地址转换成ascii
	NetServerDataOut[3] = getHexToChar(pack.AddrHigh >>4) ;
	NetServerDataOut[4] =  getHexToChar(pack.AddrHigh & 0x0f) ;
	NetServerDataOut[5] = getHexToChar(pack.AddrLow >>4) ;
	NetServerDataOut[6] =  getHexToChar(pack.AddrLow & 0x0f);
	//cid1转换为ascii
	NetServerDataOut[7] = getHexToChar(pack.cCid1 >>4);
	NetServerDataOut[8] = getHexToChar( pack.cCid1 & 0x0f);
	//cid2转换为ascii
	NetServerDataOut[9] = getHexToChar(pack.cCid2 >>4);
	NetServerDataOut[10] =  getHexToChar(pack.cCid2 & 0x0f);
	//长度装换为ascii
	NetServerDataOut[11] = getHexToChar((uint8_t)(pack.InfoLen >> 8) >> 4) ;
	NetServerDataOut[12] = getHexToChar((uint8_t)(pack.InfoLen >> 8) & 0x0f);
	NetServerDataOut[13] = getHexToChar((uint8_t)(pack.InfoLen&0x00ff) >> 4) ;
	NetServerDataOut[14] = getHexToChar((pack.InfoLen&0x00ff) & 0x0f) ;
	len = pack.InfoLen;
	dataLen = 15;
	for(i=1;i<=len;i++)
	{
		
		NetServerDataOut[dataLen++] = getHexToChar((uint8_t)(pack.Info[i-1] >> 4)) ;
		NetServerDataOut[dataLen++] = getHexToChar((uint8_t)(pack.Info[i-1] & 0x0f)) ;
	}
	
	//dataLen为数据包实际长度
	chkSum = GetCheckSum(NetServerDataOut,len*2);
	NetServerDataOut[dataLen] = getHexToChar((uint8_t)(chkSum >> 4)) ;
	NetServerDataOut[dataLen+1] = getHexToChar((uint8_t)(chkSum & 0x0f)) ;
	dataLen +=2;
	NetServerDataOut[dataLen] = 0x0d;
	NetServerDataOutLen = dataLen +1;
}
