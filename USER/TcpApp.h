#ifndef __TCPAPP_H__
#define __TCPAPP_H__
#include "includes.h"
/*
 *@描述：初始化TCP通信
 */
void InitTCP(void);
/*
 *@描述：TCP 通信应用函数，uip回调
 */
void TcpApp(void);
/*
 *@描述：链接远程服务器,准备发送数据
 */
void ConnectServer(void);
#endif
