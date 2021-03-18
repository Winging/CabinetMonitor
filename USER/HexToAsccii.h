#ifndef __HEX_TO_ASCII__
#define __HEX_TO_ASCII__
#include "includes.h"
#include "PacketAnalysis.h"

void Packet2DataOut(Package pack);
char getHexToChar(uint8_t hex);
uint8_t getCharToHex(char ch);
#endif
