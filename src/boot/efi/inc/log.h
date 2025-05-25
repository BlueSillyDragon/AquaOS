#pragma once

enum DEBUG_TYPE {INFO, WARNING, ERROR, NONE};

void initSerialServices();

void serPutchar(char c);

void bdebug(enum DEBUG_TYPE type, char *string, ...);