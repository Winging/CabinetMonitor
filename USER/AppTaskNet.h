#ifndef __APP_TASK_NET_H__
#define __APP_TASK_NET_H__
#include "includes.h"
#include "AppDebug.h"
//uipͨ���������
#define BUF ((struct uip_eth_hdr *)&uip_buf[0]) //��������ָ��
void AppTaskNet(void * pdata);
#endif
