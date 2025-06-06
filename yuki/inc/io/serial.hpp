#pragma once
#include <cstdint>

#define PORT 0x3F8

int serialInit();
void writeSerial(char a);
void stringToSerial(char *string);