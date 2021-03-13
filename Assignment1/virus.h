#ifndef __VIRUS__
#define __VIRUS__


#include "skiplist.h"
#include "bloomfilter.h"

struct List{
    char* name;
    struct BloomFilter* vacc_bloom;
    VacSkipList vaccinated;
    struct BloomFilter* not_vacc_bloom;
    NotVacSkipList not_vacc;
    struct List* next;
};


void addinlist(struct List** virus_list, char* name);
int checkifinlist(struct List* virus_list, char* name);
int deletelist(struct List** virus_list);
struct List* getelemfromlist(struct List* virus_list, char* element);
void printlist(struct List* virus_list);

#endif