#ifndef __APP_TASK_NET_H__
#define __APP_TASK_NET_H__
#include "includes.h"
#include "AppDebug.h"
//uip通信相关声明
#define BUF ((struct uip_eth_hdr *)&uip_buf[0]) //网络数据指针
void AppTaskNet(void * pdata);
#endif
