#ifndef __CONSOLE__
#define __CONSOLE__


#include "virus.h"
#include "citizens.h"
#include "countries.h"

void console(struct List** VirusList, Hashtable citizens, CountryHash countries);

void populationStatus_one(struct List *VirusList, Hashtable citizens, CountryHash countries, char* country, char* virusname, char* date1, char* date2);
void populationStatus_all(struct List *VirusList, Hashtable citizens, CountryHash countries, char* virusname, char* date1, char* date2);

void popStatusByAge_all(struct List *VirusList, Hashtable citizens, CountryHash countries, char* virusname, char* date1, char* date2);
void popStatusByAge_one(struct List *VirusList, Hashtable citizens, CountryHash countries, char* country, char* virusname, char* date1, char* date2);

int datecmp(char* date1, char* date2);
char *get_cur_date();

#endif