#ifndef __BEEP_H__
#define __BEEP_H__
#include "includes.h"
#include "AppPersist.h"

void BeepInit();
void BeepOn();
void BeepOff();
void BeepOpen();
void BeepLock();
uint8_t GetBeepState();
#endif
