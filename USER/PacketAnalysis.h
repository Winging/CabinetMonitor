#ifndef __PACKET_ANALYSIS__
#define __PACKET_ANALYSIS__

#include "includes.h"

#define PACKET_SOI 0x7E //包起始
#define PACKET_EOI 0X0D //包结束
#define PACKET_DATA_STPOS 15 //数据从第15个字节开始

#define COM_START   1
#define COM_DATA    8
#define COM_STOP    1
#define COM_SPEED   9600

/*CID2 中返回码 RTN 定义*/
#define RTN_OK              0x00    //正常
#define RTN_VER_ERROR       0x01    //VER 错
#define RTN_CHKSUM_ERROR    0x02    //CHKSUM 错
#define RTN_LCHKSUM         0x03    //暂时没用
#define RTN_CID2_INVALID    0x04    //CID2无效
#define RTN_COMMAND_ERROR   0X05    //命令格式错误
#define RTN_INVALID_DATE    0x06    //无效数据
#define RTN_OTHER_ERROR     0x07    //其它错误

/*设备类型编码分配表(CID1)*/
#define CID1_AC                 0x40    //开关电源系统(交流配电)
#define CID1_ALL                0x41    //开关电源系统(整流模块)
#define CID1_DC                 0x42    //开关电源系统(直流配电)
#define CID1_ENV_SYS            0xE1    //环境监控系统
#define CID1_SET_SYS_STATUS     0x01    //获取环境监控系统状态
#define CID1_SET_SYS_TH         0x02    //设置环境温湿度增益、偏移量
#define CID1_GET_SYS_TH         0x03    //获取环境温湿度增益、偏移量
#define CID1_SET_AC_DC          0x04    //设置直流交流电压增益跟偏移量
#define CID1_GET_AC_DC          0x05    //获取直流交流电压增益跟偏移量
#define CID1_SET_DC_ALARM       0x06    //设置直流电压过、欠、缺压告警点
#define CID1_GET_DC_ALARM       0x07    //获取直流电压过、欠、缺压告警点
#define CID1_SET_AC_ALARM       0x08    //设置交流电压过、欠、缺压告警点
#define CID1_GET_AC_ALARM       0x09    //获取交流电压过、欠、缺压告警点
#define CID1_SET_TH_HIGH_LOWER  0x0A    //设置温湿度过高过低告警点
#define CID1_GET_TH_HIGH_LOWER  0x0B    //获取温湿度过高过低告警点
#define CID1_SET_CLOCK          0x12  

  //设置时钟(年、月、日、时、分)
#define CID1_GET_CLOCK          0x13    //获取时钟(年、月、日、时、分)
#define CID1_SET_FAN            0x1F    //设置风扇控制参数
#define CID1_GET_FAN           0x20    //获取风扇控制参数
#define CID1_SET_HEAT           0x21    //设置加热器控制参数
#define CID1_GET_HEAT           0x22    //获取加热器控制参数


/***************************
 *除 SOI 和 EOI 

是以 16 进制传输外,其余各项都是以 16 进制-ASCII 码方式传输,每
个字节用两个 ASCII 码表示,如当 

CID2=4BH 时,传输时传送 34H(’4’的 ASCII 码)
和 42H(’B’的 ASCII 码)两个字节
 

***************************/
typedef struct PackageRec{
    uint8_t            cSoi;//起始位,0x7E
    uint8_t            cVer;//通信协议版本号
    uint8_t            AddrHigh;//地址 高位
	uint8_t            AddrLow;//地址 低位
    uint8_t            cCid1;//控制标识码
    uint8_t            cCid2;//命令信息:控制标识码(数据或动作类型描述) 响应信息:返回码 RTN
    uint16_t		InfoLen;//INFO 16字节
    uint8_t	        Info[70];//INFO;INFO 传送时的字节数,如果 INFO 为空,则 LENTH=0x00H;(需要的时候申请动态变量)
    uint8_t            cChksum;//校验和码;CHKSUM 的计算是除 SOI、EOI 和 CHKSUM 外,其他字符以传输格式累加求和,所得结果模 255 余数
    uint8_t            cEoi;//结束码,0x0D
} Package;



/*函数声明***************************************************************************/
uint8_t GetCheckSum(uint8_t packet[],uint32_t infoLen);
#endif // STDAFX_H
