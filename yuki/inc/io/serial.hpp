#pragma once
#include <cstdint>

#define PORT 0x3F8

int serialInit();
void write_serial(char a);