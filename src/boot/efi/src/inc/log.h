#pragma once

enum DEBUG_TYPE {INFO, WARNING, ERROR};

void init_serial_services();

void ser_putchar(char c);

void bdebug(enum DEBUG_TYPE type, char *string);