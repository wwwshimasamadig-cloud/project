#ifndef IO_H
#define IO_H

#include "sheet.h"

void savesheet(sheet *s, const char name[]);
void loadsheet(sheet *s, const char name[]);

#endif