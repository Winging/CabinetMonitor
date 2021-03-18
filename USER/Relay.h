#ifndef __RELAY_H__
#define __RELAY_H__
#include "includes.h"
void InitRelay(void);
void RelayOutput(uint8_t number,uint8_t output);
void AlarmRelayOut(uint8_t number);
void AlarmResumeRelayOut(uint8_t number);
#endif
