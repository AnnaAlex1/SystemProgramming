//records.h

#ifndef __RECORDS__
#define __RECORDS__

#include <stdbool.h>

#include "citizens.h"
#include "virus.h"
#include "countries.h"


#define LINE_LEN 60


#define ID 1
#define FIRST 2
#define LAST 3
#define COUNTRY 4
#define AGE 5
#define VIRUS 6
#define DONE 7
#define DATE 8


struct CitRecord{
    char *citizenID;
    char *firstname; 
    char *lastname;
    char *country;
    int age;
    
    char *virus;
    bool done;
    char *date_vacc;
};




int read_file(char* filename, Hashtable ht, struct List** virus_list, CountryHash countries);

void insert_record(Hashtable ht,  struct List** virus_list, CountryHash countries, struct Citizen citizen, char* virus, bool done, char* date, bool from_console);

bool check_done(bool done, char* id, char* virus, struct List *virus_list, bool from_console);
bool check_date(bool done, char* id, char* virus, char* date, struct List *virus_list);

bool is_date(char* string);


#endif