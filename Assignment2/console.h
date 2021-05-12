#ifndef __CONSOLE__
#define __CONSOLE__


#include "virus.h"
#include "citizens.h"
#include "countries.h"
#include "sh_pipes.h"

void console( struct MonitorStruct *commun, CountryMainHash countries);

int datecmp(char* date1, char* date2);
int in_last_six(char *date);
char *get_cur_date();

#endif